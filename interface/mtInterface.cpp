

#include "mtStructs.h"

#include "mtFileManager.h"


#include "mtInstrumentEditor.h"
#include "mtInterfaceDefs.h"
#include "mtProjectEditor.h"
#include "mtSampleBankEditor.h"
#include "mtStepEditor.h"
#include "mtConfigEditor.h"
#include "mtConfig.h"


#include "mtInterface.h"
#include "modulesBase.h"




cMtInterface mtInterface;





//=======================================================================
//=======================================================================
//=======================================================================
//=======================================================================
void cMtInterface::begin()
{
	operatingMode = mtOperatingModeStartup;
	startupTimer = 0;






	mtProjectEditor.setEventFunct(projectEditorEvent);
	mtInstrumentEditor.setEventFunct(instrumentEditorEvent);
	mtStepEditor.setEventFunct(stepEditorEvent);
	mtSampleBankEditor.setEventFunct(sampleBankEditorEvent);
	mtConfigEditor.setEventFunct(configEditorEvent);

	readConfig(CONFIG_EEPROM_ADDRESS, &mtConfig);


}

//=======================================================================
//=======================================================================
//=======================================================================
void cMtInterface::update()
{
	processOperatingMode();


 	if(activeModules[mtModuleProjectEditor]) 		mtProjectEditor.update();
	if(activeModules[mtModuleInstrumentEditor]) 	mtInstrumentEditor.update();
	if(activeModules[mtModuleStepEditor]) 			mtStepEditor.update();
	if(activeModules[mtModuleSampleBankEditor]) 	mtSampleBankEditor.update();
	if(activeModules[mtModuleConfigEditor]) 		mtConfigEditor.update();


}



//=======================================================================
//=======================================================================
//=======================================================================
void cMtInterface::processOperatingMode()
{
	if(operatingMode == mtOperatingModeStartup)
	{
		if(startupTimer > MT_INTERFACE_STARTUP_TIME)
		{
			operatingMode = mtOperatingModeRun;


			//activateModule(mtModuleProjectEditor);
			//mtProjectEditor.start(mtProjectStartModeOpenLast);
		}
	}
	else if(operatingMode == mtOperatingModeRun)
	{

	}


}

//=======================================================================
//=======================================================================
//=======================================================================
void cMtInterface::activateModule(uint8_t module)
{

	activeModules[module] = 1;
	onScreenModule = module;

}


void cMtInterface::deactivateModule(uint8_t module)
{
	activeModules[module] = 0;
}


//=======================================================================
//=======================================================================
//=======================================================================




















