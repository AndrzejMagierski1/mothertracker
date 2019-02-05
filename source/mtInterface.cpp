


#include "mtProjectEditor.h"
#include "mtInstrumentEditor.h"


#include "mtInterface.h"
#include "mtInterfaceDefs.h"



extern void sampleEditorEvent(uint8_t event, void* param);


cMtInterface mtInterface;





//=======================================================================
void cMtInterface::begin()
{
	setOperatingMode(mtOperatingModeStartup);
	startupTimer = 0;

	mtInstrumentEditor.begin();





}


//=======================================================================
void cMtInterface::update()
{
	processOperatingMode();



	mtInstrumentEditor.update();

	//mtProjectEditor.update();

}


void cMtInterface::setOperatingMode(uint8_t mode)
{
	switch(mode)
	{
	case mtOperatingModeNone:
	{

		break;
	}
	case mtOperatingModeStartup:
	{

		break;
	}
	case mtOperatingModeSongEditor:
	{

		break;
	}
	case mtOperatingModeProjectEditor:
	{
		mtProjectEditor.loadLastProject();
		break;
	}
	case mtOperatingModePaternEditor:
	{

		break;
	}
	case mtOperatingModeInstrumentEditor:
	{
		mtInstrumentEditor.startExisting(0);
		break;
	}
	case mtOperatingModeFileManager:
	{

		break;
	}
	case mtOperatingModeRecorder:
	{

		break;
	}
	case mtOperatingModeMixer:
	{

		break;
	}
	case mtOperatingModeConfig:
	{

		break;
	}

	}


	operatingMode = mode;
}


void cMtInterface::processOperatingMode()
{
	if(operatingMode == mtOperatingModeStartup)
	{
		if(startupTimer > MT_INTERFACE_STARTUP_TIME)
		{
			mtDisplay.setMode(mtDisplayModeNormal);
			setOperatingMode(mtOperatingModeProjectEditor);
		}
	}

}

