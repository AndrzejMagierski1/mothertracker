

#include "mtStructs.h"
#include "fileManagerDefs.h"

#include "SD.h"
#include "debugLog.h"

#include "FastCRC.h"
#include "FastCRC_cpu.h"
#include "FastCRC_tables.h"

#include "mtSequencer.h"

#include "fileTransfer.h"



cFileTransfer fileTransfer;




uint8_t cFileTransfer::loadFileToMemory(const char* file, void* memory, uint32_t memSize, uint8_t mode)
{
	if(transferStep == 0)
	{
		if(!SD.exists(file)) return fileTransferError;

		if(transferFile.open(file))
		{
			memStep = (mode == fileWholeOnce) ? memSize : memoryReadPerPart;
			memComplited = 0;
			transferStep = 1;

			memset(memory,0,memSize);
		}
	}

	if(transferStep == 1)
	{
		int32_t result = transferFile.read(memPtr, memStep);

		if(result >= 0)
		{
			memPtr += result;
			memComplited += result;

			if(memComplited >= memTotal)
			{
				transferStep = 2;
			}
		}
	}

	if(transferStep == 2)
	{
		memory = memPtr;
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
	delete (char*)memPtr;
}
