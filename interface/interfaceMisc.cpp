

#include <interface.h>

#include <projectEditor.h>

#include "mtFileManager.h"
#include <display.h>

elapsedMillis startScreenRefresh;

extern cProjectEditor* PE;

static uint8_t functHide();

uint8_t hidePressFlag = 0;

//==================================================================================================
//==================================================================================================
//==================================================================================================
uint8_t cInterface::detectStartState()
{
	if(startupTimer > MT_INTERFACE_STARTUP_TIME) return 1; // zabiezpieczenie czasowe

	if(hidePressFlag) return 1;



	startSampleLoadingFlag = fileManager.samplesLoader.getStateFlag();

	if(startSampleLoadingFlag == loaderStateTypeInProgress)
	{
		startSampleLoadingProgress = fileManager.samplesLoader.getCurrentProgress();

		return 0;
	}





	return 1;
}


//==================================================================================================
void cInterface::initStartScreen()
{
	char beta[2];

	if(mtConfig.firmware.beta) beta[0] = 'b';
	else 						beta[0] = 0;

	beta[1] = 0;


	sprintf(startScreenData.versionLabel, "v%d.%d.%d%s", mtConfig.firmware.ver_1, mtConfig.firmware.ver_2, mtConfig.firmware.ver_3, beta);


	strControlProperties prop;
	prop.x = 190;
	prop.y = 280;
	//prop.w = 800/4-10;
	prop.style = controlStyleShow;
	prop.h = 100;
	prop.w = 420;

	prop.data  = &startScreenData;
//	prop.value = 70;
//	prop.text = "loading...";
	if(startScreenControl == nullptr)  startScreenControl = display.createControl<cStartScreen>(&prop);


	uiFM.setButtonObj(interfaceButton7, buttonPress, functHide);


}

void cInterface::showStartScreen()
{
	if(startScreenRefresh < 100) return;

	startScreenRefresh = 0;

	display.setControlValue(startScreenControl, startSampleLoadingProgress);
	display.setControlText(startScreenControl, "Opening last project...");
	display.refreshControl(startScreenControl);
}

void cInterface::destroyStartScreen()
{
	display.destroyControl(startScreenControl);
	startScreenControl = nullptr;
}


//==================================================================================================
void cInterface::openStartupProject()
{
	if(mtConfig.startup.startMode == interfaceOpenLastProject)
	{
		char currentPatch[PATCH_SIZE];


		strcpy(currentPatch,"Workspace/project.bin");
		if(SD.exists(currentPatch))
		{
			if(fileManager.loadProjectFromWorkspace())
			{
				projectEditor.loadProjectValues();
				sprintf(currentPatch,"Projects/%s",mtConfig.startup.lastProjectName);
				if(SD.exists(currentPatch))
				{
					strcpy(fileManager.currentProjectName,mtConfig.startup.lastProjectName);
					sprintf(fileManager.currentProjectPatch,"Projects/%s",mtConfig.startup.lastProjectName);
				}
				else
				{
					PE->newProjectNotSavedFlag = 1;
					memset(fileManager.currentProjectPatch,0,PATCH_SIZE);
					memset(fileManager.currentProjectName,0,PROJECT_NAME_SIZE);
				}

			}
			else
			{
				strcpy(currentPatch,"Templates/New/project.bin");

				if(!SD.exists(currentPatch)) fileManager.createEmptyTemplateProject((char*)"New");

				fileManager.openProject((char*)"New",projectTypeExample); // można to odpalić bez zadnych flag i progresow bo nowy projekt nie ma sampli

				PE->newProjectNotSavedFlag = 1;
				memset(fileManager.currentProjectPatch,0,PATCH_SIZE);
				memset(fileManager.currentProjectName,0,PROJECT_NAME_SIZE);
			}
		}
		else
		{
			strcpy(currentPatch,"Templates/New/project.bin");

			if(!SD.exists(currentPatch)) fileManager.createEmptyTemplateProject((char*)"New");

			fileManager.openProject((char*)"New",projectTypeExample); // można to odpalić bez zadnych flag i progresow bo nowy projekt nie ma sampli

			PE->newProjectNotSavedFlag = 1;
			memset(fileManager.currentProjectPatch,0,PATCH_SIZE);
			memset(fileManager.currentProjectName,0,PROJECT_NAME_SIZE);
		}
	}




}


static uint8_t functHide()
{
	hidePressFlag = 1;
	return 0;
}


//=======================================================================
//=======================================================================
//=======================================================================















