
#include "mtDisplay.h"
#include "mtSampleEditor.h"


#include "mtInterface.h"
#include "mtInterfaceDefs.h"

extern void sampleEditorEvent(uint8_t event, void* param);



cMtInterface mtInterface;





//=======================================================================
void cMtInterface::begin()
{
	setOperatingMode(mtOperatingModeStartup);
	startupTimer = 0;

	mtSampleEditor.begin();



}



//=======================================================================
void cMtInterface::update()
{
	processOperatingMode();



	mtSampleEditor.update();


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
	case mtOperatingModePaternEditor:
	{

		break;
	}
	case mtOperatingModeSampleEditor:
	{
		mtSampleEditor.start("BUM.WAV");
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
			setOperatingMode(mtOperatingModeSampleEditor);
		}
	}












}

