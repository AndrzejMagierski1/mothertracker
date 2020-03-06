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

void cFileManager::importModFileInit()
{

	char byteBuffer[4];

	sprintf(modFilePath, "%s/%s", cModsPath, modToImportFilename);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														modFilePath,
														(uint8_t*) byteBuffer,
														4,
														1080,
														fileWholeOnce); // fileDivIntoParts

//	moveToNextOperationStep();
	if (loadStatus == fileTransferEnd)
	{

		if (strstr(byteBuffer, "M.K.") != NULL)
		{
			modFileInstrumentsCount = 31;
			modFileChannelsCount = 4;
			moveToNextOperationStep();
		}
		else
		{
			throwError(2);
		}

	}
	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}
//	moveToNextOperationStep();
}
void cFileManager::importModFile_GetInstrumentData()
{

	char byteBuffer[modSampleInfoSize];
	uint8_t buffOffset = 0;

	sprintf(modFilePath, "%.10s/%.200s", cModsPath, modToImportFilename);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														modFilePath,
														(uint8_t*) byteBuffer,
														modSampleInfoSize, // memo
			20 + modFileInstruments_actualIndex * modSampleInfoSize, // offset
			fileWholeOnce); // fileDivIntoParts

	if (loadStatus == fileTransferEnd)
	{

		//sampleName
//		uint8_t a = 0;
		memset(modSampleData.sampleName, 0, sizeof(modSampleData.sampleName));
		for (uint8_t a = 0; a < modSampleNameSize; a++)
		{
			if (byteBuffer[a] != 0)
			{
				modSampleData.sampleName[a] = byteBuffer[a];
			}
			else
				break;
		}
		buffOffset = modSampleNameSize;

		//sampleLengthInWords
		modSampleData.sampleLengthInWords = byteBuffer[buffOffset] << 8 | byteBuffer[buffOffset + 1];
		buffOffset += 2;
		modSampleData.finetune = byteBuffer[buffOffset] & 0b00001111;
		buffOffset++;
		modSampleData.volume = byteBuffer[buffOffset++];

		modSampleData.repeatPointInWords = byteBuffer[buffOffset] << 8 | byteBuffer[buffOffset + 1];
		buffOffset += 2;
		modSampleData.repeatLengthInWords = byteBuffer[buffOffset] << 8 | byteBuffer[buffOffset + 1];
		buffOffset += 2;

		//;TUTAJ UTWORZYC PLIK INSTRUMENTU

		moveToNextOperationStep();

	}
	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}

//	moveToNextOperationStep();
}

void cFileManager::importMod_SaveInstrument()
{

	strInstrument * instr = &mtProject.instrument[modFileInstruments_actualIndex];

	instr->isActive = 1;
	instr->sample.type = mtSampleTypeWaveFile;

	strcpy(instr->sample.file_name, modSampleData.sampleName);

	if (modSampleData.repeatLengthInWords > 1)
	{

		instr->playMode = loopForward;
	}
	else
	{
		instr->playMode = singleShot;
	}

	// todo: to oblicza z zaladowanegolu sampla
	instr->sample.length = modSampleData.sampleLengthInWords;

	instr->startPoint = 0;

	if (instr->playMode == loopForward)
	{

//	instr->loopPoint1 = modSampleData.repeatPointInWords * 2;

		instr->loopPoint1 = map(
								modSampleData.repeatPointInWords,
								0,
								modSampleData.sampleLengthInWords,
								0,
								MAX_16BIT);

		instr->loopPoint2 = map(
				modSampleData.repeatPointInWords + modSampleData.repeatLengthInWords,
				0,
				modSampleData.sampleLengthInWords,
				0,
				MAX_16BIT);
	}

	instr->endPoint = MAX_16BIT;

	if (saveInstrument(modFileInstruments_actualIndex))
	{
		modFileInstruments_actualIndex++;
		if (modFileInstruments_actualIndex >= modFileInstrumentsCount)
		{
			moveToNextOperationStep();
		}
		else
		{
			currentOperationStep--;
		}
	}

}

void cFileManager::importModFile_SongInit()
{

	char byteBuffer[modSongInfoSize];
	uint8_t buffOffset = 0;

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														modFilePath,
														(uint8_t*) byteBuffer,
														modSongInfoSize, // memo
			20 + modFileInstrumentsCount * modSampleInfoSize, // offset
			fileWholeOnce
			);

	memcpy(&modSong, &byteBuffer, modSongInfoSize);

	for (uint8_t a = 0; a < 128; a++)
	{
		if (modSong.playlist[a] > modFilePatterns_count)
		{
			modFilePatterns_count = modSong.playlist[a];
		}
	}

	moveToNextOperationStep();
}

void cFileManager::importModFile_Patterns()
{

	char byteBuffer[modPatternSize];
	uint8_t buffOffset = 0;

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
			modFilePath,
			(uint8_t*) byteBuffer,
			modPatternSize, // memo
			20 + modFileInstrumentsCount * modSampleInfoSize + modSongInfoSize + modPatternSize * modFilePatterns_actualIndex, // offset
			fileWholeOnce
			);

	for (uint16_t a = 0; a < 256; a++)
	{
		if (a % 4 == 0)
		{
			Serial.println();
		}
		// todo: kiedyś poprawić
		uint32_t chData;
		*((uint8_t*) &chData + 3) = byteBuffer[a * 4];
		*((uint8_t*) &chData + 2) = byteBuffer[a * 4+1];
		*((uint8_t*) &chData + 1) = byteBuffer[a * 4+2];
		*((uint8_t*) &chData + 0) = byteBuffer[a * 4+3];


		uint8_t instrument = (chData >> 12) & 0x0f;

		uint16_t period = (chData >> 16) & 0xfff;
		uint8_t note = periodToNote(period);

		uint8_t fx3 = chData & 0x00f;
		uint8_t fx2 = (chData & 0x0f0) >> 4;
		uint8_t fx1 = (chData & 0xf00) >> 8;



		Serial.printf("inst %d note %d, fx1 %d, fx2 %d, fx3 %d - ", instrument, note, fx1, fx2, fx3);
//		0b 11111111 11111111 11111111 11111111

	}

	moveToNextOperationStep();
}

uint8_t cFileManager::periodToNote(uint16_t period)
{
	switch (period)
	{
	case 1712:
		return 0;
	case 1616:
		return 1;
	case 1525:
		return 2;
	case 1440:
		return 3;
	case 1357:
		return 4;
	case 1281:
		return 5;
	case 1209:
		return 6;
	case 1141:
		return 7;
	case 1077:
		return 8;
	case 1017:
		return 9;
	case 961:
		return 10;
	case 907:
		return 11;
	case 856:
		return 12;
	case 808:
		return 13;
	case 762:
		return 14;
	case 720:
		return 15;
	case 678:
		return 16;
	case 640:
		return 17;
	case 604:
		return 18;
	case 570:
		return 19;
	case 538:
		return 20;
	case 508:
		return 21;
	case 480:
		return 22;
	case 453:
		return 23;
	case 428:
		return 24;
	case 404:
		return 25;
	case 381:
		return 26;
	case 360:
		return 27;
	case 339:
		return 28;
	case 320:
		return 29;
	case 302:
		return 30;
	case 285:
		return 31;
	case 269:
		return 32;
	case 254:
		return 33;
	case 240:
		return 34;
	case 226:
		return 35;
	case 214:
		return 36;
	case 202:
		return 37;
	case 190:
		return 38;
	case 180:
		return 39;
	case 170:
		return 40;
	case 160:
		return 41;
	case 151:
		return 42;
	case 143:
		return 43;
	case 135:
		return 44;
	case 127:
		return 45;
	case 120:
		return 46;
	case 113:
		return 47;
	case 107:
		return 48;
	case 101:
		return 49;
	case 95:
		return 50;
	case 90:
		return 51;
	case 85:
		return 52;
	case 80:
		return 53;
	case 76:
		return 54;
	case 71:
		return 55;
	case 67:
		return 56;
	case 64:
		return 57;
	case 60:
		return 58;
	case 57:
		return 59;

	default:
		return 0;
	}
	return 0;
}

void cFileManager::printNote(uint8_t note)
{
	switch (note % 12)
	{
	case 0:
		Serial.print("C");
		break;
	case 1:
		Serial.print("C#");
		break;
	case 2:
		Serial.print("D");
		break;
	case 3:
		Serial.print("D#");
		break;
	case 4:
		Serial.print("E");
		break;
	case 5:
		Serial.print("F");
		break;
	case 6:
		Serial.print("F");
		break;
	case 7:
		Serial.print("G");
		break;
	case 8:
		Serial.print("G#");
		break;
	case 9:
		Serial.print("A");
		break;
	case 10:
		Serial.print("A#");
		break;
	case 11:
		Serial.print("B");
		break;
	}

	Serial.print(note / 12);
}
void cFileManager::importModFileWaves()
{

	moveToNextOperationStep();
}
void cFileManager::importModFileFinish()
{
	status = fmLoadEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;

}

void cFileManager::importModFileError()
{
	importModFileAfterNewProject = 0;

}
