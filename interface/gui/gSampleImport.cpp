

#include <sampleImporter.h>


static uint16_t framesPlaces[2][4] =
{
	{0+2, 		31, 800/4-5, 387},
	{(800/4)*3+2, 31, 800/4-5, 387},
};



void cSampleImporter::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	//	prop2.style = 	( controlStyleShow | controlStyleRightX | controlStyleCenterY);
	//	prop2.x = 769;
	//	if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);


	strControlProperties prop;
	frameData.placesCount = 2;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	prop.style = 0;
	prop.value = 0;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);


	prop2.style = 	(controlStyleBackground | controlStyleCenterX | controlStyleCenterY);
	prop2.x = (800/8);
	prop2.y = 452;
	prop2.w = 800/4-6;
	prop2.h = 58;
	if(topLabel[0] == nullptr) topLabel[0] = display.createControl<cLabel>(&prop2);


	for(uint8_t i = 2; i<6; i++)
	{
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 452;
		prop2.w = 800/8-6;
		prop2.h = 58;
		if(topLabel[i-1] == nullptr) topLabel[i-1] = display.createControl<cLabel>(&prop2);
	}

	prop2.x = (800/8)*6+(800/8);
	prop2.y = 452;
	prop2.w = 800/4-6;
	prop2.h = 58;
	if(topLabel[5] == nullptr) topLabel[5] = display.createControl<cLabel>(&prop2);

	explorerList.linesCount = 5;
	explorerList.start = 0;
	explorerList.length = locationExplorerCount;
	explorerList.data = explorerNames;

	//explorerList.selectTab=selectionTab;

	prop.x = 0+8;
	prop.y = 37;
	prop.w = 800/4-16;
	prop.h = 25;
	prop.data = &explorerList;
	if(explorerListControl == nullptr)  explorerListControl = display.createControl<cList>(&prop);

	instrumentList.linesCount = 5;
	instrumentList.start = 0;
	instrumentList.length = 0;
	//instrumentList.selectTab=inSelection;

	//strControlProperties prop;
	prop.x = (800/4)*3+8;
	//prop.y = 10;
	//prop.w = 800/4-10;
	//prop.h = 25;
	prop.data = &instrumentList;
	if(instrumentListControl == nullptr)  instrumentListControl = display.createControl<cList>(&prop);

	prop.x = 458;
//	prop.colors = &color[1];
	//prop.y = 10;
	//prop.w = 800/4-10;
	prop.style = controlStyleCompareTwoValues;
	prop.h = 380;
	prop.data = &memoryUsageAdd;
	//prop.value = memoryUsage;
	if(memoryBarControl == nullptr)  memoryBarControl = display.createControl<cBar>(&prop);




	prop.x = 190;
//	prop.colors = &color[0];
	prop.y = 170;
	//prop.w = 800/4-10;
	prop.style = controlStyleValue_0_100;
	prop.h = 100;
	prop.w = 420;
//	prop.value = 70;
//	prop.text = "loading...";
	if(loadHorizontalBarControl == nullptr)  loadHorizontalBarControl = display.createControl<cHorizontalBar>(&prop);
}


void cSampleImporter::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

//	display.destroyControl(instrumentLabel);
//	instrumentLabel = nullptr;

	display.destroyControl(explorerListControl);
	explorerListControl = nullptr;

	display.destroyControl(instrumentListControl);
	instrumentListControl = nullptr;

	for(uint8_t i = 0; i<6; i++)
	{
		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}

	display.destroyControl(memoryBarControl);
	memoryBarControl = nullptr;

	display.destroyControl(loadHorizontalBarControl);
	loadHorizontalBarControl = nullptr;

	display.destroyControl(frameControl);
	frameControl = nullptr;
}

void cSampleImporter::showDefaultScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Sample Loader");
	display.refreshControl(titleLabel);

//	showActualInstrument();


	display.setControlText(topLabel[0], "Micro SD");
	display.setControlText(topLabel[1], "");
	display.setControlText(topLabel[2], "Preview");
	display.setControlText(topLabel[3], "");
	display.setControlText(topLabel[4], "");
	display.setControlText(topLabel[5], "Instruments");

	for(uint8_t i = 0; i<6; i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	display.setControlHide(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);

	display.synchronizeRefresh();

	AddOrEnter();
	displayDelete(selectedPlace);

}


//==============================================================================================================

void cSampleImporter::showFilesTree()
{
	explorerList.start = selectedFile;
	explorerList.length = locationExplorerCount;
	explorerList.linesCount = 15;
	explorerList.data = explorerNames;

	display.setControlData(explorerListControl,  &explorerList);
	display.setControlShow(explorerListControl);
	display.refreshControl(explorerListControl);
}


void cSampleImporter::showInstrumentsList()
{
	instrumentList.start = selectedSlot;
	instrumentList.length = INSTRUMENTS_COUNT;
	instrumentList.linesCount = 15;
	instrumentList.data = interfaceGlobals.ptrIntrumentsNames;

	display.setControlData(instrumentListControl,  &instrumentList);
	display.setControlShow(instrumentListControl);
	display.refreshControl(instrumentListControl);
}

void cSampleImporter::showMemoryUsage()
{

	if(!fullMemoryFlag)
	{
		display.setControlColors(memoryBarControl, defaultColors);
		display.setControlValue(memoryBarControl, memoryUsage);
		display.setControlData(memoryBarControl,&memoryUsageAdd);
		display.setControlShow(memoryBarControl);
		display.refreshControl(memoryBarControl);
	}
	else
	{
		display.setControlColors(memoryBarControl, barColorsRed);
		display.setControlValue(memoryBarControl, 100);

		display.setControlData(memoryBarControl,(strCompareValue *)&memoryUsageAddOnMemoryFull);
		display.setControlShow(memoryBarControl);
		display.refreshControl(memoryBarControl);
	}
}

void cSampleImporter::showLoadHorizontalBar()
{
	display.setControlValue(loadHorizontalBarControl, loadProgress);
	display.setControlText(loadHorizontalBarControl, "loading...");
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

void cSampleImporter::showCopyingHorizontalBar()
{
	display.setControlValue(loadHorizontalBarControl, copyingProgress);
	display.setControlText(loadHorizontalBarControl, "copying...");
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

//==============================================================================================================

void cSampleImporter::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

//==============================================================================================================
void cSampleImporter::showActualInstrument()
{
	static char actualInstrName[SAMPLE_NAME_SIZE+4];

	uint8_t i = mtProject.values.lastUsedInstrument;

	sprintf(actualInstrName, "%d. ", i+1);

	strncat(&actualInstrName[0], mtProject.instrument[i].sample.file_name, SAMPLE_NAME_SIZE);


	display.setControlText(instrumentLabel,  actualInstrName);
	display.refreshControl(instrumentLabel);
}

void cSampleImporter::AddOrEnter()
{
	if(locationExplorerList[selectedFile][0] == '/')
	{
		display.setControlText(topLabel[1], "Enter");
	}
	else
	{
		display.setControlText(topLabel[1], "Add");
	}

	display.refreshControl(topLabel[1]);
}

void cSampleImporter::rewindListToBeggining()
{
	selectedFile=0;
	display.setControlValue(explorerListControl, selectedFile);
	display.refreshControl(explorerListControl);
}

void cSampleImporter::setSelect(uint8_t place)
{
	memset(selectionTab,0,sizeof(selectionTab));

	if(place == 0)
	{
		explorerList.selectTab=selectionTab;
		instrumentList.selectTab=NULL;
	}
	else
	{
		instrumentList.selectTab=selectionTab;
		explorerList.selectTab=NULL;
	}

	//display.setControlData(explorerListControl, &explorerList);
	//display.setControlData(instrumentListControl, &instrumentList);
}

// mode = 0 - bialy , mode = 1 - czerwony
void cSampleImporter::frameSelectMode(uint8_t place,uint8_t mode)
{
	if(place == 0)
	{
		explorerList.selectionActive=mode;
		display.refreshControl(explorerListControl);
	}
	else
	{
		instrumentList.selectionActive=mode;
		display.refreshControl(instrumentListControl);
	}
}

void cSampleImporter::displayDelete(uint8_t onOff)
{
	if(onOff)
	{
		display.setControlText(topLabel[4], "Delete");
	}
	else
	{
		display.setControlText(topLabel[4], "");
	}

	display.refreshControl(topLabel[4]);
}

//==============================================================================================================

