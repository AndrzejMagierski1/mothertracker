
#include <Arduino.h>
#include "load_sd_wav_to_memory.h"
#include "spi_interrupt.h"
#include "sdram.h"



int32_t loadSdWavToMemory(const char *filename, uint16_t * buf)
{

	strWavFileHeader sampleHead;
	uint32_t sampleLength = 0;

	uint16_t bufferLength=0;
	uint32_t accBufferLength=0;
	uint16_t * bufTemp;
	uint8_t numChannel=0;

	FsFile wavfile;


//	uint8_t buf8[512];
	uint16_t buf16[256];

	__disable_irq();


//	buf+=2;

	wavfile = SD.open(filename);

//	bufferLength = wavfile.read(buf8, 512);

	wavfile.read(&sampleHead, 44);

	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 1  || sampleHead.bitsPerSample != 16  || sampleHead.sampleRate != 44100 )
	{
		wavfile.close();
		__enable_irq();
		return -1;
	}


//	if( (buf8[8] != 'W') || (buf8[9] != 'A') ||  (buf8[10] != 'V') || (buf8[11] != 'E') ||
//	(buf8[20] != 1) || (buf8[34] != 16) || (buf8[24] != 68) || (buf8[25] != 172) ) return 0; // błędny format
//	numChannel=buf8[22];

	if(sampleHead.numChannels == 1) sampleLength = sampleHead.subchunk2Size;
	else if(sampleHead.numChannels == 2) sampleLength = sampleHead.subchunk2Size/2;
	else
	{
		wavfile.close();
		__enable_irq();
		return -2;
	}

	buf = new __NOINIT(EXTERNAL_RAM) uint16_t[sampleLength/2];

	bufTemp = buf;

//	for(int i=44;i<512;i++)
//	{
//		*buf++ = buf8[i];
//	}
//	accBufferLength += (bufferLength - 44);



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


	*bufTemp = (0x8100 | ((accBufferLength/4) >> 24));
	bufTemp++;
	*bufTemp |= ((uint16_t) (accBufferLength/4) & 0xFFFF);


	__enable_irq();
	return accBufferLength/4;
}



