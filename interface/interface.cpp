

#include <interface.h>
#include <patternEditor.h>
#include <projectEditor.h>
#include <sampleImporter.h>
#include <samplePlayback.h>
#include "songEditor.h"
#include "instrumentEditor.h"
#include "sampleEditor.h"
#include "sampleRecorder.h"
#include "configEditor.h"



#include "mtStructs.h"


/*

#include "keyScanner.h"
#include "mtLED.h"

#include "mtInstrumentEditor.h"
#include "mtInterfaceDefs.h"
#include "mtProjectEditor.h"
#include "mtSampleBankEditor.h"
#include "mtStepEditor.h"
#include "mtConfigEditor.h"

*/

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

__NOINIT(EXTERNAL_RAM) int16_t sdram_sampleBank[4*1024*1024];
__NOINIT(EXTERNAL_RAM) int16_t sdram_effectsBank[4*1024*1024];


//=======================================================================
//							SETUP INTERFEJSU
//=======================================================================
//=======================================================================

const uint8_t cInterface::modulesCount = 9;
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

};


const uint8_t cInterface::modulesButtonsCount = 12;
const uint32_t cInterface::modulesButtons[modulesButtonsCount][3] =
{
	{interfaceButtonEnvelopes,  5, mtInstEditModeEnv},
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
	{interfaceButtonInstr, 		5, mtInstEditModeInstrList},
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


	//readConfig(CONFIG_EEPROM_ADDRESS, &mtConfig);

	//ramMonitor.initialize();

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



	if(ramInfoTimer > 5000)
	{
		ramInfoTimer = 0;

		//ramMonitor.run();
		//ramMonitor.report_ram();

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
			readConfig();
			openStartupProject();
			initStartScreen();
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

//=======================================================================
//=======================================================================
//=======================================================================
void cInterface::activateModule(hModule module, uint32_t options)
{
	uiFM.clearAllButtons();
	uiFM.clearAllPots();
	uiFM.clearAllPads();

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

	display.resetControlQueue();
	module->stop();
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



	default: break;
	}

}


















