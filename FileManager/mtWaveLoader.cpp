#include "mtWaveLoader.h"
#include "mtHardware.h"
WaveLoader externalWaveLoader;

void WaveLoader::update()
{
	if(state == loaderStateTypeInProgress)
	{
		int32_t bufferLength;
		int32_t forConstrain;

		if(sampleHead.AudioFormat == 1)
		{

			if(sampleHead.bitsPerSample == 24)
			{
				uint8_t buf8[1536];
				if(sampleHead.numChannels == 1)
				{
					for(uint16_t i = 0 ; i< BUFFER_COUNT; i++)
					{
						if( wavfile.available() )
						{
							bufferLength = wavfile.read(buf8, 1536);

							accBufferLength += bufferLength;

							forConstrain =  bufferLength/3;

							uint8_t * wsk = &buf8[1];
							for(int i=0; i< forConstrain; i++)
							{
								*(currentAddress++)=*((int16_t *) wsk);
								if(i != (forConstrain-1) ) wsk+=3;
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
					for(uint16_t i = 0 ; i< BUFFER_COUNT; i++)
					{
						if( wavfile.available() )
						{
							bufferLength = wavfile.read(buf8, 1536);

							accBufferLength += bufferLength;

							forConstrain =  bufferLength/6;

							uint8_t * wsk = &buf8[1];
							for(int i=0; i< forConstrain; i++)
							{
								*(currentAddress++)=*((int16_t *) wsk);
								if(i != (forConstrain-1) ) wsk+=6;
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
			else
			{
				int16_t buf16[256];
				if(sampleHead.numChannels == 1)
				{
					for(uint16_t i = 0 ; i< BUFFER_COUNT; i++)
					{
						if( wavfile.available() )
						{
							bufferLength = wavfile.read(buf16, 512);

							accBufferLength += bufferLength;

							forConstrain =  bufferLength/2;
							for(int i=0; i< forConstrain; i++)
							{
								*(currentAddress++)=buf16[i];
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
					for(uint16_t i = 0 ; i< BUFFER_COUNT; i++)
					{
						if (wavfile.available() )
						{
							bufferLength = wavfile.read(buf16, 512);

							accBufferLength += bufferLength;
							forConstrain =  bufferLength/2;
							for(int i=0; i< forConstrain; i+=2)
							{
								*(currentAddress++)=buf16[i];
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
		else if(sampleHead.AudioFormat == 3)
		{
			float bufFloat[256];
			if(sampleHead.numChannels == 1)
			{
				for(uint16_t i = 0 ; i< BUFFER_COUNT; i++)
				{
					if( wavfile.available() )
					{
						bufferLength = wavfile.read(bufFloat, 1024);

						accBufferLength += bufferLength;
						forConstrain =  bufferLength/4;
						for(int i=0; i< forConstrain; i++)
						{
							*(currentAddress++) = ( ( (bufFloat[i] + 1.0) * 65535.0 ) / 2.0)  - 32768.0 ;
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
				for(uint16_t i = 0 ; i< BUFFER_COUNT; i++)
				{
					if (wavfile.available() )
					{
						bufferLength = wavfile.read(bufFloat, 1024);

						accBufferLength += bufferLength;
						forConstrain =  bufferLength/4;
						for(int i=0; i< forConstrain; i+=2)
						{
							*(currentAddress++) = ( ((bufFloat[i] + 1.0) * 65535.0 ) / 2.0)  - 32768.0 ;

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
		state = loaderStateTypeEnded;
		return 0;
	}
	accBufferLength = 0;
	wavfile = SD.open(filename);
	if(!wavfile)
	{
		wavfile.close();

		state = loaderStateTypeEnded;
		stopFlag = 0;
		return 0;
	}
	readHeader(&sampleHead,&wavfile);
	currentAddress = buf;
//	if ( (sampleHead.numChannels == 1 && (sampleHead.subchunk2Size > 8388608 )) &&  (sampleHead.numChannels == 2 && (sampleHead.subchunk2Size > 16777216)))
//	{
//		wavfile.close();
//		if(hardwareTest)
//		{
//			Serial.println("too long file");
//		}
//		state = loaderStateTypeEnded;
//		stopFlag = 0;
//		return 0;
//	}
	if( (sampleHead.format != 1163280727) || ( (sampleHead.AudioFormat != 1) && (sampleHead.AudioFormat != 3) ) || ( (sampleHead.bitsPerSample != 16) && (sampleHead.bitsPerSample != 24) && (sampleHead.bitsPerSample != 32)) || sampleHead.sampleRate != 44100 )
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
		else
		{
			if(sampleHead.bitsPerSample == 24) return sampleHead.subchunk2Size/3;
			else return sampleHead.subchunk2Size/2;
		}
	}
	else if(sampleHead.numChannels == 2)
	{
		if(sampleHead.AudioFormat == 3) return sampleHead.subchunk2Size/8;
		else
		{
			if(sampleHead.bitsPerSample == 24) return sampleHead.subchunk2Size/6;
			else return sampleHead.subchunk2Size/4;
		}
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
	if(!wavfile)
	{
//		SD.begin(SdioConfig(DMA_SDIO));
		wavfile.close();
		return 0;

	}
	readHeader(&localSampleHead,&wavfile);
	wavfile.close();

//	if ( (localSampleHead.numChannels == 1 && (localSampleHead.subchunk2Size > 8388608 )) &&  (localSampleHead.numChannels == 2 && (localSampleHead.subchunk2Size > 16777216)))
//	{
//		return 0;
//	}
	if((localSampleHead.format != 1163280727) ||( (localSampleHead.AudioFormat != 1) && (localSampleHead.AudioFormat != 3) )  || ((localSampleHead.bitsPerSample != 16) && (localSampleHead.bitsPerSample != 24) && (localSampleHead.bitsPerSample != 32) )|| localSampleHead.sampleRate != 44100 )
	{
		return 0;
	}

	if(localSampleHead.numChannels == 1)
	{
		if(localSampleHead.AudioFormat == 3) return localSampleHead.subchunk2Size/4;
		else
		{
			if(localSampleHead.bitsPerSample == 24) return localSampleHead.subchunk2Size/3;
			else return localSampleHead.subchunk2Size/2;
		}
	}
	else if(localSampleHead.numChannels == 2)
	{
		if(localSampleHead.AudioFormat == 3) return localSampleHead.subchunk2Size/8;
		else
		{
			if(localSampleHead.bitsPerSample == 24) return localSampleHead.subchunk2Size/6;
			else return localSampleHead.subchunk2Size/4;
		}
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
		if(sampleHead.bitsPerSample == 24)
		{
			if(sampleHead.numChannels == 1)
			{
				return accBufferLength/3;
			}
			else if(sampleHead.numChannels == 2)
			{
				return accBufferLength/6;
			}
		}
		else
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
	return 0;
}