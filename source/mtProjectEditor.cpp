


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
	mtProject.sampleBank.samples_count = 8;

	// trzeba czytac z pliku projektu zawierajacego opis banku sampli
	// pod jaki index tablicy sampli 0-32 zapisywac dany sampel
	// teraz domyslnie zajmowane 0-7

	mtProject.sampleBank.sample[0].file_name = (char*)"3.wav";
	mtProject.sampleBank.sample[1].file_name = (char*)"3.wav";
	mtProject.sampleBank.sample[2].file_name = (char*)"3.wav";
	mtProject.sampleBank.sample[3].file_name = (char*)"3.wav";
	mtProject.sampleBank.sample[4].file_name = (char*)"1.wav";
	mtProject.sampleBank.sample[5].file_name = (char*)"1.wav";
	mtProject.sampleBank.sample[6].file_name = (char*)"2.wav";
	mtProject.sampleBank.sample[7].file_name = (char*)"2.wav";

	// parametry instrumentow ========================================
	mtProject.instruments_count = 8;

	for(uint8_t i = 0; i < mtProject.instruments_count; i++)
	{
		mtProject.instrument[i].sampleIndex = i;

		mtProject.instrument[i].playMode = 0;
		mtProject.instrument[i].startPoint = 0;
		mtProject.instrument[i].loopPoint1 = 0;
		mtProject.instrument[i].loopPoint2 = 0;
		mtProject.instrument[i].endPoint = 1000;

		mtProject.instrument[i].envelopeAmp.delay = 0;
		mtProject.instrument[i].envelopeAmp.attack = 0;
		mtProject.instrument[i].envelopeAmp.hold = 0;
		mtProject.instrument[i].envelopeAmp.decay = 0;
		mtProject.instrument[i].envelopeAmp.sustain = 100;
		mtProject.instrument[i].envelopeAmp.release = 1000;

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

	for(uint8_t i = 0; i < mtProject.sampleBank.samples_count; i++)
	{
		size = loadSdWavToMemory(mtProject.sampleBank.sample[i].file_name, mtProject.sampleBank.sample[i].address);

		if(size > 0)
		{
			mtProject.sampleBank.used_memory += size;
			mtProject.sampleBank.sample[i].loaded = 1;
			mtProject.sampleBank.sample[i].length = size;
		}
		else return 2; // blad ladowania wave

		if(i+1 < mtProject.sampleBank.samples_count)
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

	if(loadSamplesBank()) return 1;


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
	uint32_t * bufStart;
	int16_t buf16[256];
	FsFile wavfile;

	//__disable_irq();

	bufStart = (uint32_t*)buf;
	buf+=2;

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

	*bufStart = (accBufferLength/4);

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



