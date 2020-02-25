
#ifndef FILEMANAGER_FILEMANAGERDEFS_H_
#define FILEMANAGER_FILEMANAGERDEFS_H_

#include <stdint.h>
#include <cr_section_macros.h>


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



enum enFileManagerErrorSource
{
	errorSourceUnknow = 0,
	errorSourceProject,
	errorSourcePattern,
	errorSourceInstrument,
	errorSourceSample,

};


const char cNewProjectName[] = "New Project";


const char cWorkspacePath[] = "Workspace";
const char cProjectFileNameInWorkspace[] 	= "Workspace/project.mt";
const char cWorkspacePatternsPath[] 		= "Workspace/patterns";
const char cWorkspacePatternFileFormat[] 	= "Workspace/patterns/pattern_%02d.mtp";
const char cWorkspaceInstrumentsPath[] 		= "Workspace/instruments";
const char cWorkspaceInstrumentFileFormat[] = "Workspace/instruments/instrument_%02d.mti";
const char cWorkspaceSamplesPath[] 			= "Workspace/samples";
const char cWorkspaceSamplesFilesFormat[] 	= "Workspace/samples/instr%02d.wav";


const char cProjectsPath[] 					= "Projects";
const char cProjectsPathFormat[] 			= "Projects/%s";
const char cProjectFileInProjectsFormat[] 	= "Projects/%s/project.mt";
const char cProjectsPatternsPathFormat[] 	= "Projects/%s/patterns";
const char cProjectsPatternFileFormat[] 	= "Projects/%s/patterns/pattern_%02d.mtp";
const char cProjectsInstrumentsPathFormat[]	= "Projects/%s/instruments";
const char cProjectsInstrumentFileFormat[]	= "Projects/%s/instruments/instrument_%02d.mti";
const char cProjectsSamplesPathFormat[] 	= "Projects/%s/samples";
const char cProjectsSamplesFilesFormat[] 	= "Projects/%s/samples/instr%02d.wav";



const char cProjectFileName[] = "project.mt";







#endif /* FILEMANAGER_FILEMANAGERDEFS_H_ */
