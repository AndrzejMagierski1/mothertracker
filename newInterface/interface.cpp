
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




#include "projectEditor.h"

#include "interface.h"


cInterface mtInterface;



strMtConfig mtConfig;
strMtProject mtProject;

__NOINIT(EXTERNAL_RAM) int16_t sdram_sampleBank[4*1024*1024];


//=======================================================================
//							SETUP INTERFEJSU
//=======================================================================
//=======================================================================

const uint8_t cInterface::modulesCount = 1;
const hModule cInterface::modules[modulesCount] =
{
		&projectEditor,





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


	readConfig(CONFIG_EEPROM_ADDRESS, &mtConfig);

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
}



//=======================================================================
//=======================================================================
//=======================================================================
void cInterface::processOperatingMode()
{
	if(operatingMode == mtOperatingModeStartup)
	{
		if(startupTimer > MT_INTERFACE_STARTUP_TIME)
		{
			operatingMode = mtOperatingModeRun;

			//activateModule(projectEditor);
			projectEditor.start(mtProjectStartModeOpenLast);
		}
	}
	else if(operatingMode == mtOperatingModeRun)
	{

	}


}

//=======================================================================
//=======================================================================
//=======================================================================
void cInterface::activateModule(uint8_t module)
{

	activeModules[module] = 1;
	onScreenModule = module;

}


void cInterface::deactivateModule(uint8_t module)
{
	activeModules[module] = 0;
}


//=======================================================================
//=======================================================================
//=======================================================================




















