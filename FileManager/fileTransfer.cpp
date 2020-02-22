

#include "mtStructs.h"
#include "fileManagerDefs.h"

#include "SD.h"
#include "debugLog.h"



#include "mtSequencer.h"

#include "fileTransfer.h"


extern uint8_t sdram_writeLoadBuffer[32768];

cFileTransfer fileTransfer;

SdFile transferFile;

strWavFileHeader sampleHead;





uint8_t cFileTransfer::loadFileToMemory(const char* file, uint8_t* memory, uint32_t memSize, uint8_t mode)
{
	if(transferStep == 0)
	{
		if(!SD.exists(file)) return fileTransferFileNoExist;

		if(transferFile.open(file))
		{
			memTotal = memSize;
			memStep = (mode == fileWholeOnce) ? memTotal : memoryReadPerPart;
			memComplited = 0;
			transferStep = 1;

			memset(memory,0,memSize);
		}
	}

	if(transferStep == 1)
	{
		int32_t result = transferFile.read(memory+memComplited, memStep);

		if(result >= 0)
		{
			memComplited += result;
			if(memComplited >= memSize)
			{
				transferStep = 2;
			}
			else
			{
				return fileTransferInProgress;
			}
		}
		//else wysypuje error na koncu funkcji \/
	}

	if(transferStep == 2)
	{
		transferStep = 0;
		transferFile.close();
		return fileTransferEnd;
	}

	transferStep = 0;
	transferFile.close();
	return fileTransferError;
}


uint8_t cFileTransfer::loadSampleToMemory(const char* file, int16_t* memory, uint32_t* outSampleCount)
{
	if(transferStep == 0)
	{
		if(!SD.exists(file)) return fileTransferFileNoExist;

		if(transferFile.open(file))
		{
			memStep = 32640; // wielokrotnosc 4*6*8 (rozne probkowania stereo w bajtach)
			memComplited = 0;
			transferStep = 1;
			convertedDataSize = 0;

			readHeader(&sampleHead, &transferFile);

			if(!checkSampleValid())
			{
				transferFile.close();
				return fileTransferFileNoValid;
			}

			memTotal = transferFile.available(); // pobiera wielkosc dopiero po przesciu przez header
		}
	}

	if(transferStep == 1)
	{
		int32_t result = transferFile.read(sdram_writeLoadBuffer, memStep);

		if(result >= 0)
		{
			convertedDataSize += convertAudioData(memory+convertedDataSize, result);

			memComplited += result;
			if(memComplited >= memTotal || memComplited >= sampleHead.subchunk2Size) // przekroczenie mniejszego zatrzyma ladowanie
			{
				transferStep = 2;
			}
			else
			{
				return fileTransferInProgress;
			}
		}
		//else wysypuje error na koncu funkcji \/
	}

	if(transferStep == 2)
	{
		//wybiera wielkosc mniejsza z naglowka lub suma read
		uint32_t sizeFromHead = sampleHead.subchunk2Size/sampleHead.blockAlign;
		*outSampleCount = (convertedDataSize < sizeFromHead) ? convertedDataSize : sizeFromHead;

		transferStep = 0;
		transferFile.close();
		return fileTransferEnd;
	}

	transferStep = 0;
	transferFile.close();
	return fileTransferError;
}



void cFileTransfer::endTransfer()
{
	transferFile.close();
}

//zwraca wielkosc w probkach
uint32_t cFileTransfer::convertAudioData(int16_t* outPtr, int32_t input_size)
{
	uint8_t bytes_per_sample = (sampleHead.bitsPerSample/8)*sampleHead.numChannels;

	if(input_size < bytes_per_sample) return 0; // nie obliczaj jezeli przekazano za malo danych

	uint32_t samplesToConvert = input_size/bytes_per_sample;
	//uint8_t channelNum = sampleHead.numChannels;

	if(sampleHead.AudioFormat == 1) //pcm 16/24bity
	{
		uint8_t* ptrTemp = (uint8_t*)sdram_writeLoadBuffer;
		if(sampleHead.bitsPerSample == 24) ptrTemp += 1; //uciecie LSB

		for(uint32_t i = 0; i < samplesToConvert; i++)
		{
			(*outPtr++) = *((int16_t*)ptrTemp);
			ptrTemp += bytes_per_sample;
		}
	}
	else if(sampleHead.AudioFormat == 3) //float 32bity
	{
		float *buf = (float*)sdram_writeLoadBuffer;
		uint8_t floates_per_sample = bytes_per_sample/4;

		for(uint32_t i = 0; i < samplesToConvert; i++)
		{
			(*outPtr++) =  ( (((*buf + 1.0f) * 65535.0f ) / 2.0f)  - 32768.0f );
			buf += floates_per_sample;
		}
	}

	return samplesToConvert;
}


bool cFileTransfer::checkSampleValid()
{
	if((sampleHead.format != 1163280727)
	||((sampleHead.AudioFormat != 1) && (sampleHead.AudioFormat != 3))
	||((sampleHead.bitsPerSample != 16) && (sampleHead.bitsPerSample != 24) && (sampleHead.bitsPerSample != 32))
	||((sampleHead.numChannels != 1) && (sampleHead.bitsPerSample != 2))
	||  sampleHead.sampleRate != 44100)
	{
		return false;
	}

	return true;
}


