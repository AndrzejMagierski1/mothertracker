

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




uint8_t cFileTransfer::loadFileToMemory(const char* file, uint8_t* memory, uint32_t memSize, uint8_t mode)
{
	if(transferStep == 0)
	{
		if(!SD.exists(file)) return fileTransferFileNoExist;

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
