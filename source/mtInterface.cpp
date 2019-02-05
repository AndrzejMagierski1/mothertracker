


#include "mtProjectEditor.h"
#include "mtInstrumentEditor.h"
#include "mtStepEditor.h"


#include "mtInterface.h"
#include "mtInterfaceDefs.h"



cMtInterface mtInterface;





//=======================================================================
void cMtInterface::begin()
{
	setOperatingMode(mtOperatingModeStartup);
	startupTimer = 0;

	mtProjectEditor.setEventFunct(projectEditorEvent);
	mtInstrumentEditor.setEventFunct(instrumentEditorEvent);
	mtStepEditor.setEventFunct(stepEditorEvent);


}


//=======================================================================
void cMtInterface::update()
{
	processOperatingMode();


 	if(activeModules[mtModuleProjectEditor]) 		mtProjectEditor.update();
	if(activeModules[mtModuleInstrumentEditor]) 	mtInstrumentEditor.update();
	if(activeModules[mtModuleStepEditor]) 			mtStepEditor.update();




}




void cMtInterface::processOperatingMode()
{
	if(lastOperatingMode == operatingMode) return;

	if(operatingMode == mtOperatingModeStartup)
	{
		if(startupTimer > MT_INTERFACE_STARTUP_TIME)
		{
			lastOperatingMode = operatingMode;
			mtDisplay.setMode(mtDisplayModeNormal);
			setOperatingMode(mtOperatingModeProjectEditor);
		}
	}
	else if(operatingMode == mtOperatingModeProjectEditor)
	{
		lastOperatingMode = operatingMode;
		activateModule(mtModuleProjectEditor);
		activateModule(mtModuleStepEditor);
		mtProjectEditor.startProject();
	}
	else if(operatingMode == mtOperatingModeInstrumentEditor)
	{
		lastOperatingMode = operatingMode;
		activateModule(mtModuleInstrumentEditor);
		onScreenModule = mtModuleInstrumentEditor;
		mtInstrumentEditor.startExisting(0);
	}


}


void cMtInterface::setOperatingMode(uint8_t mode)
{
	operatingMode = mode;
}

void cMtInterface::activateModule(uint8_t module)
{

	activeModules[module] = 1;

}

void cMtInterface::deactivateModule(uint8_t module)
{
	activeModules[module] = 0;
}
