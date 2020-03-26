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

// elementy pliku IT
uint16_t OrdNum;

uint16_t InsNum;
uint16_t SmpNum;
uint16_t PatNum;

uint16_t Cwt;
uint16_t Cmwt;
uint16_t Flags;
uint16_t Special;
uint16_t GV;
uint16_t MV;
uint16_t IS;
uint16_t IT;

uint16_t Sep;
uint16_t PWD;
uint16_t MsgLgth;

uint32_t MessageOffset;
uint8_t oldImpulseInstrumentFormat;

const uint8_t FLAGS__STEREO = 0b00000001;
const uint8_t FLAGS__USE_INSTRUMENTS = 0b00000100;
const uint8_t SPECIAL__SONG_MESSAGE = 0b00000001;

// zmienne wewnetrzne

uint8_t processedPattern = 0;
uint8_t processedInstrument = 0;
uint8_t processedSample = 0;

extern Sequencer::strPattern fileManagerPatternBuffer;

void cFileManager::importItFile_Init()
{
	importModFileType = checkImportFileType();
	if (importModFileType != importModFiletype_it)
		return;

	// init wartosci
	processedPattern = 0;
	processedInstrument = 0;
	processedSample = 0;

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
	if (loadStatus == fileTransferEnd)
	{

		uint16_t PHiligt = readUint(&byteBuffer[0x1e], 2);
		OrdNum = readUint(&byteBuffer[0x20], 2);

		InsNum = readUint(&byteBuffer[0x22], 2);
		SmpNum = readUint(&byteBuffer[0x24], 2);
		PatNum = readUint(&byteBuffer[0x26], 2);
		Cwt = readUint(&byteBuffer[0x28], 2);
		Cmwt = readUint(&byteBuffer[0x2A], 2);
		Flags = readUint(&byteBuffer[0x2C], 2);
		Special = readUint(&byteBuffer[0x2E], 2);
		GV = readUint(&byteBuffer[0x30], 1);
		MV = readUint(&byteBuffer[0x31], 1);
		IS = readUint(&byteBuffer[0x32], 1);
		IT = readUint(&byteBuffer[0x33], 1);
		Sep = readUint(&byteBuffer[0x34], 1);
		PWD = readUint(&byteBuffer[0x35], 1);
		MsgLgth = readUint(&byteBuffer[0x36], 1);
		MessageOffset = readUint(&byteBuffer[0x38], 4);
		oldImpulseInstrumentFormat = Cmwt < 0x200;

		Serial.println(PHiligt);
		Serial.printf("OrdNum = %d\n", OrdNum);
		Serial.printf("InsNum = %d\n", InsNum);
		Serial.printf("SmpNum = %d\n", SmpNum);
		Serial.printf("PatNum = %d\n", PatNum);
		Serial.printf("Cwt = %d\n", Cwt);

		Serial.println(
				oldImpulseInstrumentFormat ? "old impulse format" : "new impulse format");
		Serial.println();

		Serial.println(Special, BIN);
		Serial.printf("MsgLgth = %d\n", MsgLgth);
		Serial.printf("MessageOffset = %d\n", MessageOffset);

		if (oldImpulseInstrumentFormat)
		{
			throwError(1);
		}
		else
		{
			moveToNextOperationStep();
		}

	}
	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}
}

void cFileManager::importItFile_ProcessSong()
{
	uint8_t byteBuffer[OrdNum];

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														impFileData.path,
														(uint8_t*) byteBuffer,
														sizeof(byteBuffer),
														0x00C0, // offset
														fileWholeOnce); // fileDivIntoParts

	if (loadStatus == fileTransferEnd)
	{
		Serial.println("song:");
		for (uint8_t &a : byteBuffer)
		{
			Serial.println(a);
		}
		moveToNextOperationStep();
	}

	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}
}
uint32_t cFileManager::getInstrumentOffset(uint8_t index)
{
	if (index >= InsNum) return 0;

	uint8_t byteBuffer[4];

	uint32_t fileOffset = 0x00C0 + OrdNum + index * sizeof(uint32_t);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														impFileData.path,
														byteBuffer,
														sizeof(uint32_t),
														fileOffset, // offset
														fileWholeOnce); // fileDivIntoParts

	if (loadStatus == fileTransferEnd)
	{
		return readUint(byteBuffer, 4);
	}

	return 0;
}

uint32_t cFileManager::getSampleOffset(uint8_t index)
{
	if (index >= PatNum) return 0;

	uint8_t byteBuffer[4];

	uint32_t fileOffset = 0x00C0 + OrdNum + InsNum * 4 + index * sizeof(uint32_t);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														impFileData.path,
														byteBuffer,
														sizeof(uint32_t),
														fileOffset, // fileOffset
														fileWholeOnce); // fileDivIntoParts

	if (loadStatus == fileTransferEnd)
	{
		return readUint(byteBuffer, 4);
	}

	return 0;
}

uint32_t cFileManager::getPatternOffset(uint8_t index)
{
	if (index >= PatNum) return 0;

	uint8_t byteBuffer[4];

	uint32_t fileOffset = 0x00C0 + OrdNum + InsNum * 4 + SmpNum * 4 + index * sizeof(uint32_t);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														impFileData.path,
														byteBuffer,
														sizeof(uint32_t),
														fileOffset, // fileOffset
														fileWholeOnce); // fileDivIntoParts

	if (loadStatus == fileTransferEnd)
	{
		return readUint(byteBuffer, 4);
	}

	return 0;
}
void cFileManager::importItFile_ProcessOffsets()
{

	for (uint8_t a = 0; a < InsNum; a++)
	{
		Serial.printf("offset instr %d: %08X\n", a,
						getInstrumentOffset(a));
	}

	for (uint8_t a = 0; a < SmpNum; a++)
	{

		Serial.printf("offset sample %d: %08X\n", a,
						getSampleOffset(a));
	}
	for (uint8_t a = 0; a < PatNum; a++)
	{

		Serial.printf("offset pattern %d: %08X\n", a,
						getPatternOffset(a));
	}

	moveToNextOperationStep();

}

void cFileManager::importItFile_ProcessInstruments()
{

	uint8_t byteBuffer[0x10];

	uint32_t fileOffset = getInstrumentOffset(processedPattern);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														impFileData.path,
														byteBuffer,
														sizeof(uint32_t),
														fileOffset, // fileOffset
														fileWholeOnce); // fileDivIntoParts

}
