

#include "core/interface.h"
#include "patternEditor/patternEditor.h"
#include "projectEditor/projectEditor.h"
#include "sampleImporter/sampleImporter.h"
#include "samplePlayback/samplePlayback.h"
#include "songEditor/songEditor.h"
#include "instrumentEditor/instrumentEditor.h"
#include "sampleEditor/sampleEditor.h"
#include "sampleRecorder/sampleRecorder.h"
#include "configEditor/configEditor.h"
#include "imageViewer/imageViewer.h"
#include "performanceMode/performanceMode.h"
#include "core/interfacePopups.h"
#include "game/game.h"
#include "masterParams/masterParams.h"
#include "core/songTimer.h"

#include "fileManager.h"
#include "mtTest.h"
#include "mtStructs.h"
#include "mtConfig.h"
#include "mtPadBoard.h"

#include "mtHardware.h"
#include "SD.h"
#include "sdram.h"
#include "sdCardDetect.h"
#include "mtSleep.h"
#include "debugLog.h"

cFunctionMachine uiFM(mtInterfacePotsDef::interfacePotsCount, mtInterfaceButtonsDef::interfaceButtonsCount, mtInterfacePadsDef::interfacePadsCount);
cInterface mtInterface(&uiFM);


strMtConfig mtConfig;
strMtProject mtProject;
strInterfaceGlobals interfaceGlobals;
strGrid mtGrid;

__NOINIT(EXTERNAL_RAM) int16_t sdram_soundMemory[SOUND_MEMORY_TOTAL/2];

int16_t* sdram_ptrSampleBank = sdram_soundMemory;
int16_t* sdram_ptrEffectsBank = sdram_soundMemory+(REC_EDIT_MEM_OFFEST/2);
int16_t* sdram_ptrDelayMemory = sdram_soundMemory+(SAMPLE_MEMORY_SIZE/2);

__NOINIT(EXTERNAL_RAM) uint8_t sdram_writeLoadBuffer[32640]; // 32768
//__NOINIT(EXTERNAL_RAM) uint8_t sdram_writeLoadBuffer2[32768];

__NOINIT(EXTERNAL_RAM) uint8_t sdram_mtpIndex[8192];
__NOINIT(EXTERNAL_RAM) uint16_t externalRamBufReverb[16384];

//__NOINIT(EXTERNAL_RAM) uint8_t undo_Bank[1024*1024];

extern void unmuteAudioOutByKey();
extern void muteAudioOutByKey();
extern uint8_t isAudioOutMuted();

//=======================================================================
//							SETAP INTERFEJSU
//=======================================================================
//=======================================================================

const uint8_t cInterface::modulesCount = 13;
const hModule cInterface::modules[modulesCount] =
{
		&projectEditor,     // 0
		&sampleImporter,    // 1
		&patternEditor,     // 2
		&samplePlayback,    // 3
		&songEditor,        // 4
		&instrumentEditor,  // 5
		&sampleEditor,      // 6
		&configEditor,      // 7
		&sampleRecorder,    // 8
		&performanceMode,	// 9
		&imageViewer,		// 10
		&gameModule,		// 11
		&masterParams,		// 12
};


const uint8_t cInterface::modulesButtonsCount = 11;
const uint32_t cInterface::modulesButtons[modulesButtonsCount][3] =
{
	{interfaceButtonPerformance,9, 0},
	{interfaceButtonMaster,  	12, 0},
	{interfaceButtonParams, 	5, mtInstEditModeParams},
	{interfaceButtonFile, 		0, 0},
	{interfaceButtonPattern, 	2, 0},
	{interfaceButtonSamplePlay, 3, 0},
	{interfaceButtonSampleEdit, 6, 0},
	{interfaceButtonSampleRec, 	8, 0},
	{interfaceButtonSampleLoad, 1, 0},
	{interfaceButtonSong, 		4, 0},
	{interfaceButtonConfig, 	7, 0},
};




//=======================================================================
//=======================================================================
//=======================================================================
//=======================================================================
void cInterface::begin()
{
	operatingMode = mtOperatingModeStartup;

	for(uint8_t i = 0; i<modulesCount; i++)
	{
		modules[i]->setEventFunct(interfaceEnvents);
		modules[i]->FM = &uiFM;
	}

	popupConfig.time = 2;
	popupConfig.w = 300;
	popupConfig.h = 70;
	popupConfig.x = 800/2-150;
	popupConfig.y = 480/2-50;
	popupConfig.lineColor[0] = 0xffffff;
	popupConfig.lineStyle[0] = controlStyleCenterX | controlStyleCenterY;
	mtPopups.config(4, &popupConfig);

	FM->unblockAllInputs();
}

//=======================================================================
//=======================================================================
//=======================================================================
void cInterface::update()
{
	processOperatingMode();

	for(uint8_t i = 0; i<modulesCount; i++)
	{
		if(modules[i]->moduleRefresh) modules[i]->update();
	}

	mtPopups.update();

	mtTest.testLoop();

	debugLog.update();

	commonThingsUpdate();
}


//=======================================================================
//=======================================================================
//=======================================================================

void cInterface::processOperatingMode()
{
	switch(operatingMode)
	{
		case mtOperatingModeStartup:
		{
			doStartTasks(); // odczyt configu, wersji, init controlek popupow, ekran startowy/braku karty
			operatingMode = mtOperatingModeOpenProject;
			break;
		}
		case mtOperatingModeOpenProject:
		{
			if(sdCardDetector.isCardInitialized())
			{
				hideDisplayNoSdCard();
				openStartupProject();
				operatingMode = mtOperatingModeShowStartScreen;
			}
			break;
		}
		case mtOperatingModeShowStartScreen:
		{
			processStartScreen();
			break;
		}
		case mtOperatingModePowerOffSequence:
		{
			processPowerOffSequence();
			break;
		}
		case mtOperatingModeSleep:
		{

			break;
		}
		case mtOperatingModeRun:
		{
			if (isAudioOutMuted())
			{
				unmuteAudioOutByKey();
			}
			eepromUpdate(false);
			break;
		}
		default: break; //mtOperatingModeRun
	}
}

void cInterface::doStartTasks()
{
	readConfig();

	// dzialania z uzyciem odczytanych wartosci z configu
	mtPadBoard.setPadNotes(	mtConfig.values.padBoardScale,
							mtConfig.values.padBoardNoteOffset,
							mtConfig.values.padBoardRootNote);

	////////////////////////////
	//readSdConfig();

	if (mtConfig.firmware.beta)
	{
		sprintf(interfaceGlobals.currentFirmwareVersion,
				firmwareVersionLabelFormatBeta,
				mtConfig.firmware.ver_1,
				mtConfig.firmware.ver_2,
				mtConfig.firmware.ver_3,
				mtConfig.firmware.beta);
	}
	else
	{
		sprintf(interfaceGlobals.currentFirmwareVersion,
				firmwareVersionLabelFormat,
				mtConfig.firmware.ver_1,
				mtConfig.firmware.ver_2,
				mtConfig.firmware.ver_3);
	}

	songTimer.initControls();
	mtPopups.initPopupsDisplayControls();

	initStartScreen();

	if(!sdCardDetector.isCardInitialized())
	{
		//hideStartScreen();
		showDisplayNoSdCard();
	}

}

void cInterface::processStartScreen()
{
	if(sdCardDetector.isCardInitialized())
	{
		refreshStartScreen();

		if(detectProjectLoadState())
		{
			destroyStartScreen();
			hideDisplayNoSdCard();
			operatingMode = mtOperatingModeRun;
			activateModule(&patternEditor, 0);
			unmuteAudioOutByKey();
		}
	}
	else
	{
		hideStartScreen();
		showDisplayNoSdCard();
		operatingMode = mtOperatingModeOpenProject;
	}
}


void cInterface::processPowerOffSequence()
{
	if(lowPower.getLowPowerState() == shutdownStateStart)
	{
		refreshDisplayShutdown();

		 if(shutdownSaveFlag == 0 && lowPower.getShutdownProgress() > 10 && lowPower.getShutdownProgress() < 70)
		 {
			if(newFileManager.saveProjectToWorkspace())
			{
				 shutdownSaveFlag = 1;
			}
		 }
		if (lowPower.getShutdownProgress() < 50 && !isAudioOutMuted())
		{
			muteAudioOutByKey();
		}

		if(lowPower.getTimeLeft() == 0 && newFileManager.getStatus() == fmIdle)
		{
			operatingMode = mtOperatingModeSleep;
			lowPower.goLowPower();
		}
	}
}

void cInterface::handleCardSlotAction(uint8_t state)
{
	if(operatingMode != mtOperatingModeRun) return;

	if(state)	activateInterface();
	else		deactivateInterfaceNoSd();
}

void cInterface::handlePowerButtonAction(uint8_t state)
{
	if(state) // press
	{
		if(lowPower.getLowPowerState() == shutdownStateSleep)
		{
			lowPower.wakeUp();
			return;
		}
		else if(operatingMode != mtOperatingModeRun) return;

		operatingMode = mtOperatingModePowerOffSequence;
		lowPower.startPowerOffSequence();

		// specjalne zachowanie w zaleznosci od stanu sd
		if(!sdCardDetector.isCardInserted())
		{
			hideDisplayNoSdCard();

			deactivateInterfaceShutdown();
			//showDisplayShutdown();
		}
		else
		{
			deactivateInterfaceShutdown();
		}
	}
	else // relase
	{
		if(operatingMode != mtOperatingModePowerOffSequence) return;

		shutdownSaveFlag = 0;
		lowPower.stopPowerOffSequence();
		operatingMode = mtOperatingModeRun;

		// specjalne zachowanie w zaleznosci od stanu sd
		if(sdCardDetector.isCardInserted())
		{
			activateInterface();
		}
		else
		{
			hideDisplayShutdown();
			showDisplayNoSdCard();
		}
	}
}
//=======================================================================
//=======================================================================
//=======================================================================
void cInterface::activateModule(hModule module, uint32_t options)
{
	uiFM.clearAllButtons();
	uiFM.clearAllPots();
	uiFM.clearAllPads();

	if(module != &patternEditor) songTimer.show();
	module->initDisplayControls();
	module->start(options);
	onScreenModule = module;
	lastOptions = options;
}


void cInterface::deactivateModule(hModule module)
{
	uiFM.clearAllButtons();
	uiFM.clearAllPots();
	uiFM.clearAllPads();

	module->stop();
	mtPopups.hideStepPopups();
	songTimer.hide();
	display.resetControlQueue();
	module->destroyDisplayControls();
	if(module == onScreenModule) onScreenModule = nullptr;
	previousModule = module;
	previousModuleOptions = lastOptions;
}

void cInterface::switchModuleByButton(hModule module, uint8_t button, uint8_t options)
{
	int8_t index = getButtonIndex(button);
	if(index < 0 ) return;
	if(modules[modulesButtons[index][1]] == module) return;


	uint8_t localOptions = options > 0 ? options : modulesButtons[index][2];

	deactivateModule(module);
	activateModule(modules[modulesButtons[index][1]], localOptions );
}

void cInterface::switchModuleToPrevious(hModule module, uint8_t options)
{
	if(previousModule == module) return;

	hModule prevModule = previousModule;
	uint32_t prevModuleOptions = previousModuleOptions;

	uint8_t localOptions = options > 0 ? options : prevModuleOptions;


	deactivateModule(module);
	activateModule(prevModule, localOptions);
}

int8_t cInterface::getButtonIndex(uint8_t button)
{
	for(uint8_t i = 0; i < modulesButtonsCount; i++)
	{
		if(button == modulesButtons[i][0])
		{
			return i;
		}
	}
	return -1;
}


void cInterface::toggleActiveModule(uint8_t state)
{
	if(state)
	{
		if(toggledState && onScreenModule == nullptr)
		{
			toggledState = 0;

			if(previousModule != nullptr)
			{
				hModule prevModule = previousModule;
				uint32_t prevModuleOptions = previousModuleOptions;

				activateModule(prevModule, prevModuleOptions);
				return;
			}
		}
	}
	else
	{
		if(onScreenModule != nullptr)
		{
			mtInterface.deactivateModule(onScreenModule);
			toggledState = 1;
		}
	}

}
//=======================================================================
//=======================================================================
//=======================================================================


//=======================================================================
//=======================================================================
//=======================================================================


void interfaceEnvents(uint8_t event, void* param1, void* param2, void* param3)
{
	switch(event)
	{
		case eventSwitchModule:
		{
			// param1 = uchwyt do modułu, param2 = index przycisku
			mtInterface.switchModuleByButton((hModule)param1, *((uint8_t*)param2), *((uint8_t*)param3));
			break;
		}
		case eventSwitchToPreviousModule:
		{
			// param1 = uchwyt do modułu, param2 = opcje startwe
			mtInterface.switchModuleToPrevious((hModule)param1, *((uint8_t*)param2));
			break;
		}
		case eventActivateGameModule:
		{
			mtInterface.deactivateModule((hModule)param1);
			mtInterface.activateModule(&gameModule,0);
			break;
		}
		case eventActivateImageViewer:
		{
			mtInterface.deactivateModule((hModule)param1);
			mtInterface.activateModule(&imageViewer, *((uint8_t*)param2));
			break;
		}
		case eventToggleActiveModule:
		{
			mtInterface.toggleActiveModule(*((uint8_t*)param3));
			break;
		}
		case eventActivateTestingProcedure:
		{
			mtInterface.deactivateModule((hModule)param1);
			mtTest.runTestingProcedure(mtInterface.FM, interfaceEnvents);
			break;
		}



	default: break;
	}

}


















