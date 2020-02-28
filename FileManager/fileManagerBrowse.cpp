



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
	listOnlyWavFromActualPath(explorerListLength);
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



void cFileManager::listOnlyWavFromActualPath(uint8_t startPoint)
{
	sdLocation.close();
	sdLocation.open(explorerCurrentPath, O_READ);

	uint8_t filesFound = sdLocation.createFilesList(0, explorerList+startPoint, (list_length_max-startPoint), 4000, 2);
	sdLocation.close();
	uint8_t afterFolderNum = explorerListLength;

	bool notSorted = 1;
	while (notSorted)
	{
		notSorted = 0;
		for (uint8_t a = startPoint; a < startPoint + filesFound - 1; a++)
		{
			if (strcasecmp(explorerList[a], explorerList[a + 1]) > 0)
			{
				std::swap(explorerList[a], explorerList[a+1]);
				notSorted = 1;
			}
		}
	}

	explorerListLength += filesFound;

	if(explorerListLength <= startPoint)
	{
		// finisz
		browseFinish();
	}
	else
	{
		openCalcStart = startPoint;
		openCalcEnd = explorerListLength;
		openCurrentPos = openCalcStart;

		moveToNextOperationStep();
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

	currentFolderMemoryFileUsage[openCurrentPos] = 2 * getWaveSizeIfValid(filePath);

	openCurrentPos++;

	uint8_t progress = (((openCurrentPos - openCalcStart) * 100) / (openCalcEnd-openCalcStart));

	//showOpeningHorizontalBar(progress); //todo progress

	if(openCurrentPos == openCalcEnd)
	{
		sdLocation.close();

		for(uint8_t i = openCalcStart; i < openCalcEnd; i++)
		{
			explorerList[i] = explorerList[i];
		}

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
	sdLocation.close();

	if(!sdLocation.open(cProjectsPath, O_READ))
	{
		stopOperationWithError(fmBrowseProjectsError);
		return;
	}

	uint8_t projectsfoundCount = sdLocation.createProjectsList(projectsList, list_length_max, 3000);
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

	uint8_t locationFileCount = sdLocation.createFilesList(0, firmwaresList, list_length_max, 4000, 3);

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
