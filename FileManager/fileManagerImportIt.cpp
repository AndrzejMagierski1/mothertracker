#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "FastCRC.h"
#include "FastCRC_cpu.h"
#include "FastCRC_tables.h"

#include "mtSequencer.h"

#include "Arduino.h"

#include "fileTransfer.h"
#include "fileManager.h"

extern int16_t sdram_sampleBank[4 * 1024 * 1024];

extern Sequencer::strPattern fileManagerPatternBuffer;

void cFileManager::importItFile_Init()
{
	importModFileType = checkImportFileType();
	if (importModFileType != importModFiletype_it)
		return;
	moveToNextOperationStep();
}

uint32_t readUint(uint8_t *buffStart, uint8_t size)
{
	if (size > 4) return 0;

	uint32_t retVal = 0;
	for (uint8_t a = 0; a < size; a++)
	{
		retVal |= *(buffStart + a) << (a * 8);
	}

	return retVal;
}

void cFileManager::importItFile_ProcessHeader()
{
	// wew
	uint8_t byteBuffer[0x40];

	sprintf(impFileData.path, "%.10s/%.200s", cModsPath, impFileData.filename);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														impFileData.path,
														(uint8_t*) byteBuffer,
														sizeof(byteBuffer),
														0,
														fileWholeOnce); // fileDivIntoParts

	uint16_t PHiligt = readUint(&byteBuffer[0x1e], 2);
	uint16_t OrdNum = readUint(&byteBuffer[0x20], 2);

	uint16_t InsNum = readUint(&byteBuffer[0x22], 2);
	uint16_t SmpNum = readUint(&byteBuffer[0x24], 2);
	uint16_t PatNum = readUint(&byteBuffer[0x26], 2);
	uint16_t Cwt = readUint(&byteBuffer[0x28], 2);
	uint16_t Cmwt = readUint(&byteBuffer[0x2A], 2);
	uint16_t Flags = readUint(&byteBuffer[0x2C], 2);
	uint16_t Special = readUint(&byteBuffer[0x2E], 2);
	uint16_t GV = readUint(&byteBuffer[0x30], 1);
	uint16_t MV = readUint(&byteBuffer[0x31], 1);
	uint16_t IS = readUint(&byteBuffer[0x32], 1);
	uint16_t IT = readUint(&byteBuffer[0x33], 1);
	uint16_t Sep = readUint(&byteBuffer[0x34], 1);
	uint16_t PWD = readUint(&byteBuffer[0x35], 1);
	uint16_t MsgLgth = readUint(&byteBuffer[0x36], 1);
	uint32_t MessageOffset = readUint(&byteBuffer[0x38], 4);
	uint8_t oldImpulseFormat = Cmwt < 0x200;

	if (loadStatus == fileTransferEnd)
	{

		Serial.println(PHiligt);
		Serial.println(OrdNum);
		Serial.println(InsNum);
		Serial.println(SmpNum);
		Serial.println(PatNum);
		Serial.println(Cwt);
		Serial.println(oldImpulseFormat ? "old impulse format":"new impulse format");
		Serial.println();
		Serial.println(Special, BIN);
		Serial.println(MsgLgth);
		Serial.println(MessageOffset);
		moveToNextOperationStep();

	}
	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}
}
