

#include <sampleImporter.h>




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
	prop.y = 10;
	prop.w = 800/4-10;
	prop.h = 25;
	prop.data = &fileList;
	if(fileListControl == nullptr)  fileListControl = display.createControl<cList>(&prop);


	instrumentList.linesCount = 5;
	instrumentList.start = 0;
	instrumentList.length = 0;
	//strControlProperties prop;
	prop.x = (800/4)*2+5;
	prop.y = 10;
	prop.w = 800/4-10;
	prop.h = 25;
	prop.data = &instrumentList;
	if(instrumentListControl == nullptr)  instrumentListControl = display.createControl<cList>(&prop);

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
}

void cSampleImporter::showDefaultScreen()
{

	display.setControlText(topLabel[0], "SD");
	display.setControlText(topLabel[1], "");
	display.setControlText(topLabel[2], "Instruments");
	display.setControlText(topLabel[3], "Memory");

	display.setControlText(bottomLabel[0], " \\\/           /\\\ ");
	display.setControlText(bottomLabel[1], " \\\/           /\\\ ");
	display.setControlText(bottomLabel[2], " \\\/           /\\\ ");
	display.setControlText(bottomLabel[3], " Add       Delete");

	for(uint8_t i = 0; i<4; i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);

		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}

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
}


void cSampleImporter::showFilesTree()
{
	fileList.start = 0;
	fileList.length = locationFileCount;
	fileList.linesCount = 16;
	fileList.data = fileNames;

	display.setControlData(fileListControl,  &fileList);
	display.setControlShow(fileListControl);
	display.refreshControl(fileListControl);
}


//==============================================================================================================

void cSampleImporter::activateLabelsBorder()
{
	for(uint8_t i = 0; i<4; i++)
	{
		display.setRemoveControlStyle(bottomLabel[i], controlStyleBorder);
		display.refreshControl(bottomLabel[i]);
	}

	if(selectedLabel > 0)
	{
		display.setAddControlStyle(bottomLabel[selectedLabel-1], controlStyleBorder);
	}
}

