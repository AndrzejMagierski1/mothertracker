#include "wavHeaderReader.h"

void readHeader(strWavFileHeader* header, FsFile * wavfile)
{
	struct strSubchunkHeader
	{
	  uint8_t id[4];
	  uint32_t size;
	} subchunkHeader;

	uint16_t subchunkCounter = 0;

	wavfile->read(&subchunkHeader,8);

	if((subchunkHeader.id[0] != 'R') || (subchunkHeader.id[1] != 'I') || (subchunkHeader.id[2] != 'F') || (subchunkHeader.id[3] != 'F'))
	{
		memset(header,0,sizeof(*header));
		return;
	}

	memcpy(&header->chunkId,subchunkHeader.id,4);
	header->chunkSize = subchunkHeader.size;
	wavfile->read(&header->format,4);

	while(subchunkCounter < 10)
	{
		wavfile->read(&subchunkHeader,8);
		subchunkCounter++;

		if((subchunkHeader.id[0] == 'f') && (subchunkHeader.id[1] == 'm') && (subchunkHeader.id[2] == 't') && (subchunkHeader.id[3] == ' '))
		{
			memcpy(&header->subchunk1Id,subchunkHeader.id,4);
			header->subchunk1Size = subchunkHeader.size;
			wavfile->read(&header->AudioFormat,16);
			wavfile->seekCur(subchunkHeader.size-16);
			break;
		}
		else
		{
			wavfile->seekCur(subchunkHeader.size);
		}
	}
	if(subchunkCounter == 10)
	{
		memset(header,0,sizeof(*header));
		return;
	}

	subchunkCounter = 0;

	while(subchunkCounter < 10)
	{
		wavfile->read(&subchunkHeader,8);
		subchunkCounter++;

		if((subchunkHeader.id[0] == 'd') && (subchunkHeader.id[1] == 'a') && (subchunkHeader.id[2] == 't') && (subchunkHeader.id[3] == 'a'))
		{
			memcpy(&header->subchunk2Id,subchunkHeader.id,4);
			header->subchunk2Size = subchunkHeader.size;
			break;
		}
		else
		{
			wavfile->seekCur(subchunkHeader.size);
		}
	}

	if(subchunkCounter == 10)
	{
		memset(header,0,sizeof(*header));
		return;
	}
}

void readHeader(strWavFileHeader* header, ExFatFile * wavfile)
{
	struct strSubchunkHeader
	{
	  uint8_t id[4];
	  uint32_t size;
	} subchunkHeader;

	uint16_t subchunkCounter = 0;

	wavfile->read(&subchunkHeader,8);

	if((subchunkHeader.id[0] != 'R') || (subchunkHeader.id[1] != 'I') || (subchunkHeader.id[2] != 'F') || (subchunkHeader.id[3] != 'F'))
	{
		memset(header,0,sizeof(*header));
		return;
	}

	memcpy(&header->chunkId,subchunkHeader.id,4);
	header->chunkSize = subchunkHeader.size;
	wavfile->read(&header->format,4);

	while(subchunkCounter < 10)
	{
		wavfile->read(&subchunkHeader,8);
		subchunkCounter++;

		if((subchunkHeader.id[0] == 'f') && (subchunkHeader.id[1] == 'm') && (subchunkHeader.id[2] == 't') && (subchunkHeader.id[3] == ' '))
		{
			memcpy(&header->subchunk1Id,subchunkHeader.id,4);
			header->subchunk1Size = subchunkHeader.size;
			wavfile->read(&header->AudioFormat,16);
			wavfile->seekCur(subchunkHeader.size-16);
			break;
		}
		else
		{
			wavfile->seekCur(subchunkHeader.size);
		}
	}
	if(subchunkCounter == 10)
	{
		memset(header,0,sizeof(*header));
		return;
	}

	subchunkCounter = 0;

	while(subchunkCounter < 10)
	{
		wavfile->read(&subchunkHeader,8);
		subchunkCounter++;

		if((subchunkHeader.id[0] == 'd') && (subchunkHeader.id[1] == 'a') && (subchunkHeader.id[2] == 't') && (subchunkHeader.id[3] == 'a'))
		{
			memcpy(&header->subchunk2Id,subchunkHeader.id,4);
			header->subchunk2Size = subchunkHeader.size;
			break;
		}
		else
		{
			wavfile->seekCur(subchunkHeader.size);
		}
	}

	if(subchunkCounter == 10)
	{
		memset(header,0,sizeof(*header));
		return;
	}
}

void readHeader(strWavFileHeader* header, FatFile * wavfile)
{
	struct strSubchunkHeader
	{
	  uint8_t id[4];
	  uint32_t size;
	} subchunkHeader;

	uint16_t subchunkCounter = 0;

	wavfile->read(&subchunkHeader,8);

	if((subchunkHeader.id[0] != 'R') || (subchunkHeader.id[1] != 'I') || (subchunkHeader.id[2] != 'F') || (subchunkHeader.id[3] != 'F'))
	{
		memset(header,0,sizeof(*header));
		return;
	}

	memcpy(&header->chunkId,subchunkHeader.id,4);
	header->chunkSize = subchunkHeader.size;
	wavfile->read(&header->format,4);

	while(subchunkCounter < 10)
	{
		wavfile->read(&subchunkHeader,8);
		subchunkCounter++;

		if((subchunkHeader.id[0] == 'f') && (subchunkHeader.id[1] == 'm') && (subchunkHeader.id[2] == 't') && (subchunkHeader.id[3] == ' '))
		{
			memcpy(&header->subchunk1Id,subchunkHeader.id,4);
			header->subchunk1Size = subchunkHeader.size;
			wavfile->read(&header->AudioFormat,16);
			wavfile->seekCur(subchunkHeader.size-16);
			break;
		}
		else
		{
			wavfile->seekCur(subchunkHeader.size);
		}
	}
	if(subchunkCounter == 10)
	{
		memset(header,0,sizeof(*header));
		return;
	}

	subchunkCounter = 0;

	while(subchunkCounter < 10)
	{
		wavfile->read(&subchunkHeader,8);
		subchunkCounter++;

		if((subchunkHeader.id[0] == 'd') && (subchunkHeader.id[1] == 'a') && (subchunkHeader.id[2] == 't') && (subchunkHeader.id[3] == 'a'))
		{
			memcpy(&header->subchunk2Id,subchunkHeader.id,4);
			header->subchunk2Size = subchunkHeader.size;
			break;
		}
		else
		{
			wavfile->seekCur(subchunkHeader.size);
		}
	}

	if(subchunkCounter == 10)
	{
		memset(header,0,sizeof(*header));
		return;
	}
}

