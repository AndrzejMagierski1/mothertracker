
#include <stdint.h>

#include "elapsedMillis.h"
#include "mtSequencer.h"

#include "mtInterface.h"
#include "mtInstrumentEditor.h"
#include "mtProjectEditor.h"
#include "mtSampleBankEditor.h"
#include "mtStepEditor.h"
#include "mtConfigEditor.h"


void defaultPadsPressFuct(uint8_t pad);

//=======================================================================
//=======================================================================
//=======================================================================
void instrumentEditorEvent(uint8_t event, void* param1, void* param2, void* param3)
{
	switch(event)
	{

	case mtInstrumentEditorSeqButtonsPress:

		mtInterface.activateModule(mtModuleStepEditor);
		if(*(uint8_t*)param2 > 0) mtStepEditor.showStep(*(uint8_t*)param1,*(uint8_t*)param2);

	break;

	case mtInstrumentEditorEventPadPress:

		defaultPadsPressFuct(*(uint8_t*)param1);

	break;

	default: break;
	}

}


//=======================================================================
//=======================================================================
//=======================================================================
void projectEditorEvent(uint8_t event, void* param1, void* param2, void* param3)
{
	switch(event)
	{
	case mtPriojectEditorEventLoadLastProjFailed:

	break;

	case mtPriojectEditorEventPadPress:

		defaultPadsPressFuct(*(uint8_t*)param1);

	break;


	case mtProjectEditorEventLoadSampleBank:

		mtSampleBankEditor.loadSamplesBank();

	break;




	default: break;
	}

}


//=======================================================================
//=======================================================================
//=======================================================================
void stepEditorEvent(uint8_t event, void* param1, void* param2, void* param3)
{
	switch(event)
	{
	case mtStepEditorEventPadPress:

		defaultPadsPressFuct(*(uint8_t*)param1);

	break;

	case mtStepEditorEventSeqButtonsPress:

	break;

	default: break;
	}

}

//=======================================================================
//=======================================================================
//=======================================================================


void configEditorEvent(uint8_t event, void* param1, void* param2, void* param3)
{
	switch(event)
	{
	case mtConfigEditorEventPadPress:

		defaultPadsPressFuct(*(uint8_t*)param1);

	break;

	case mtConfigEditorEventSeqButtonsPress:

	break;

	default: break;
	}

}

//=======================================================================
//=======================================================================
//=======================================================================
void sampleBankEditorEvent(uint8_t event, void* param1, void* param2, void* param3)
{
	switch(event)
	{
	case mtSampleBankEditorEventPadPress:

		defaultPadsPressFuct(*(uint8_t*)param1);

	break;

	case mtSampleBankEditorEventSeqButtonsPress:

	break;

	default: break;
	}

}


void defaultPadsPressFuct(uint8_t pad)
{
	switch(pad)
	{
	case interfacePadPlay:
	{
		sequencer.play();
		break;
	}
	case interfacePadStop:
	{
		sequencer.stop();
		break;
	}
	case interfacePadProjectEditor:
	{
		mtInterface.activateModule(mtModuleProjectEditor);
		mtProjectEditor.start(mtProjectStartModeDoNothing);
		break;
	}
	case interfacePadSampleBank:
	{
		mtInterface.activateModule(mtModuleSampleBankEditor);
		mtSampleBankEditor.start();
		break;
	}
	case interfacePadInstrumentEditor:
	{
		mtInterface.activateModule(mtModuleInstrumentEditor);
		mtInstrumentEditor.startExisting(mtProject.values.lastUsedInstrument);
		break;
	}
	case interfacePadConfig:
	{
		mtInterface.activateModule(mtModuleConfigEditor);
		mtConfigEditor.start(mtConfigEditorStartModeConfig);
		break;
	}
	case interfacePadSettings:
	{
		mtInterface.activateModule(mtModuleConfigEditor);
		mtConfigEditor.start(mtConfigEditorStartModeGlobals);
		break;
	}
	case interfacePadRecorder:
	{
		mtInterface.activateModule(mtModuleSampleBankEditor);
		mtSampleBankEditor.startRecorder();
		break;
	}


	default: break;
	}


}

