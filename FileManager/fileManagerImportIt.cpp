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



}
