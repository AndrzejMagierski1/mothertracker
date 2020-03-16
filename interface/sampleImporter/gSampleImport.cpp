

#include "sampleImporter/sampleImporter.h"
#include "mtLED.h"
#include "core/interfacePopups.h"

static uint16_t framesPlaces[2][4] =
{
	{0+1, 		29, 800/4-3, 391},
	{(800/4)*3+1, 29, 800/4-3, 391},
};

static uint32_t barColorsRed[5] =
{
		one_true_red,
		one_true_red,
		one_true_red,
		0x080808,
		0x0a0a0a
};

static uint32_t defaultBarColors[5] =
{
		0xFFFFFF,
		one_true_red,
		one_true_green,	//0xf13c3c,
		0x080808,
		0x0a0a0a
};


void cSampleImporter::initDisplayControls()
{
	strControlProperties prop;

	prop.x = 190;
	prop.y = 170;
	prop.style = controlStyleValue_0_100;
	prop.h = 100;
	prop.w = 420;
	if(loadHorizontalBarControl == nullptr)  loadHorizontalBarControl = display.createControl<cHorizontalBar>(&prop);

	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY | controlStyleFont4);
	prop2.x = 9;
	prop2.y = 13;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	//	prop2.style = 	( controlStyleShow | controlStyleRightX | controlStyleCenterY);
	//	prop2.x = 769;
	//	if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 2;
	prop2.y = 0;
	prop2.w = 795;
	prop2.h = 26;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);


	frameData.placesCount = 2;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	prop.style = 0;
	prop.value = 0;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);


	labelArrow[0].bitmaps[0].bitmapIndex = displayArrowU;
	labelArrow[0].bitmaps[0].xValue =  (800/8)*0+(800/16);
	labelArrow[0].bitmaps[0].yValue = 460;
	labelArrow[0].bitmaps[1].bitmapIndex = displayArrowD;
	labelArrow[0].bitmaps[1].xValue =  (800/8)*1+(800/16);
	labelArrow[0].bitmaps[1].yValue = 460;

	labelArrow[1].bitmaps[0].bitmapIndex = displayArrowU;
	labelArrow[1].bitmaps[0].xValue =  (800/8)*6+(800/16);
	labelArrow[1].bitmaps[0].yValue = 460;
	labelArrow[1].bitmaps[1].bitmapIndex = displayArrowD;
	labelArrow[1].bitmaps[1].xValue =  (800/8)*7+(800/16);
	labelArrow[1].bitmaps[1].yValue = 460;


	prop2.value = 1;
	prop2.colors = interfaceGlobals.activeLabelsColors;
	prop2.style = 	(controlStyleCenterX | controlStyleFont3);
	prop2.x = (800/8);
	prop2.y = 424;
	prop2.w = 800/4-6;
	prop2.h = 55;
	if(label[0] == nullptr) label[0] = display.createControl<cLabel>(&prop2);

	for(uint8_t i = 0; i < 8; i++)
	{
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;

		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);
	}

	prop2.text = nullptr;
	prop2.colors = interfaceGlobals.activeBgLabelsColors;
	prop2.value = 125;
	prop2.style = controlStyleNoTransparency | controlStyleShow;
	prop2.x = 0;
	prop2.w = 800;
	prop2.y = 424;
	prop2.h =  55;
	if(bgLabel == nullptr) bgLabel = display.createControl<cBgLabel>(&prop2);



	explorerList.start = selectedFile;
	explorerList.length = locationExplorerCount;
	explorerList.linesCount = 14;
	explorerList.data = explorerNames;
	explorerList.lineLengthMax = 22;
	prop.x = 0+1;
	prop.y = 29;
	prop.w = 800/4-3;
	prop.h = 394;
	prop.style = controlStyleBackground;
	prop.data = &explorerList;
	if(explorerListControl == nullptr)  explorerListControl = display.createControl<cList>(&prop);

	instrumentList.linesCount = 14;
	instrumentList.start = 0;
	instrumentList.length = 0;
	instrumentList.lineLengthMax = 22;
	prop.x = (800/4)*3+1;
	prop.data = &instrumentList;
	if(instrumentListControl == nullptr)  instrumentListControl = display.createControl<cList>(&prop);


	prop.x = (800/8)*5+1;
	prop.w = 800/8-3;
	prop.style = controlStyleCompareTwoValues | controlStyleBackground;
	prop.data = &memoryUsageAdd;
	if(memoryBarControl == nullptr)  memoryBarControl = display.createControl<cBar>(&prop);


	strControlProperties prop3;
	prop3.x = 13;
	prop3.y = 143;
	prop3.w = 780;
	prop3.h = 260;
	if(keyboardControl == nullptr)  keyboardControl = display.createControl<cKeyboard>(&prop3);

	strControlProperties prop4;
	prop4.text = (char*)"";
	prop4.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleFont2);
	prop4.x = 400;
	prop4.y = 29;
	prop4.w = 795;
	prop4.h = 90;
	if(editName == nullptr)  editName = display.createControl<cEdit>(&prop4);
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

	for(uint8_t i = 0; i < 8; i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;
	}

	display.destroyControl(bgLabel);
	bgLabel = nullptr;


	display.destroyControl(memoryBarControl);
	memoryBarControl = nullptr;

	display.destroyControl(loadHorizontalBarControl);
	loadHorizontalBarControl = nullptr;

	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(keyboardControl);
	keyboardControl = nullptr;

	display.destroyControl(editName);
	editName = nullptr;

	mtPopups.hideInfoPopup();
}

void cSampleImporter::showDefaultScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Sample Loader");
	display.refreshControl(titleLabel);

//	showActualInstrument();
	display.setControlData(label[0], &labelArrow[0]);
	display.setControlData(label[6], &labelArrow[1]);

	display.setAddControlStyle(label[0], controlStyleShowBitmap);
	display.setAddControlStyle(label[6], controlStyleShowBitmap);

	display.setControlText(label[0], "Micro SD");
	display.setControlText(label[1], "");
	display.setControlText(label[2], "");
	display.setControlText(label[4], "Preview");
	display.setControlText(label[5], "Memory");
	display.setControlText(label[6], "Instruments");
	display.setControlText(label[7],"");

	display.setControlPosition(label[0],  (800/8)*0+(800/8),  424);
	display.setControlSize(label[0],  800/4-6,  55);

	display.setControlPosition(label[6],  (800/8)*6+(800/8),  424);
	display.setControlSize(label[6],  800/4-6,  55);


	display.setControlColors(label[0], interfaceGlobals.activeLabelsColors);
	display.setControlColors(label[1], interfaceGlobals.activeLabelsColors);
	display.setControlColors(label[2], interfaceGlobals.activeButtonLabelsColors);
	display.setControlColors(label[3], interfaceGlobals.activeButtonLabelsColors);
	display.setControlColors(label[4], interfaceGlobals.activeButtonLabelsColors);
	display.setControlColors(label[5], interfaceGlobals.activeLabelsColors);
	display.setControlColors(label[6], interfaceGlobals.activeLabelsColors);
	display.setControlColors(label[7], interfaceGlobals.activeLabelsColors);


	for(uint8_t i = 0; i < 8; i++)
	{
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.setControlValue(bgLabel, 125);
	display.refreshControl(bgLabel);


	display.setControlHide(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);


	keyboardManager.deactivateKeyboard();

	display.setControlHide(editName);
	display.refreshControl(editName);

	//displayDelete(selectedPlace);

	if(locationExplorerCount != 0)
	{

		AddEnterOrRename();
		AddNextControl();

	}

	activateLabelsBorder();

	display.synchronizeRefresh();

}

//==============================================================================================================
void cSampleImporter::deactivateGui()
{
	showDefaultScreen();

	for(uint8_t i = 0; i < 8; i++)
	{
		display.setControlColors(label[i], interfaceGlobals.inactiveLabelsColors);
		display.refreshControl(label[i]);
	}

	display.setControlHide(explorerListControl);
	display.setControlHide(frameControl);

	display.synchronizeRefresh();
}

//==============================================================================================================

void cSampleImporter::showFilesTree()
{
	explorerList.start = selectedFile;
	explorerList.length = locationExplorerCount;
	explorerList.linesCount = 14;
	explorerList.data = explorerNames;

	display.setControlData(explorerListControl,  &explorerList);
	display.setControlShow(explorerListControl);
	display.refreshControl(explorerListControl);
	display.synchronizeRefresh();
}


void cSampleImporter::showInstrumentsList()
{
	instrumentList.start = selectedSlot;
	instrumentList.length = INSTRUMENTS_COUNT;
	instrumentList.linesCount = 14;
	instrumentList.data = interfaceGlobals.ptrIntrumentsNames;

	display.setControlData(instrumentListControl,  &instrumentList);
	display.setControlShow(instrumentListControl);
	display.refreshControl(instrumentListControl);
}

//void cSampleImporter::setInstrumentsListPos(uint8_t pos)
//{
//	display.setControlValue(instrumentListControl, pos);
//	display.setControlShow(instrumentListControl);
//	display.refreshControl(instrumentListControl);
//}


void cSampleImporter::refreshInstrumentsList()
{
	display.setControlShow(instrumentListControl);
	display.refreshControl(instrumentListControl);
}

void cSampleImporter::showMemoryUsage()//xxx
{

	if(!fullMemoryFlag)
	{
		display.setControlColors(memoryBarControl, defaultBarColors);
		display.setControlValue(memoryBarControl, memoryUsage);
		display.setControlData(memoryBarControl,&memoryUsageAdd);
		display.setControlShow(memoryBarControl);
		display.refreshControl(memoryBarControl);

		sprintf(memoryUsageValue, "%d%c", memoryUsage,'%');
		display.setControlText2(label[5], memoryUsageValue);
		display.refreshControl(label[5]);

	}
	else
	{
		display.setControlColors(memoryBarControl, barColorsRed);
		display.setControlValue(memoryBarControl, 100);

		display.setControlData(memoryBarControl,(strCompareValue *)&memoryUsageAddOnMemoryFull);
		display.setControlShow(memoryBarControl);
		display.refreshControl(memoryBarControl);


		display.setControlText2(label[5], "100%");
		display.refreshControl(label[5]);
	}
}

void cSampleImporter::showLoadHorizontalBar()
{
	display.setControlValue(loadHorizontalBarControl, loadProgress);
	display.setControlText(loadHorizontalBarControl, "Loading...");
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

void cSampleImporter::showCopyingHorizontalBar()
{
	if(copyElementsCount > 1) sprintf(copyingInfo, "Copying %d/%d ...",copyElement,copyElementsCount);
	else strcpy(copyingInfo,"copying...");
	display.setControlValue(loadHorizontalBarControl, copyingProgress);
	display.setControlText(loadHorizontalBarControl, copyingInfo);
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

void cSampleImporter::showDeletingHorizontalBar(uint8_t progress)
{
	display.setControlValue(loadHorizontalBarControl, progress);
	display.setControlText(loadHorizontalBarControl, "Deleting...");
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

void cSampleImporter::showOpeningHorizontalBar(uint8_t progress)
{
	display.setControlValue(loadHorizontalBarControl, progress);
	display.setControlText(loadHorizontalBarControl, "Opening...");
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

void cSampleImporter::hideHorizontalBar()
{
	display.setControlHide(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

void cSampleImporter::showStopPatternPopup()
{
	mtPopups.showInfoPopup("This action will stop the pattern.", "Do you want to continue?");

	for(uint8_t i = 0 ; i < 8; i ++)
	{
		display.setControlStyle(label[i], (controlStyleCenterX | controlStyleFont3));
		display.setControlStyle2(label[i], (controlStyleCenterX | controlStyleFont2));
		display.setControlPosition(label[i],  (800/8)*i+(800/16),  424);
		display.setControlSize(label[i],  800/8-6,  55);
		display.setControlText(label[i],"");
		display.setControlText2(label[i],"");
	}
	display.setControlText(label[6], "No");
	display.setControlShow(label[6]);
	display.setControlText(label[7], "Yes");
	display.setControlShow(label[7]);

	for(uint8_t i = 0 ; i < 8; i ++)
	{
		display.refreshControl(label[i]);
	}


	display.setControlValue(bgLabel, 255);
	display.refreshControl(bgLabel);

	display.synchronizeRefresh();
}
void cSampleImporter::hideStopPatternPopup()
{
	mtPopups.hideInfoPopup();

	showDefaultScreen();
	showMemoryUsage();

	display.synchronizeRefresh();
}

//==============================================================================================================

void cSampleImporter::activateLabelsBorder()
{
	frameSelectMode(selectedPlace, 1);

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

	if(i < INSTRUMENTS_COUNT)
	{
		sprintf(actualInstrName, "%d. ", i+1);
		strncat(&actualInstrName[0], mtProject.instrument[i].sample.file_name, SAMPLE_NAME_SIZE);
	}
	else
	{
		//i = i-(INSTRUMENTS_COUNT-1);
		sprintf(actualInstrName, "%d. MIDI Channel %d",  i+3, i);
	}

	display.setControlText(instrumentLabel,  actualInstrName);
	display.refreshControl(instrumentLabel);
}

void cSampleImporter::AddNextDisplay(uint8_t active)
{
	display.setControlText(label[3], "Add next");

	if(active)
	{
		display.setControlColors(label[3],  interfaceGlobals.activeButtonLabelsColors);
	}
	else
	{
		display.setControlColors(label[3], interfaceGlobals.inactiveButtonLabelsColors);
	}

	display.refreshControl(label[3]);
	//display.synchronizeRefresh();
}

void cSampleImporter::AddEnterOrRename()
{
	if(selectedPlace == 0)
	{
		if(*explorerNames[selectedFile] == '/')
		{
			display.setControlText(label[2], "Enter");
		}
		else
		{
			display.setControlText(label[2], "Add");
		}
	}
	else
	{
		display.setControlText(label[2], "Rename");
	}

	renameColorControl();

	display.refreshControl(label[2]);
	display.synchronizeRefresh();
}

void cSampleImporter::previewColorControl()
{
	uint32_t *colors = interfaceGlobals.activeButtonLabelsColors;
	if(selectedPlace == 0)
	{
		if(*explorerNames[selectedFile] == '/')
		{
			colors = interfaceGlobals.inactiveButtonLabelsColors;
		}
	}
	else
	{
		if(mtProject.instrument[selectedSlot].isActive != 1)
		{
			colors = interfaceGlobals.inactiveButtonLabelsColors;
		}
	}

	display.setControlColors(label[4], colors);
	display.refreshControl(label[4]);

	display.synchronizeRefresh();
}

void cSampleImporter::renameColorControl()
{
	uint32_t *colors = interfaceGlobals.activeButtonLabelsColors;

	if(selectedPlace == 1)
	{
		if(mtProject.instrument[selectedSlot].isActive != 1)
		{
			colors = interfaceGlobals.inactiveButtonLabelsColors;
		}
	}

	display.setControlColors(label[2], colors);
	// refreshowany jest w innej funkcji
}

void cSampleImporter::deleteColorControl()
{
	uint32_t *colors = interfaceGlobals.activeButtonLabelsColors;

	if(selectedPlace == 1)
	{
		if((selectionLength[listInstruments] > 1) && (currSelectPlace == 1))
		{
			if(checkIfAnyInstrActive() == 0)
			{
				colors = interfaceGlobals.inactiveButtonLabelsColors;
			}
		}
		else
		{
			if(mtProject.instrument[selectedSlot].isActive != 1)
			{
				colors = interfaceGlobals.inactiveButtonLabelsColors;
			}
		}
	}

	display.setControlColors(label[5], colors);
	display.synchronizeRefresh();
	// refreshowany jest w innej funkcji
}


void cSampleImporter::showFileList()
{
	display.setControlShow(explorerListControl);
	display.refreshControl(explorerListControl);
}

void cSampleImporter::rewindListToBeggining()
{
	selectedFile=0;
	display.setControlValue(explorerListControl, selectedFile);
	display.refreshControl(explorerListControl);
}

void cSampleImporter::moveInstrListToEnd()
{
	selectedSlot = 47;
	display.setControlValue(instrumentListControl, selectedSlot);
	display.refreshControl(instrumentListControl);
}

void cSampleImporter::setSelect()
{
	explorerList.selectTab = &selectionTab[listFiles][0];
	instrumentList.selectTab = &selectionTab[listInstruments][0];
}

// mode = 0 - bialy , mode = 1 - czerwony
void cSampleImporter::frameSelectMode(uint8_t place,uint8_t mode)
{
	if(place == 0)
	{
		explorerList.selectionActive = mode;
		display.refreshControl(explorerListControl);

		instrumentList.selectionActive = 0;
		display.refreshControl(instrumentListControl);
	}
	else
	{
		instrumentList.selectionActive = mode;
		display.refreshControl(instrumentListControl);

		explorerList.selectionActive = 0;
		display.refreshControl(explorerListControl);
	}
}

//void cSampleImporter::displayDelete(uint8_t onOff)
//{
//	if(onOff)
//	{
//		display.setControlText(label[5], "Delete");
//		deleteColorControl();
//	}
//	else
//	{
//		display.setControlText(label[5], "");
//	}
//
//	display.refreshControl(label[5]);
//	//display.synchronizeRefresh();
//}
//




//todo
void cSampleImporter::showRenameKeyboard()
{
	display.setControlPosition(label[0],  (800/8)*0+(800/16),  424);
	display.setControlSize(label[0],  800/8-6,  55);

	display.setControlPosition(label[6],  (800/8)*6+(800/16),  424);
	display.setControlSize(label[6],  800/8-6,  55);

	for(uint8_t i = 0; i < 8; i++)
	{
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");
		display.setRemoveControlStyle(label[i], controlStyleShowBitmap);
		display.setControlColors(label[i], interfaceGlobals.activeButtonLabelsColors);
	}

	display.setControlText(label[0],"Enter");
	display.setControlText(label[5],"Auto name");
	display.setControlText(label[6],"Cancel");
	display.setControlText(label[7],"Rename");


	for(uint8_t i = 0; i < 8; i++)
	{
		display.refreshControl(label[i]);
	}


	display.setControlHide(fileListControl);
	display.refreshControl(fileListControl);
	display.setControlHide(folderListControl);
	display.refreshControl(folderListControl);
	display.setControlHide(explorerListControl);
	display.refreshControl(explorerListControl);
	display.setControlHide(instrumentListControl);
	display.refreshControl(instrumentListControl);
	display.setControlHide(memoryBarControl);
	display.refreshControl(memoryBarControl);
	display.setControlHide(frameControl);
	display.refreshControl(frameControl);



	display.setControlValue(bgLabel, 226);
	display.refreshControl(bgLabel);

	display.synchronizeRefresh();

}


//==============================================================================================================

