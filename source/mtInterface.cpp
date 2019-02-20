


#include "mtProjectEditor.h"
#include "mtInstrumentEditor.h"
#include "mtStepEditor.h"


#include "mtInterface.h"
#include "mtInterfaceDefs.h"
#include "mtFileManager.h"


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

/*	fileManager.writeInstrumentFile("IMPORT/Instrument_001.mti",&mtProject.instrument[0]);
	fileManager.writeInstrumentFile("IMPORT/Instrument_002.mti",&mtProject.instrument[1]);
	fileManager.writeInstrumentFile("IMPORT/Instrument_003.mti",&mtProject.instrument[2]);
	fileManager.writeInstrumentFile("IMPORT/Instrument_004.mti",&mtProject.instrument[3]);
	fileManager.writePatternFile("IMPORT/Pattern_001.mtp");*/

/*	fileManager.createNewProject("Project_001");
	fileManager.importSampleToProject(NULL,"2.WAV","2.WAV",0,mtSampleTypeWavetable);
	fileManager.importSampleToProject(NULL,"4.WAV","4.WAV",1,mtSampleTypeWavetable);
	fileManager.importSampleToProject(NULL,"7.WAV","7.WAV",2,mtSampleTypeWavetable);
	fileManager.importSampleToProject(NULL,"8.WAV","8.WAV",3,mtSampleTypeWavetable);
	fileManager.importSampleToProject(NULL,"3.WAV","3.WAV",4,mtSampleTypeWavetable);
	fileManager.importSampleToProject(NULL,"5.WAV","5.WAV",5,mtSampleTypeWavetable);
	fileManager.importSampleToProject(NULL,"6.WAV","6.WAV",6,mtSampleTypeWavetable);
	fileManager.importSampleToProject(NULL,"1.WAV","1.WAV",7,mtSampleTypeWavetable);*/
//	fileManager.importPatternToProject("IMPORT", "Pattern_001.mtp", "Pattern_001.mtp", 7);
//	fileManager.importInstrumentToProject("IMPORT", "Instrument_001.mti","Instrument_001.mti", 0);
//	fileManager.importInstrumentToProject("IMPORT", "Instrument_002.mti","Instrument_002.mti", 1);
//	fileManager.importInstrumentToProject("IMPORT", "Instrument_003.mti","Instrument_003.mti", 2);
//	fileManager.importInstrumentToProject("IMPORT", "Instrument_004.mti","Instrument_004.mti", 3);
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
