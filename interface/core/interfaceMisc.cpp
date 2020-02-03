

#include "core/interface.h"

#include "projectEditor/projectEditor.h"

#include "mtFileManager.h"
#include <display.h>
#include "MTP.h"
#include "mtSleep.h"


elapsedMillis startScreenRefresh;
elapsedMillis shutdownScreenRefresh;

extern cProjectEditor* PE;


//==================================================================================================
//============================= LADOWANIE PROJEKTU PRZY STARCE =====================================
//==================================================================================================
void cInterface::openStartupProject()
{
	startupTimer = 0;

	if(mtConfig.startup.startMode == interfaceOpenLastProject)
	{
		char currentPatch[PATCH_SIZE];

		strcpy(currentPatch,"Workspace/project.mt");
		if(SD.exists(currentPatch))
		{
			if(fileManager.loadProjectFromWorkspaceStart())
			{
				openFromWorkspaceFlag = 1;
			}
		}

		if(!openFromWorkspaceFlag)
		{
			//strcpy(currentPatch,"Templates/New/project.mt");

			fileManager.createEmptyTemplateProject((char*)"New");

			fileManager.openProjectStart((char*)"New", projectTypeExample);

			PE->newProjectNotSavedFlag = 1;
			strcpy(fileManager.currentProjectName, "New Project");
		}
	}
}

uint8_t cInterface::detectProjectLoadState()
{
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
//
//	if(startupTimer < 1000) // minimalny czas start screenu
//	{
//		return 0;
//	}
//
	// na koniec wlacza mtp i opoznia start o 500 ms
	if(mtpd.state == 0)
	{
		if(mtConfig.general.mtpState) mtpd.state = 1;
		//startupTimer = 500;
	}

	return 1;
}

//=====================================================================================
//============================== START SCREEN =========================================
//=====================================================================================
void cInterface::initStartScreen()
{
	char beta[2];
	if(mtConfig.firmware.beta)	beta[0] = 'b';
	else 						beta[0] = 0;
	beta[1] = 0;

	sprintf(startScreenData.versionLabel, "v%d.%d.%d%s", mtConfig.firmware.ver_1, mtConfig.firmware.ver_2, mtConfig.firmware.ver_3, beta);

	strControlProperties prop;
	prop.x = 190;
	prop.y = 280;
	//prop.w = 800/4-10;
	prop.style = 0; //controlStyleShow;
	prop.h = 100;
	prop.w = 420;
	prop.data  = &startScreenData;
//	prop.value = 70;
//	prop.text = "loading...";
	if(startScreenControl == nullptr)  startScreenControl = display.createControl<cStartScreen>(&prop);

	display.setControlValue(startScreenControl, startSampleLoadingProgress);
	display.setControlText(startScreenControl, projectLoadText);
	display.refreshControl(startScreenControl);

	//uiFM.setButtonObj(interfaceButton7, buttonPress, functHide);
}

void cInterface::refreshStartScreen()
{
	if(startScreenRefresh < 100) return;
	startScreenRefresh = 0;

	display.setControlShow(startScreenControl);
	display.setControlValue(startScreenControl, startSampleLoadingProgress);
	display.setControlText(startScreenControl, projectLoadText);
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

//=====================================================================================
//========================== AKTYWACJA INTERFEJSU =====================================
//=====================================================================================
void cInterface::activateInterface()
{
	hideDisplayNoSdCard();
	hideDisplayShutdown();

	uint8_t state = 1;
	interfaceEnvents(eventToggleActiveModule,0,0,&state);

}

//=====================================================================================
//========================= DEAKTYWACJA INTERFEJSU ==================================
//=====================================================================================

void cInterface::deactivateInterfaceNoSd()
{
	uint8_t state = 0;
	interfaceEnvents(eventToggleActiveModule,0,0,&state);

	//display.turnOn();
	showDisplayNoSdCard();
}

void cInterface::deactivateInterfaceShutdown()
{
	uint8_t state = 0;
	interfaceEnvents(eventToggleActiveModule,0,0,&state);

	showDisplayShutdown();
}


//=====================================================================================
//========================= EKRAN BRAKU KARTY SD ======================================
//=====================================================================================
void cInterface::showDisplayNoSdCard()
{
	strControlProperties prop;
	prop.x = 400;
	prop.y = 240;
	prop.w = 240;
	prop.h = 25;
	prop.style = (controlStyleCenterY | controlStyleCenterX);
	if(noSdTextControl == nullptr)  noSdTextControl= display.createControl<cSimpleText>(&prop);

	display.setControlText(noSdTextControl, noSdText);
	display.setControlShow(noSdTextControl);
	display.refreshControl(noSdTextControl);
}

void cInterface::hideDisplayNoSdCard()
{
	display.destroyControl(noSdTextControl);
	noSdTextControl = nullptr;
}


//=====================================================================================
//============================ EKRAN WYLACZANIA  ======================================
//=====================================================================================

void cInterface::showDisplayShutdown()
{
	strControlProperties prop;

	prop.x = 190;
	prop.y = 170;
	prop.style = controlStyleValue_0_100;
	prop.h = 100;
	prop.w = 420;


	if(turnOffProgressBar == nullptr)  turnOffProgressBar = display.createControl<cHorizontalBar>(&prop);

	refreshDisplayShutdown();
}

void cInterface::refreshDisplayShutdown()
{
	if(shutdownScreenRefresh < 30) return;
	shutdownScreenRefresh = 0;

//	sprintf(turnOffText, "Shutdown in %.1fs", (lowPower.getTimeLeft()/1000.0));
	sprintf(turnOffText, "Shutting down");

	display.setControlValue(turnOffProgressBar, lowPower.getShutdownProgress());
	display.setControlText(turnOffProgressBar, turnOffText);
	display.setControlShow(turnOffProgressBar);
	display.refreshControl(turnOffProgressBar);
}

void cInterface::hideDisplayShutdown()
{
	display.destroyControl(turnOffProgressBar);
	turnOffProgressBar = nullptr;
}



