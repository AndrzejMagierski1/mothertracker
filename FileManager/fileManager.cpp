

#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "mtStructs.h"
#include "SD.h"
#include "mtConfig.h"


#include "fileManager.h"


cFileManager newFileManager;


//====================================================================================
void cFileManager::update()
{
	if(currentOperation == fmNoOperation) return;

	switch(currentOperation)
	{
	case fmOpenWorkspaceProject:
	{
		updateOpenWorkspaceProject();
		break;
	}

	}







}

//====================================================================================
//==========================     UPDATEs     =========================================
//====================================================================================
void cFileManager::updateOpenWorkspaceProject()
{
	switch(currentOperationStep)
	{

	}



}




//====================================================================================
//====================================================================================
//====================================================================================
bool cFileManager::loadProjectFromWorkspace()
{
	if(currentOperation != fmNoOperation) return false;

	if(!SD.exists(cWorkspacePath)) return false;
	if(!SD.exists(cProjectFileNameInWorkspace)) return false;
	if(!readProjectFile(cProjectFileNameInWorkspace, &mtProject)) return false;

//	if(loadPattern(mtProject.values.actualPattern))
//	{
//		sequencer.switchRamPatternsNow();
//	}

//	memset(&loadFromWorkspaceHandle, 0, sizeof(save_load_handle_t));
//	moveToNextStage(&loadFromWorkspaceHandle);
//	loadingInProgress = 1;

	mtProject.used_memory = 0;

	currentOperationStep = 1;
	currentOperation = fmOpenWorkspaceProject;


	return true;
}

bool cFileManager::createNewProjectInWorkspace()
{
	createWorkspaceDirs();





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


void clearWorkspace()
{
	SD.removeDirWithFiles(cWorkspacePath);
}


