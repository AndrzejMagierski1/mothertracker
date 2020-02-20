
#ifndef FILEMANAGER_FILETRANSFER_H_
#define FILEMANAGER_FILETRANSFER_H_


#include <stdint.h>


enum enTranferFileStatus
{
	fileTransferIdle = 0,
	fileTransferInProgress,
	fileTransferEnd,
	fileTransferError,

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
	uint8_t loadFileToMemory(const char* file, void* memory, uint32_t memSize, uint8_t mode);
	uint8_t saveMemoryToFile();
	void endTransfer();

private:
	uint32_t memStep;
	uint32_t memComplited;
	uint32_t memTotal;

	uint8_t transferStep = 0;

	SdFile transferFile;

	void* memPtr;
	uint8_t actualFileType;

};


extern cFileTransfer fileTransfer;







#endif /* FILEMANAGER_FILETRANSFER_H_ */
