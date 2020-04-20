#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "mtSequencer.h"

#include "Arduino.h"

#include "fileTransfer.h"
#include "fileManager.h"
/*
 * https://github.com/schismtracker/schismtracker/wiki/ITTECH.TXT
 */

extern int16_t *sdram_ptrSampleBank;

char file[255];

uint16_t expOrdNum = 0;
uint16_t expInst = 0;
uint16_t expSmp = 0;
uint16_t expWave = 0;

SdFile exportedFile;
// wystartuj operację

/*
 * export wave
 */
enum enWaveWriteStat
{
	waveWrite_idle,
	waveWrite_inProgress,
	waveWrite_finished,

};

uint32_t waveTotalBytes = 0;
uint32_t waveBytesLeft = 0;
uint8_t waveWriteStatus = waveWrite_idle;

uint8_t *waveSourceDataPtr = (uint8_t*) sdram_ptrSampleBank;

uint8_t cFileManager::exportItFile()
{
	if (status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if (currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject)
		return false;

	currentOperationStep = 0;
	status = fmExportingMod;
	currentOperation = fmExportModFile;

	return true;
}

void cFileManager::exportItFile_Init()
{
	Serial.println("exportItFile_Init");

	waveTotalBytes = 0;
	waveBytesLeft = 0;
	waveWriteStatus = waveWrite_idle;
	waveSourceDataPtr = (uint8_t*) sdram_ptrSampleBank;

	expOrdNum = 0;
	expInst = 0;
	expSmp = 0;
	expWave = 0;

	if (!SD.exists("Export")) SD.mkdir(0, "Export");

	sprintf(file,
			cExportModFilePathFormat,
			newFileManager.getCurrentProjectName());

	if (SD.exists(file))
	{
		SD.remove(file);
	}

	if (exportedFile.open(file, FILE_WRITE))
	{
		moveToNextOperationStep();
	}
	else
	{
		exportItFile_Error();
	}
}

// wpisz little endian
uint8_t* writeLE(uint8_t *dest, uint32_t val, uint8_t size)
{

	switch (size)
	{
	case 1:
		*dest = val;
		break;

	case 2:
		*((uint16_t*) dest) = val;
		break;

	case 4:
		*((uint32_t*) dest) = val;
		break;

	default:
		for (int8_t a = size - 1; a >= 0; a--)
		{
			*(dest + a) = (val >> (a * 8)) & 0xff;
		}
		break;

	}

	return dest + size;

}
void cFileManager::exportItFile_InitHeader()
{
	Serial.println("exportItFile_InitHeader");

	uint8_t buff32[0x20] { 0 };

	// 0x00
	sprintf((char*) buff32, "IMPM%.26s",
			newFileManager.getCurrentProjectName());
	exportedFile.write(buff32, sizeof(buff32));

	// 0x20
	memset(buff32, 0, sizeof(buff32));

	expOrdNum = 200;
	uint16_t InsNum = INSTRUMENTS_COUNT;
	uint16_t SmpNum = INSTRUMENTS_COUNT;
	uint16_t PatNum = 199;
	uint16_t Cwt = 0x0217;
	uint16_t Cmwt = 0x200;
	uint16_t Flags =
			(1 << 2) | //Bit 2: On = Use instruments, Off = Use samples.
			(1 << 3); //Bit 3: On = Linear slides, Off = Amiga slides.

	uint16_t Special = 0b00000000;

	uint16_t GV = 128;
	uint16_t MV = 128;
	uint16_t IS = 6;
	uint16_t IT = mtProject.values.globalTempo;
	uint16_t Sep = 0;
	uint16_t PWD = 0;
	uint16_t MsgLgth = 0;
	uint16_t MsgOffset = 0;

	uint8_t *ptr = &buff32[0];
	ptr = writeLE(ptr, expOrdNum, 2);
	ptr = writeLE(ptr, InsNum, 2);
	ptr = writeLE(ptr, SmpNum, 2);
	ptr = writeLE(ptr, PatNum, 2);
	ptr = writeLE(ptr, Cwt, 2);
	ptr = writeLE(ptr, Cmwt, 2);
	ptr = writeLE(ptr, Flags, 2);
	ptr = writeLE(ptr, Special, 2);

	ptr = writeLE(ptr, GV, 2);
	ptr = writeLE(ptr, MV, 2);
	ptr = writeLE(ptr, IS, 2);
	ptr = writeLE(ptr, IT, 2);
	ptr = writeLE(ptr, Sep, 2);
	ptr = writeLE(ptr, PWD, 2);
	ptr = writeLE(ptr, MsgLgth, 2);
	ptr = writeLE(ptr, MsgOffset, 2);

	exportedFile.write(buff32, sizeof(buff32));

	// 0x40  Chnl Pan (64 bytes)
	// 0 (absolute left) to 64 (absolute right). 32 = central pan
	uint8_t buff64[0x40] { 0 };
	for (uint8_t a = 0; a < 64; a++)
	{
		buff64[a] = 32;
	}
	exportedFile.write(buff64, sizeof(buff64));

	// 0x40  Chnl Vol (64 bytes)
	// 0->64
	memset(buff64, 0, sizeof(buff64));

	for (uint16_t a = 0; a < 64; a++)
	{
		buff64[a] = 64;
	}
	exportedFile.write(buff64, sizeof(buff64));

	// 0x60  orders
	for (uint16_t a = 0; a < expOrdNum; a++)
	{
		exportedFile.write(0);
	}

	// instruments
	for (uint16_t a = 0; a < InsNum * 4; a++)
	{
		exportedFile.write(0);
	}

	// samples
	for (uint16_t a = 0; a < SmpNum * 4; a++)
	{
		exportedFile.write(0);
	}

	// patterns
	for (uint16_t a = 0; a < PatNum * 4; a++)
	{
		exportedFile.write(0);
	}

	moveToNextOperationStep();
}

void cFileManager::exportItFile_storeInstrumentOffset()
{
	uint32_t endOfFile = exportedFile.size();
	exportedFile.seek(0x00C0 + expOrdNum + expInst * 4);
	uint8_t buff[4];
	writeLE(buff, endOfFile, 4);
	exportedFile.write(buff, 4);

	exportedFile.seek(endOfFile);
}
void cFileManager::exportItFile_storeSampleOffset()
{
	uint32_t endOfFile = exportedFile.size();
	exportedFile.seek(0x00C0 + expOrdNum + INSTRUMENTS_COUNT * 4 + expSmp * 4);
	uint8_t buff[4];
	writeLE(buff, endOfFile, 4);
	exportedFile.write(buff, 4);

	exportedFile.seek(endOfFile);
}
void cFileManager::exportItFile_storeWaveOffset()
{
	uint32_t endOfFile = exportedFile.size();
	// get sample offset

	uint32_t offSetForSampleOffset = 0x00C0 + expOrdNum + INSTRUMENTS_COUNT * 4 + expWave * 4;

	exportedFile.seek(offSetForSampleOffset);

	//	czytamy offset pliku sampla
	uint32_t sampleFileOffset;
	exportedFile.read((uint8_t*) &sampleFileOffset, 4);

	// wpisujemy offset wave do pliku sampla, zmienna SamplePointer z docsa
	exportedFile.seek(sampleFileOffset + 0x48);
	uint8_t buff[4];
	writeLE(buff, endOfFile, 4);
	exportedFile.write(buff, 4);

	// wracamy na koniec
	exportedFile.seek(endOfFile);
}

void cFileManager::exportItFile_ProcessInstruments()
{

	strInstrument *instr = &mtProject.instrument[expInst];
	exportItFile_storeInstrumentOffset();

	uint8_t buff0x20[0x20] { 0 };

	// 0x00
	sprintf((char*) buff0x20, "IMPIInstr%.3d.mti",
			expInst + 1);

	uint8_t *ptr = &buff0x20[0x10];
	ptr = writeLE(ptr, 0, 1);	//00h
	ptr = writeLE(ptr, 2, 1);	//NNA
	ptr = writeLE(ptr, 3, 1);	//DCT
	ptr = writeLE(ptr, 0, 1);	//DCA
	ptr = writeLE(ptr, 0, 2);	//FadeOut
	ptr = writeLE(ptr, 0, 1);	//PPS
	ptr = writeLE(ptr, 0, 1);//        PPC: Pitch-Pan center: C-0 to B-9 represented as 0->119 inclusive
	ptr = writeLE(ptr, 128, 1);	//     GbV: Global Volume, 0->128
	ptr = writeLE(ptr, 0, 1);//        DfP: Default Pan, 0->64, &128 => Don't use
	ptr = writeLE(ptr, 0, 1);//        RV: Random volume variation (percentage)
	ptr = writeLE(ptr, 0, 1);//        RP: Random panning variation (panning change - not implemented yet)
	ptr = writeLE(ptr, 0, 2);	//TrkVers
	ptr = writeLE(ptr, 1, 1);	//NoS
	ptr = writeLE(ptr, 0, 1);	//x

	exportedFile.write(buff0x20, sizeof(buff0x20));

	//0020: │ Instrument Name, max 26 bytes, includes NUL...................│
	memset(buff0x20, 0, sizeof(buff0x20));

	sprintf((char*) buff0x20, "%.25s",
			instr->sample.file_name);

	ptr = &buff0x20[0x1a];
	ptr = writeLE(ptr, 0, 1);	//IFC = Initial Filter cutoff
	ptr = writeLE(ptr, 0, 1);	//IFR = Initial Filter resonance
	ptr = writeLE(ptr, 0, 1);	//MCh = MIDI Channel
	ptr = writeLE(ptr, 0, 1);	//MPr = MIDI Program (Instrument)
	ptr = writeLE(ptr, 0, 2);	//MIDIBnk ???

	exportedFile.write(buff0x20, sizeof(buff0x20));

	// Note-Sample/Keyboard Table, Length = 240 bytes
	for (uint16_t a = 0; a < 120; a++)
	{
		exportedFile.write(a);
		exportedFile.write(expInst + 1);
	}

	// pseudoenvelope
	memset(buff0x20, 0, sizeof(buff0x20));
	for (uint16_t a = 0; a < 3; a++)
	{
		exportedFile.write(buff0x20, 0x10);
	}

	/*
	 *  ENVELOPES todo
	 */

	expInst++;
	if (expInst >= INSTRUMENTS_COUNT)
	{
		moveToNextOperationStep();
	}
}

void cFileManager::exportItFile_ProcessSamples()
{
	strInstrument *instr = &mtProject.instrument[expSmp];
	exportItFile_storeSampleOffset();

	uint8_t buff0x50[0x50] { 0 };

	// 0x00
	sprintf((char*) buff0x50, "IMPSInstr%.3d.mts",
			expSmp + 1);

	bool isLoop = instr->playMode == loopPingPong ||
			instr->playMode == loopForward ||
			instr->playMode == loopBackward;
	bool isLoopPingPong = instr->playMode == loopPingPong;

	uint8_t instrVolume = map(instr->volume, 0, 100, 0, 64);

	uint8_t Flg =
			(0 << 0) |	// Bit 0. On = sample associated with header.
			(1 << 1) |	// Bit 1. On = 16 bit, Off = 8 bit.
			(0 << 2) |	// Bit 2. On = stereo, Off = mono. Stereo samples not supported yet
			(0 << 3) |	// Bit 3. On = compressed samples.
			(isLoop << 4) |	// Bit 4. On = Use loop
			(isLoop << 5) | // Bit 5. On = Use sustain loop
			(isLoopPingPong << 6) | //Bit 6. On = Ping Pong loop, Off = Forwards loop
			(isLoopPingPong << 7); // Bit 7. On = Ping Pong Sustain loop, Off = Forwards Sustain loop

	uint8_t *ptr = &buff0x50[0x10];
	ptr = writeLE(ptr, 0, 1);	//00h
	ptr = writeLE(ptr, instrVolume, 1);	//GvL:      Global volume for instrument, ranges from 0->64
	ptr = writeLE(ptr, Flg, 1);	//Flg
	ptr = writeLE(ptr, instrVolume, 1);	//Vol

	sprintf((char*) ptr, "%.25s",
			instr->sample.file_name);

	ptr = &buff0x50[0x30];
	ptr = writeLE(ptr, instr->sample.length, 4);	//	Length
	ptr = writeLE(ptr,
					map((float) instr->loopPoint1, 0, MAX_16BIT, 0,
						instr->sample.length),
					4);										//	Loop Begin
	ptr = writeLE(ptr,
					map((float) instr->loopPoint2, 0, MAX_16BIT, 0,
						instr->sample.length),
					4);										//	Loop End
	ptr = writeLE(ptr, 44100, 4);	//	C5Speed

	ptr = writeLE(ptr,
					map((float) instr->loopPoint1, 0, MAX_16BIT, 0,
						instr->sample.length),
					4);										//	SusLoop Begin
	ptr = writeLE(ptr,
					map((float) instr->loopPoint2, 0, MAX_16BIT, 0,
						instr->sample.length),
					4);										//	SusLoop End

	ptr = writeLE(ptr, 0, 4);	//	SamplePointer - todo tymczasowe zero
	ptr = writeLE(ptr, 0, 1);	//	ViS:      Vibrato Speed, ranges from 0->64
	ptr = writeLE(ptr, 0, 1);	//	ViD:      Vibrato Depth, ranges from 0->64
	ptr = writeLE(ptr, 0, 1);//	ViR:      Vibrato Rate, rate at which vibrato is applied (0->64)
	ptr = writeLE(ptr, 0, 1);	//	ViT:      Vibrato waveform type.

	exportedFile.write(buff0x50, sizeof(buff0x50));

	expSmp++;
	if (expSmp >= INSTRUMENTS_COUNT)
	{
		moveToNextOperationStep();
	}

}

void cFileManager::exportItFile_ProcessWaves()
{
	strInstrument *instr = &mtProject.instrument[expWave];

	if (waveWriteStatus == waveWrite_idle)
	{
		exportItFile_storeWaveOffset();

		waveTotalBytes = instr->sample.length * 2;
		waveBytesLeft = waveTotalBytes;
		waveWriteStatus = waveWrite_inProgress;
	}

	if (waveWriteStatus == waveWrite_inProgress)
	{
		uint32_t bytesToWrite = waveBytesLeft;
		if (bytesToWrite > 512) bytesToWrite = 512;

		exportedFile.write(waveSourceDataPtr, bytesToWrite);

		waveSourceDataPtr += bytesToWrite;	//przesuwam wskaznik
		waveBytesLeft -= bytesToWrite; // zmniejszam ikosc danych do kopiowania

		if (waveBytesLeft <= 0)
		{
			waveWriteStatus = waveWrite_finished;
		}
	}
	if (waveWriteStatus == waveWrite_finished)
	{
		waveWriteStatus = waveWrite_idle;

		expWave++;
		if (expWave >= INSTRUMENTS_COUNT)
		{
			moveToNextOperationStep();
		}
	}

}

void cFileManager::exportItFile_Finish()
{
	Serial.println("exportItFile_Finish");
	if (exportedFile.isOpen())
	{
		exportedFile.close();
	}

	moveToNextOperationStep();
}
void cFileManager::exportItFile_Error()
{
	Serial.println("exportItFile_Error");
	throwError(1);
	if (exportedFile.isOpen())
	{
		exportedFile.close();
	}

}
