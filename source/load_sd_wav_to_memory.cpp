
#include <Arduino.h>
#include "load_sd_wav_to_memory.h"
#include "spi_interrupt.h"

uint32_t AudioLoadSdWav::loadSdWavToMemory(const char *filename, int16_t * buf)
{
	uint16_t bufferLength=0;
	uint32_t accBufferLength=0;
	int16_t * bufTemp;
	uint8_t numChannel=0;

	File wavfile;

	union buffors
	{
		uint8_t buf8[512];
		uint16_t buf16[256];
	} buffor;

	__disable_irq();

	bufTemp=buf;
	buf+=2;

	wavfile = SD.open(filename);

	bufferLength = wavfile.read(buffor.buf8, 512);
	if( (buffor.buf8[8] != 'W') || (buffor.buf8[9] != 'A') ||  (buffor.buf8[10] != 'V') || (buffor.buf8[11] != 'E') ||
	(buffor.buf8[20] != 1) || (buffor.buf8[34] != 16) || (buffor.buf8[24] != 68) || (buffor.buf8[25] != 172) ) return 0; // błędny format

	numChannel=buffor.buf8[22];

	for(int i=44;i<512;i++)
	{
		*buf++ = buffor.buf8[i];
	}
	accBufferLength += (bufferLength - 44);

	if(numChannel == 1)
	{
		while ( wavfile.available() )
		{
			bufferLength = wavfile.read(buffor.buf8, 512);
			accBufferLength += bufferLength;
			for(int i=0; i< 256; i++)
			{
				if(bufferLength <= i ) *buf=0;
				else *buf=buffor.buf16[i];
				buf++;
			}
		}
	}
	else if ( numChannel == 2)
	{
		while ( wavfile.available() )
		{

			bufferLength = wavfile.read(buffor.buf8, 512);
			accBufferLength += bufferLength;
			for(int i=0; i< 256; i+=2)
			{
				if(bufferLength <= i ) *buf=0;
				else *buf=buffor.buf16[i];
				buf++;
			}

		}
	}


	wavfile.close();


	*bufTemp = (0x8100 | ((accBufferLength/4) >> 24));
	bufTemp++;
	*bufTemp |= ((int16_t) (accBufferLength/4) & 0xFFFF);

	__enable_irq();

	return accBufferLength/4;
}



