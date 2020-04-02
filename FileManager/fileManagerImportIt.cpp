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

extern Sequencer::strPattern fileManagerPatternBuffer;

extern int16_t sdram_sampleBank[4 * 1024 * 1024];
int16_t *itFile_sampleDest_ptr = sdram_sampleBank;

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

	itFile_sampleDest_ptr = sdram_sampleBank;

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
		Serial.printf("Cmwt = 0h%04X\n", Cwt);

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
uint32_t cFileManager::getFileVariable(uint32_t subFileOffset,
										uint32_t offset,
										uint8_t varSize)
{

	uint8_t byteBuffer[varSize];

	uint32_t fileOffset = subFileOffset + offset;

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														impFileData.path,
														byteBuffer,
														sizeof(varSize),
														fileOffset, // fileOffset
														fileWholeOnce); // fileDivIntoParts

	if (loadStatus == fileTransferEnd)
	{
		return readUint(byteBuffer, varSize);
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
uint8_t sampleNumber = 0;
void cFileManager::importItFile_ProcessInstruments()
{
	strInstrument *instr = &mtProject.instrument[processedInstrument];
	setDefaultActiveInstrument(instr);

	uint32_t fileOffset = getInstrumentOffset(processedInstrument);

	uint8_t tempSampleNumber = 0;
	bool found = 0;
	for (uint8_t a = 0; a < 120; a++)
	{
		tempSampleNumber = getFileVariable(fileOffset, 0x41 + a * 2, 1);
//		Serial.printf("tempSampleNumber: %d\n", tempSampleNumber);

		if (tempSampleNumber > 0 && !found)
		{
			sampleNumber = tempSampleNumber;
			found = 1;
			break; // powyżej na potrzeby testow
		}

	}

	uint8_t GbV = getFileVariable(fileOffset, 0x18, 1); //Global Volume, 0->128

	instr->volume = map(GbV, 0, 128, 0, 100);

	Serial.printf("Insstrument: %d, NoS: %d, smp no: %d, GbV %d\n",
					processedInstrument,
					getFileVariable(fileOffset, 0x1e, 1),
					sampleNumber,
					GbV);

	// pobieranie nazwy
	char nameBuffer[26];
	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														impFileData.path,
														(uint8_t*) nameBuffer,
														sizeof(nameBuffer),
														fileOffset + 0x20, // fileOffset
														fileWholeOnce); // fileDivIntoParts
	if (loadStatus == fileTransferEnd)
	{
		if (nameBuffer[0] != 0)
		{
			strcpy(instr->sample.file_name, nameBuffer);
			Serial.println(instr->sample.file_name);
		}
		else
		{
//			sprintf(instr->sample.file_name, "(untitled %d)",
//					processedInstrument + 1);
		}
	}

	if (sampleNumber > 0)
	{
		moveToNextOperationStep();
	}
	else if (sampleNumber == 0 && (getSampleOffset(processedInstrument) > 0))
	{
		sampleNumber = processedInstrument + 1;
		moveToNextOperationStep();

	}
	else // jesli nie ma sampla to powtarzamy krok
	{
		processedInstrument++;
		// jesli skonczyly sie instrumenty to 2 kroki dalej
		if (processedInstrument >= InsNum ||
				processedInstrument > INSTRUMENTS_MAX)
		{
			moveToNextOperationStep();
			moveToNextOperationStep();
		}
	}

}

void cFileManager::importItFile_OpenSample()
{
	strInstrument *instr = &mtProject.instrument[processedInstrument];

	uint32_t fileOffset = getSampleOffset(sampleNumber - 1);
	if (fileOffset <= 0)
	{
		processedInstrument++;
		// jesli skonczyly sie instrumenty to 2 kroki dalej
		if (processedInstrument >= InsNum ||
				processedInstrument > INSTRUMENTS_MAX)
		{
			moveToNextOperationStep();
		}
		else
		{
			moveToPrevOperationStep();
		}
		return;
	}

	uint8_t sampleHeader[0x50];

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														impFileData.path,
														(uint8_t*) sampleHeader,
														sizeof(sampleHeader),
														fileOffset, // fileOffset
														fileWholeOnce); // fileDivIntoParts

	if (loadStatus == fileTransferEnd)
	{

		uint32_t Flg = readUint(&sampleHeader[0x12], 4);
		uint32_t length = readUint(&sampleHeader[0x30], 4);
		uint32_t loopBegin = readUint(&sampleHeader[0x34], 4);
		uint32_t loopEnd = readUint(&sampleHeader[0x38], 4);
		uint32_t C5Speed = readUint(&sampleHeader[0x3c], 4);
		uint32_t SmpPoint = readUint(&sampleHeader[0x48], 4); //'Long' Offset of sample in file.

		bool isHeader = (Flg & 0b00000001);
		bool is16or8bit = (Flg & 0b00000010);
		bool isStereo = (Flg & 0b00000100);
		bool isCompression = (Flg & 0b00001000);
		bool isLoop = (Flg & 0b00010000);
		bool isPingPong = (Flg & 0b01000000);
		bool isPingPongSustain = (Flg & 0b10000000);

		Serial.printf(
				"flags: %s %s %s %s %s %s %s \n",
				isHeader ? "header" : "no header",
				is16or8bit ? "16bit" : "8bit",
				isStereo ? "stereo" : "mono",
				isCompression ? "compressed" : "no compress",
				isLoop ? "ude loop" : "no loop",
				isPingPong ? "ping pong" : "forward",
				isPingPongSustain ? "ping pong sutain" : "forward sustain");

		Serial.printf(
				"length: %d, loopBegin: %d, loopEnd %d, C5Speed: %d, SmpPoint %d\n",
				length,
				loopBegin,
				loopEnd,
				C5Speed,
				SmpPoint
				);

		if (sampleNumber > 0 && length > 0) // 0 == no sample
		{
			instr->isActive = 1;
			instr->sample.type = mtSampleTypeWaveFile;

			instr->sample.length = length;
			instr->startPoint = 0;

			instr->loopPoint1 = map((float) loopBegin,
									0,
									length,
									0,
									MAX_16BIT);
			instr->loopPoint2 = map((float) loopEnd,
									0,
									length,
									0,
									MAX_16BIT);

			instr->endPoint = MAX_16BIT;

			// todo save instrument

			if (isLoop)
			{
				if (isPingPongSustain)
				{
					instr->playMode = loopPingPong;
				}
				else
				{
					instr->playMode = loopForward;
				}
			}
			else
			{
				instr->playMode = singleShot;
				instr->loopPoint1 = 1;
				instr->loopPoint2 = MAX_16BIT - 1;
			}

			Serial.printf(
							"loopPoint1: %d, loopPoint2 %d\n",
							instr->loopPoint1,
							instr->loopPoint2
							);

			// kopiujemy dane sampli:
			if (is16or8bit)
			{
				loadStatus = fileTransfer.loadFileToMemory(
						impFileData.path,
						(uint8_t*) itFile_sampleDest_ptr,
						instr->sample.length * 2, // memo
						SmpPoint, // offset
						fileWholeOnce
						);

				instr->sample.address = itFile_sampleDest_ptr;

				itFile_sampleDest_ptr += instr->sample.length;

				mtProject.used_memory += instr->sample.length * 2;
			}
			else // 8bit
			{
				uint32_t totalToRead = instr->sample.length;
				uint8_t buff[512] { 0 };

				uint16_t *tempPtr = (uint16_t*) itFile_sampleDest_ptr; // to je zle
				while (totalToRead)
				{
					uint32_t bytesToRead =
							totalToRead >= 512 ? 512 : totalToRead;

					loadStatus = fileTransfer.loadFileToMemory(
							impFileData.path,
							(uint8_t*) buff,
							bytesToRead, // memo
							SmpPoint + (instr->sample.length - totalToRead), // offset
							fileWholeOnce
							);

					for (uint16_t s = 0; s < bytesToRead; s++)
					{
						*tempPtr = map(buff[s],
										0,
										MAX_8BIT,
										0,
										MAX_16BIT);

						tempPtr++;
					}

					totalToRead -= bytesToRead;

				}

				instr->sample.address = itFile_sampleDest_ptr;

				itFile_sampleDest_ptr += instr->sample.length;

				mtProject.used_memory += instr->sample.length * 2;

			}

			saveInstrument(processedInstrument);

		}
		else
		{
			instr->isActive = 0;
			instr->sample.length = 0;
		}

		Serial.println();

		processedInstrument++;
		// jesli skonczyly sie instrumenty to 2 kroki dalej
		if (processedInstrument >= InsNum ||
				processedInstrument > INSTRUMENTS_MAX)
		{
			moveToNextOperationStep();
		}
		else
		{
			moveToPrevOperationStep();
		}
	}

	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}

}

void cFileManager::importItFile_InitPattern()
{

	uint32_t patternOffset = getPatternOffset(processedPattern);

	if (patternOffset == 0)
	{
		Serial.printf("patt %d, len: %d, rows: %d\n",
						processedPattern,
						0,
						0);
		/*
		 Note that if the (long) offset to a pattern = 0, then the
		 pattern is assumed to be a 64 row empty pattern.
		 */
		processedPattern++;
		if (processedPattern >= PatNum)
		{
			moveToNextOperationStep();
			moveToNextOperationStep();
		}
		return;

	}

	uint8_t byteBuffer[4];
	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														impFileData.path,
														byteBuffer,
														sizeof(byteBuffer),
														patternOffset, // fileOffset
														fileWholeOnce); // fileDivIntoParts

	if (loadStatus == fileTransferEnd)
	{

		uint16_t length = readUint(&byteBuffer[0], 2);
		uint16_t rows = readUint(&byteBuffer[2], 2);

		Serial.printf("patt %d, len: %d, rows: %d\n",
						processedPattern,
						length,
						rows);

		importItFile_setPattern(processedPattern + 1, rows - 1);

		importItFile_ProcessPattern(patternOffset,
									length,
									rows);

		importItFile_savePattern();

		processedPattern++;
		if (processedPattern >= PatNum)
		{
			moveToNextOperationStep();
		}
	}
	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}
}

void cFileManager::importItFile_ProcessPattern(uint32_t patternOffset,
												uint16_t length,
												uint16_t rows)
{
	if (length > 512) return;
	uint8_t byteBuffer[length];
	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														impFileData.path,
														byteBuffer,
														sizeof(byteBuffer),
														patternOffset + 8, // fileOffset
														fileWholeOnce); //
	uint8_t previousmaskvariable[8] { 0 };
	uint8_t lastNote[8] { 0 };
	uint8_t lastInstrument[8] { 0 };
	uint8_t lastVolume[8] { 0 };
	uint8_t lastCommand[8] { 0 };
	uint8_t lastCommandValue[8] { 0 };
	if (loadStatus == fileTransferEnd)
	{
		uint16_t byteIndex = 0;
		uint8_t channel = 0;
		uint8_t maskvariable = 0;
		uint8_t note = 0;
		uint8_t instrument = 0;
		uint8_t volume = 0;
		uint8_t command = 0;
		uint8_t commandValue = 0;
		uint8_t row = 0;

		while (byteIndex < length)
		{

			uint8_t channelvariable = byteBuffer[byteIndex++];

			if (channelvariable == 0)
			{
				Serial.println("end of row");
				row++;
			}
			else
			{

				channel = (channelvariable - 1) & 63;

				if (channelvariable & 128)
				{
					maskvariable = byteBuffer[byteIndex++];
					previousmaskvariable[channel] = maskvariable;
				}
				else
				{
					maskvariable = previousmaskvariable[channel];
				}

				if (maskvariable & 1)
				{
					note = byteBuffer[byteIndex++];
					lastNote[channel] = note;
				}
				if (maskvariable & 2)
				{
					instrument = byteBuffer[byteIndex++];
					lastInstrument[channel] = instrument;
				}
				if (maskvariable & 4)
				{
					volume = byteBuffer[byteIndex++];
					lastVolume[channel] = volume;
				}
				if (maskvariable & 8)
				{
					command = byteBuffer[byteIndex++];
					commandValue = byteBuffer[byteIndex++];

					lastCommand[channel] = command;
					lastCommandValue[channel] = commandValue;
				}
				if (maskvariable & 16)
				{
					note = lastNote[channel];
				}
				if (maskvariable & 32)
				{
					instrument = lastInstrument[channel];
				}
				if (maskvariable & 64)
				{
					volume = lastVolume[channel];
				}
				if (maskvariable & 128)
				{
					command = lastCommand[channel];
					commandValue = lastCommandValue[channel];
				}

				Serial.printf("row: %d, chan: %d, note: %d, ins: %d\n",
								row,
								channel,
								note,
								instrument);

				importItFile_setStep(row,
										channel,
										note,
										instrument,
										0,
										0);

			}

		}

	}
	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}

}

void cFileManager::importItFile_setStep(uint8_t step,
										uint8_t track,
										uint8_t note,
										uint8_t instrument,
										uint8_t fx,
										uint8_t fxVal)
{

	if (step > Sequencer::MAXSTEP) return;
	if (track > Sequencer::MAXTRACK) return;
	if (instrument > INSTRUMENTS_MAX) return;

	Sequencer::strPattern::strTrack::strStep *pattStep = &sequencer.getActualPattern()->track[track].step[step];

	pattStep->note = note;
	pattStep->instrument = instrument;

}

void cFileManager::importItFile_setPattern(uint8_t index, uint8_t length)
{
	Sequencer::strPattern *patt = sequencer.getActualPattern();
	length = constrain(length, 0, Sequencer::MAXSTEP);

	sequencer.clearPattern(patt);
	mtProject.values.actualPattern = index;
	for (uint8_t a = 0; a <= Sequencer::MAXTRACK; a++)
	{
		patt->track[a].length = length;
	}

}

void cFileManager::importItFile_savePattern()
{

	Sequencer::strPattern *patt = sequencer.getActualPattern();

	updatePatternBitmask(mtProject.values.actualPattern - 1, (uint8_t*) patt);

	if (!writePatternToFileStruct(sequencer.getPatternToSaveToFile(),
									(uint8_t*) &fileManagerPatternBuffer))
	{
		throwError(0);
	}

	char patternToSave[PATCH_SIZE];
	sprintf(patternToSave, cWorkspacePatternFileFormat,
			mtProject.values.actualPattern);

	uint8_t saveStatus = fileTransfer.saveMemoryToFile(
			(uint8_t*) &fileManagerPatternBuffer, patternToSave,
			sizeof(Sequencer::strPattern));

	if (saveStatus == fileTransferEnd)
	{

	}
	else
	{
		// todo ogarnąć indexy err
		throwError(2);
	}
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
