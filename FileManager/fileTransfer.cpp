

#include "mtStructs.h"
#include "fileManagerDefs.h"

#include "SD.h"
#include "debugLog.h"



#include "mtSequencer.h"

#include "fileTransfer.h"



//#define READ_WRITE_BUFOR_SIZE 32640
//extern uint8_t sdram_writeLoadBuffer[READ_WRITE_BUFOR_SIZE];

cFileTransfer fileTransfer;

SdFile transferFile;
SdFile transferFile2;

strWavFileHeader sampleHead;

#define READ_WRITE_BUFOR_SIZE 9600
extern uint8_t g_bufferReadWrite[READ_WRITE_BUFOR_SIZE];


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

			if(result == 0) //koniec pliku
			{
				transferStep = 2;
			}
			else if(memComplited >= memSize) //wczytany wiekszy plik << niebezpieczne przekroczenie pamieci
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
			memStep = READ_WRITE_BUFOR_SIZE; // wielokrotnosc 4*6*8 (rozne probkowania stereo w bajtach)
			memComplited = 0;
			convertedDataSize = 0;

			readHeader(&sampleHead, &transferFile);

			if(!checkSampleValid())
			{
				transferFile.close();
				return fileTransferFileNoValid;
			}

			//wylacznie konwersji kiedy format juz sie zgadza
			if(sampleHead.bitsPerSample == 16 && sampleHead.numChannels == 1 && sampleHead.AudioFormat == 1) conversionEnabled = 0;
			else  conversionEnabled = 1;

			transferStep = 1;
			memTotal = transferFile.available(); // pobiera wielkosc dopiero po przesciu przez header
		}
	}

	if(transferStep == 1)
	{
		int32_t result = transferFile.read(g_bufferReadWrite, memStep);

		if(result >= 0)
		{
			if(conversionEnabled)
			{
				convertedDataSize += convertAudioData(memory+convertedDataSize, result); // tu convertedDataSize to ilosc probek
			}
			else
			{
				memcpy(memory+convertedDataSize, g_bufferReadWrite, result);
				convertedDataSize += result/2;
			}

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


uint8_t cFileTransfer::copyFile(const char* src, const char* dest)
{
	if(transferStep == 0)
	{
		if(!SD.exists(src)) return fileTransferFileNoExist;
		if(SD.exists(dest))
		{
			SD.remove(dest);
		}

		if(transferFile.open(src))
		{
			if(transferFile2.open(dest, FILE_WRITE))
			{
				memTotal = transferFile.size();
				memStep = READ_WRITE_BUFOR_SIZE; // caly mozliwy bufor
				memComplited = 0;
				transferStep = 1;
			}
		}
	}

	if(transferStep == 1)
	{
		int32_t read_result = transferFile.read(g_bufferReadWrite, memStep);

		if(read_result == 0) // koniec pliku - koncz kopiowanie
		{
			transferStep = 2;
		}
		else if(read_result > 0)
		{
			int32_t write_result = transferFile2.write(g_bufferReadWrite, read_result);

			if(write_result >= 0)
			{
				memComplited += write_result;
				if(write_result < memStep) // koniec pliku
				{
					transferStep = 2;
				}
				else if (read_result == write_result)
				{
					return fileTransferInProgress;
				}
			}
		}
	}

	if(transferStep == 2)
	{
		transferStep = 0;
		transferFile.close();
		transferFile2.close();
		return fileTransferEnd;
	}

	transferStep = 0;
	transferFile.close();
	transferFile2.close();
	return fileTransferError;
}

//todo
uint8_t cFileTransfer::copySample(const char* src, const char* dest)
{
	if(transferStep == 0)
	{
		if(!SD.exists(src)) return fileTransferFileNoExist;
		if(SD.exists(dest))
		{
			SD.remove(dest);
		}

		if(transferFile.open(src))
		{
			readHeader(&sampleHead, &transferFile);

			if(!checkSampleValid())
			{
				transferFile.close();
				return fileTransferFileNoValid;
			}

			if(transferFile2.open(dest, FILE_WRITE))
			{
				transferFile2.seek(44);
				memTotal = transferFile.available(); // pobiera wielkosc dopiero po przesciu przez header
				memTotal = (memTotal < sampleHead.subchunk2Size) ? memTotal : sampleHead.subchunk2Size;
				memStep = READ_WRITE_BUFOR_SIZE; // wielokrotnosc 4*6*8 (rozne probkowania stereo w bajtach)
				memComplited = 0;

				//wylacznie konwersji kiedy format juz sie zgadza
				if(sampleHead.bitsPerSample == 16 && sampleHead.numChannels == 1 && sampleHead.AudioFormat == 1) conversionEnabled = 0;
				else  conversionEnabled = 1;

				convertedDataSize = 0;
				transferStep = 1;
			}
		}
	}
//--------------------------------------------------------
	if(transferStep == 1)
	{
		if(memTotal-memComplited < memStep) memStep = memTotal-memComplited;
		int32_t read_result = transferFile.read(g_bufferReadWrite, memStep);

		if(read_result == 0) // koniec pliku - koncz kopiowanie
		{
			transferStep = 2;
		}
		else if(read_result > 0)
		{
			int32_t converted_bytes;
			if(conversionEnabled)
			{
				converted_bytes = convertAudioData((int16_t*)g_bufferReadWrite, read_result) * 2; // liczba probek razy wielkosc probki !
			}
			else
			{
				converted_bytes = read_result;
			}

			if(converted_bytes > 0)
			{
				int32_t write_result = transferFile2.write(g_bufferReadWrite, converted_bytes);

				convertedDataSize += converted_bytes;  // tu convertedDataSize to ilsoc bajtow
				memComplited += read_result;

				if(write_result > 0)
				{
					if(memComplited >= memTotal || memComplited >= sampleHead.subchunk2Size) // przekroczenie mniejszego zatrzyma ladowanie
					{
						transferStep = 2;
					}
					else if (converted_bytes == write_result)
					{
						return fileTransferInProgress;
					}
				}
			}
			else
			{
				transferStep = 2;
			}
		}
		//else wysypuje error na koncu funkcji \/
	}
//--------------------------------------------------------
	if(transferStep == 2)
	{
		fillHeader();
		transferFile2.seek(0);
		transferFile2.write(&sampleHead, sizeof(sampleHead));

		transferStep = 0;
		transferFile.close();
		transferFile2.close();
		return fileTransferEnd;
	}

	transferStep = 0;
	transferFile.close();
	transferFile2.close();
	return fileTransferError;
}


uint8_t cFileTransfer::saveMemoryToFile(uint8_t* memory, const char* file, uint32_t memSize)
{
	if(transferStep == 0)
	{
		if(SD.exists(file))
		{
			SD.remove(file);
		}

		if(transferFile.open(file, FILE_WRITE))
		{
			memTotal = memSize;
			memStep = (memSize < READ_WRITE_BUFOR_SIZE) ? memSize : READ_WRITE_BUFOR_SIZE; // maksymalne paczki jak bufor mimo ze nie kozystanie z bufora
			memComplited = 0;
			transferStep = 1;
		}
	}

	if(transferStep == 1)
	{
		int32_t write_result = transferFile.write(memory+memComplited, memStep);

		if(write_result >= 0)
		{
			memComplited += write_result;
			if(memComplited >=  memTotal) // koniec pliku
			{
				transferStep = 2;
			}
			else if (write_result == memStep)
			{
				return fileTransferInProgress;
			}
		}
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


void cFileTransfer::endTransfer()
{
	transferFile.close();
}

uint32_t czas;

//zwraca wielkosc w probkach
uint32_t cFileTransfer::convertAudioData(int16_t* outPtr, int32_t input_size)
{
	uint8_t bytes_per_sample = (sampleHead.bitsPerSample/8)*sampleHead.numChannels;
	if(input_size < bytes_per_sample) return 0; // nie obliczaj jezeli przekazano za malo danych

	uint32_t samplesToConvert = input_size/bytes_per_sample;
	//uint8_t channelNum = sampleHead.numChannels;

	if(sampleHead.AudioFormat == 1) //pcm 16/24bity
	{
		uint8_t* ptrTemp = (uint8_t*)g_bufferReadWrite;
		if(sampleHead.bitsPerSample == 24) ptrTemp += 1; //uciecie LSB

		for(uint32_t i = 0; i < samplesToConvert; i++)
		{
			(*outPtr++) = *((int16_t*)ptrTemp);
			ptrTemp += bytes_per_sample;
		}
	}
	else if(sampleHead.AudioFormat == 3) //float 32bity
	{
		float *buf = (float*)g_bufferReadWrite;
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
	||((sampleHead.numChannels != 1) && (sampleHead.numChannels != 2))
	||  sampleHead.sampleRate != 44100)
	{
		return false;
	}

	return true;
}

void cFileTransfer::fillHeader()
{
	memset(&sampleHead, 0, sizeof(sampleHead));

	sampleHead.chunkId = 0x46464952; 																// "RIFF"
	sampleHead.chunkSize = convertedDataSize + 36;
	sampleHead.format = 0x45564157;																	// "WAVE"
	sampleHead.subchunk1Id = 0x20746d66;															// "fmt "
	sampleHead.subchunk1Size = 16;
	sampleHead.AudioFormat = 1;
	sampleHead.numChannels = 1;
	sampleHead.sampleRate = 44100;
	sampleHead.bitsPerSample = 16;
	sampleHead.byteRate = sampleHead.sampleRate * sampleHead.numChannels * (sampleHead.bitsPerSample/8);
	sampleHead.blockAlign = sampleHead.numChannels * (sampleHead.bitsPerSample/8);
	sampleHead.subchunk2Id = 0x61746164;															// "data"
	sampleHead.subchunk2Size = convertedDataSize;

}



