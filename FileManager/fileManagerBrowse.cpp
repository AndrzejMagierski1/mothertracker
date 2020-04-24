



#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"
#include "mtConfig.h"
#include "debugLog.h"


#include "mtSequencer.h"

#include "fileManager.h"




bool cFileManager::browseSdCard(uint8_t* index)
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;

	// dla bezpieczenstwa reset sciezki jesli jestesmy na poziomie 0
	if(explorerDirLevel == 0)
	{
		strcpy(explorerCurrentPath, "/");
	}

	if(index == nullptr) // tylko odswiez
	{
		status = fmBrowsingSamples;
		currentOperationStep = 0;
		currentOperation = fmBrowseSamples;

		return true;
	}

	if(explorerDirLevel == 0)
	{
		explorerDirLevel = 1;

		strcpy(explorerCurrentPath, explorerList[*index]);


		explorerPositionTable[explorerCurrentPosition]=*index;
		explorerCurrentPosition++;

		*index=0;

	}
	else
	{
		if(*index > 0)
		{
			if(explorerCurrentPosition<PREVIOUS_POSITION_LIFO)
			{
				explorerDirLevel++;

				explorerPositionTable[explorerCurrentPosition]=*index;
				explorerCurrentPosition++;


				strcat(explorerCurrentPath, explorerList[*index]);

				*index=0;

			}

		}
		else
		{
			explorerDirLevel--;

			if(explorerDirLevel > 0)
			{
				goUpInActualPath();
			}
			else
			{
				strcpy(explorerCurrentPath, "/");
			}

			explorerCurrentPosition--;
			*index=explorerPositionTable[explorerCurrentPosition];

		}
	}



	status = fmBrowsingSamples;
	currentOperationStep = 0;
	currentOperation = fmBrowseSamples;

	return true;
}



// wymusza przy nastepnym wywolaniu browseSdCard()
// wyzerowanie pokazywanej lokalizaji do poczatku karty SD
void cFileManager::resetBrowse()
{
	explorerDirLevel = 0;
	explorerCurrentPosition = 0;
	//strcpy(explorerCurrentPath, "/");
}

uint8_t cFileManager::getBrowsedFilesList(char*** list, uint32_t** memoryList)
{
	*list = explorerList;
	*memoryList = currentFolderMemoryFileUsage;

	return explorerListLength;
}


void cFileManager::browseCurrentLocation()
{
	explorerListLength = 0;

	listOnlyFolderNames();
	moveToNextOperationStep();
}


void cFileManager::listOnlyFolderNames()
{
	uint8_t allFoldersNum;
	sdLocation.close();
	sdLocation.open(explorerCurrentPath, O_READ);

	if(explorerDirLevel == 0)
	{
		allFoldersNum = sdLocation.createFilesList(0,explorerList, list_length_max, 5000, 1);
	}
	else
	{
		allFoldersNum = sdLocation.createFilesList(1,explorerList, list_length_max, 5000, 1);
		explorerList[0] = new char[4];
		strcpy(explorerList[0], "/..");
	}

	sdLocation.close();

	bool notSorted = 1;
	//char strBuff[40];
	while (notSorted)
	{
		notSorted = 0;
		for (uint8_t a = 0; a < allFoldersNum - 1; a++)
		{

			if (strcasecmp(explorerList[a], explorerList[a + 1]) > 0)
			{
				std::swap(explorerList[a], explorerList[a+1]);
				notSorted = 1;
			}
		}
	}

	explorerListLength += allFoldersNum;

}



void cFileManager::listOnlyWavFromActualPath()
{

	if(listWavStage == 0) // stage 0
	{
		sdLocation.close();
		sdLocation.open(explorerCurrentPath, O_READ);

		uint8_t filesFound = sdLocation.createFilesList(0, explorerList+explorerListLength, (list_length_max-explorerListLength), 8000, 2);
		sdLocation.close();

		if(filesFound == 0) // nie ma plikow wav w folderze - koncz listowanie
		{
			browseFinish();
			return;
		}

		openCalcStart = explorerListLength;
		openCurrentPos = openCalcStart;
		explorerListLength += filesFound;
		openCalcEnd = explorerListLength;

		listWavStage = 1;
	}
	else if(listWavStage == 1) // stage 1 - przefiltruj znalezione wav o nie obslugiwane
	{

		char wav_file[255];
		strcpy(wav_file, explorerCurrentPath);
		strcat(wav_file, "/");
		strcat(wav_file, explorerList[openCurrentPos]);

		if(wavfile.open(wav_file))
		{
			strWavFileHeader localHeader;
			readHeader(&localHeader, &wavfile);
			wavfile.close();

			if ((localHeader.sampleRate != 44100)
			|| ((localHeader.AudioFormat != 1) && (localHeader.AudioFormat != 3))
			|| ((localHeader.bitsPerSample != 16) && (localHeader.bitsPerSample != 24) && (localHeader.bitsPerSample != 32)))
			{

				if(openCurrentPos < explorerListLength-1)
				{
					std::swap(explorerList[openCurrentPos], explorerList[explorerListLength-1]);
				}
				else
				{
					browseFinish();
					listWavStage = 2;
				}
				explorerListLength--;
			}
			else
			{
				openCurrentPos++;
				if(openCurrentPos >= explorerListLength)
				{
					listWavStage = 2;
				}
			}

		}
		else
		{
			if(openCurrentPos > 1)	explorerListLength = openCurrentPos;
			else explorerListLength = 1;
			browseFinish();
			listWavStage = 2;
		}
	}
	else //  stage 2 - posortuj alfabetycznie
	{
		if(openCalcStart < explorerListLength-1)
		{
			bool notSorted = 1;
			while (notSorted)
			{
				notSorted = 0;
				for (uint8_t a = openCalcStart; a < explorerListLength - 1; a++)
				{
					if (strcasecmp(explorerList[a], explorerList[a + 1]) > 0)
					{
						std::swap(explorerList[a], explorerList[a+1]);
						notSorted = 1;
					}
				}
			}
		}

		openCurrentPos = openCalcStart;
		moveToNextOperationStep();
		listWavStage = 0;
	}
}



void cFileManager::processDirFileSizes() // wykonywalne w petli
{
	char filePath[255];

	strcpy(filePath, explorerCurrentPath);
	if(explorerDirLevel > 0)
	{
		strcat(filePath, "/");
	}

	strcat(filePath, explorerList[openCurrentPos]);

	currentFolderMemoryFileUsage[openCurrentPos] = getWaveSizeIfValid(filePath);

	openCurrentPos++;

	//uint8_t progress = (((openCurrentPos - openCalcStart) * 100) / (openCalcEnd-openCalcStart));
	//todo progress

	if(openCurrentPos >= explorerListLength)
	{
		sdLocation.close();

		browseFinish();
		//finisz
	}
}

void cFileManager::browseFinish()
{
	status = fmBrowseSamplesEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}



void cFileManager::goUpInActualPath()
{
	uint8_t length = strlen(explorerCurrentPath);

	explorerCurrentPath[length] = 0;

	char* ptr = strrchr(explorerCurrentPath, '/');

	if(ptr == nullptr)
	{
		strcpy(explorerCurrentPath, "/");
		return;
	}

	explorerCurrentPath[(ptr-explorerCurrentPath)] = 0;
}


///====================================================================================================
///====================================================================================================
///====================================================================================================
void cFileManager::browseProjectsLocation()
{
	if(!SD.exists(cProjectsPath))
	{
		SD.mkdir(0, cModsPath);
	}

	sdLocation.close();

	if(!sdLocation.open(cProjectsPath, O_READ))
	{
		stopOperationWithError(fmBrowseProjectsError);
		return;
	}

	uint8_t projectsfoundCount = sdLocation.createProjectsList(projectsList, list_length_max, 4000);
	sdLocation.close();

	for (uint8_t i = 0; i < (projectsfoundCount/2); i++)
	{
		std::swap(projectsList[i], projectsList[projectsfoundCount-i-1]);
	}

	projectsListLength = projectsfoundCount;

	status = fmBrowseProjectsEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}



bool cFileManager::browseProjects()
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;

	status = fmBrowsingProjects;
	currentOperationStep = 0;
	currentOperation = fmBrowseProjects;

	return true;
}

uint8_t cFileManager::getProjectsList(char*** list)
{
	*list = projectsList;
	return projectsListLength;
}

///====================================================================================================
///==================================MODZ==============================================================
///====================================================================================================
void cFileManager::browseModsLocation()
{
	if(!SD.exists(cModsPath))
	{
		SD.mkdir(0, cModsPath);
	}


	sdLocation.close();

	if (!sdLocation.open(cModsPath, O_READ))
	{
		stopOperationWithError(fmBrowseModsError);
		return;
	}

	uint8_t modsfoundCount = sdLocation.createFilesList(
			0,
			modsList,
			list_length_max,
			3000,
			SdDir::enListFileFilter_mod);
	sdLocation.close();

	for (uint8_t i = 0; i < (modsfoundCount / 2); i++)
	{
		std::swap(modsList[i], modsList[modsfoundCount - i - 1]);
	}

	modsListLength = modsfoundCount;

	status = fmBrowseModsEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}

bool cFileManager::browseMods()
{
	if (status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if (currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject)
		return false;

	status = fmBrowsingMods;
	currentOperationStep = 0;
	currentOperation = fmBrowseMods;

	return true;
}

uint8_t cFileManager::getModsList(char*** list)
{
	*list = modsList;
	return modsListLength;
}

///====================================================================================================
///====================================================================================================
///====================================================================================================
void cFileManager::browseFirmwaresLocation()
{
	sdLocation.close();

	if(!sdLocation.open("Firmware", O_READ))
	{
		stopOperationWithError(fmBrowseFirmwaresError);
		return;
	}

	uint8_t locationFileCount = sdLocation.createFilesList(0, firmwaresList, list_length_max, 6000, 3);

	sdLocation.close();

	bool notSorted = 1;
	while (notSorted)
	{
		notSorted = 0;
		for (uint8_t a = 0; a < locationFileCount - 1; a++)
		{
			if (strcasecmp(firmwaresList[a],
					firmwaresList[a + 1]) > 0)
			{
				std::swap(firmwaresList[a], firmwaresList[a+1]);

				notSorted = 1;
			}
		}
	}

	firmwaresListLength = locationFileCount;


	status = fmBrowseFirmwaresEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}


bool cFileManager::browseFirmwares()
{
	status = fmBrowsingFirmwares;
	currentOperationStep = 0;
	currentOperation = fmBrowseFirmwares;

	return true;
}


uint8_t cFileManager::getFirmwaresList(char*** list)
{
	*list = firmwaresList;
	return firmwaresListLength;
}
