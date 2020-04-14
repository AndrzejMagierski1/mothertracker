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
char file[255];

uint32_t headerSize = 0;

uint16_t expOrdNum = 0;

SdFile exportedFile;
// wystartuj operację
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

	expOrdNum = 255;
	uint16_t InsNum = INSTRUMENTS_COUNT;
	uint16_t SmpNum = INSTRUMENTS_COUNT;
	uint16_t PatNum = 255;
	uint16_t Cwt = 0x200;
	uint16_t Cmwt = 0x200;
	uint16_t Flags = 0b00000000;
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

	headerSize = exportedFile.size();
	Serial.printf("headerSize: %d\n", headerSize);

	moveToNextOperationStep();
}

uint16_t expInst = 0;
uint32_t fileOffset = 0;

void cFileManager::exportItFile_setInstrumentOffset()
{
	uint32_t endOfFile = exportedFile.size();
	exportedFile.seek(0x00C0 + expOrdNum + expInst * 4);
	uint8_t buff[4];
	writeLE(buff, endOfFile, 4);
	exportedFile.write(buff, 4);

	exportedFile.seek(endOfFile);

}

void cFileManager::exportItFile_ProcessInstruments()
{

	strInstrument *instr = &mtProject.instrument[expInst];
	uint32_t instrumentOffset = exportedFile.size() - 1;
	exportItFile_setInstrumentOffset();



	uint8_t buff0x20[0x20] { 0 };

	// 0x00
	sprintf((char*) buff0x20, "IMPIInstr%.3d.mti",
			expInst + 1);

	uint8_t *ptr = &buff0x20[0x10];
	ptr = writeLE(ptr, 0, 1);	//00h
	ptr = writeLE(ptr, 2, 1);	//NNA
	ptr = writeLE(ptr, 0, 1);	//DCT
	ptr = writeLE(ptr, 0, 1);	//DCA
	ptr = writeLE(ptr, 0, 2);	//FadeOut
	ptr = writeLE(ptr, 0, 1);	//PPS
	ptr = writeLE(ptr, 0, 1);//        PPC: Pitch-Pan center: C-0 to B-9 represented as 0->119 inclusive
	ptr = writeLE(ptr, 128, 1);	//        GbV: Global Volume, 0->128
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
