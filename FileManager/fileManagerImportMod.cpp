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
int16_t *modFile_sample_ptr = sdram_sampleBank;
uint32_t byteSampleOffset = 0;

void setFx(Sequencer::strPattern::strTrack::strStep *step,
			uint8_t,
			uint8_t, uint8_t);

void cFileManager::importModFile_Init()
{
	// wart wspólne
	byteSampleOffset = 0;
	modFileData.patternsActualIndex = 0;
	modFileData.instrumentsActualIndex = 0;
	modFileData.sampleActualIndex = 0;

	modFileData.waveWriteFlag = 0;

	modFileData.patternsMax = 1;

	modFile_sample_ptr = sdram_sampleBank;
	memset(sdram_sampleBank, 0, sizeof(sdram_sampleBank));

	// wew
	char byteBuffer[4];

	sprintf(modFileData.path, "%.10s/%.200s", cModsPath, modFileData.filename);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														modFileData.path,
														(uint8_t*) byteBuffer,
														4,
														1080,
														fileWholeOnce); // fileDivIntoParts

//	moveToNextOperationStep();
	if (loadStatus == fileTransferEnd)
	{

		if (strstr(byteBuffer, "M.K.") != NULL)
		{
			modFileData.instrumentsCount = 31;
			modFileData.channelsCount = 4;
			moveToNextOperationStep();
		}
		else
		{
			modFileData.instrumentsCount = 15;
			modFileData.channelsCount = 4;
			moveToNextOperationStep();
//			throwError(2);
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

	char byteBuffer[modFileData.sampleInfoSize];
	uint8_t buffOffset = 0;

	sprintf(modFileData.path, "%.10s/%.200s", cModsPath, modFileData.filename);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
			modFileData.path,
			(uint8_t*) byteBuffer,
			modFileData.sampleInfoSize, // memo
			20 + modFileData.instrumentsActualIndex * modFileData.sampleInfoSize, // offset
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

void cFileManager::importModFile_SaveInstrument()
{

	strInstrument *instr = &mtProject.instrument[modFileData.instrumentsActualIndex];
	setDefaultActiveInstrument(instr);

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
	instr->volume = map(modSampleData.volume,
						0,
						64,
						0,
						MAX_INSTRUMENT_VOLUME);

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
//	instr->volume = 100;
//	instr->volume = 50;

	if (saveInstrument(modFileData.instrumentsActualIndex))
	{
		modFileData.instrumentsActualIndex++;
		if (modFileData.instrumentsActualIndex >= modFileData.instrumentsCount)
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

	char byteBuffer[modFileData.songInfoSize];
//	uint8_t buffOffset = 0;

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
			modFileData.path,
			(uint8_t*) byteBuffer,
			modFileData.songInfoSize, // memo
			20 + modFileData.instrumentsCount * modFileData.sampleInfoSize, // offset
			fileWholeOnce
			);

	if (loadStatus == fileTransferEnd)
	{

		memcpy(&modSong, &byteBuffer, modFileData.songInfoSize);

		for (uint8_t a = 0; a < 128; a++)
		{
			if (modSong.playlist[a] > modFileData.patternsMax)
			{
				modFileData.patternsMax = modSong.playlist[a];
			}
		}

		moveToNextOperationStep();
	}
	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}
}

void cFileManager::importModFile_Patterns()
{

	char byteBuffer[modFileData.patternSize];
//	uint8_t buffOffset = 0;

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
			modFileData.path,
			(uint8_t*) byteBuffer,
			modFileData.patternSize, // memo
			20 + modFileData.instrumentsCount * modFileData.sampleInfoSize + modFileData.songInfoSize + modFileData.patternSize * modFileData.patternsActualIndex, // offset
			fileWholeOnce
			);
	Sequencer::strPattern *patt = sequencer.getActualPattern();

	sequencer.clearPattern(patt);

	mtProject.values.actualPattern = modFileData.patternsActualIndex + 1;

	// przepisuję playlistę
	for (uint8_t a = 0; a < 128; a++)
	{
		mtProject.song.playlist[a] = modSong.playlist[a] + 1;
	}

	// mod narzuca length 64
	for (uint8_t a = 0; a < 8; a++)
	{
		patt->track[a].length = 63;
	}

	for (uint16_t cell = 0, step = 0; cell < 256; cell++)
	{

		// todo: kiedyś odwrócić te bajty
		uint32_t chData;
		uint8_t chan = cell % 4;

		*((uint8_t*) &chData + 3) = byteBuffer[cell * 4];
		*((uint8_t*) &chData + 2) = byteBuffer[cell * 4 + 1];
		*((uint8_t*) &chData + 1) = byteBuffer[cell * 4 + 2];
		*((uint8_t*) &chData + 0) = byteBuffer[cell * 4 + 3];

		uint8_t instrument = (chData >> 12) & 0x0f;

		uint16_t period = (chData >> 16) & 0xfff;
		int8_t note = periodToNote(period) - 5;

		uint8_t fx3 = chData & 0x00f;
		uint8_t fx2 = (chData & 0x0f0) >> 4;
		uint8_t fx1 = (chData & 0xf00) >> 8;

		Serial.printf("inst %d note %d, fx1 %d, fx2 %d, fx3 %d\t\t",
						instrument,
						note,
						fx1,
						fx2, fx3);

		if (step <= Sequencer::MAXSTEP)
		{
			if (note >= 0)
			{
				patt->track[chan].step[step].note = note;
				patt->track[chan].step[step].instrument = instrument - 1;

			}
			setFx(&patt->track[chan].step[step], fx1, fx2, fx3);
		}
		if (chan == 3)
		{
			step++;
			Serial.println();
		}

	}

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
		sequencer.saveToFileDone();
		if (modFileData.patternsActualIndex >= modFileData.patternsMax)
		{
			moveToNextOperationStep();
		}
		else
		{
			modFileData.patternsActualIndex++;
		}

	}
	else
	{
		// todo ogarnąć indexy err
		throwError(2);
	}
}

void setFx(Sequencer::strPattern::strTrack::strStep *step,
			uint8_t fx1,
			uint8_t fx2, uint8_t fx3)
{
	uint16_t modfxTempo;

	switch (fx1)
	{
	case 0:
		break;
	case 3:
		/*
		 Where [3][x][y] means "smoothly change the period of current sample
		 by x*16+y after each tick in the division, never sliding beyond
		 current period". Any note in this channel's division is not played,
		 but changes the "remembered" note - it can be thought of as a
		 parameter to this effect. Sliding to a note is similar to effects
		 [1] and [2], but the slide will not go beyond the given period, and
		 the direction is implied by that period. If x and y are both 0,
		 then the old slide will continue.
		 */
		step->fx[0].type = sequencer.fx.FX_TYPE_GLIDE;
		step->fx[0].value = constrain(
				fx2 * 16 + fx3,
				sequencer.getFxMin(sequencer.fx.FX_TYPE_GLIDE),
				sequencer.getFxMax(sequencer.fx.FX_TYPE_GLIDE));

		break;
	case 9:
		/*
		 * [9]: Set sample offset
		 Where [9][x][y] means "play the sample from offset x*4096 + y*256".
		 The offset is measured in words. If no sample is given, yet one is
		 still playing on this channel, it should be retriggered to the new
		 offset using the current volume..*/

		step->fx[0].type = sequencer.fx.FX_TYPE_MICROMOVE;
		step->fx[0].value = constrain(
				map(fx2 * 16 + fx3, 0, 256,
					sequencer.getFxMin(sequencer.fx.FX_TYPE_MICROMOVE),
					sequencer.getFxMax(sequencer.fx.FX_TYPE_MICROMOVE)),
				sequencer.getFxMin(sequencer.fx.FX_TYPE_MICROMOVE),
				sequencer.getFxMax(sequencer.fx.FX_TYPE_MICROMOVE));

		break;
	case 12:
		/*
		 [12]: Set volume
		 Where [12][x][y] means "set current sample's volume to x*16+y".
		 Legal volumes are 0..64.*/

		step->fx[0].type = sequencer.fx.FX_TYPE_VELOCITY;
		step->fx[0].value = constrain(
				map(fx2 * 16 + fx3, 0, 64,
					sequencer.getFxMin(sequencer.fx.FX_TYPE_VELOCITY),
					sequencer.getFxMax(sequencer.fx.FX_TYPE_VELOCITY)),
				sequencer.getFxMin(sequencer.fx.FX_TYPE_VELOCITY),
				sequencer.getFxMax(sequencer.fx.FX_TYPE_VELOCITY));

		break;
	case 13:
		/*
		 * break
		 */

		step->fx[0].type = sequencer.fx.FX_TYPE_BREAK_PATTERN;
		step->fx[0].value = sequencer.getFxDefault(
				sequencer.fx.FX_TYPE_BREAK_PATTERN);

		break;
	case 15:
		/*
		 * TEMPO
		 *  20-256*/
		modfxTempo = constrain(
								fx2 * 16 + fx3,
								sequencer.getFxMin(sequencer.fx.FX_TYPE_TEMPO),
								sequencer.getFxMax(sequencer.fx.FX_TYPE_TEMPO));

		if (modfxTempo > 32)
		{
			step->fx[0].type = sequencer.fx.FX_TYPE_TEMPO;
			step->fx[0].value = modfxTempo;
		}

		break;

	default:
		break;
	}

}

// pobieramy wave z mod
void cFileManager::importModFile_ImportWave()
{

//	mtProject.used_memory

	strInstrument *instr = &mtProject.instrument[modFileData.sampleActualIndex];

	//sdram_sampleBank
//	char byteBuffer[modFileData.modPatternSize];
	//	uint8_t buffOffset = 0;

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
			modFileData.path,
			(uint8_t*) modFile_sample_ptr,
			instr->sample.length * 2, // memo
			20 + modFileData.instrumentsCount * modFileData.sampleInfoSize + modFileData.songInfoSize + modFileData.patternSize * (modFileData.patternsMax + 1) + byteSampleOffset, // offset
			fileWholeOnce
			);

	instr->sample.address = modFile_sample_ptr;
	if (loadStatus == fileTransferEnd)
	{
		mtProject.used_memory += instr->sample.length * 2;
		if (modFileData.sampleActualIndex >= modFileData.instrumentsCount)
		{
			// 2 do przodu
			moveToNextOperationStep();
			moveToNextOperationStep();
		}
		else
		{

			moveToNextOperationStep();

		}
	}
	else
	{

	}

	//	instr->sample.address = modSampleData.sampleLengthInWords;

}

// zapisujemy do pliku
void cFileManager::importModFile_WriteWave()
{

	strInstrument *instr = &mtProject.instrument[modFileData.sampleActualIndex];

	if (!modFileData.waveWriteFlag)
	{
		modFileData.waveWriteFlag = 1;
		modFileData.modImport_saveLength = instr->sample.length * 2;
		if (modFileData.modImport_saveLength > 1)
		{

			char currentFilename[PATCH_SIZE];
			sprintf(currentFilename, cWorkspaceSamplesFilesFormat,
					modFileData.sampleActualIndex + 1);

			if (SD.exists(currentFilename)) SD.remove(currentFilename);
			wavfile = SD.open(currentFilename, SD_FILE_WRITE);

			// todo: to jest chyba bez sensu
			wavfile.write(currentFilename, sizeof(strWavFileHeader));

			modFileData.waveSrcPtr = instr->sample.address;
		}
		else
		{
			modFileData.waveWriteFlag = 0;
		}

	}

	if (modFileData.modImport_saveLength > 1)
	{
		if (modFileData.modImport_saveLength > 2048)
		{
			wavfile.write(modFileData.waveSrcPtr, 2048);

			modFileData.waveSrcPtr += 1024;
			modFileData.modImport_saveLength -= 2048;

		}
		else
		{
			modFileData.waveWriteFlag = 0;

			wavfile.write(modFileData.waveSrcPtr, modFileData.modImport_saveLength);
			modFileData.modImport_saveLength = 0;

			// uzupełnienie headera
			strWavFileHeader header;

			header.chunkId = 0x46464952; 							// "RIFF"
			header.chunkSize = instr->sample.length * 2 + 36;
			header.format = 0x45564157;								// "WAVE"
			header.subchunk1Id = 0x20746d66;						// "fmt "
			header.subchunk1Size = 16;
			header.AudioFormat = 1;
			header.numChannels = 1;
			header.sampleRate = 44100;
			header.bitsPerSample = 16;
			header.byteRate = header.sampleRate * header.numChannels * (header.bitsPerSample / 8);
			header.blockAlign = header.numChannels * (header.bitsPerSample / 8);
			header.subchunk2Id = 0x61746164;						// "data"
			header.subchunk2Size = instr->sample.length * 2;

			wavfile.seek(0);
			wavfile.write(&header, sizeof(header));
			wavfile.close();

			// tu warunek na zmianę kroków
			currentOperationStep--;

			byteSampleOffset += instr->sample.length * 2;
			modFile_sample_ptr += instr->sample.length;
			modFileData.sampleActualIndex++;
		}
	}
	else
	{
		currentOperationStep--;

		byteSampleOffset += instr->sample.length * 2;
		modFile_sample_ptr += instr->sample.length;
		modFileData.sampleActualIndex++;
	}

	// dokonczyc

	/*
	 * koniec zapisu
	 */

}
void cFileManager::importModFile_Finish()
{

	setProjectStructChanged();

	importModFileAfterNewProject = 0;

	status = fmLoadEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;

}

void cFileManager::importModFile_Error()
{
	importModFileAfterNewProject = 0;

}

int8_t cFileManager::periodToNote(uint16_t period)
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
		return -1;
	}
	return -1;
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
