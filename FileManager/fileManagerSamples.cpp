
#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"


#include "fileTransfer.h"
#include "fileManager.h"

extern int16_t sdram_effectsBank[4*1024*1024];
extern int16_t sdram_sampleBank[4*1024*1024];
extern uint8_t sdram_writeLoadBuffer[READ_WRITE_BUFOR_SIZE];



static char sampleToLoad[WORKSPACE_SAMPLES_FORMAT_MAX_LENGTH];
static int16_t* ptrSampleMemory;


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     LOAD     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::loadSamplesFromWorkspace()
{
	if(currentSample >= INSTRUMENTS_COUNT) { moveToNextOperationStep(); return;} //zabiezpeiczenie
	if(!sampleInProgress) startSampleLoad(); // wykonaj funkcje tylko 1 raz

	uint8_t loadStatus = fileTransfer.loadSampleToMemory(sampleToLoad, ptrSampleMemory, &currentSampleSamplesCount);

	if(loadStatus == fileTransferEnd && currentSampleSamplesCount > 0)
	{
		mtProject.used_memory += currentSampleSamplesCount*2; // zwieksz uzycie pamieci
		completeLoadedSampleStruct(true); // wypelnij strukture dodanego sampla dodatkowymi danymi
		lastActiveInstrument = currentSample; // do ustalania przydzielania pamieci
		sampleInProgress = 0;
		moveToNextOperationStep();
	}
	else if(loadStatus == fileTransferFileNoExist || loadStatus == fileTransferFileNoValid || loadStatus == fileTransferEnd)
	{
		completeLoadedSampleStruct(false);
		sampleInProgress = 0;
		moveToNextOperationStep();
	}
	else if(loadStatus >= fileTransferError)
	{
		sampleThrowError();
	}
}

void cFileManager::startSampleLoad()
{
	sampleInProgress = 1;

	if(currentSample == 0)
	{
		mtProject.instrument[0].sample.length = 0;
		mtProject.instrument[0].sample.address = sdram_sampleBank;
	}

	mtProject.instrument[currentSample].sample.address =
			mtProject.instrument[lastActiveInstrument].sample.address + mtProject.instrument[lastActiveInstrument].sample.length;

	ptrSampleMemory = mtProject.instrument[currentSample].sample.address;

	sprintf(sampleToLoad, cWorkspaceSamplesFilesFormat, currentSample+1);
}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     COPY     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::copySamples()
{
	if(currentOperation == fmCopyWorkspaceToProjects)
	{
		sprintf(currentCopySrcPath, cWorkspaceSamplesFilesFormat, currentSample+1); // nazwa pliku od 1
		sprintf(currentCopyDestPath, cProjectsSamplesFilesFormat, currentProjectName, currentSample+1); // nazwa pliku od 1
	}
	else if(currentOperation == fmCopyProjectsToWorkspace)
	{
		sprintf(currentCopySrcPath, cProjectsSamplesFilesFormat, currentProjectName, currentSample+1); // nazwa pliku od 1
		sprintf(currentCopyDestPath, cWorkspaceSamplesFilesFormat, currentSample+1); // nazwa pliku od 1
	}
	else // import
	{
		sprintf(currentCopySrcPath, "%s/%s", explorerCurrentPath, explorerList[importCurrentFile]); // nazwa pliku od 1
		sprintf(currentCopyDestPath, cWorkspaceSamplesFilesFormat, currentSample+1);
	}

	uint8_t loadStatus = fileTransfer.copySample(currentCopySrcPath, currentCopyDestPath);

	if(loadStatus == fileTransferEnd)
	{
		//continueSampleProcess();
		moveToNextOperationStep();
	}
	else if(loadStatus == fileTransferFileNoExist)
	{
		//continueSampleProcess();
		moveToNextOperationStep();
	}
	else if(loadStatus >= fileTransferError)
	{
		sampleThrowError();
	}
}


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     SAVE     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::saveSamplesToWorkspace()
{

	moveToNextOperationStep();
}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     IMPORT     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     DELETE     ---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::deleteSamplesFromWorkspace()
{
	for(uint8_t sampl = currentSample; sampl <= deleteEndSample; sampl++)
	{
		char file_path[WORKSPACE_SAMPLES_FORMAT_MAX_LENGTH];
		sprintf(file_path, cWorkspaceSamplesFilesFormat, sampl+1);

		if(SD.exists(file_path))
		{
			SD.remove(file_path);
		}
	}
	moveToNextOperationStep();
}


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     XXXX     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::moveSampleMemory()
{
	moveToNextOperationStep();
	if(firstSlotToMoveInMemory >= 48) // nie ma sampli do przesuniecia w pamieci
	{
		return;
	}

	uint8_t last_instrument_to_move = firstSlotToMoveInMemory;
	uint8_t i = firstSlotToMoveInMemory;

	// znajdz ostatni instrument do przesuniecia
	while(i < INSTRUMENTS_COUNT )
	{
		if(mtProject.instrument[i].isActive == 1)
		{
			last_instrument_to_move = i;
		}

		i++;
	}

	// wyznacz przesuniecie i blok do przesuniecia

	int32_t memory_offset;
	if(currentOperation == fmImportSamplesToWorkspace)
	{
		memory_offset = fileTransfer.getConvertedSampleSize();
	}
	else if(currentOperation == fmDeleteInstruments)
	{
		if(currentInstrument > 0)
		{
			memory_offset = (mtProject.instrument[currentInstrument-1].sample.address
							+ mtProject.instrument[currentInstrument-1].sample.length*2)
							- mtProject.instrument[firstSlotToMoveInMemory].sample.address;
		}
		else
		{
			memory_offset = sdram_sampleBank - mtProject.instrument[firstSlotToMoveInMemory].sample.address;
		}

	}



	uint8_t* begining_address = (uint8_t*)mtProject.instrument[firstSlotToMoveInMemory].sample.address;
	uint8_t* end_address = (uint8_t*)mtProject.instrument[last_instrument_to_move].sample.address
									+ mtProject.instrument[last_instrument_to_move].sample.length*2;

	int32_t memory_size = end_address-begining_address;

	// przesuń
	do
	{
		if(READ_WRITE_BUFOR_SIZE <= memory_size)
		{
			memcpy(sdram_writeLoadBuffer, begining_address+memory_size, READ_WRITE_BUFOR_SIZE);
			memory_size -= READ_WRITE_BUFOR_SIZE;
			memcpy(memory_offset+begining_address+memory_size, sdram_writeLoadBuffer, READ_WRITE_BUFOR_SIZE);
		}
		else
		{
			memcpy(sdram_writeLoadBuffer, begining_address+memory_size, memory_size);
			memory_size = 0;
			memcpy(memory_offset+begining_address+memory_size, sdram_writeLoadBuffer, memory_size);
		}
	}
	while(memory_size <= 0);

}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     XXXX     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

void cFileManager::sampleThrowError()
{
	sampleInProgress = 0;
	currentSample = 0;

	throwError(0);
}

void cFileManager::completeLoadedSampleStruct(bool success)
{
	if(success == false)
	{
		memset(mtProject.instrument[currentSample].sample.file_name, 0, SAMPLE_NAME_SIZE);
		mtProject.instrument[currentSample].isActive = 0;
		mtProject.instrument[currentSample].sample.length = 0;
		mtProject.instrument[currentSample].sample.wavetable_window_size = 0;

		return;
	}


	mtProject.instrument[currentSample].isActive = 1;
	mtProject.instrument[currentSample].sample.length = currentSampleSamplesCount;

	if(mtProject.instrument[currentSample].granular.grainLength > mtProject.instrument[currentSample].sample.length)
			mtProject.instrument[currentSample].granular.grainLength = mtProject.instrument[currentSample].sample.length;

	if(mtProject.instrument[currentSample].playMode == playModeWavetable)
	{
		mtProject.instrument[currentSample].sample.type = mtSampleTypeWavetable;
		uint32_t lastCurrentWindow = mtProject.instrument[currentSample].wavetableCurrentWindow;
		uint32_t lastWindowNumber = mtProject.instrument[currentSample].sample.wavetableWindowNumber;
		mtProject.instrument[currentSample].sample.wavetable_window_size =
				(mtProject.instrument[currentSample].sample.length >= mtProject.instrument[currentSample].sample.wavetable_window_size) ?
				mtProject.instrument[currentSample].sample.wavetable_window_size : mtProject.instrument[currentSample].sample.length;

		//*******************************wavetable window size moze byc tylko potęgą 2
		// Jezeli length nie jest potega 2 trzeba go zrownac do najwiekszej mozliwej potegi 2
		uint16_t localMask = 2048;
		while( !(mtProject.instrument[currentSample].sample.wavetable_window_size & localMask) )
		{
			if((mtProject.instrument[currentSample].sample.wavetable_window_size == 0 )) break;
			localMask>>=1;
		}

		mtProject.instrument[currentSample].sample.wavetable_window_size &= localMask;
		//**************************************************************************
		mtProject.instrument[currentSample].sample.wavetableWindowNumber =
				mtProject.instrument[currentSample].sample.wavetable_window_size
				? mtProject.instrument[currentSample].sample.length/mtProject.instrument[currentSample].sample.wavetable_window_size
				: 0;
		mtProject.instrument[currentSample].wavetableCurrentWindow =
				map(lastCurrentWindow, 0, mtProject.instrument[currentSample].sample.wavetableWindowNumber, 0, lastWindowNumber);
	}
}

uint32_t cFileManager::calcWorkspaceSamplesSize()
{
	char currentPatch[PATCH_SIZE];
	uint32_t size = 0;

	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		sprintf(currentPatch, cWorkspaceSamplesFilesFormat, i);

		size += getWaveSizeIfValid(currentPatch);
	}

	return size;
}

uint32_t cFileManager::getWaveSizeIfValid(const char *filename)
{
	uint8_t wav_len = strlen(filename);
	if(wav_len<5) return 0;

	if(((filename[wav_len - 1] != 'V') && (filename[wav_len - 1] != 'v'))
	|| ((filename[wav_len - 2] != 'A') && (filename[wav_len - 2] != 'a'))
	|| ((filename[wav_len - 3] != 'W') && (filename[wav_len - 3] != 'w'))
	||  (filename[wav_len - 4] != '.')) return 0;

	if(!SD.exists(filename)) return 0;

	wavfile.open(filename);
	if(!wavfile)
	{
		wavfile.close();
		return 0;
	}

	strWavFileHeader localSampleHead;
	readHeader(&localSampleHead,&wavfile);
	wavfile.close();

	if((localSampleHead.format != 1163280727)
		||((localSampleHead.AudioFormat != 1) && (localSampleHead.AudioFormat != 3) )
		||((localSampleHead.bitsPerSample != 16) && (localSampleHead.bitsPerSample != 24) && (localSampleHead.bitsPerSample != 32))
		|| localSampleHead.sampleRate != 44100 )
	{
		return 0;
	}

	return localSampleHead.subchunk2Size/localSampleHead.blockAlign;
}


uint32_t cFileManager::getActualSampleMemoryLoaded()
{
	return (ptrSampleMemory-sdram_sampleBank)+fileTransfer.getBytesComplited();
}
