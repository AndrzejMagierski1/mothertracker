

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

#include "game.h"
#include "performanceMode.h"

#include "mtFileManager.h"





#include "mtStructs.h"

#include "interfacePopups.h"


#include "mtConfig.h"
#include "mtHardware.h"
#include "SD.h"
#include "sdram.h"

#include "RamMonitor.h"





cInterface mtInterface;

elapsedMillis ramInfoTimer;

strMtConfig mtConfig;
strMtProject mtProject;
strInterfaceGlobals interfaceGlobals;

__NOINIT(EXTERNAL_RAM) int16_t sdram_sampleBank[SAMPLE_MEMORY_MAX/2];
__NOINIT(EXTERNAL_RAM) int16_t sdram_effectsBank[SAMPLE_MEMORY_MAX/2];
__NOINIT(EXTERNAL_RAM) uint8_t sdram_writeLoadBuffer[32768];
//__NOINIT(EXTERNAL_RAM) uint8_t undo_Bank[1024*1024];

//=======================================================================
//							SETUP INTERFEJSU
//=======================================================================
//=======================================================================

const uint8_t cInterface::modulesCount = 11;
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
		&gameModule,		// 10
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

//	case interfaceButton10: activateModule(modules[0], 0); break;
//	case interfaceButton11: activateModule(modules[4], 0); break;
//	case interfaceButton12: activateModule(modules[7], 0); break;
//	case interfaceButton13: activateModule(modules[2], 0); break;
//	case interfaceButton14: activateModule(modules[3], 0); break;
//	case interfaceButton15: activateModule(modules[6], 0); break;
//	case interfaceButton16: activateModule(modules[0], 0); break;
//	case interfaceButton17: activateModule(modules[1], 0); break;
//	case interfaceButton23: activateModule(modules[5], mtInstEditModeVolume); break;
//	case interfaceButton24: activateModule(modules[5], mtInstEditModeFilter); break;
//	case interfaceButton25: activateModule(modules[5], mtInstEditModeParams); break;
//	}

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

	//ramMonitor.initialize();

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



	mtPopups.update();


	if(ramInfoTimer > 5000)
	{
		ramInfoTimer = 0;

		//ramMonitor.run();
		//ramMonitor.report_ram();

	}

	if(fileManager.configChangedRefresh > 10000)
	{
		fileManager.configChangedRefresh = 0;
		if(fileManager.configIsChangedFlag == 1)
		{
			fileManager.autoSaveProject();
		}
	}
	if(fileManager.instrumentRefresh > 10000)
	{
		fileManager.instrumentRefresh = 0;
		for(uint8_t i = 0; i< INSTRUMENTS_COUNT; i++)
		{
			if(fileManager.instrumentIsChangedFlag[i] == 1 )
			{
				fileManager.instrumentIsChangedFlag[i] = 0;
				fileManager.saveInstrument(i);
			}
		}
	}
	if(fileManager.instrumentForcedSaveFlag)
	{
		fileManager.instrumentForcedSaveFlag = 0;

		for(uint8_t i = 0; i< INSTRUMENTS_COUNT; i++)
		{
			if(fileManager.instrumentIsChangedFlag[i] == 1 )
			{
				fileManager.instrumentIsChangedFlag[i] = 0;
				fileManager.saveInstrument(i);
			}
		}
	}


	if(fileManager.patternRefresh > 10000)
	{
		fileManager.patternRefresh = 0;

		if(fileManager.patternIsChangedFlag == 1 )
		{
			fileManager.patternIsChangedFlag = 0;
			fileManager.savePattern(mtProject.values.actualPattern);
		}

	}
}



//=======================================================================
//=======================================================================
//=======================================================================



void cInterface::processOperatingMode()
{
	if(operatingMode == mtOperatingModeStartup)
	{
		if(doOnStart)
		{
			doOnStart = 0;
			doStartTasks();
		}




		if(detectStartState())
		{
			destroyStartScreen();
			operatingMode = mtOperatingModeRun;

			activateModule(&patternEditor, 0);
		}

		showStartScreen();

	}
//	else if(operatingMode == mtOperatingModeRun)
//	{
//
//	}


}

void cInterface::doStartTasks()
{
	mtPopups.initPopupsDisplayControls();

	readConfig();
	readSdConfig();

	openStartupProject();

	initStartScreen();
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

void cInterface::switchModuleByButton(hModule module, uint8_t button)
{
	int8_t index = getButtonIndex(button);
	if(index < 0 ) return;
	if(modules[modulesButtons[index][1]] == module) return;

	deactivateModule(module);
	activateModule(modules[modulesButtons[index][1]], modulesButtons[index][2]);
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


void cInterface::toggleActiveModule()
{
	if(onScreenModule == nullptr)
	{
		hModule prevModule = previousModule;
		uint32_t prevModuleOptions = previousModuleOptions;

		activateModule(prevModule, prevModuleOptions);
		return;
	}

	mtInterface.deactivateModule(onScreenModule);
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
			mtInterface.switchModuleByButton((hModule)param1, *((uint8_t*)param2));
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
		case eventToggleActiveModule:
		{
			mtInterface.toggleActiveModule();
			break;
		}



	default: break;
	}

}


















