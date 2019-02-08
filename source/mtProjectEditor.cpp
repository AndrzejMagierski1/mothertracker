


//#include "mtDisplay.h"
#include "mtProjectEditor.h"
#include "mtHardware.h"
#include "SD.h"
#include "sdram.h"
//#include "Arduino.h"
#include "spi_interrupt.h"
#include "sdram.h"
//#include "AudioStream.h"


//cr_section_macros.h

cMtProjectEditor mtProjectEditor;


strMtProject mtProject;



__NOINIT(EXTERNAL_RAM) int16_t sdram_sampleBank[4*1024*1024];


void cMtProjectEditor::update()
{

	if(commandsToDo[ProjEditCommandOpenLastProject])
	{
		commandsToDo[ProjEditCommandOpenLastProject] = 0;

		uint8_t result = loadLastProject();
		if(result)
		{
			// jesli nie mozna zaladowac ostatneigo projektu
			// to poinformuj o tym interfejs
			//projectEditorEvent(ProjEditEventLoadLastProjFailed, &result,0,0);
		}
	}




}


void cMtProjectEditor::startProject()
{
	commandsToDo[ProjEditCommandOpenLastProject] = 1;




}

//------------------------------------------------------------------------------
uint8_t cMtProjectEditor::readProjectConfig()
{
	// parametry sampli ==============================================

	// trzeba czytac z pliku projektu zawierajacego opis banku sampli
	// pod jaki index tablicy sampli 0-32 zapisywac dany sampel
	// teraz domyslnie zajmowane 0-7

	for(uint8_t i = 0; i < 8; i++) // max do 9
	{
											// mtSampleTypeWaveFile
		mtProject.sampleBank.sample[i].type = mtSampleTypeWavetable;
		mtProject.sampleBank.sample[i].file_name[0] = i+49;
		mtProject.sampleBank.sample[i].file_name[1] = '.';
		mtProject.sampleBank.sample[i].file_name[2] = 'w';
		mtProject.sampleBank.sample[i].file_name[3] = 'a';
		mtProject.sampleBank.sample[i].file_name[4] = 'v';
		mtProject.sampleBank.sample[i].file_name[5] = 0;
		mtProject.sampleBank.sample[i].wavetable_window_size = 1024;
	}

	//mtProject.sampleBank.sample[1].wavetable_window_size = 1024;
	//mtProject.sampleBank.sample[1].type = mtSampleTypeWaveFile;

	// parametry instrumentow ========================================
	mtProject.instruments_count = 8;

	for(uint8_t i = 0; i < mtProject.instruments_count; i++)
	{
		mtProject.instrument[i].sampleIndex = i;

		if(mtProject.sampleBank.sample[i].type == mtSampleTypeWavetable)
		{
			mtProject.instrument[i].playMode = wavetable;
			mtProject.instrument[i].wavetableWindowSize = mtProject.sampleBank.sample[mtProject.instrument[i].sampleIndex].wavetable_window_size;
		}
		else
		{
			mtProject.instrument[i].playMode = 1;
		}

		mtProject.instrument[i].startPoint = 0;
		mtProject.instrument[i].loopPoint1 = 10000;
		mtProject.instrument[i].loopPoint2 = 20000;
		mtProject.instrument[i].endPoint = SAMPLE_POINT_POS_MAX;

		mtProject.instrument[i].envelope[envAmp].delay = 0;

		mtProject.instrument[i].envelope[envAmp].attack = 0;
		mtProject.instrument[i].envelope[envAmp].hold = 0;
		mtProject.instrument[i].envelope[envAmp].decay = 0;
		mtProject.instrument[i].envelope[envAmp].sustain = 1.0;
		mtProject.instrument[i].envelope[envAmp].release = 1000;
		mtProject.instrument[i].envelope[envAmp].amount = 1.0;
		mtProject.instrument[i].envelope[envAmp].enable = envelopeOn;

		mtProject.instrument[i].envelope[envFilter].delay = 0;
		mtProject.instrument[i].envelope[envFilter].attack = 3000;
		mtProject.instrument[i].envelope[envFilter].hold = 0;
		mtProject.instrument[i].envelope[envFilter].decay = 0;
		mtProject.instrument[i].envelope[envFilter].sustain = 1.0;
		mtProject.instrument[i].envelope[envFilter].release = 1000;
		mtProject.instrument[i].envelope[envFilter].amount = 1.0;
		mtProject.instrument[i].envelope[envFilter].enable = envelopeOff;

		mtProject.instrument[i].cutOff = 1.0;
		mtProject.instrument[i].filterEnable = filterOff;
		mtProject.instrument[i].filterType = lowPass;
		mtProject.instrument[i].resonance = 0;
		mtProject.instrument[i].panning = 50;
		mtProject.instrument[i].glide = 10000;
		mtProject.instrument[i].volume = 100;

		if(i >= 10)
		{
			mtProject.instrument[i].name[0] = (i+1)/10 + 48;
			mtProject.instrument[i].name[1] = (i+1)%10 + 48;
			mtProject.instrument[i].name[2] = 0;
		}
		else
		{
			mtProject.instrument[i].name[0] = (i+1)%10 + 48;
			mtProject.instrument[i].name[1] = 0;
		}

	}

	mtProject.instrument[0].startPoint = 19000;
	mtProject.instrument[0].loopPoint1 = 19000; //PRO
	mtProject.instrument[0].loopPoint2 = 29000;

	mtProject.instrument[0].lfo[lfoA].rate=600;
	mtProject.instrument[0].lfo[lfoA].wave=0;
	mtProject.instrument[0].lfo[lfoA].amount=4095;
	mtProject.instrument[0].lfo[lfoA].sync=0;
	mtProject.instrument[0].lfo[lfoA].enable=lfoOff;



	// parametry paternu ========================================



	return 0;
}

//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::loadSamplesBank()
{
	//zaladowanie banku sampli
	int32_t size;
	mtProject.sampleBank.sample[0].address = sdram_sampleBank;
	mtProject.sampleBank.samples_count = 0;

	for(uint8_t i = 0; i < SAMPLES_MAX; i++)
	{
		if(mtProject.sampleBank.sample[i].type == mtSampleTypeWavetable)
		{
			size = loadWavetableStandard(mtProject.sampleBank.sample[i].file_name, mtProject.sampleBank.sample[i].address);
		}
		else
		{
			size = loadSample(mtProject.sampleBank.sample[i].file_name, mtProject.sampleBank.sample[i].address);
		}

		if(size > 0)
		{
			mtProject.sampleBank.used_memory += size*2;
			mtProject.sampleBank.sample[i].loaded = 1;
			mtProject.sampleBank.sample[i].length = size;

			mtProject.sampleBank.samples_count++;
		}
		else
		{
			mtProject.sampleBank.sample[i].loaded = 0;
			mtProject.sampleBank.sample[i].length = 0;
			mtProject.sampleBank.sample[i].file_name[0] = '-';
			mtProject.sampleBank.sample[i].file_name[1] = 'e';
			mtProject.sampleBank.sample[i].file_name[2] = 'm';
			mtProject.sampleBank.sample[i].file_name[3] = 'p';
			mtProject.sampleBank.sample[i].file_name[4] = 't';
			mtProject.sampleBank.sample[i].file_name[5] = 'y';
			mtProject.sampleBank.sample[i].file_name[6] = '-';
			mtProject.sampleBank.sample[i].file_name[7] = 0;
			size = 0;
			//return 2; // blad ladowania wave
		}

		if(i+1 < SAMPLES_MAX)
		{
			mtProject.sampleBank.sample[i+1].address = mtProject.sampleBank.sample[i].address+size;
		}
		if(mtProject.sampleBank.used_memory > mtProject.sampleBank.max_memory) return 1; // out of memory
	}

	return 0;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::loadLastProject()
{
	if(readProjectConfig())
	{
		mtPrintln("loading config file failed!");
		return 1;
	}

	if(loadSamplesBank())
	{
		mtPrintln("loading samples failed!");
		return 2;
	}
	else
	{
//		mtPrint(mtProject.sampleBank.samples_count);
//		mtPrintln(" samples loaded successfully");
//		mtPrint("sample memory used: ");
//		mtPrint( int((mtProject.sampleBank.used_memory * 100 ) / mtProject.sampleBank.max_memory));
//		mtPrintln(" %");
	}





	return 0;
}


//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::isProjectLoaded()
{


	return 0;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::padsChange(uint8_t type, uint8_t n, uint8_t velo)
{
	eventFunct(mtPriojectEditorEventPadPress, &n, 0, 0);

	return 0;
}

void cMtProjectEditor::buttonChange(uint8_t button, uint8_t value)
{


}

void cMtProjectEditor::potChange(uint8_t pot, int16_t value)
{


}

void cMtProjectEditor::seqButtonChange(uint8_t type, uint8_t x, uint8_t y)
{


}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------



int32_t loadSample(const char *filename, int16_t * buf)
{
	strWavFileHeader sampleHead;
	uint16_t bufferLength=0;
	uint32_t accBufferLength=0;
//	uint32_t * bufStart;
	int16_t buf16[256];
	FsFile wavfile;

	//__disable_irq();

	//bufStart = (uint32_t*)buf;
	//buf+=2;

	wavfile = SD.open(filename);
	wavfile.read(&sampleHead, 44);

	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 1  || sampleHead.bitsPerSample != 16  || sampleHead.sampleRate != 44100 )
	{
		wavfile.close();
//		__enable_irq();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
			mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		return -1;
	}
	else
	{
		if(hardwareTest)
		{
			Serial.println("load WAV header to SDRAM succesfull");
			mtPrint("load WAV header to SDRAM succesfull");
		}
	}
/*
	if(sampleHead.numChannels == 1) sampleLength = sampleHead.subchunk2Size;
	else if(sampleHead.numChannels == 2) sampleLength = sampleHead.subchunk2Size/2;
	else
	{
		wavfile.close();
//		__enable_irq();
		return -2;
	}
*/
	if(sampleHead.numChannels == 1)
	{
		while ( wavfile.available() )
		{
			bufferLength = wavfile.read(buf16, 512);

			accBufferLength += bufferLength;

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
			//Serial.println(bufferLength);
			accBufferLength += bufferLength;
			for(int i=0; i< 256; i+=2)
			{
				if(bufferLength <= i ) *buf=0;
				else *buf=buf16[i];
				buf++;
			}

		}
	}

	wavfile.close();

//	*bufStart = (accBufferLength/4);
	if(sampleHead.numChannels == 1)
	{
		accBufferLength = accBufferLength/2;
	}
	else if(sampleHead.numChannels == 2)
	{
		accBufferLength = accBufferLength/4;
	}


/*
 	if(sampleLength != accBufferLength)
	{
		return -3; // rozne wielkosc danych
	}
*/
//	*bufStart = (0x8100 | ((accBufferLength/4) >> 24));
//	bufStart++;
//	*bufStart |= ((uint16_t) (accBufferLength/4) & 0xFFFF);


//	__enable_irq();
	return accBufferLength;
}

int32_t loadWavetableStandard(const char *filename, int16_t * buf)
{
	strWavFileHeader sampleHead;
	uint16_t bufferLength=0;
	uint32_t accBufferLength=0;
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
//		__enable_irq();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
			mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		return -1;
	}
	else
	{
		if(hardwareTest)
		{
			Serial.println("load WAV header to SDRAM succesfull");
			mtPrint("load WAV header to SDRAM succesfull");
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

				accBufferLength += bufferLength;

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
				accBufferLength += bufferLength;
				for(int i=0; i< 256; i+=2)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=buf16[i];
					buf++;
				}
			}
		}

		wavfile.close();


		if(sampleHead.numChannels == 1)
		{
			accBufferLength = accBufferLength/2;
		}
		else if(sampleHead.numChannels == 2)
		{
			accBufferLength = accBufferLength/4;
		}
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
	strSerumWavetableFileHeader sampleHead;
	uint16_t bufferLength=0;
	uint32_t accBufferLength=0;
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
	wavfile.read(&sampleHead, 136);



	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 3  || sampleHead.bitsPerSample != 16  || sampleHead.sampleRate != 44100 )
	{
		wavfile.close();
//		__enable_irq();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
			mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		return -1;
	}
	else
	{
		if(hardwareTest)
		{
			Serial.println("load WAV header to SDRAM succesfull");
			mtPrint("load WAV header to SDRAM succesfull");
		}
	}

	numberOfWindows= (sampleHead.subchunk2Size/4)/STANDARD_WAVETABLE_WINDOW_LEN;
	if(numberOfWindows == STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		if(sampleHead.numChannels == 1)
		{
			while ( wavfile.available() )
			{
				bufferLength = wavfile.read(buf16, 512);

				accBufferLength += bufferLength;

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
				accBufferLength += bufferLength;
				for(int i=0; i< 256; i+=2)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=buf16[i];
					buf++;
				}
			}
		}

		wavfile.close();


		if(sampleHead.numChannels == 1)
		{
			accBufferLength = accBufferLength/2;
		}
		else if(sampleHead.numChannels == 2)
		{
			accBufferLength = accBufferLength/4;
		}
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

