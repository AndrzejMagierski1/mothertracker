
#include <stdint.h>


#include "mtInterface.h"
#include "mtProjectEditor.h"


void instrumentEditorEvent(uint8_t event, void* param)
{
	switch(event)
	{
//	case mtFileManagerFileCallbackOpenFile:

//	break;

	default: break;
	}

}



void projectEditorEvent(uint8_t event, void* param)
{
	switch(event)
	{
	case ProjEditEventLoadLastProjFailed:

	break;

	default: break;
	}

}
