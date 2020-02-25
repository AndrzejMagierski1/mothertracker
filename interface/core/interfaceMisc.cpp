

#include "core/interface.h"

#include "projectEditor/projectEditor.h"

#include "mtFileManager.h"
#include "fileManager.h"

#include <display.h>
#include "MTP.h"
#include "mtSleep.h"
#include "debugLog.h"


elapsedMillis startScreenRefresh;
elapsedMillis shutdownScreenRefresh;

extern cProjectEditor* PE;


//==================================================================================================
//============================= LADOWANIE PROJEKTU PRZY STARCE =====================================
//==================================================================================================
void cInterface::openStartupProject()
{
	if(!newFileManager.openProjectFromWorkspace())
	{
		newFileManager.createNewProjectInWorkspace();
		newFileManager.openProjectFromWorkspace();
	}

}

uint8_t cInterface::detectProjectLoadState()
{

	uint8_t startStatus = newFileManager.getStatus();

	if(startStatus != fmLoadEnd)
	{
		if(startStatus >= fmError)
		{
			newFileManager.clearStatus();

			newFileManager.createNewProjectInWorkspace();
			newFileManager.openProjectFromWorkspace();
		}

		return 0;
	}

	if(startProjectLoadingProgress < 100) return 0;

	newFileManager.clearStatus();

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

	minStartTimeCounter = 0;
	startProjectLoadingProgress = 0;
	display.setControlValue(startScreenControl, startProjectLoadingProgress);
	display.setControlText(startScreenControl, projectLoadText);
	display.refreshControl(startScreenControl);

	//uiFM.setButtonObj(interfaceButton7, buttonPress, functHide);
}

void cInterface::refreshStartScreen()
{
	if(startScreenRefresh < 100) return;
	startScreenRefresh = 0;

	startProjectLoadingProgress = newFileManager.getProgress();
	//debugLog.setMaxLineCount(1);
	debugLog.addLine("Load progress: ");
	debugLog.addValue(startProjectLoadingProgress);
	minStartTimeCounter++;
	 // minimalny czas startu 2sek
	if(startProjectLoadingProgress > minStartTimeCounter*5) startProjectLoadingProgress = minStartTimeCounter*5;

	display.setControlShow(startScreenControl);
	display.setControlValue(startScreenControl, startProjectLoadingProgress);
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



