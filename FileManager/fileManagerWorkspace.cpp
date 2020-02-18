

#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "mtSequencer.h"

#include "FastCRC.h"
#include "FastCRC_cpu.h"
#include "FastCRC_tables.h"



#include "fileManager.h"




bool cFileManager::createNewProjectInWorkspace()
{
	clearWorkspace();
	createWorkspaceDirs();


	getDefaultProject(&mtProject);
	mtProject.patterns_count++;
	mtProject.values.actualPattern = 1;
	saveActualPattern(cWorkspacePatternsPath, mtProject.values.actualPattern);
	sequencer.saveToFileDone(); //xxx ?

	writeProjectFile(cProjectFileNameInWorkspace, &mtProject);

	//todo jakas flaga ze nie zapisany?

	return true;
}


bool cFileManager::createWorkspaceDirs()
{
	if(!SD.exists(cWorkspacePath))
	{
		SD.mkdir(1, cWorkspacePath);
	}

	if(!SD.exists(cWorkspaceInstrumentsPath))
	{
		SD.mkdir(1, cWorkspaceInstrumentsPath);
	}

	if(!SD.exists(cWorkspacePatternsPath))
	{
		SD.mkdir(1, cWorkspacePatternsPath);
	}

	if(!SD.exists(cWorkspaceSamplesPath))
	{
		SD.mkdir(1, cWorkspaceSamplesPath);
	}

	return true;
}


void cFileManager::clearWorkspace()
{
	SD.removeDirWithFiles(cWorkspacePath);
}



