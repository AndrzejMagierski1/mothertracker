

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

	report(" crating new project ");


	strcpy(currentProjectName, cNewProjectName);

	getDefaultProject(&mtProject);
	//mtProject.patterns_count++;
	mtProject.values.projectNotSavedFlag = 2; // nie zapiany do folderu Projects
	mtProject.values.actualPattern = 1;

	//saveActualPattern(cWorkspacePatternsPath, mtProject.values.actualPattern);
	sequencer.loadFromFileERROR(); // czyszczenie sekwencji przed pierwszym zapisem
	changesFlags.pattern[mtProject.values.actualPattern] = 1;
	changesFlags.project = 1;
	savePatternToWorkspace();
	sequencer.saveToFileDone(); //xxx ?

	changesFlags.project = 1;
	saveProjectFileToWorkspace();

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



// opoznienie operacji o ~100 przebiegów petli glownej
void cFileManager::organizeClearWorkspace()
{

	if(organizeWorkspaceStep == 100) clearWorkspace();

	else if(organizeWorkspaceStep == 101)  createWorkspaceDirs();

	// progress
	else if(organizeWorkspaceStep == 102)  calcTotalMemoryToTransfer();

	organizeWorkspaceStep++;

	if(organizeWorkspaceStep == 103) moveToNextOperationStep();
}

