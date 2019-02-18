


#include "mtProjectEditor.h"
#include "mtInstrumentEditor.h"
#include "mtStepEditor.h"


#include "mtInterface.h"
#include "mtInterfaceDefs.h"



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

//	mtProjectEditor.writeInstrumentFile("instrument_000.mti", &mtProject.instrument[0]);
//	mtProjectEditor.readInstrumentFile("instrument_000.mti", &mtProject.instrument[0]);
//	mtProjectEditor.writePatternFile("pattern_000.mtp");
//	mtProjectEditor.readPatternFile("pattern_000.mtp");
//	mtProjectEditor.writeProjectFile("project_000.bin", &mtProject.mtProjectRemote);
//	mtProjectEditor.readProjectFile("project_000.bin", &mtProject.mtProjectRemote);
	mtProjectEditor.createNewProject(NULL,"Project_001");
	mtProjectEditor.importSampleToProject("/Project_001",NULL,"1.WAV",1,mtSampleTypeWaveFile);
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

			mtDisplay.setMode(mtDisplayModeNormal);
			activateModule(mtModuleProjectEditor);
			mtProjectEditor.startProject();
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

//=======================================================================
//=======================================================================
//=======================================================================
void cMtInterface::deactivateModule(uint8_t module)
{
	activeModules[module] = 0;
}
