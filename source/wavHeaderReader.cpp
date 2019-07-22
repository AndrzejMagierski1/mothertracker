#include "wavHeaderReader.h"

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

void readHeader(strWavFileHeader* header, ExFatFile * wavfile)
{
	uint8_t tab[200];
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
			break;
		}

	}
}

void readHeader(strWavFileHeader* header, FatFile * wavfile)
{
	uint8_t tab[200];
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

			break;
		}

	}
}

