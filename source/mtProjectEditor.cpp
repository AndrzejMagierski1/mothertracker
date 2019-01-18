


//#include "mtDisplay.h"
#include "mtProjectEditor.h"

#include "SD.h"
#include "sdram.h"
//#include "Arduino.h"
#include "spi_interrupt.h"
#include "sdram.h"
//#include "AudioStream.h"


//cr_section_macros.h

cMtProjectEditor mtProjectEditor;


strMtProject mtProject;
strPatern mtPatern;



__NOINIT(EXTERNAL_RAM) int16_t sdram_sampleBank[4*1024*1024];


//------------------------------------------------------------------------------
uint8_t cMtProjectEditor::readProjectConfig()
{
	// parametry sampli ==============================================

	// trzeba czytac z pliku projektu zawierajacego opis banku sampli
	// pod jaki index tablicy sampli 0-32 zapisywac dany sampel
	// teraz domyslnie zajmowane 0-7


	mtProject.sampleBank.sample[0].file_name[0] = '1';
	mtProject.sampleBank.sample[0].file_name[1] = '.';
	mtProject.sampleBank.sample[0].file_name[2] = 'w';
	mtProject.sampleBank.sample[0].file_name[3] = 'a';
	mtProject.sampleBank.sample[0].file_name[4] = 'v';
	mtProject.sampleBank.sample[0].file_name[5] = 0;

	mtProject.sampleBank.sample[1].file_name[0] = '2';
	mtProject.sampleBank.sample[1].file_name[1] = '.';
	mtProject.sampleBank.sample[1].file_name[2] = 'w';
	mtProject.sampleBank.sample[1].file_name[3] = 'a';
	mtProject.sampleBank.sample[1].file_name[4] = 'v';
	mtProject.sampleBank.sample[1].file_name[5] = 0;

	mtProject.sampleBank.sample[2].file_name[0] = '3';
	mtProject.sampleBank.sample[2].file_name[1] = '.';
	mtProject.sampleBank.sample[2].file_name[2] = 'w';
	mtProject.sampleBank.sample[2].file_name[3] = 'a';
	mtProject.sampleBank.sample[2].file_name[4] = 'v';
	mtProject.sampleBank.sample[2].file_name[5] = 0;

	mtProject.sampleBank.sample[3].file_name[0] = '4';
	mtProject.sampleBank.sample[3].file_name[1] = '.';
	mtProject.sampleBank.sample[3].file_name[2] = 'w';
	mtProject.sampleBank.sample[3].file_name[3] = 'a';
	mtProject.sampleBank.sample[3].file_name[4] = 'v';
	mtProject.sampleBank.sample[3].file_name[5] = 0;

	mtProject.sampleBank.sample[4].file_name[0] = '5';
	mtProject.sampleBank.sample[4].file_name[1] = '.';
	mtProject.sampleBank.sample[4].file_name[2] = 'w';
	mtProject.sampleBank.sample[4].file_name[3] = 'a';
	mtProject.sampleBank.sample[4].file_name[4] = 'v';
	mtProject.sampleBank.sample[4].file_name[5] = 0;

	mtProject.sampleBank.sample[5].file_name[0] = '6';
	mtProject.sampleBank.sample[5].file_name[1] = '.';
	mtProject.sampleBank.sample[5].file_name[2] = 'w';
	mtProject.sampleBank.sample[5].file_name[3] = 'a';
	mtProject.sampleBank.sample[5].file_name[4] = 'v';
	mtProject.sampleBank.sample[5].file_name[5] = 0;

	mtProject.sampleBank.sample[6].file_name[0] = '7';
	mtProject.sampleBank.sample[6].file_name[1] = '.';
	mtProject.sampleBank.sample[6].file_name[2] = 'w';
	mtProject.sampleBank.sample[6].file_name[3] = 'a';
	mtProject.sampleBank.sample[6].file_name[4] = 'v';
	mtProject.sampleBank.sample[6].file_name[5] = 0;

	mtProject.sampleBank.sample[7].file_name[0] = '8';
	mtProject.sampleBank.sample[7].file_name[1] = '.';
	mtProject.sampleBank.sample[7].file_name[2] = 'w';
	mtProject.sampleBank.sample[7].file_name[3] = 'a';
	mtProject.sampleBank.sample[7].file_name[4] = 'v';
	mtProject.sampleBank.sample[7].file_name[5] = 0;


	// parametry instrumentow ========================================
	mtProject.instruments_count = 8;

	for(uint8_t i = 0; i < mtProject.instruments_count; i++)
	{
		mtProject.instrument[i].sampleIndex = i;

		mtProject.instrument[i].playMode = 1;

		mtProject.instrument[i].startPoint = 0;
		mtProject.instrument[i].loopPoint1 = 10000;
		mtProject.instrument[i].loopPoint2 = 20000;
		mtProject.instrument[i].endPoint = SAMPLE_POINT_POS_MAX;

		mtProject.instrument[i].ampDelay = 0;
		mtProject.instrument[i].ampAttack = 0;
		mtProject.instrument[i].ampHold = 0;
		mtProject.instrument[i].ampDecay = 0;
		mtProject.instrument[i].ampSustain = 100;
		mtProject.instrument[i].ampRelease = 1000;

		mtProject.instrument[i].panning = 0;
	}


	// parametry paternu ========================================

	for(uint8_t i = 0; i < 8; i++)
	{
		for(uint8_t j=0; j<32; j++)
		{
			mtPatern.track[i].step[j].instrumentIndex = i;
			mtPatern.track[i].step[j].volume = 100;
			mtPatern.track[i].step[j].note = j;

		}


		mtPatern.track[i].volume = 100;
		mtPatern.track[i].enabled = 1;


	}

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
		size = loadSdWavToMemory(mtProject.sampleBank.sample[i].file_name, mtProject.sampleBank.sample[i].address);

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
uint8_t cMtProjectEditor::loadProject()
{
	readProjectConfig();

	if(loadSamplesBank())
	{
		mtPrintln("loading samples failed!");
	}
	else
	{
		mtPrint(mtProject.sampleBank.samples_count);
		mtPrintln(" samples loaded successfully");
		mtPrint("sample memory used: ");
		mtPrint( int((mtProject.sampleBank.used_memory * 100 ) / mtProject.sampleBank.max_memory));
		mtPrintln(" %");
	}





	return 0;
}


//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::isProjectLoaded()
{


	return 0;
}





int32_t loadSdWavToMemory(const char *filename, int16_t * buf)
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
		return -1;
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

	accBufferLength = accBufferLength/4;

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



