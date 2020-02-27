#include "mtSamplesLoader.h"
extern int16_t sdram_effectsBank[4*1024*1024];
extern int16_t sdram_sampleBank[4*1024*1024];

uint32_t SamplesLoader::calcSamplesFolderSize()
{
	char currentPatch[PATCH_SIZE];

	uint32_t size = 0;

	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		sprintf(currentPatch,"Workspace/samples/instr%02d.wav",i);

		size += waveLoader.getInfoAboutWave(currentPatch);
	}

	return size;
}


void SamplesLoader::update()
{
	if(state == loaderStateTypeInProgress)
	{
		char currentPatch[PATCH_SIZE];

		if(waveLoader.getState() == loaderStateTypeEnded)
		{

			sprintf(currentPatch,"Workspace/samples/instr%02d.wav",currentIndex);


			if(mtProject.instrument[currentIndex].isActive == 0)
			{
				mtProject.instrument[currentIndex].sample.length = 0;
				mtProject.instrument[currentIndex].sample.wavetable_window_size = 0;


				if(currentIndex + 1 < INSTRUMENTS_COUNT)
				{
					mtProject.instrument[currentIndex+1].sample.address = mtProject.instrument[currentIndex].sample.address;
					currentIndex++;
				}
				else
				{
					memoryUsageChange = 1;
					if(firstLoadFlag) firstLoadFlag = 0;
					state = loaderStateTypeEnded;
				}
				currentStepLoadSize = 0;
				return;
			}
//			if(mtProject.instrument[currentIndex].sample.type == mtSampleTypeWavetable)
//			{
//
//			}
//			else
//			{
			currentSize = waveLoader.start(currentPatch, mtProject.instrument[currentIndex].sample.address);
//				if(mtProject.used_memory + currentSize*2 > mtProject.max_memory) bieda rozwiazanie
//				{
//					waveLoader.stop();
//					waveLoader.setStopStatus(0);
//					// obsluga przepelnienia
//				}
//				if(currentSize == 0)
//				{
//					mtProject.instrument[currentIndex].sample.loaded = 0;
//					mtProject.instrument[currentIndex].sample.length = 0;
//					mtProject.instrument[currentIndex].sample.wavetable_window_size = 0;
//
//					if((currentIndex + 1) < INSTRUMENTS_COUNT)
//					{
//						mtProject.instrument[currentIndex + 1].sample.address = mtProject.instrument[currentIndex].sample.address;
//						currentIndex++;
//					}
//					else
//					{
//						state = loaderStateTypeEnded;
//					}
//
//				}
//				return;
//			}
		}

		waveLoader.update();
		currentLoadSize -=  currentStepLoadSize;
		currentStepLoadSize = waveLoader.getCurrentWaveLoadedMemory();
		currentLoadSize += currentStepLoadSize;

		if(waveLoader.getStopStatus() == 0)
		{
			mtProject.instrument[currentIndex].isActive = 0;
			mtProject.instrument[currentIndex].sample.length = 0;
			mtProject.instrument[currentIndex].sample.wavetable_window_size = 0;


			if(currentIndex + 1 < INSTRUMENTS_COUNT)
			{
				mtProject.instrument[currentIndex+1].sample.address = mtProject.instrument[currentIndex].sample.address;
				currentIndex++;
			}
			else
			{
				memoryUsageChange = 1;
				if(firstLoadFlag) firstLoadFlag = 0;
				state = loaderStateTypeEnded;
			}
			currentStepLoadSize = 0;
			waveLoader.setStopStatus(2); // status readed
		}
		else if(waveLoader.getStopStatus() == 1)
		{
			int32_t actuallyLoadedBytes = waveLoader.getCurrentWaveLoadedMemory();;
			mtProject.used_memory += currentSize*2;
			mtProject.instrument[currentIndex].isActive=1;
			mtProject.instrument[currentIndex].sample.length = actuallyLoadedBytes < currentSize ? actuallyLoadedBytes : currentSize;
			if(mtProject.instrument[currentIndex].granular.grainLength > mtProject.instrument[currentIndex].sample.length) mtProject.instrument[currentIndex].granular.grainLength = mtProject.instrument[currentIndex].sample.length;

			if(mtProject.instrument[currentIndex].playMode == playModeWavetable)
			{
				mtProject.instrument[currentIndex].sample.type = mtSampleTypeWavetable;
				uint32_t lastCurrentWindow = mtProject.instrument[currentIndex].wavetableCurrentWindow;
				uint32_t lastWindowNumber = mtProject.instrument[currentIndex].sample.wavetableWindowNumber;
				mtProject.instrument[currentIndex].sample.wavetable_window_size =
						(mtProject.instrument[currentIndex].sample.length >= mtProject.instrument[currentIndex].sample.wavetable_window_size) ?
						mtProject.instrument[currentIndex].sample.wavetable_window_size : mtProject.instrument[currentIndex].sample.length;

				//*******************************wavetable window size moze byc tylko potęgą 2
				// Jezeli length nie jest potega 2 trzeba go zrownac do najwiekszej mozliwej potegi 2
				uint16_t localMask = 2048;
				while( !(mtProject.instrument[currentIndex].sample.wavetable_window_size & localMask) )
				{
					if((mtProject.instrument[currentIndex].sample.wavetable_window_size == 0 )) break;
					localMask>>=1;
				}

				mtProject.instrument[currentIndex].sample.wavetable_window_size &= localMask;
				//**************************************************************************
				mtProject.instrument[currentIndex].sample.wavetableWindowNumber = mtProject.instrument[currentIndex].sample.wavetable_window_size ? mtProject.instrument[currentIndex].sample.length/mtProject.instrument[currentIndex].sample.wavetable_window_size : 0;
				mtProject.instrument[currentIndex].wavetableCurrentWindow = map(lastCurrentWindow,0,mtProject.instrument[currentIndex].sample.wavetableWindowNumber,0,lastWindowNumber);
			}

			loadedFlagChange = 1;
			if( (currentIndex+1) < INSTRUMENTS_COUNT)
			{
				mtProject.instrument[currentIndex+1].sample.address = mtProject.instrument[currentIndex].sample.address+mtProject.instrument[currentIndex].sample.length;
				currentIndex++;
			}
			else
			{
				memoryUsageChange = 1;
				if(firstLoadFlag) firstLoadFlag = 0;
				state = loaderStateTypeEnded;
			}

			currentStepLoadSize = 0;
			mtProject.instruments_count++;
			waveLoader.setStopStatus(2); // status readed
		}

	}
}

uint8_t SamplesLoader::getMemoryUsageChangeFlag()
{
	return memoryUsageChange;
}

void SamplesLoader::clearMemoryUsageChangeFlag()
{
	memoryUsageChange = 0;
}

uint8_t SamplesLoader::getLoadChangeFlag()
{
	return loadedFlagChange;
}
void SamplesLoader::clearLoadChangeFlag()
{
	loadedFlagChange = 0;
}

uint8_t SamplesLoader::getStateFlag()
{
	return state;
}
uint8_t  SamplesLoader::getFirstLoadFlag()
{
	return firstLoadFlag;
}

void SamplesLoader::setFilesToLoad(uint8_t filesNum)
{
	filesToLoad = filesNum;
}

void SamplesLoader::start(uint8_t startIndex, char * projectPatch, uint8_t firstLoad  )
{
	strcpy(currentProjectPatch,projectPatch);
	state =  loaderStateTypeInProgress;
	firstLoadFlag = firstLoad;
	currentLoadSize = 0;
	currentStepLoadSize = 0;
	sizeAllFiles = 0;
	currentIndex = startIndex;
	mtProject.used_memory = 0;
	mtProject.instruments_count = 0;

/*
	if(filesToLoad < 1)
	{
		filesToLoad = 1;
	}
*/
	// przeliczanie adresu nowow ladowanego sampla na podstawie wczesniejszych sampli jesli startowy > 0
	for(uint8_t i = 0; i < startIndex; i++)
	{
		if(mtProject.instrument[i].isActive)
		{
			mtProject.used_memory += 2*mtProject.instrument[i].sample.length;
			mtProject.instrument[startIndex].sample.address = mtProject.instrument[i].sample.address + mtProject.instrument[i].sample.length;
			mtProject.instruments_count++;
		}
	}

	// obliczanie dlugosci wszsytkich plikow po miejscu wstawiania
	for(uint8_t i = startIndex + 1; i < INSTRUMENTS_COUNT; i ++)
	{
		if(mtProject.instrument[i].isActive)
		{
			sizeAllFiles += mtProject.instrument[i].sample.length;
		}
	}

	if(firstLoad) sizeAllFiles = calcSamplesFolderSize();

	char currentPatch[PATCH_SIZE];

	for(uint8_t file = 0; file < filesToLoad; file++)
	{

		sprintf(currentPatch,"Workspace/samples/instr%02d.wav",startIndex+file);

		if(SD.exists(currentPatch))
		{
			sizeAllFiles+= waveLoader.getInfoAboutWave(currentPatch);
		}

	}

	if(mtProject.instruments_count == 0)  mtProject.instrument[startIndex].sample.address = sdram_sampleBank;
}

uint8_t SamplesLoader::getCurrentProgress()
{
	uint8_t progress;

	progress = ((currentLoadSize * 100) / sizeAllFiles);


	return progress;
}
