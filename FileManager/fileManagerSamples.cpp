
#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"


#include "debugLog.h"

#include "fileTransfer.h"
#include "fileManager.h"

extern int16_t sdram_effectsBank[4*1024*1024];
extern int16_t sdram_sampleBank[4*1024*1024];


#define READ_WRITE_BUFOR_SIZE 32640
extern uint8_t sdram_writeLoadBuffer[READ_WRITE_BUFOR_SIZE];
//extern uint8_t sdram_writeLoadBuffer2[READ_WRITE_BUFOR_SIZE];



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
		//lastActiveInstrument = currentSample; // do ustalania przydzielania pamieci
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

	mtProject.instrument[0].sample.address = sdram_sampleBank;

	if(currentSample == 0)
	{
		mtProject.instrument[0].sample.length = 0;
	}
	else
	{
		uint8_t activeBefore = currentInstrument-1;

		while(mtProject.instrument[activeBefore].isActive == 0)
		{
			if(activeBefore == 0) break;
			activeBefore--; // jesli sprawdzilo wszystkie to koczny
		}

		mtProject.instrument[currentSample].sample.address =
				mtProject.instrument[activeBefore].sample.address + mtProject.instrument[activeBefore].sample.length;

	}

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
	else if(currentOperation == fmSaveRecordedSound)
	{
		sprintf(currentCopySrcPath, "Recorded/%s.wav",  getRecordingFileName());
		sprintf(currentCopyDestPath, cWorkspaceSamplesFilesFormat, currentSample+1); // nazwa pliku od 1
	}
	else if(currentOperation == fmCopyInstrumentsInWorkspace) // copy
	{
		sprintf(currentCopySrcPath, cWorkspaceSamplesFilesFormat,  copySrcSlot+1);
		sprintf(currentCopyDestPath, cWorkspaceSamplesFilesFormat, currentSample+1); // nazwa pliku od 1
	}
	else // import
	{
		sprintf(currentCopySrcPath, "%s/%s", explorerCurrentPath, explorerList[importCurrentFile]);
		sprintf(currentCopyDestPath, cWorkspaceSamplesFilesFormat, currentSample+1);// nazwa pliku od 1
	}

	uint8_t loadStatus = fileTransfer.copySample(currentCopySrcPath, currentCopyDestPath);

	if(loadStatus == fileTransferEnd)
	{
		samplesMemoryCopied += fileTransfer.getBytesComplited();
		moveToNextOperationStep();
	}
	else if(loadStatus == fileTransferFileNoExist)
	{
		memset(mtProject.instrument[currentSample].sample.file_name, 0, SAMPLE_NAME_SIZE);
		mtProject.instrument[currentSample].isActive = 0;
		moveToNextOperationStep();
	}
	else if(loadStatus >= fileTransferError)
	{
		memset(mtProject.instrument[currentSample].sample.file_name, 0, SAMPLE_NAME_SIZE);
		mtProject.instrument[currentSample].isActive = 0;
		moveToNextOperationStep();
		//uszkodzony plik wav - "od-aktywuj" instrument ale nie wyrzucaj bledu
		//sampleThrowError();
	}
}


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     SAVE     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
// save pojedynczego pliku wav
void cFileManager::saveSamplesToWorkspace()
{
	if(currentSample >= INSTRUMENTS_COUNT) { moveToNextOperationStep(); return; } //zabiezpeiczenie
	//if(!sampleInProgress) startSampleLoad(); // wykonaj funkcje tylko 1 raz

	sprintf(currentCopyDestPath, cWorkspaceSamplesFilesFormat, currentSample+1); // nazwa pliku od 1

	uint8_t saveStatus = fileTransfer.saveSampleToFile(importFromSampleEditorAddress, currentCopyDestPath, importFromSampleEditorLength);

	if(saveStatus == fileTransferEnd)
	{
		moveToNextOperationStep();
	}
	else if(saveStatus >= fileTransferError)
	{
		sampleThrowError();
	}
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
	int32_t memory_offset = 0;
	if(currentOperation == fmImportSamplesToWorkspace)
	{
		// roznica wielkosci nowych sampli i wielkosci starych nadpisanych sampli
		memory_offset = newSamplesSize - oldSamplesSize;
	}
	else if(currentOperation == fmCopyInstrumentsInWorkspace)
	{
		// roznica wielkosci kopiowanych i wielksoci sampli w miejscu docelowym
		memory_offset = newSamplesSize - oldSamplesSize;
	}
	else if(currentOperation == fmDeleteInstruments)
	{
		if(currentInstrument == 0)
		{
			memory_offset = (uint8_t*)sdram_sampleBank - (uint8_t*)mtProject.instrument[firstSlotToMoveInMemory].sample.address;
		}
		else
		{
			// znajdz początkowy adres
			int8_t activeBefore = currentInstrument-1;

			while(activeBefore >= 0 && mtProject.instrument[activeBefore].isActive == 0)
			{
				activeBefore--; // jesli sprawdzilo wszystkie to koczny
			}

			if(activeBefore < 0)
			{
				// brak aktywnego poprzedzajacego skasowany
				memory_offset = (uint8_t*)sdram_sampleBank - (uint8_t*)mtProject.instrument[firstSlotToMoveInMemory].sample.address;
			}
			else
			{
				// znaleziony aktywny poprzedzajacy skasowany
				memory_offset = ((uint8_t*)mtProject.instrument[activeBefore].sample.address
						+ mtProject.instrument[activeBefore].sample.length*2)
						- (uint8_t*)mtProject.instrument[firstSlotToMoveInMemory].sample.address;
			}
		}
	}
	else if(currentOperation == fmImportSampleFromSampleEditor)
	{
		// roznica wielkosci importowanego sampla i wielksoci sampla w miejscu docelowym
		memory_offset = newSamplesSize - oldSamplesSize;
	}

	// jak nic sie nie przesuwa to kończ
	if(memory_offset == 0) return;

	uint8_t* begining_address = (uint8_t*)mtProject.instrument[firstSlotToMoveInMemory].sample.address;
	uint8_t* end_address = (uint8_t*)mtProject.instrument[last_instrument_to_move].sample.address
									+ mtProject.instrument[last_instrument_to_move].sample.length*2;


	moveMemory(begining_address, end_address, memory_offset);

	// zmien adresy przesunietch instrumentów
	for(uint8_t i = firstSlotToMoveInMemory; i<=last_instrument_to_move; i++)
	{
		if(mtProject.instrument[i].isActive)
		{
			mtProject.instrument[i].sample.address = (int16_t*)(((uint8_t*) mtProject.instrument[i].sample.address) + memory_offset);
		}
	}
}


void cFileManager::moveMemory(uint8_t* memoryStart, uint8_t* memoryEnd, int32_t memoryOffset)
{
	char message[100];
	sprintf(message, "Move memory from %d to %d by %d", memoryStart-((uint8_t*)sdram_sampleBank), memoryEnd-((uint8_t*)sdram_sampleBank), memoryOffset);
	debugLog.addLine(message);
	debugLog.forceRefresh();


	volatile int32_t memory_size = memoryEnd-memoryStart;
	int32_t stepSize = READ_WRITE_BUFOR_SIZE;

	if(memoryOffset > 0)
	{
		if(stepSize > memoryOffset) 		stepSize = memoryOffset;

		// przesuń do góry
		do
		{
			if(memory_size >= stepSize)
			{
				memcpy(sdram_writeLoadBuffer, (memoryStart+memory_size)-stepSize, stepSize);
				memcpy((memoryStart+memory_size+memoryOffset)-stepSize, sdram_writeLoadBuffer, stepSize);
				memory_size -= stepSize;
			}
			else
			{
				stepSize = memory_size;
				memcpy(sdram_writeLoadBuffer, (memoryStart+memory_size)-stepSize, stepSize);
				memcpy((memoryStart+memoryOffset+memory_size)-stepSize, sdram_writeLoadBuffer, stepSize);
				memory_size -= stepSize;
			}
		}
		while(memory_size > 0);

	}
	else if(memoryOffset < 0)
	{
		if(stepSize > -memoryOffset) 	stepSize = -memoryOffset;

		int32_t total_moved = 0;
		// przesuń w dól
		do
		{
			if(memory_size-total_moved >= stepSize)
			{
				memcpy(sdram_writeLoadBuffer, memoryStart+total_moved, stepSize);
				memcpy(memoryStart+memoryOffset+total_moved, sdram_writeLoadBuffer, stepSize);
				total_moved += stepSize;
			}
			else
			{
				stepSize = memory_size-total_moved;
				memcpy(sdram_writeLoadBuffer, memoryStart+total_moved, stepSize);
				memcpy(memoryStart+memoryOffset+total_moved, sdram_writeLoadBuffer, stepSize);
				total_moved += stepSize;
			}
		}
		while(total_moved < memory_size);

	}



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


uint32_t cFileManager::calcProjectSamplesSize()
{
	char currentPatch[PATCH_SIZE];
	uint32_t size = 0;

	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		sprintf(currentPatch, cProjectsSamplesFilesFormat, currentProjectName, i);

		size += getWaveSizeIfValid(currentPatch);
	}

	return size;
}

uint32_t cFileManager::calcImportSamplesSize()
{
	char currentPatch[PATCH_SIZE];
	uint32_t size = 0;

	for(uint8_t i = importCurrentFile; i < (importCurrentFile+(importEndSlot-importStartSlot)+1); i++)
	{
		sprintf(currentPatch, "%s/%s", explorerCurrentPath, explorerList[i]);

		size += getFileSizePlus(currentPatch);
	}

	return size;
}


// wielksoc wava w bajtach po konwersji na mono-16bit
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

	return (localSampleHead.subchunk2Size/localSampleHead.blockAlign) * 2;
}

// wielksoc pliku wave w bajtach + wielkosc po konwersji
uint32_t cFileManager::getFileSizePlus(const char *filename)
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

	uint32_t fileSize = wavfile.size();

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

	return fileSize + (localSampleHead.subchunk2Size/localSampleHead.blockAlign) * 2;
}

uint32_t cFileManager::getActualSampleMemoryLoaded()
{
	return ((uint8_t*)ptrSampleMemory-(uint8_t*)sdram_sampleBank)
			+ (fileTransfer.getTransferStep() == 1 ? fileTransfer.getBytesComplited() : 0);
}


uint32_t cFileManager::getTotalSampleMemoryCopied()
{
	return samplesMemoryCopied;
			//+ (fileTransfer.getTransferStep() == 1 ? fileTransfer.getBytesComplited() : 0);
}
