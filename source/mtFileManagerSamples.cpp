
#include "mtHardware.h"
#include "mtStructs.h"


#include "mtFileManager.h"

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------


//********************************************************************SAMPLE LOADER************************************************************************************//

void SamplesLoader::update()
{
	if(state == loaderStateTypeInProgress)
	{
		char currentPatch[PATCH_SIZE];
		char number[3];

		if(waveLoader.getState() == loaderStateTypeEnded)
		{

			number[0] = ((currentIndex-currentIndex%10)/10) + 48;
			number[1] = currentIndex%10 + 48;
			number[2] = 0;

			if(fileManager.currentProjectPatch != NULL)
			{
				memset(currentPatch, 0, PATCH_SIZE);
				strcpy(currentPatch, fileManager.currentProjectPatch);
				strcat(currentPatch, "/samples/instr");
				strcat(currentPatch, number);
				strcat(currentPatch, ".wav");
			}

			if(mtProject.instrument[currentIndex].sample.type == mtSampleTypeWavetable)
			{

			}
			else
			{
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
			}
		}

		waveLoader.update();
		currentLoadSize -=  currentStepLoadSize;
		currentStepLoadSize = waveLoader.getCurrentWaveLoadedMemory();
		currentLoadSize += currentStepLoadSize;

		if(waveLoader.getStopStatus() == 0)
		{
			mtProject.instrument[currentIndex].sample.loaded = 0;
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
				state = loaderStateTypeEnded;
			}
			currentStepLoadSize = 0;
			waveLoader.setStopStatus(2); // status readed
		}
		else if(waveLoader.getStopStatus() == 1)
		{
			mtProject.used_memory += currentSize*2;
			mtProject.instrument[currentIndex].sample.loaded = 1;
			mtProject.instrument[currentIndex].sample.length = currentSize;
			loadedFlagChange = 1;
			if( (currentIndex+1) < INSTRUMENTS_COUNT)
			{
				mtProject.instrument[currentIndex+1].sample.address = mtProject.instrument[currentIndex].sample.address+currentSize;
				currentIndex++;
				mtProject.samples_count++;
			}
			else
			{
				memoryUsageChange = 1;
				state = loaderStateTypeEnded;
			}

			currentStepLoadSize = 0;
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

void SamplesLoader::start(uint8_t startIndex)
{
	state =  loaderStateTypeInProgress;
	currentLoadSize = 0;
	currentStepLoadSize = 0;
	sizeAllFiles = 0;
	currentIndex = startIndex;
	mtProject.used_memory = 0;
	mtProject.samples_count = 0;
	for(uint8_t i = 0; i < startIndex; i++)
	{
		if(mtProject.instrument[i].sample.loaded)
		{
			mtProject.used_memory += 2*mtProject.instrument[i].sample.length;
			mtProject.instrument[startIndex].sample.address = mtProject.instrument[i].sample.address + mtProject.instrument[i].sample.length;

			mtProject.samples_count ++;
		}
	}
	for(uint8_t i = startIndex + 1; i < INSTRUMENTS_COUNT; i ++)
	{
		if(mtProject.instrument[i].sample.loaded)
		{
			sizeAllFiles += mtProject.instrument[i].sample.length;
		}

	}
	char currentPatch[PATCH_SIZE];
	char number [3];



	number[0] = ((startIndex-startIndex%10)/10) + 48;
	number[1] = startIndex%10 + 48;
	number[2] = 0;

	if(fileManager.currentProjectPatch != NULL)
	{
		memset(currentPatch, 0, PATCH_SIZE);
		strcpy(currentPatch, fileManager.currentProjectPatch);
		strcat(currentPatch, "/samples/instr");
		strcat(currentPatch, number);
		strcat(currentPatch, ".wav");
	}

	if(SD.exists(currentPatch))
	{
		sizeAllFiles+= fileManager.samplesLoader.waveLoader.getInfoAboutWave(currentPatch);
	}

	if(mtProject.samples_count == 0)  mtProject.instrument[startIndex].sample.address = sdram_sampleBank;
}

uint8_t SamplesLoader::getCurrentProgress()
{
	return ((currentLoadSize * 100) / sizeAllFiles);
}

//**********************************************************************WAVELOADER************************************************************************************//
void WaveLoader::update()
{
	if(state == loaderStateTypeInProgress)
	{
		int32_t bufferLength;

		if(sampleHead.AudioFormat == 1)
		{
			int16_t buf16[256];
			if(sampleHead.numChannels == 1)
			{
				for(uint16_t i = 0 ; i< BUFOR_COUNT; i++)
				{
					if( wavfile.available() )
					{
						bufferLength = wavfile.read(buf16, 512);

						accBufferLength += bufferLength;

						for(int i=0; i< 256; i++)
						{
							if(bufferLength <= i ) break;
							else *currentAddress=buf16[i];
							currentAddress++;
						}
					}
					else
					{
						stopFlag = stop();
						break;
					}
				}

			}
			else if (sampleHead.numChannels == 2)
			{
				for(uint16_t i = 0 ; i< BUFOR_COUNT; i++)
				{
					if (wavfile.available() )
					{
						bufferLength = wavfile.read(buf16, 512);

						accBufferLength += bufferLength;
						for(int i=0; i< 256; i+=2)
						{
							if(bufferLength <= i ) break;
							else *currentAddress=buf16[i];
							currentAddress++;
						}
					}
					else
					{
						stopFlag = stop();
						break;
					}
				}
			}
		}
		else if(sampleHead.AudioFormat == 3)
		{
			float bufFloat[256];
			if(sampleHead.numChannels == 1)
			{
				for(uint16_t i = 0 ; i< BUFOR_COUNT; i++)
				{
					if( wavfile.available() )
					{
						bufferLength = wavfile.read(bufFloat, 1024);

						accBufferLength += bufferLength;

						for(int i=0; i< 256; i++)
						{
							if(bufferLength <= i ) break;
							else *currentAddress= ( ( (bufFloat[i] + 1.0) * 65535.0 ) / 2.0)  - 32768.0 ;
							currentAddress++;
						}
					}
					else
					{
						stopFlag = stop();
						break;
					}
				}

			}
			else if (sampleHead.numChannels == 2)
			{
				for(uint16_t i = 0 ; i< BUFOR_COUNT; i++)
				{
					if (wavfile.available() )
					{
						bufferLength = wavfile.read(bufFloat, 1024);

						accBufferLength += bufferLength;
						for(int i=0; i< 256; i+=2)
						{
							if(bufferLength <= i ) break;
							else *currentAddress=( ((bufFloat[i] + 1.0) * 65535.0 ) / 2.0)  - 32768.0 ;
							currentAddress++;
						}
					}
					else
					{
						stopFlag = stop();
						break;
					}
				}
			}

		}



	}
}
uint32_t WaveLoader::start(const char *filename, int16_t * buf)
{
	if(buf == NULL)
	{
		stopFlag = 0;
		return 0;
	}
	accBufferLength = 0;
	wavfile = SD.open(filename);
	readHeader(&sampleHead,&wavfile);
	currentAddress = buf;
	if ( (sampleHead.numChannels == 1 && (sampleHead.subchunk2Size > 8388608 )) &&  (sampleHead.numChannels == 2 && (sampleHead.subchunk2Size > 16777216)))
	{
		wavfile.close();
		if(hardwareTest)
		{
			Serial.println("too long file");
		}
		state = loaderStateTypeEnded;
		stopFlag = 0;
		return 0;
	}
	if(sampleHead.format != 1163280727 || ( (sampleHead.AudioFormat != 1) && (sampleHead.AudioFormat != 3) ) || ( (sampleHead.bitsPerSample != 16) && (sampleHead.bitsPerSample != 32)) || sampleHead.sampleRate != 44100 )
	{
		wavfile.close();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		state = loaderStateTypeEnded;
		stopFlag = 0;
		return 0;
	}

	if(hardwareTest)
	{
		Serial.println("load WAV header to SDRAM succesfull");
	}
	state = loaderStateTypeInProgress;
	stopFlag = -1;
	if(sampleHead.numChannels == 1)
	{
		if(sampleHead.AudioFormat == 3) return sampleHead.subchunk2Size/4;
		else return sampleHead.subchunk2Size/2;
	}
	else if(sampleHead.numChannels == 2)
	{
		if(sampleHead.AudioFormat == 3) return sampleHead.subchunk2Size/8;
		else return sampleHead.subchunk2Size/4;
	}
	else return 0;
}
uint8_t WaveLoader::stop()
{
	wavfile.close();
	state = loaderStateTypeEnded;
//	if(accBufferLength == sampleHead.subchunk2Size) return 1;
//	else return 0;
	return 1;
}

uint8_t WaveLoader::getState()
{
	return state;
}
uint8_t WaveLoader::getStopStatus()
{
	return stopFlag;
}
void WaveLoader::setStopStatus(uint8_t s)
{
	stopFlag = s;
}

uint32_t WaveLoader::getInfoAboutWave(const char *filename)
{
	strWavFileHeader localSampleHead;

	wavfile = SD.open(filename);
	readHeader(&localSampleHead,&wavfile);
	wavfile.close();

	if ( (localSampleHead.numChannels == 1 && (localSampleHead.subchunk2Size > 8388608 )) &&  (localSampleHead.numChannels == 2 && (localSampleHead.subchunk2Size > 16777216)))
	{
		return 0;
	}
	if(localSampleHead.format != 1163280727 || ( (localSampleHead.AudioFormat != 1) && (localSampleHead.AudioFormat != 3) )  || ((localSampleHead.bitsPerSample != 16) &&  (localSampleHead.bitsPerSample != 32) )|| localSampleHead.sampleRate != 44100 )
	{
		return 0;
	}

	if(localSampleHead.numChannels == 1)
	{
		if(localSampleHead.AudioFormat == 3) return localSampleHead.subchunk2Size/4;
		else return localSampleHead.subchunk2Size/2;
	}
	else if(localSampleHead.numChannels == 2)
	{
		if(localSampleHead.AudioFormat == 3) return localSampleHead.subchunk2Size/8;
		else return localSampleHead.subchunk2Size/4;
	}
	else return 0;
}

uint8_t WaveLoader::getCurrentWaveProgress()
{
	return ((accBufferLength * 100) / sampleHead.subchunk2Size);
}


uint32_t WaveLoader::getCurrentWaveLoadedMemory()
{
	if(sampleHead.AudioFormat == 1)
	{

		if(sampleHead.numChannels == 1)
		{
			return accBufferLength/2;
		}
		else if(sampleHead.numChannels == 2)
		{
			return accBufferLength/4;
		}
	}
	else if(sampleHead.AudioFormat == 3)
	{
		if(sampleHead.numChannels == 1)
		{
			return accBufferLength/4;
		}
		else if(sampleHead.numChannels == 2)
		{
			return accBufferLength/8;
		}
	}
	else return 0;
}
//**********************************************************************WAVETABLE LOADER******************************************************************************//
void WavetableLoader::update()
{
 return;
}
uint8_t WavetableLoader::start(const char *filename, int16_t * buf)
{
 return 0;
}
uint32_t WavetableLoader::stop()
{
 return 0;
}


int32_t WavetableLoader::fmLoadWavetable(const char *filename, int16_t * buf ,uint16_t * windowSize)
{
	strWavFileHeader sampleHead;
	FsFile wavfile;
	int32_t size=-1;

	wavfile = SD.open(filename);
	readHeader(&sampleHead,&wavfile);
	wavfile.close();
	if(sampleHead.AudioFormat == 1)
	{
		size=loadWavetableStandard(filename,buf);
		*windowSize = STANDARD_WAVETABLE_WINDOW_LEN;
	}
	else if(sampleHead.AudioFormat == 3)
	{
		size=loadWavetableSerum(filename,buf);
		*windowSize = SERUM_WAVETABLE_WINDOW_LEN;
	}
	return size;

}
//********************************************************************************************************************************************************************//
void FileManager::update()
{
	samplesLoader.update();
}


int32_t loadWavetableStandard(const char *filename, int16_t * buf)
{
	strWavFileHeader sampleHead;
	uint16_t bufferLength=0;
	int16_t buf16[256];
	int16_t buf16_1024[1024];
	int16_t windowDivider=0;
	int16_t currentWave[1024];
	int16_t nextWave[1024];

	FsFile wavfile;

	uint16_t numberOfWindows=0;
	uint16_t missingWindows=0;
	float windowsControl=0;
	float windowsCounter=0;
	uint16_t buforCounter=0;

	wavfile = SD.open(filename);
	wavfile.read(&sampleHead, 44);



	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 1  || sampleHead.bitsPerSample != 16  || sampleHead.sampleRate != 44100 )
	{
		wavfile.close();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
			//mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		return -1;
	}
	else
	{
		if(hardwareTest)
		{
			Serial.println("load WAV header to SDRAM succesfull");
			//mtPrint("load WAV header to SDRAM succesfull");
		}
	}

	numberOfWindows= (sampleHead.subchunk2Size/2)/STANDARD_WAVETABLE_WINDOW_LEN;
	if(numberOfWindows == STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		if(sampleHead.numChannels == 1)
		{
			while ( wavfile.available() )
			{
				bufferLength = wavfile.read(buf16, 512);
				for(int i=0; i< 256; i++)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=buf16[i];
					buf++;
				}
			}
		}
		else if (sampleHead.numChannels == 2)
		{
			while (wavfile.available() )
			{
				bufferLength = wavfile.read(buf16, 512);
				for(int i=0; i< 256; i+=2)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=buf16[i];
					buf++;
				}
			}
		}

		wavfile.close();

	}

	else if(numberOfWindows > STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		;
	}
	else if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		missingWindows=STANDARD_WAVETABLE_WINDOWS_NUMBER-numberOfWindows;
		windowsControl=(float)STANDARD_WAVETABLE_WINDOWS_NUMBER/(missingWindows+1);

		if(sampleHead.numChannels == 1)
		{
			while ( wavfile.available() )
			{
				if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
				{
					bufferLength=wavfile.read(buf16_1024, 2048);
					for(int i=0; i< 1024; i++)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=buf16_1024[i];
						buf++;
						currentWave[i]=buf16_1024[i];
					}
					buforCounter++;
				}
				else
				{
					bufferLength=wavfile.read(nextWave, 2048);
					windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);

					for(uint8_t i=0;i<windowDivider;i++)
					{
						for(uint16_t j=0; j< 1024; j++)
						{
							*buf=currentWave[j]+(((nextWave[j]-currentWave[j])/(windowDivider+1))*i);
							buf++;
						}
					}
					for(int i=0; i< 1024; i++)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=nextWave[i];
						buf++;

						currentWave[i]=nextWave[i];
					}
					buforCounter++;
					buforCounter+=(windowDivider);
					windowsCounter+=(windowDivider*windowsControl);
				}
			}
		}
		else if (sampleHead.numChannels == 2)
		{
			// nie trzeba ale zostawiam jakieś bałagany
			/*while ( wavfile.available() )
			{
				if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
				{
					bufferLength=wavfile.read(buf16_2048, 4096);
					for(int i=0; i< 2048; i+=2)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=buf16_2048[i];
						buf++;
						currentWave_2048[i]=buf16_2048[i];
					}
					buforCounter++;
				}
				else
				{
					bufferLength=wavfile.read(buf16_2048, 4096);
					for(int i=0; i< 2048; i+=2)
					{
						if(bufferLength <= i ) nextWave_2048[i]=0;
						else nextWave_2048[i]=buf16_2048[i];
					}
					windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);

					for(uint8_t i=0;i<windowDivider;i++)
					{
						for(uint16_t j=0; j< 1024; j++)
						{
							*buf=currentWave_2048[j]+(((nextWave_2048[j]-currentWave_2048[j])/(windowDivider+1))*i);
							buf++;
						}
					}
					for(int i=0; i< 1024; i++)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=nextWave_2048[i];
						buf++;

						currentWave_2048[i]=nextWave_2048[i];
					}
					buforCounter++;
					buforCounter+=(windowDivider);
					windowsCounter+=(windowDivider*windowsControl);
				}
			}*/
		}

		wavfile.close();
	}

	return STANDARD_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
}


int32_t loadWavetableSerum(const char *filename, int16_t * buf)
{
	//strSerumWavetableFileHeader sampleHead;
	strWavFileHeader sampleHead;
	uint16_t bufferLength=0;

	float bufFloat[2048];
	float currentWave[2048];
	float nextWave[2048];

	int16_t windowDivider=0;
	uint16_t numberOfWindows=0;
	uint16_t missingWindows=0;
	float windowsControl=0;
	float windowsCounter=0;
	uint16_t buforCounter=0;


	int16_t currentInt=0;
	int16_t nextInt=0;
	FsFile wavfile;



	wavfile = SD.open(filename);
	readHeader(&sampleHead,&wavfile);



	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 3  || sampleHead.bitsPerSample != 32  || sampleHead.sampleRate != 44100)
	{
		wavfile.close();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
			//mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		return -1;
	}
	else
	{
		if(hardwareTest)
		{
			Serial.println("load WAV header to SDRAM succesfull");
			//mtPrint("load WAV header to SDRAM succesfull");
		}
	}

	numberOfWindows= (sampleHead.subchunk2Size/4)/SERUM_WAVETABLE_WINDOW_LEN;
	if(numberOfWindows == STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{

		while ( wavfile.available() )
		{
			bufferLength = wavfile.read(bufFloat, 8192);

			for(int i=0; i< 2048; i++)
			{
				if(bufferLength <= i ) *buf=0;
				else *buf=fmap(bufFloat[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,MIN_SIGNED_16BIT,MAX_SIGNED_16BIT);
				buf++;
			}
		}

		wavfile.close();
	}

	else if(numberOfWindows > STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		;
	}
	else if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		missingWindows=STANDARD_WAVETABLE_WINDOWS_NUMBER-numberOfWindows;
		windowsControl=(float)STANDARD_WAVETABLE_WINDOWS_NUMBER/(missingWindows+1);


		while ( wavfile.available() )
		{
			if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
			{
				bufferLength=wavfile.read(bufFloat, 8192);
				for(int i=0; i< 2048; i++)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=fmap(bufFloat[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
					buf++;
					currentWave[i]=bufFloat[i];
				}
				buforCounter++;
			}
			else
			{
				bufferLength=wavfile.read(nextWave, 8192);
				windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);

				for(uint8_t i=0;i<windowDivider;i++)
				{
					for(uint16_t j=0; j< 2048; j++)
					{
						currentInt=fmap(currentWave[j],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
						nextInt=fmap(nextWave[j],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
						*buf=currentInt+(((nextInt-currentInt)/(windowDivider+1))*i);
						buf++;
					}
				}
				for(int i=0; i< 2048; i++)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=fmap(nextWave[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);;
					buf++;

					currentWave[i]=nextWave[i];
				}
				buforCounter++;
				buforCounter+=(windowDivider);
				windowsCounter+=(windowDivider*windowsControl);
			}
		}
		wavfile.close();


	}

	return SERUM_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
}


uint8_t getMiddleWindow(uint16_t cnt, float windowsCnt, float windowsControl)
{
	uint8_t localCounter=0;
	while(cnt >= (uint16_t)windowsCnt)
	{
		localCounter++;
		cnt++;
		windowsCnt+=windowsControl;
	}
	return localCounter;

}

int16_t fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
	  return (int16_t)( (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void readHeader(strWavFileHeader* header, FsFile * wavfile)
{
	uint8_t tab[200];
	uint8_t headerSize=0;
	wavfile->read(tab,200);
	header->chunkId = *( (uint32_t *)(&tab[0]) );
	header->chunkSize = *( (uint32_t *)(&tab[4]) );
	header->format = *( (uint32_t *)(&tab[8]) );
	for(int i=12;i<200;i++)
	{
		if((tab[i] == 'f') && (tab[i+1] == 'm') && (tab[i+2] == 't') && (tab[i+3] == ' '))
		{
			header->subchunk1Id = *((uint32_t *)(&tab[i]));
			header->subchunk1Size = *((uint32_t *)(&tab[i+4]));
			header->AudioFormat = *((uint16_t *)(&tab[i+8]));
			header->numChannels = *((uint16_t *)(&tab[i+10]));
			header->sampleRate = *((uint32_t *)(&tab[i+12]));
			header->byteRate = *((uint32_t *)(&tab[i+16]));
			header->blockAlign = *((uint16_t *)(&tab[i+20]));
			header->bitsPerSample = *((uint16_t *)(&tab[i+22]));
		}

		if((tab[i] == 'd') && (tab[i+1] == 'a') && (tab[i+2] == 't') && (tab[i+3] == 'a'))
		{
			header->subchunk2Id = *((uint32_t *)(&tab[i]));
			header->subchunk2Size = *((uint32_t *)(&tab[i+4]));
			headerSize=i+8;
			wavfile->seek(headerSize);
			break;
		}

	}
}


/*int32_t loadFullWavetableSerum(const char *baseName, int16_t * buf)
{
	strWavFileHeader sampleHead;
	uint8_t tabSize=0;
	uint16_t bufferLength=0;
	float currentWave[2048];
	FsFile wavfile;
	while( *(baseName+tabSize) != 0 )
	{
		tabSize++;
	}
	tabSize+=9;


	char name[tabSize];

	for(uint8_t i=0;i< tabSize; i++)
	{
		if(i< (tabSize-9)) name[i]=baseName[i];
		else if(i== (tabSize -1 ) ) name[i] = 0;
		else if(i== (tabSize -2 ) ) name[i] = 'V';
		else if(i== (tabSize -3 ) ) name[i] = 'A';
		else if(i== (tabSize -4 ) ) name[i] = 'W';
		else if(i== (tabSize -5 ) ) name[i] = '.';
		else if(i== (tabSize -6 ) ) name[i] = '1';
		else if(i== (tabSize -9 ) ) name[i] = '_';
		else name[i] ='0';
	}

	for(uint16_t a=0; a < STANDARD_WAVETABLE_WINDOWS_NUMBER;a++)
	{
		wavfile = SD.open(name);
		wavfile.read(&sampleHead,44);

		bufferLength = wavfile.read(currentWave, 8192);

		for(int j=0; j< SERUM_WAVETABLE_WINDOW_LEN; j++)
		{
			if(bufferLength <= j ) *buf=0;
			else *buf=fmap(currentWave[j],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,MIN_SIGNED_16BIT,MAX_SIGNED_16BIT);
			buf++;
		}
		wavfile.close();

		if(name[tabSize - 6 ] < '9') name[tabSize - 6 ]++;
		else if(name[tabSize - 6 ] == '9')
		{
			name[tabSize - 6 ] = '0';
			if( name[tabSize - 7 ] < '9') name[tabSize - 7 ]++;
			else if(name[tabSize - 7 ] == '9')
			{
				name[tabSize - 7 ]= '0';
				name[tabSize - 8]++;
			}
		}
	}

	return STANDARD_WAVETABLE_WINDOWS_NUMBER* SERUM_WAVETABLE_WINDOW_LEN;

}*/

void loadFullWavetableSerum(const char *baseName)
{
	strWavFileHeader sampleHead;
	uint8_t tabSize=0;
	uint8_t newTabSize=0;
	float currentWave[2048];
	FsFile wavfile;
	FsFile result;
	while( *(baseName+tabSize) != 0 )
	{
		tabSize++;
	}
	newTabSize=tabSize+5;
	tabSize+=9;

	char newName[newTabSize];
	char name[tabSize];

	for(uint8_t i=0;i< (newTabSize - 5); i++)
	{
		newName[i]=baseName[i];
	}
	newName[newTabSize -1] = 0;
	newName[newTabSize -2] = 'v';
	newName[newTabSize -3] = 'a';
	newName[newTabSize -4] = 'w';
	newName[newTabSize -5] = '.';

	if(SD.exists(newName)) return;
	for(uint8_t i=0;i< tabSize; i++)
	{
		if(i< (tabSize-9)) name[i]=baseName[i];
		else if(i== (tabSize -1 ) ) name[i] = 0;
		else if(i== (tabSize -2 ) ) name[i] = 'V';
		else if(i== (tabSize -3 ) ) name[i] = 'A';
		else if(i== (tabSize -4 ) ) name[i] = 'W';
		else if(i== (tabSize -5 ) ) name[i] = '.';
		else if(i== (tabSize -6 ) ) name[i] = '1';
		else if(i== (tabSize -9 ) ) name[i] = '_';
		else name[i] ='0';
	}


	result = SD.open(newName,FILE_WRITE);
	for(uint16_t a=0; a < STANDARD_WAVETABLE_WINDOWS_NUMBER;a++)
	{

		if(!a)
		{
			wavfile = SD.open(name);
			wavfile.read(&sampleHead,44);

			sampleHead.subchunk2Size=STANDARD_WAVETABLE_WINDOWS_NUMBER* SERUM_WAVETABLE_WINDOW_LEN*4;
			result.write(&sampleHead,44);
		}
		else
		{
			wavfile = SD.open(name);
			wavfile.read(&sampleHead,44);
		}


		wavfile.read(currentWave, 8192);
		result.write(currentWave, 8192);


		wavfile.close();

		if(name[tabSize - 6 ] < '9') name[tabSize - 6 ]++;
		else if(name[tabSize - 6 ] == '9')
		{
			name[tabSize - 6 ] = '0';
			if( name[tabSize - 7 ] < '9') name[tabSize - 7 ]++;
			else if(name[tabSize - 7 ] == '9')
			{
				name[tabSize - 7 ]= '0';
				name[tabSize - 8]++;
			}
		}
	}
	result.close();
}


