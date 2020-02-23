
#ifndef FILEMANAGER_FILETRANSFER_H_
#define FILEMANAGER_FILETRANSFER_H_


#include <stdint.h>
#include "wavHeaderReader.h"

enum enTranferFileStatus
{
	fileTransferIdle = 0,
	fileTransferInProgress,
	fileTransferEnd,
	fileTransferError,
	fileTransferFileNoExist,
	fileTransferFileNoValid,

};

enum enTranferFileMode
{
	fileWholeOnce = 0,
	fileDivIntoParts,
};


const uint16_t memoryReadPerPart = 512;


class cFileTransfer
{
public:
	uint8_t getFileTransferStatus();
	uint8_t loadFileToMemory(const char* file, uint8_t* memory, uint32_t memSize, uint8_t mode);
	uint8_t loadSampleToMemory(const char* file, int16_t* memory, uint32_t* outSampleCount);
	uint8_t saveMemoryToFile();

	uint8_t copyFile(const char* src, const char* dest);

	uint32_t convertAudioData(int16_t* outPtr, int32_t input_size);
	bool checkSampleValid();

	uint8_t getFileProgress() 		{ return (memComplited*100)/memTotal; }
	uint32_t getBytesComplited() 	{ return memComplited; }
	void endTransfer();

private:
	uint8_t transferStep = 0;

	uint32_t memStep;
	uint32_t memComplited;
	uint32_t memTotal;
	uint32_t convertedDataSize;

};


extern cFileTransfer fileTransfer;







#endif /* FILEMANAGER_FILETRANSFER_H_ */
