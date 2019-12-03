

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
	//if(hidePressFlag) return 1;

	uint8_t status = fileManager.getLoadingStatus();

	if(openFromWorkspaceFlag)
	{
		if(status)
		{
			fileManager.refreshLoadProjectFromWorkspace();
			return 0;
		}
		else
		{
			char currentPatch[PATCH_SIZE];

			sprintf(currentPatch,"Projects/%s",mtConfig.startup.lastProjectName);
			if(SD.exists(currentPatch))
			{
				strcpy(fileManager.currentProjectName,mtConfig.startup.lastProjectName);
				sprintf(fileManager.currentProjectPatch,"Projects/%s",mtConfig.startup.lastProjectName);

				projectEditor.loadProjectValues();
			}
			else
			{
				strcpy(fileManager.currentProjectName, "New Project");
				PE->newProjectNotSavedFlag = 1;

				projectEditor.loadProjectValues();
			}
		}
	}
	else
	{
		if(status)
		{
			fileManager.refreshProjectOpening();
			return 0;
		}
	}

	if(startupTimer < 1000) // minimalny czas start screenu
	{
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
	char beta[2];
	if(mtConfig.firmware.beta) beta[0] = 'b';
	else 						beta[0] = 0;
	beta[1] = 0;
	sprintf(startScreenData.versionLabel, "v%d.%d.%d%s", mtConfig.firmware.ver_1, mtConfig.firmware.ver_2, mtConfig.firmware.ver_3, beta);

	display.setControlShow(startScreenControl);
	display.setControlValue(startScreenControl, startSampleLoadingProgress);
	display.setControlText(startScreenControl, "Opening last project...");
	display.refreshControl(startScreenControl);
}

void cInterface::hideStartScreen()
{
	display.setControlHide(startScreenControl);
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
	startupTimer = 0;

	if(mtConfig.startup.startMode == interfaceOpenLastProject)
	{
		char currentPatch[PATCH_SIZE];

		strcpy(currentPatch,"Workspace/project.bin");
		if(SD.exists(currentPatch))
		{
			if(fileManager.loadProjectFromWorkspaceStart())
			{
				openFromWorkspaceFlag = 1;
			}
		}

		if(!openFromWorkspaceFlag)
		{
			//strcpy(currentPatch,"Templates/New/project.bin");

			fileManager.createEmptyTemplateProject((char*)"New");

			fileManager.openProjectStart((char*)"New", projectTypeExample);

			PE->newProjectNotSavedFlag = 1;
			strcpy(fileManager.currentProjectName, "New Project");
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















