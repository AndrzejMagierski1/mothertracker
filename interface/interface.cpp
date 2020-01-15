

#include "interface.h"
#include "patternEditor.h"
#include "projectEditor.h"
#include "sampleImporter.h"
#include "samplePlayback.h"
#include "songEditor.h"
#include "instrumentEditor.h"
#include "sampleEditor.h"
#include "sampleRecorder.h"
#include "configEditor.h"
#include "imageViewer.h"

#include "game.h"
#include "performanceMode.h"

#include "mtFileManager.h"

#include "mtTest.h"

#include "mtStructs.h"

#include "interfacePopups.h"


#include "mtConfig.h"
#include "mtHardware.h"
#include "SD.h"
#include "sdram.h"
#include "sdCardDetect.h"
//#include "mtCardChecker.h"
#include "mtSleep.h"

#include "debugLog.h"


cInterface mtInterface;


strMtConfig mtConfig;
strMtProject mtProject;
strInterfaceGlobals interfaceGlobals;


__NOINIT(EXTERNAL_RAM) int16_t sdram_sampleBank[SAMPLE_MEMORY_MAX/2];
__NOINIT(EXTERNAL_RAM) int16_t sdram_effectsBank[SAMPLE_MEMORY_MAX/2];
__NOINIT(EXTERNAL_RAM) uint8_t sdram_writeLoadBuffer[32768];

__NOINIT(EXTERNAL_RAM) uint8_t sdram_mtpIndex[8192];

//__NOINIT(EXTERNAL_RAM) uint8_t undo_Bank[1024*1024];

//=======================================================================
//							SETAP INTERFEJSU
//=======================================================================
//=======================================================================

const uint8_t cInterface::modulesCount = 12;
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
};


const uint8_t cInterface::modulesButtonsCount = 11;
const uint32_t cInterface::modulesButtons[modulesButtonsCount][3] =
{
	{interfaceButtonPerformance,9, 0},
	{interfaceButtonMaster,  	7, mtConfigModeMaster},
	{interfaceButtonParams, 	5, mtInstEditModeParams},
	{interfaceButtonFile, 		0, 0},
	{interfaceButtonPattern, 	2, 0},
	{interfaceButtonSamplePlay, 3, 0},
	{interfaceButtonSampleEdit, 6, 0},
	{interfaceButtonSampleRec, 	8, 0},
	{interfaceButtonSampleLoad, 1, 0},
	{interfaceButtonSong, 		4, 0},
	{interfaceButtonConfig, 	7, mtConfigModeDefault},
};


uint8_t cFunctionMachine::potsCount = 		interfacePotsCount;
uint8_t cFunctionMachine::buttonsCount = 	interfaceButtonsCount;
uint8_t cFunctionMachine::padsCount = 		interfacePadsCount;
cFunctionMachine::strPotObject cFunctionMachine::pots			[interfacePotsCount] 	= {0};
cFunctionMachine::strButtonObject cFunctionMachine::buttons		[interfaceButtonsCount] = {0};
cFunctionMachine::strPadObject cFunctionMachine::pads			[interfacePadsCount] 	= {0};



//=======================================================================
//=======================================================================
//=======================================================================
//=======================================================================
void cInterface::begin()
{
	operatingMode = mtOperatingModeStartup;
	startupTimer = 0;

	for(uint8_t i = 0; i<modulesCount; i++)
	{
		modules[i]->setEventFunct(interfaceEnvents);
		modules[i]->FM = &uiFM;
	}

	popupConfig.time = 2;
	popupConfig.w = 300;
	popupConfig.h = 100;
	popupConfig.x = 800/2-150;
	popupConfig.y = 480/2-50;
	popupConfig.lineColor[0] = 0xffffff;
	popupConfig.lineStyle[0] = controlStyleCenterX;
	mtPopups.config(4, &popupConfig);
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


	handleGlobalActions();

	mtPopups.update();

	mtTest.testLoop();

	debugLog.update();
}


//=======================================================================
//=======================================================================
//=======================================================================


void cInterface::handleGlobalActions()
{
	hideAllGlobalActions();

	handleShutdown();
	//handleNoSdCard();
}

void cInterface::handleNoSdCard()
{
	uint32_t selfPrio = noSdCardPriority;

	if((globalActionPriority & ~selfPrio) <= selfPrio)
	{
		if(!sdCardDetector.isCardInserted())
		{
			if(globalActionPriority == 0)
			{
				uint8_t state = 0;
				interfaceEnvents(eventToggleActiveModule,0,0,&state);
			}

			globalActionPriority |= selfPrio;

			if(noSdCardInitFlag == 0)
			{
				initDisplayNoSdCard();
				noSdCardInitFlag = 1;
			}

			refreshDsiplayNoSdCard();
		}
		else
		{
			if(noSdCardInitFlag == 1)
			{
				if(sdCardDetector.isCardInitialized())
				{
					deinitDisplayNoSdCard();
					globalActionPriority &= ~selfPrio;

					if(globalActionPriority == 0)
					{
						uint8_t state = 1;
						interfaceEnvents(eventToggleActiveModule,0,0,&state);
					}

					noSdCardInitFlag = 0;
				}
			}
		}
	}
}

void cInterface::handleShutdown()
{
	if(isBooted)
	{
		uint32_t selfPrio = powerButtonActionPriority;

		if((globalActionPriority & ~selfPrio) <= selfPrio)
		{
			if(lowPower.getShutdownRequest())
			{
				if(globalActionPriority == 0)
				{
					uint8_t state = 0;
					interfaceEnvents(eventToggleActiveModule,0,0,&state);
				}

				globalActionPriority |= selfPrio;

				if(shutdownScreenInitFlag == 0)
				{
					shutdownRequestTimestamp = shutdownTimer;
					shutdownScreenInitFlag = 1;
					initDisplayCountDown();
				}

				int32_t timeToShutdown_ms = 0;
				uint8_t progress;

				timeToShutdown_ms = (TURN_OFF_TIME_MS - (shutdownTimer - shutdownRequestTimestamp));
				progress = ((timeToShutdown_ms * 100) / TURN_OFF_TIME_MS);

				refreshDisplayCountDown(timeToShutdown_ms, progress);

				if(timeToShutdown_ms <= 0)
				{
					lowPower.goLowPower();
				}
			}
			else
			{
				if(shutdownScreenInitFlag == 1)
				{
					globalActionPriority &= ~selfPrio;
					shutdownScreenInitFlag = 0;
					deinitDisplayCountDown();

					if(globalActionPriority == 0)
					{
						uint8_t state = 1;
						interfaceEnvents(eventToggleActiveModule,0,0,&state);
					}
				}
			}
		}
	}
}

void cInterface::processOperatingMode()
{
	switch(operatingMode)
	{
		case mtOperatingModeStartup:
		{
			doStartTasks();
			operatingMode = mtOperatingModeOpenProject;
			break;
		}
		case mtOperatingModeOpenProject:
		{
			if(sdCardDetector.isCardInitialized())
			{
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
		default: break;
	}
}

void cInterface::doStartTasks()
{
	readConfig();
	//readSdConfig();
	mtPopups.initPopupsDisplayControls();
	initStartScreen();
}

void cInterface::processStartScreen()
{
	if(sdCardDetector.isCardInitialized())
	{
		showStartScreen();

		if(detectStartState())
		{
			destroyStartScreen();
			deinitDisplayNoSdCard();
			operatingMode = mtOperatingModeRun;
			activateModule(&patternEditor, 0);
		}
	}
	else
	{
		hideStartScreen();
		initDisplayNoSdCard();
		refreshDsiplayNoSdCard();
		operatingMode = mtOperatingModeOpenProject;
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
	uiFM.clearSdDetection();

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
	uiFM.clearSdDetection();

	display.resetControlQueue();
	module->stop();
	mtPopups.hideStepPopups();
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

void cInterface::switchModuleToPrevious(hModule module)
{
	if(previousModule == module) return;

	hModule prevModule = previousModule;
	uint32_t prevModuleOptions = previousModuleOptions;

	deactivateModule(module);
	activateModule(prevModule, prevModuleOptions);
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
			// param1 = uchwyt do modułu, param2 = index przycisku
			mtInterface.switchModuleToPrevious((hModule)param1);
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
			mtInterface.activateModule(&imageViewer,0);
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
			mtTest.runTestingProcedure(&mtInterface.uiFM, interfaceEnvents);
			break;
		}



	default: break;
	}

}


















