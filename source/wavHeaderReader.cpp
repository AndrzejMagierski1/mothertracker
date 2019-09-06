#include "wavHeaderReader.h"

void readHeader(strWavFileHeader* header, FsFile * wavfile)
{
	uint8_t chunkTab[12];
	uint8_t chunk1Tab[24];
	uint8_t chunk2Tab[8];

	uint16_t headerSize = 0;


	uint8_t i = 0;
	while(i < 12)
	{
		chunkTab[i] = wavfile->read();
		if(i == 0)
		{
			if(chunkTab[i] == 'R') i++;
		}
		else if( i == 1)
		{
			if(chunkTab[i] == 'I') i++;
			else i = 0;
		}
		else if( i == 2)
		{
			if(chunkTab[i] == 'F') i++;
			else i = 0;
		}
		else if( i == 3)
		{
			if(chunkTab[i] == 'F') i++;
			else i = 0;
		}
		else i++;
		headerSize++;
		if(headerSize > 100)
		{
			memset(header,0,sizeof(*header));
			return;
		}
	}

	i = 0;

	while( i < 24)
	{
		chunk1Tab[i] = wavfile->read();
		if(i == 0)
		{
			if(chunk1Tab[i] == 'f') i++;
		}
		else if( i == 1)
		{
			if(chunk1Tab[i] == 'm') i++;
			else i = 0;
		}
		else if( i == 2)
		{
			if(chunk1Tab[i] == 't') i++;
			else i = 0;
		}
		else if( i == 3)
		{
			if(chunk1Tab[i] == ' ') i++;
			else i = 0;
		}
		else i++;
		headerSize++;
		if(headerSize > 300)
		{
			memset(header,0,sizeof(*header));
			return;
		}
	}
	i = 0;

	while( i < 8)
	{
		chunk2Tab[i] = wavfile->read();
		if(i == 0)
		{
			if(chunk2Tab[i] == 'd') i++;
		}
		else if( i == 1)
		{
			if(chunk2Tab[i] == 'a') i++;
			else i = 0;
		}
		else if( i == 2)
		{
			if(chunk2Tab[i] == 't') i++;
			else i = 0;
		}
		else if( i == 3)
		{
			if(chunk2Tab[i] == 'a') i++;
			else i = 0;
		}
		else i++;
		headerSize++;
		if(headerSize > 20000)
		{
			memset(header,0,sizeof(*header));
			return;
		}
	}

	header->chunkId = *((uint32_t *) (&chunkTab[0]));
	header->chunkSize = *((uint32_t *) (&chunkTab[4]));
	header->format = *((uint32_t *) (&chunkTab[8]));

	header->subchunk1Id = *((uint32_t *) (&chunk1Tab[0]));
	header->subchunk1Size = *((uint32_t *) (&chunk1Tab[4]));
	header->AudioFormat = *((uint16_t *) (&chunk1Tab[8]));
	header->numChannels = *((uint16_t *) (&chunk1Tab[10]));
	header->sampleRate = *((uint32_t *) (&chunk1Tab[12]));
	header->byteRate = *((uint32_t *) (&chunk1Tab[16]));
	header->blockAlign = *((uint16_t *) (&chunk1Tab[20]));
	header->bitsPerSample = *((uint16_t *) (&chunk1Tab[22]));

	header->subchunk2Id = *((uint32_t *) (&chunk2Tab[0]));
	header->subchunk2Size = *((uint32_t *) (&chunk2Tab[4]));

	wavfile->seek(headerSize);
//	wavfile->read(tab, 200);
//	header->chunkId = *((uint32_t *) (&tab[0]));
//	header->chunkSize = *((uint32_t *) (&tab[4]));
//	header->format = *((uint32_t *) (&tab[8]));
//	for (int i = 12; i < 197; i++)
//	{
//		if ((tab[i] == 'f') && (tab[i + 1] == 'm') && (tab[i + 2] == 't') && (tab[i + 3] == ' '))
//		{
//			header->subchunk1Id = *((uint32_t *) (&tab[i]));
//			header->subchunk1Size = *((uint32_t *) (&tab[i + 4]));
//			header->AudioFormat = *((uint16_t *) (&tab[i + 8]));
//			header->numChannels = *((uint16_t *) (&tab[i + 10]));
//			header->sampleRate = *((uint32_t *) (&tab[i + 12]));
//			header->byteRate = *((uint32_t *) (&tab[i + 16]));
//			header->blockAlign = *((uint16_t *) (&tab[i + 20]));
//			header->bitsPerSample = *((uint16_t *) (&tab[i + 22]));
//		}
//
//		if ((tab[i] == 'd') && (tab[i + 1] == 'a') && (tab[i + 2] == 't') && (tab[i + 3] == 'a'))
//		{
//			header->subchunk2Id = *((uint32_t *) (&tab[i]));
//			header->subchunk2Size = *((uint32_t *) (&tab[i + 4]));
//			headerSize = i + 8;
//			wavfile->seek(headerSize);
//			break;
//		}
//
//		if(i == 196) memset(header,0,sizeof(*header));
//	}
}

void readHeader(strWavFileHeader* header, ExFatFile * wavfile)
{
	uint8_t chunkTab[12];
	uint8_t chunk1Tab[24];
	uint8_t chunk2Tab[8];

	uint16_t headerSize = 0;


	uint8_t i = 0;
	while(i < 12)
	{
		chunkTab[i] = wavfile->read();
		if(i == 0)
		{
			if(chunkTab[i] == 'R') i++;
		}
		else if( i == 1)
		{
			if(chunkTab[i] == 'I') i++;
			else i = 0;
		}
		else if( i == 2)
		{
			if(chunkTab[i] == 'F') i++;
			else i = 0;
		}
		else if( i == 3)
		{
			if(chunkTab[i] == 'F') i++;
			else i = 0;
		}
		else i++;
		headerSize++;
		if(headerSize > 100)
		{
			memset(header,0,sizeof(*header));
			return;
		}
	}

	i = 0;

	while( i < 24)
	{
		chunk1Tab[i] = wavfile->read();
		if(i == 0)
		{
			if(chunk1Tab[i] == 'f') i++;
		}
		else if( i == 1)
		{
			if(chunk1Tab[i] == 'm') i++;
			else i = 0;
		}
		else if( i == 2)
		{
			if(chunk1Tab[i] == 't') i++;
			else i = 0;
		}
		else if( i == 3)
		{
			if(chunk1Tab[i] == ' ') i++;
			else i = 0;
		}
		else i++;
		headerSize++;
		if(headerSize > 300)
		{
			memset(header,0,sizeof(*header));
			return;
		}
	}
	i = 0;

	while( i < 8)
	{
		chunk2Tab[i] = wavfile->read();
		if(i == 0)
		{
			if(chunk2Tab[i] == 'd') i++;
		}
		else if( i == 1)
		{
			if(chunk2Tab[i] == 'a') i++;
			else i = 0;
		}
		else if( i == 2)
		{
			if(chunk2Tab[i] == 't') i++;
			else i = 0;
		}
		else if( i == 3)
		{
			if(chunk2Tab[i] == 'a') i++;
			else i = 0;
		}
		else i++;
		headerSize++;
		if(headerSize > 20000)
		{
			memset(header,0,sizeof(*header));
			return;
		}
	}

	header->chunkId = *((uint32_t *) (&chunkTab[0]));
	header->chunkSize = *((uint32_t *) (&chunkTab[4]));
	header->format = *((uint32_t *) (&chunkTab[8]));

	header->subchunk1Id = *((uint32_t *) (&chunk1Tab[0]));
	header->subchunk1Size = *((uint32_t *) (&chunk1Tab[4]));
	header->AudioFormat = *((uint16_t *) (&chunk1Tab[8]));
	header->numChannels = *((uint16_t *) (&chunk1Tab[10]));
	header->sampleRate = *((uint32_t *) (&chunk1Tab[12]));
	header->byteRate = *((uint32_t *) (&chunk1Tab[16]));
	header->blockAlign = *((uint16_t *) (&chunk1Tab[20]));
	header->bitsPerSample = *((uint16_t *) (&chunk1Tab[22]));

	header->subchunk2Id = *((uint32_t *) (&chunk2Tab[0]));
	header->subchunk2Size = *((uint32_t *) (&chunk2Tab[4]));


//	uint8_t tab[200];
//	wavfile->read(tab, 200);
//	header->chunkId = *((uint32_t *) (&tab[0]));
//	header->chunkSize = *((uint32_t *) (&tab[4]));
//	header->format = *((uint32_t *) (&tab[8]));
//	for (int i = 12; i < 197; i++)
//	{
//		if ((tab[i] == 'f') && (tab[i + 1] == 'm') && (tab[i + 2] == 't') && (tab[i + 3] == ' '))
//		{
//			header->subchunk1Id = *((uint32_t *) (&tab[i]));
//			header->subchunk1Size = *((uint32_t *) (&tab[i + 4]));
//			header->AudioFormat = *((uint16_t *) (&tab[i + 8]));
//			header->numChannels = *((uint16_t *) (&tab[i + 10]));
//			header->sampleRate = *((uint32_t *) (&tab[i + 12]));
//			header->byteRate = *((uint32_t *) (&tab[i + 16]));
//			header->blockAlign = *((uint16_t *) (&tab[i + 20]));
//			header->bitsPerSample = *((uint16_t *) (&tab[i + 22]));
//		}
//
//		if ((tab[i] == 'd') && (tab[i + 1] == 'a') && (tab[i + 2] == 't') && (tab[i + 3] == 'a'))
//		{
//			header->subchunk2Id = *((uint32_t *) (&tab[i]));
//			header->subchunk2Size = *((uint32_t *) (&tab[i + 4]));
//			break;
//		}
//		if(i == 196) memset(header,0,sizeof(*header));
//	}
}

void readHeader(strWavFileHeader* header, FatFile * wavfile)
{
	uint8_t chunkTab[12];
	uint8_t chunk1Tab[24];
	uint8_t chunk2Tab[8];

	uint16_t headerSize = 0;


	uint8_t i = 0;
	while(i < 12)
	{
		chunkTab[i] = wavfile->read();
		if(i == 0)
		{
			if(chunkTab[i] == 'R') i++;
		}
		else if( i == 1)
		{
			if(chunkTab[i] == 'I') i++;
			else i = 0;
		}
		else if( i == 2)
		{
			if(chunkTab[i] == 'F') i++;
			else i = 0;
		}
		else if( i == 3)
		{
			if(chunkTab[i] == 'F') i++;
			else i = 0;
		}
		else i++;
		headerSize++;
		if(headerSize > 100)
		{
			memset(header,0,sizeof(*header));
			return;
		}
	}

	i = 0;

	while( i < 24)
	{
		chunk1Tab[i] = wavfile->read();
		if(i == 0)
		{
			if(chunk1Tab[i] == 'f') i++;
		}
		else if( i == 1)
		{
			if(chunk1Tab[i] == 'm') i++;
			else i = 0;
		}
		else if( i == 2)
		{
			if(chunk1Tab[i] == 't') i++;
			else i = 0;
		}
		else if( i == 3)
		{
			if(chunk1Tab[i] == ' ') i++;
			else i = 0;
		}
		else i++;
		headerSize++;
		if(headerSize > 300)
		{
			memset(header,0,sizeof(*header));
			return;
		}
	}
	i = 0;

	while( i < 8)
	{
		chunk2Tab[i] = wavfile->read();
		if(i == 0)
		{
			if(chunk2Tab[i] == 'd') i++;
		}
		else if( i == 1)
		{
			if(chunk2Tab[i] == 'a') i++;
			else i = 0;
		}
		else if( i == 2)
		{
			if(chunk2Tab[i] == 't') i++;
			else i = 0;
		}
		else if( i == 3)
		{
			if(chunk2Tab[i] == 'a') i++;
			else i = 0;
		}
		else i++;
		headerSize++;
		if(headerSize > 20000)
		{
			memset(header,0,sizeof(*header));
			return;
		}
	}

	header->chunkId = *((uint32_t *) (&chunkTab[0]));
	header->chunkSize = *((uint32_t *) (&chunkTab[4]));
	header->format = *((uint32_t *) (&chunkTab[8]));

	header->subchunk1Id = *((uint32_t *) (&chunk1Tab[0]));
	header->subchunk1Size = *((uint32_t *) (&chunk1Tab[4]));
	header->AudioFormat = *((uint16_t *) (&chunk1Tab[8]));
	header->numChannels = *((uint16_t *) (&chunk1Tab[10]));
	header->sampleRate = *((uint32_t *) (&chunk1Tab[12]));
	header->byteRate = *((uint32_t *) (&chunk1Tab[16]));
	header->blockAlign = *((uint16_t *) (&chunk1Tab[20]));
	header->bitsPerSample = *((uint16_t *) (&chunk1Tab[22]));

	header->subchunk2Id = *((uint32_t *) (&chunk2Tab[0]));
	header->subchunk2Size = *((uint32_t *) (&chunk2Tab[4]));

//	uint8_t tab[200];
//	wavfile->read(tab, 200);
//	header->chunkId = *((uint32_t *) (&tab[0]));
//	header->chunkSize = *((uint32_t *) (&tab[4]));
//	header->format = *((uint32_t *) (&tab[8]));
//	for (int i = 12; i < 197; i++)
//	{
//		if ((tab[i] == 'f') && (tab[i + 1] == 'm') && (tab[i + 2] == 't') && (tab[i + 3] == ' '))
//		{
//			header->subchunk1Id = *((uint32_t *) (&tab[i]));
//			header->subchunk1Size = *((uint32_t *) (&tab[i + 4]));
//			header->AudioFormat = *((uint16_t *) (&tab[i + 8]));
//			header->numChannels = *((uint16_t *) (&tab[i + 10]));
//			header->sampleRate = *((uint32_t *) (&tab[i + 12]));
//			header->byteRate = *((uint32_t *) (&tab[i + 16]));
//			header->blockAlign = *((uint16_t *) (&tab[i + 20]));
//			header->bitsPerSample = *((uint16_t *) (&tab[i + 22]));
//		}
//
//		if ((tab[i] == 'd') && (tab[i + 1] == 'a') && (tab[i + 2] == 't') && (tab[i + 3] == 'a'))
//		{
//			header->subchunk2Id = *((uint32_t *) (&tab[i]));
//			header->subchunk2Size = *((uint32_t *) (&tab[i + 4]));
//			break;
//		}
//		if(i == 196) memset(header,0,sizeof(*header));
//	}
}

