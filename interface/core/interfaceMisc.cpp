

#include "core/interface.h"

#include "projectEditor/projectEditor.h"

//#include "mtFileManager.h"
#include "fileManager.h"
#include "mtExporterWAV.h"
#include "songTimer.h"

#include <display.h>
#include "MTP.h"
#include "mtSleep.h"
#include "debugLog.h"


elapsedMillis screenRefreshTimer;

//extern cProjectEditor* PE;


//==================================================================================================
//============================= LADOWANIE PROJEKTU PRZY STARCE =====================================
//==================================================================================================
void cInterface::openStartupProject()
{
	if(!newFileManager.openProjectFromWorkspace())
	{
		openNewProject();
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

			openNewProject();
		}

		return 0;
	}

	if(startProjectLoadingProgress < 100) return 0;

	newFileManager.clearStatus();


	// taski na koniec otwierania projektu startowego
	fileManagerPopupEnabled = 1;

	if(mtpd.state == 0)
	{
		if(mtConfig.general.mtpState) mtpd.state = 1; // TODO wylaczone mtp
	}

	return 1;
}


void cInterface::openNewProject()
{
	newFileManager.createNewProjectInWorkspace();
	newFileManager.openProjectFromWorkspace();
}
//=====================================================================================
//============================== START SCREEN =========================================
//=====================================================================================
void cInterface::initStartScreen()
{
	if (mtConfig.firmware.beta)
	{
		sprintf(startScreenData.versionLabel,
				"v%d.%d.%d b%d",
				mtConfig.firmware.ver_1,
				mtConfig.firmware.ver_2,
				mtConfig.firmware.ver_3,
				mtConfig.firmware.beta);
	}
	else
	{
		sprintf(startScreenData.versionLabel,
				"v%d.%d.%d",
				mtConfig.firmware.ver_1,
				mtConfig.firmware.ver_2,
				mtConfig.firmware.ver_3);
	}

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
	if(screenRefreshTimer < 40) return;
	screenRefreshTimer = 0;

	startProjectLoadingProgress = newFileManager.getProgress();

	 // minimalny czas startu 2sek
	minStartTimeCounter++;
	if(startProjectLoadingProgress > minStartTimeCounter*2) startProjectLoadingProgress = minStartTimeCounter*2;

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

	interfaceGlobals.refreshFileExplorer = true;
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
	prop.style = controlStyleValue_0_100 | controlStyleBackground;
	prop.h = 100;
	prop.w = 420;


	if(turnOffProgressBar == nullptr)  turnOffProgressBar = display.createControl<cHorizontalBar>(&prop);

	refreshDisplayShutdown();
}

void cInterface::refreshDisplayShutdown()
{
	if(screenRefreshTimer < 30) return;
	screenRefreshTimer = 0;

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



// takie różne globalne rzeczy
void cInterface::commonThingsUpdate()
{

	if(songTimer.enabled) songTimer.refresh();

	if(fileManagerPopupEnabled)
	{
		uint8_t managerStatus = newFileManager.getStatus();

		if(managerStatus > fmIdle &&  managerStatus <  fmLoadEnd)
		{
			if(fileManagerPopupState == 0) // pokaz popup
			{
				int8_t text_index = -1;

				switch(managerStatus)
				{
				case fmBrowsingSamples           	: text_index = 1;	break;
				case fmBrowsingProjects          	: text_index = 2;	break;
				case fmBrowsingFirmwares         	: text_index = 3;	break;
				case fmImportingSamplesToWorkspace	: text_index = 4;	break;
				case fmCopyingInstrumentsInWorkspace: text_index = 5;	break;
				case fmDeleteingInstruments      	: text_index = 6;	break;
				case fmPreviewSampleFromSd       	: break;
				case fmLoadingProjectfromWorkspace	: text_index = 8;	break;
				case fmLoadingProjectFromProjects	: text_index = 9;	break;
				case fmSavingProjectToWorkspace  	: 	break;
				case fmSavingProjectToProjects   	: text_index = 11;	break;
				case fmLoadingPatternFromWorkspace	: 	break;
				case fmExportingSoundSong			: text_index = 13;	break;
				case fmExportingSoundSongStems		: text_index = 14;	break;
				case fmExportingSoundPattern		: text_index = 15;	break;
				case fmExportingSoundPatternStems	: text_index = 16;	break;
				case fmExportingSoundRenderSelection: text_index = 17;	break;
				case fmSavingRecordedSound			: text_index = 18;	break;
				case fmSavingImportingRecordedSound	: text_index = 19;	break;
				case fmImportingSampleFromSampleEditor:text_index = 20;	break;
                case fmImportingMod   				: text_index = 21;	break;
                case fmExportingMod  				: text_index = 22;	break;
                case fmReloadingSamples   			: text_index = 23;	break;
				default: text_index = 0; break;
				
				}


				if(text_index >= 0)
				{
					fileManagerPopupState = 1;
					fileManagerLastProgress = 0;
					noProgressChangeCounter = 0;
					mtPopups.showProgressPopup(&fileManagerPopupText[text_index][0]);
				}
			}
			else // przetwarzaj popup - progress
			{
				if(screenRefreshTimer > 50)
				{
					screenRefreshTimer = 0;

					processFileManagerPopupProgress(managerStatus);
				}
			}
		}
		else // zniknij popup
		{
			if(fileManagerPopupState == 1)
			{
				mtPopups.hideProgressPopup();
				fileManagerPopupState = 0;
				fileManagerLastProgress = 0;
				noProgressChangeCounter = 0;
			}
		}
	}
}



void cInterface::processFileManagerPopupProgress(uint8_t status)
{

	int8_t progress = -1;

	switch(status)
	{
	case fmBrowsingSamples           		: 		break;
	case fmBrowsingProjects          		: 		break;
	case fmBrowsingFirmwares         		: 		break;
	case fmImportingSamplesToWorkspace		: 		progress = newFileManager.getProgress();	break;
	case fmCopyingInstrumentsInWorkspace	: 		break;
	case fmDeleteingInstruments      		: 		break;
	case fmPreviewSampleFromSd       		:		break;
	case fmLoadingProjectfromWorkspace		:		break;
	case fmLoadingProjectFromProjects		: 		progress = newFileManager.getProgress();	break;
	case fmSavingProjectToWorkspace  		:		break;
	case fmSavingProjectToProjects   		: 		progress = newFileManager.getProgress();	break;
	case fmLoadingPatternFromWorkspace		:		break;
	case fmExportingSoundSong				:
	case fmExportingSoundSongStems			:
	case fmExportingSoundPattern			:
	case fmExportingSoundPatternStems		:
	case fmExportingSoundRenderSelection	: 		progress = constrain(exporter.getProgress(), 0 , 100); break;
	case fmSavingRecordedSound				: 		break;
	case fmSavingImportingRecordedSound		: 		break;
	case fmImportingSampleFromSampleEditor	:		break;
    case fmImportingMod   					: 		progress = newFileManager.importMod_getProgress(); break;
    case fmExportingMod   					: 		progress = newFileManager.exportItFile_getProgress(); break;
	default									: 		break;

	}


	// pchanie progressu kiedy nie zmienia sie przez dluzszy czas
	if(fileManagerLastProgress == progress && fileManagerLastProgress < 100)
	{
		noProgressChangeCounter++;

		if(noProgressChangeCounter > 5)
		{
			progress = fileManagerLastProgress+1;
		}
	}


	if(progress >= 0 && fileManagerLastProgress < progress)
	{
		noProgressChangeCounter = 0;
		fileManagerLastProgress = progress;
		mtPopups.changePopupProgress(progress);
	}


}



