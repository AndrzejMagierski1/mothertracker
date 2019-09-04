

#include <interface.h>

#include <projectEditor.h>

#include "mtFileManager.h"
#include <display.h>

elapsedMillis startScreenRefresh;


static uint8_t functHide();

uint8_t hidePressFlag = 0;

//==================================================================================================
//==================================================================================================
//==================================================================================================
uint8_t cInterface::detectStartState()
{
	if(startupTimer > MT_INTERFACE_STARTUP_TIME) return 1; // zabiezpieczenie czasowe

	if(hidePressFlag) return 1;



	startSampleLoadingFlag = fileManager.samplesLoader.getStateFlag();

	if(startSampleLoadingFlag == loaderStateTypeInProgress)
	{
		startSampleLoadingProgress = fileManager.samplesLoader.getCurrentProgress();

		return 0;
	}





	return 1;
}


//==================================================================================================
void cInterface::initStartScreen()
{
	if(mtConfig.firmware.beta)
	{
		sprintf(startScreenData.versionLabel, "v%d.%d.%db", mtConfig.firmware.ver_1, mtConfig.firmware.ver_2, mtConfig.firmware.ver_3);
	}
	else
	{
		sprintf(startScreenData.versionLabel, "v%d.%d.%d", mtConfig.firmware.ver_1, mtConfig.firmware.ver_2, mtConfig.firmware.ver_3);
	}

	strControlProperties prop;
	prop.x = 190;
	prop.y = 280;
	//prop.w = 800/4-10;
	prop.style = controlStyleShow;
	prop.h = 100;
	prop.w = 420;

	prop.data  = &startScreenData;
//	prop.value = 70;
//	prop.text = "loading...";
	if(startScreenControl == nullptr)  startScreenControl = display.createControl<cStartScreen>(&prop);


	uiFM.setButtonObj(interfaceButton7, buttonPress, functHide);


}

void cInterface::showStartScreen()
{
	if(startScreenRefresh < 100) return;

	startScreenRefresh = 0;

	display.setControlValue(startScreenControl, startSampleLoadingProgress);
	display.setControlText(startScreenControl, "Opening last project...");
	display.refreshControl(startScreenControl);
}

void cInterface::destroyStartScreen()
{
	display.destroyControl(startScreenControl);
	startScreenControl = nullptr;
}


//==================================================================================================
void cInterface::openStartupProject()
{
	if(mtConfig.startup.startMode == interfaceOpenLastProject)
	{
		if(fileManager.openProject(mtConfig.startup.lastProjectName, projectTypeUserMade))
		{
			fileManager.samplesLoader.start(0,1);
			projectEditor.loadProjectValues();
		}
	}




}


static uint8_t functHide()
{
	hidePressFlag = 1;
	return 0;
}


//=======================================================================
//=======================================================================
//=======================================================================















