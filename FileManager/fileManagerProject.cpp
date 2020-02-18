

#include "mtStructs.h"
#include "fileManagerDefs.h"

#include "SD.h"
#include "debugLog.h"

#include "FastCRC.h"
#include "FastCRC_cpu.h"
#include "FastCRC_tables.h"




#include "fileManager.h"




bool cFileManager::readProjectFile(const char * name, strMtProject * proj)
{
	if(!SD.exists(name)) return 0;
	SdFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC=0;

	strProjectFile projectFile;

	file=SD.open(name);
	file.read((uint8_t*)&projectFile, sizeof(projectFile));
	//file.read((uint8_t*)&pattBitmask, sizeof(strPatternsBitmask));
	file.close();

	if (projectFile.projectDataAndHeader.projectHeader.type != fileTypeProject)
		return false;

	checkCRC = crcCalc.crc32((uint8_t *) &projectFile.projectDataAndHeader,
								sizeof(projectFile.projectDataAndHeader));

	if (FILEMANAGER_DEBUGLOG)
	{
		char line[44];
		sprintf(line,
				"opened project fw.ver: %u.%u.%u, crc %s",
				projectFile.projectDataAndHeader.projectHeader.fwVersion[0],
				projectFile.projectDataAndHeader.projectHeader.fwVersion[1],
				projectFile.projectDataAndHeader.projectHeader.fwVersion[2],
				checkCRC == projectFile.crc ? "ok" : "err"
						);
		debugLog.addLine(line);
	}


	if(1)
	{
		memcpy(&proj->song, &projectFile.projectDataAndHeader.project.song, sizeof(strSong));
		memcpy(&proj->values, &projectFile.projectDataAndHeader.project.values, sizeof(strMtValues));
		//*proj=projectFile.projectDataAndHeader.project;
		//mtProject.values=projectFile.projectDataAndHeader.project.values;
		return false;
	}
	else return true;
}

