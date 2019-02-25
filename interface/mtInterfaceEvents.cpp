
#include <stdint.h>

#include "elapsedMillis.h"
#include "mtSequencer.h"

#include "mtInterface.h"
#include "mtInstrumentEditor.h"
#include "mtProjectEditor.h"
#include "mtSampleBankEditor.h"
#include "mtStepEditor.h"
#include "mtConfigEditor.h"

//=======================================================================
//=======================================================================
//=======================================================================
void instrumentEditorEvent(uint8_t event, void* param1, void* param2, void* param3)
{
	switch(event)
	{

	case mtInstrumentEditorSeqButtonsPress:


		mtInterface.deactivateModule(mtModuleInstrumentEditor);
		mtInterface.activateModule(mtModuleStepEditor);
		if(*(uint8_t*)param2 > 0) mtStepEditor.showStep(*(uint8_t*)param1,*(uint8_t*)param2);


	break;

	case mtInstrumentEditorEventPadPress:

		if(*(uint8_t*)param1 == interfacePadPlay)
		{
			sequencer.play();
		}
		else if(*(uint8_t*)param1 == interfacePadStop)
		{
			sequencer.stop();
		}
		else if(*(uint8_t*)param1 == interfacePadProjectEditor)
		{
			mtInterface.activateModule(mtModuleProjectEditor);
			mtProjectEditor.start(mtProjectStartModeDoNothing);
		}
		else if(*(uint8_t*)param1 == interfacePadSampleBank)
		{
			mtInterface.activateModule(mtModuleSampleBankEditor);
			mtSampleBankEditor.start();
		}
		else if(*(uint8_t*)param1 == interfacePadConfig)
		{

		}

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

		if(*(uint8_t*)param1 == interfacePadPlay)
		{
			sequencer.play();
		}
		else if(*(uint8_t*)param1 == interfacePadStop)
		{
			sequencer.stop();
		}
		else if(*(uint8_t*)param1 == interfacePadInstrumentEditor)
		{
			mtInterface.activateModule(mtModuleInstrumentEditor);
			mtInstrumentEditor.startExisting(0);
		}
		else if(*(uint8_t*)param1 == interfacePadSampleBank)
		{
			mtInterface.activateModule(mtModuleSampleBankEditor);
			mtSampleBankEditor.start();
		}
		else if(*(uint8_t*)param1 == interfacePadConfig)
		{

		}


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

		if(*(uint8_t*)param1 == interfacePadPlay)
		{
			sequencer.play();
		}
		else if(*(uint8_t*)param1 == interfacePadStop)
		{
			sequencer.stop();
		}
		else if(*(uint8_t*)param1 == interfacePadInstrumentEditor)
		{
			mtInterface.activateModule(mtModuleInstrumentEditor);
			mtInstrumentEditor.startExisting(0);
		}
		else if(*(uint8_t*)param1 == interfacePadSampleBank)
		{
			mtInterface.activateModule(mtModuleSampleBankEditor);
			mtSampleBankEditor.start();
		}
		else if(*(uint8_t*)param1 == interfacePadConfig)
		{

		}

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

		if(*(uint8_t*)param1 == interfacePadPlay)
		{
			sequencer.play();
		}
		else if(*(uint8_t*)param1 == interfacePadStop)
		{
			sequencer.stop();
		}
		else if(*(uint8_t*)param1 == interfacePadInstrumentEditor)
		{
			mtInterface.activateModule(mtModuleInstrumentEditor);
			mtInstrumentEditor.startExisting(0);
		}
		else if(*(uint8_t*)param1 == interfacePadProjectEditor)
		{
			mtInterface.activateModule(mtModuleProjectEditor);
			mtProjectEditor.start(mtProjectStartModeDoNothing);
		}
		else if(*(uint8_t*)param1 == interfacePadSampleBank)
		{
			mtInterface.activateModule(mtModuleSampleBankEditor);
			mtSampleBankEditor.start();
		}


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

		if(*(uint8_t*)param1 == interfacePadPlay)
		{
			sequencer.play();
		}
		else if(*(uint8_t*)param1 == interfacePadStop)
		{
			sequencer.stop();
		}
		else if(*(uint8_t*)param1 == interfacePadInstrumentEditor)
		{
			mtInterface.activateModule(mtModuleInstrumentEditor);
			mtInstrumentEditor.startExisting(0);
		}
		else if(*(uint8_t*)param1 == interfacePadProjectEditor)
		{
			mtInterface.activateModule(mtModuleProjectEditor);
			mtProjectEditor.start(mtProjectStartModeDoNothing);
		}
		else if(*(uint8_t*)param1 == interfacePadConfig)
		{

		}

	break;

	case mtSampleBankEditorEventSeqButtonsPress:

	break;

	default: break;
	}

}




