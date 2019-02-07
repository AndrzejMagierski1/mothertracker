
#include <stdint.h>


#include "mtInterface.h"
#include "mtProjectEditor.h"
#include "mtInstrumentEditor.h"
#include "mtStepEditor.h"
#include "mtSequencer.h"


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

		if(*(uint8_t*)param1 == 0)
		{
			sequencer.play();
		}
		else if(*(uint8_t*)param1 == 2)
		{
			sequencer.stop();
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
	case ProjEditEventLoadLastProjFailed:

	break;

	case mtPriojectEditorEventPadPress:

		if(*(uint8_t*)param1 == 3)
		{
			mtInterface.activateModule(mtModuleInstrumentEditor);
			mtInstrumentEditor.startExisting(0);
		}
		else if(*(uint8_t*)param1 == 0)
		{
			sequencer.play();
		}
		else if(*(uint8_t*)param1 == 2)
		{
			sequencer.stop();
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

		if(*(uint8_t*)param1 == 3)
		{
			mtInterface.deactivateModule(mtModuleStepEditor);
			mtInterface.activateModule(mtModuleInstrumentEditor);
			mtInstrumentEditor.startExisting(0);
		}
		else if(*(uint8_t*)param1 == 0)
		{
			sequencer.play();
		}
		else if(*(uint8_t*)param1 == 2)
		{
			sequencer.stop();
		}

	break;

	case mtStepEditorEventSeqButtonsPress:

	break;

	default: break;
	}

}
