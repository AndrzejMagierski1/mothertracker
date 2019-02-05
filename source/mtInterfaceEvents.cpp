
#include <stdint.h>


#include "mtInterface.h"
#include "mtProjectEditor.h"
#include "mtInstrumentEditor.h"
#include "mtStepEditor.h"

void instrumentEditorEvent(uint8_t event, void* param1, void* param2, void* param3)
{
	switch(event)
	{

	case mtInstrumentEditorSeqButtonsPress:

		mtInterface.onScreenModule = mtModuleStepEditor;
		mtStepEditor.showStep(*(uint8_t*)param1,*(uint8_t*)param2);

	break;


	default: break;
	}

}



void projectEditorEvent(uint8_t event, void* param1, void* param2, void* param3)
{
	switch(event)
	{
	case ProjEditEventLoadLastProjFailed:

	break;

	default: break;
	}

}


void stepEditorEvent(uint8_t event, void* param1, void* param2, void* param3)
{
	switch(event)
	{
	case mtStepEditorEventPadPress:

		mtInterface.onScreenModule = mtModuleInstrumentEditor;
		mtInstrumentEditor.startExisting(0);

	break;

	case mtStepEditorEventSeqButtonsPress:

	break;

	default: break;
	}

}
