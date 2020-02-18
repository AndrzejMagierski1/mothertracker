
#ifndef FILEMANAGER_FILEMANAGERDEFS_H_
#define FILEMANAGER_FILEMANAGERDEFS_H_

#include <stdint.h>



struct strProjectFileHeader
{
	char id_file[2]; 				// typ pliku, np MT = plik projektu, TI = instrument
	uint16_t type;					// typ w liczbie
	char fwVersion[4]; 				// wersja fw w ktorej zapisano plik
	char fileStructureVersion[4];	// wersja struktury pliku
	uint16_t size;					// rozmiar
};

struct strInstrumentFile
{
	struct strInstrumentDataAndHeader
	{
		strProjectFileHeader instrHeader;
		strInstrument instrument;

	} instrumentDataAndHeader;

	uint32_t crc;
};

struct strProjectFile
{
	struct strProjectDataAndHeader
	{
		strProjectFileHeader projectHeader;
		strMtProjectRemote project;
	} projectDataAndHeader;

	uint32_t crc;
};







const char cWorkspacePath[] = "Workspace";
const char cWorkspaceSamplesPath[] = "Workspace/samples";
const char cWorkspacePatternsPath[] = "Workspace/patterns";
const char cWorkspaceInstrumentsPath[] = "Workspace/instruments";

const char cProjectFileName[] = "project.mt";
const char cProjectFileNameInWorkspace[] = "Workspace/project.mt";






#endif /* FILEMANAGER_FILEMANAGERDEFS_H_ */
