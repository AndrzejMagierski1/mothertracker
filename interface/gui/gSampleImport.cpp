

#include <sampleImporter.h>


static uint16_t framesPlaces[3][4] =
{
	{0, 0, 800/4, 480},
	{(800/4)*1, 0, 800/4, 480},
	{(800/4)*2, 0, 800/4, 480},
};

static uint32_t color[3] = {0xFF00FF, 0x0000ff, 0xff0000};


void cSampleImporter::initDisplayControls()
{
	strControlProperties prop2;
	prop2.text = (char*)"";
	prop2.style = 	( controlStyleBackground | controlStyleCenterX /*| controlStyleRoundedBorder*/);

	// inicjalizacja kontrolek
	for(uint8_t i = 0; i<4; i++)
	{
		prop2.x = (800/4)*i+(800/8);
		prop2.y = 420;
		prop2.w = 800/4-10;
		prop2.h = 26;

		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);

		//prop2.x = (800/4)*i+(800/8);
		prop2.y = 450;
		//prop2.w = 800/4-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);
	}


	folderList.linesCount = 5;
	folderList.start = 0;
	folderList.length = locationFolderCount;
	folderList.data = folderNames;
	strControlProperties prop;
	prop.x = 0+5;
	prop.y = 10;
	prop.w = 800/4-10;
	prop.h = 25;
	prop.data = &folderList;
	if(folderListControl == nullptr)  folderListControl = display.createControl<cList>(&prop);


	fileList.linesCount = 5;
	fileList.start = 0;
	fileList.length = 0;
	//strControlProperties prop;
	prop.x = (800/4)*1+5;
	//prop.y = 10;
	//prop.w = 800/4-10;
	//prop.h = 25;
	prop.data = &fileList;
	if(fileListControl == nullptr)  fileListControl = display.createControl<cList>(&prop);


	instrumentList.linesCount = 5;
	instrumentList.start = 0;
	instrumentList.length = 0;
	//strControlProperties prop;
	prop.x = (800/4)*2+5;
	//prop.y = 10;
	//prop.w = 800/4-10;
	//prop.h = 25;
	prop.data = &instrumentList;
	if(instrumentListControl == nullptr)  instrumentListControl = display.createControl<cList>(&prop);


	prop.x = (800/4)*3+5;
//	prop.colors = &color[1];
	//prop.y = 10;
	//prop.w = 800/4-10;
	prop.style = controlStyleValue_0_100;
	prop.h = 400;
	//prop.value = memoryUsage;
	if(memoryBarControl == nullptr)  memoryBarControl = display.createControl<cBar>(&prop);


	prop.x = (800/4)*3+5;
//	prop.colors = &color[0];
	//prop.y = 10;
	//prop.w = 800/4-10;
	prop.style = controlStyleValue_0_100;
	prop.h = 400;
	//prop.value = memoryUsage;
	if(addMemoryBarControl == nullptr)  addMemoryBarControl = display.createControl<cBar>(&prop);



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


	frameData.placesCount = 3;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	prop.style = 0;
	prop.value = 0;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);
}


void cSampleImporter::destroyDisplayControls()
{
	display.destroyControl(folderListControl);
	folderListControl = nullptr;
	display.destroyControl(fileListControl);
	fileListControl = nullptr;
	display.destroyControl(instrumentListControl);
	instrumentListControl = nullptr;

	for(uint8_t i = 0; i<4; i++)
	{
		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;

		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;
	}

	display.destroyControl(memoryBarControl);
	memoryBarControl = nullptr;

	display.destroyControl(addMemoryBarControl);
	addMemoryBarControl = nullptr;

	display.destroyControl(loadHorizontalBarControl);
	loadHorizontalBarControl = nullptr;

	display.destroyControl(frameControl);
	frameControl = nullptr;
}

void cSampleImporter::showDefaultScreen()
{

	display.setControlText(topLabel[0], "SD");
	display.setControlText(topLabel[1], "");
	display.setControlText(topLabel[2], "Instruments");
	display.setControlText(topLabel[3], "Memory");

	display.setControlText(bottomLabel[0], " /\\           \\/ ");
	display.setControlText(bottomLabel[1], " /\\           \\/ ");
	display.setControlText(bottomLabel[2], " /\\           \\/ ");
	display.setControlText(bottomLabel[3], " Add       Delete");

	for(uint8_t i = 0; i<4; i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);

		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}

	display.setControlHide(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);

	display.synchronizeRefresh();

}


//==============================================================================================================
void cSampleImporter::showFolderTree()
{
	folderList.start = 0;
	folderList.length = locationFolderCount;
	folderList.linesCount = 16;
	folderList.data = folderNames;

	display.setControlData(folderListControl,  &folderList);
	display.setControlShow(folderListControl);
	display.refreshControl(folderListControl);


	display.setControlText(topLabel[0],actualPath);
	display.refreshControl(topLabel[0]);
}


void cSampleImporter::showFilesTree()
{
	instrumentList.start = 0;
	instrumentList.length = locationFileCount;
	instrumentList.linesCount = 16;
	instrumentList.data = fileNames;

	display.setControlData(fileListControl,  &instrumentList);
	display.setControlShow(fileListControl);
	display.refreshControl(fileListControl);
}


void cSampleImporter::showInstrumentsList()
{
	fileList.start = selectedSlot;
	fileList.length = INSTRUMENTS_COUNT;
	fileList.linesCount = 16;
	fileList.data = ptrSlotNames;

	display.setControlData(instrumentListControl,  &fileList);
	display.setControlShow(instrumentListControl);
	display.refreshControl(instrumentListControl);
}

void cSampleImporter::showMemoryUsage()
{

	if(!fullMemoryFlag) display.setControlColors(addMemoryBarControl, defaultColors);
	else
	{
		display.setControlColors(addMemoryBarControl, barColorsRed);
	}

	display.setControlValue(addMemoryBarControl, memoryUsageAdd);
	display.setControlShow(addMemoryBarControl);
	display.refreshControl(addMemoryBarControl);

	if(!fullMemoryFlag)
	{
		display.setControlColors(memoryBarControl, defaultColors);
		display.setControlValue(memoryBarControl, memoryUsage);
		display.setControlShow(memoryBarControl);
		display.refreshControl(memoryBarControl);
	}
	else
	{
		display.setControlColors(memoryBarControl, barColorsRed);
		display.setControlValue(memoryBarControl, 100);
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

//==============================================================================================================

void cSampleImporter::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}
