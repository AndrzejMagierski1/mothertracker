
#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "FastCRC.h"
#include "FastCRC_cpu.h"
#include "FastCRC_tables.h"

#include "mtSequencer.h"

#include "fileManager.h"




bool cFileManager::saveActualPattern(const char* path, uint8_t index)
{
	char patternToSave[PATCH_SIZE];
	sprintf(patternToSave, "%s/pattern_%02d.mtp", path, index);
	return writePatternFile(patternToSave, sequencer.getPatternToSaveToFile());
}

bool cFileManager::writePatternFile(char* filePath, uint8_t* sourcePattern)
{
	SD.remove(filePath);

	SdFile file;
	FastCRC32 crcCalc;

	sequencer.setPatternHeader((Sequencer::strPattern*)sourcePattern);

	((Sequencer::strPattern*) sourcePattern)->crc =
			crcCalc.crc32(sourcePattern,
							sizeof(Sequencer::strPattern) - sizeof(uint32_t));

	file.open(filePath, FILE_WRITE);
	file.write(sourcePattern, sizeof(Sequencer::strPattern));
	file.close();

	return true;
}
