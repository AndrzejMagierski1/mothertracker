
#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"


#include "fileTransfer.h"
#include "fileManager.h"

extern int16_t sdram_effectsBank[4*1024*1024];
extern int16_t sdram_sampleBank[4*1024*1024];


static SdFile wavfile;
static char sampleToLoad[PATCH_SIZE];
static int16_t* ptrSampleMemory;


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     LOAD     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

void cFileManager::loadSamplesFromWorkspace()
{
	startSampleLoad();
	if(sampleLoadPhase > 1) continueSampleLoad(); // omija dalsze ladowanie pliku/ow jezeli byl jakis sygnal ze trzeba to zrobic

	uint8_t loadStatus = fileTransfer.loadSampleToMemory(sampleToLoad, ptrSampleMemory, &currentSampleSamplesCount);

	if(loadStatus == fileTransferEnd)
	{
		continueSampleLoad();
	}
	else if(loadStatus == fileTransferFileNoExist || loadStatus == fileTransferFileNoValid)
	{
		currentSampleSamplesCount  = 0;
		continueSampleLoad();
	}
	else if(loadStatus >= fileTransferError)
	{
		instrumentThrowError();
	}
}

void cFileManager::startSampleLoad()
{
	// wykonaj funkcje tylko 1 raz
	if(sampleLoadPhase != 0) return;
	sampleLoadPhase = 1;

	mtProject.used_memory = 0; // uzycie = 0 na start

	// zawsze od 0 !!!
	currentSample = 0;

	// znajdz pierwszy aktywny ktory trzeba wczytac, na podstawie wczytanych wczesniej plikow instrumentow
	while(mtProject.instrument[currentSample].isActive == 0)
	{
		currentSample++;
		if(currentSample >= INSTRUMENTS_COUNT)
		{
			// jesl nie znaleziono zadnego aktywnego to nie ma co robic i odrazu konczy ladowanie sampli
			sampleLoadPhase = 2; // << wymusi wywolanie continueSampleLoad();
			return;
		}
	}

	// przygotowanie strukturey pierwszego sampla do zaladowania
	mtProject.instrument[currentSample].sample.length = 0;
	mtProject.instrument[currentSample].sample.wavetable_window_size = 0;
	mtProject.instrument[currentSample].sample.address = sdram_sampleBank;
	ptrSampleMemory = mtProject.instrument[currentSample].sample.address;

	sprintf(sampleToLoad, cWorkspaceSamplesFilesFormat, currentSample);
}

void cFileManager::continueSampleLoad()
{
	if(currentSample < INSTRUMENTS_COUNT) // zakoncz ladowanie jesli currentSample >= 48
	{
		// przetworz zaladowany sampel
		if(currentSampleSamplesCount == 0) // jesli jakis blad np brak pliku wav
		{
			memset(mtProject.instrument[currentSample].sample.file_name, 0, SAMPLE_NAME_SIZE);
			mtProject.instrument[currentSample].isActive = 0;
			mtProject.instrument[currentSample].sample.length = 0;
			mtProject.instrument[currentSample].sample.wavetable_window_size = 0;
		}
		else
		{
			mtProject.used_memory += currentSampleSamplesCount*2; // zwieksz uzycie pamieci

			completeLoadedSampleStruct(); // wypelnij strukture dodanego sampla dodatkowymi danymi
		}

		// przygotuj struktury na nastepny sampel
		currentSample++;
		if(currentSample < INSTRUMENTS_COUNT) // zakoncz ladowanie jesli currentSample >= 48
		{
			mtProject.instrument[currentSample].sample.address =
					mtProject.instrument[currentSample-1].sample.address + mtProject.instrument[currentSample-1].sample.length;
			sprintf(sampleToLoad, cWorkspaceSamplesFilesFormat, currentSample);
			ptrSampleMemory = mtProject.instrument[currentSample].sample.address;

			return; // nastepny sampel poprosze
		}
	}
	// koncz ladowanie sampli
	sampleLoadPhase = 0;
	currentSample = 0;
	moveToNextOperationStep();
}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     COPY     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::copySamplesToWorkspace()
{
	sprintf(currentCopySrcPath, cProjectsSamplesFilesFormat, currentProjectName, currentSample);
	sprintf(currentCopyDestPath, cWorkspaceSamplesFilesFormat, currentSample);

	uint8_t loadStatus = fileTransfer.copyFile(currentCopySrcPath, currentCopyDestPath);

	if(loadStatus == fileTransferEnd)
	{
		continueSampleProcess();
	}
	else if(loadStatus == fileTransferFileNoExist)
	{
		continueSampleProcess();
	}
	else if(loadStatus >= fileTransferError)
	{
		sampleThrowError();
	}
}


void cFileManager::continueSampleProcess()
{
	currentSample++;
	if(currentSample >= INSTRUMENTS_COUNT)
	{
		currentSample = 0;
		moveToNextOperationStep();
	}
}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     XXXX     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

void cFileManager::sampleThrowError()
{
	sampleLoadPhase = 0;
	currentSample = 0;

	throwError(0);
}

void cFileManager::completeLoadedSampleStruct()
{
	mtProject.instrument[currentSample].isActive = 1;
	mtProject.instrument[currentSample].sample.length = currentSampleSamplesCount;

	if(mtProject.instrument[currentSample].granular.grainLength > mtProject.instrument[currentSample].sample.length) mtProject.instrument[currentSample].granular.grainLength = mtProject.instrument[currentSample].sample.length;

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
