

#include "configEditor.h"



static uint16_t framesPlacesConfig[4][4]=
{
	{(800/8)*0+1, 29, 800/4-3, 391},
	{(800/8)*2+1, 29, 600/2-3, 391},
	{(800/8)*5+1, 29, 800/4-3, 391},
	{(800/8)*5+1, 29, 600/2-3, 391},
};


static uint32_t popUpLabelColors[] =
{
	0xFFFFFF, // tekst
	0x0a0a0a, // tło
	one_true_red, // ramka
};


void cConfigEditor::initDisplayControls()
{
	// inicjalizacja kontrolek

	strControlProperties prop;
	// ramka
	prop.style = 0;
	prop.value = 0;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);


	strControlProperties prop9;
	prop9.x = 400;
	prop9.colors = popUpLabelColors;
	prop9.y = 350;
	prop9.h = 100;
	prop9.w = 800-10;
	prop9.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleFont2 | controlStyleRoundedBorder);
	prop9.text = (char*)"";
	if(popoutWindowLabel == nullptr)  popoutWindowLabel = display.createControl<cLabel>(&prop9);


	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY | controlStyleFont4);
	prop2.x = 9;
	prop2.y = 13;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 2;
	prop2.y = 0;
	prop2.w = 795;
	prop2.h = 26;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);

	labelArrow[0].bitmaps[0].bitmapIndex = displayArrowU;
	labelArrow[0].bitmaps[0].xValue =  (800/8)*0+(800/16);
	labelArrow[0].bitmaps[0].yValue = 460;
	labelArrow[0].bitmaps[1].bitmapIndex = displayArrowD;
	labelArrow[0].bitmaps[1].xValue =  (800/8)*1+(800/16);
	labelArrow[0].bitmaps[1].yValue = 460;

	labelArrow[1].bitmaps[0].bitmapIndex = displayArrowU;
	labelArrow[1].bitmaps[0].xValue =  (800/8)*2+(800/16);
	labelArrow[1].bitmaps[0].yValue = 460;
	labelArrow[1].bitmaps[1].bitmapIndex = displayArrowD;
	labelArrow[1].bitmaps[1].xValue =  (800/8)*3+(800/16);
	labelArrow[1].bitmaps[1].yValue = 460;

	labelArrow[2].bitmaps[0].bitmapIndex = displayArrowU;
	labelArrow[2].bitmaps[0].xValue =  (800/8)*5+(800/16);
	labelArrow[2].bitmaps[0].yValue = 460;
	labelArrow[2].bitmaps[1].bitmapIndex = displayArrowD;
	labelArrow[2].bitmaps[1].xValue =  (800/8)*6+(800/16);
	labelArrow[2].bitmaps[1].yValue = 460;

	for(uint8_t i = 0; i<8; i++)
	{
		prop2.value = 1;
		prop2.colors = interfaceGlobals.activeLabelsColors;
		prop2.style = 	( controlStyleCenterX | controlStyleFont3);
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 424;
		prop2.h =  55;
		if(i == 0) {prop2.style |= controlStyleShowBitmap; prop2.data = &labelArrow[0];}
		if(i == 2) {prop2.style |= controlStyleShowBitmap; prop2.data = &labelArrow[1];}
		if(i == 5) {prop2.style |= controlStyleShowBitmap; prop2.data = &labelArrow[2];}
		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);

	}

	prop2.text = nullptr;
	prop2.colors = interfaceGlobals.activeBgLabelsColors;
	prop2.value = 36; //36;
	prop2.style = controlStyleNoTransparency | controlStyleShow;
	prop2.x = 0;
	prop2.w = 800;
	prop2.y = 424;
	prop2.h =  55;
	if(bgLabel == nullptr) bgLabel = display.createControl<cBgLabel>(&prop2);




	prop.style = controlStyleBackground;
	prop.x = (800/8)*0+1;
	prop.y = 29;
	prop.w = 800/4-3;
	prop.h = 394;
	if(configBasemenuListControl == nullptr)  configBasemenuListControl = display.createControl<cList>(&prop);

	prop.x = (800/8)*2+1;
	prop.y = 29;
	prop.w = 600/2-3;
	prop.h = 394;
	if(configSubmenuListControl == nullptr) configSubmenuListControl = display.createControl<cParamValueList>(&prop);

	prop.x = (800/8)*5+1;
	prop.y = 29;
	prop.w = 600/2-3;
	prop.h = 394;
	if(configSecondSubmenuListControl == nullptr) configSecondSubmenuListControl = display.createControl<cParamValueList>(&prop);


	prop.style = controlStyleBackground;
	prop.x = (800/8)*5+1;
	prop.y = 29;
	prop.w = 800/4-3;
	prop.h = 394;
	if(configListControl == nullptr)  configListControl = display.createControl<cList>(&prop);


}


void cConfigEditor::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(configBasemenuListControl);
	configBasemenuListControl = nullptr;

	display.destroyControl(configSubmenuListControl);
	configSubmenuListControl = nullptr;

	display.destroyControl(configSecondSubmenuListControl);
	configSecondSubmenuListControl = nullptr;

	display.destroyControl(configListControl);
	configListControl = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;
	}

	display.destroyControl(bgLabel);
	bgLabel = nullptr;

	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(popoutWindowLabel);
	popoutWindowLabel = nullptr;
}

void cConfigEditor::showDefaultConfigScreen()
{

	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);

		display.setControlHide(barControl[i]);
		display.refreshControl(barControl[i]);
	}

	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Config");
	display.refreshControl(titleLabel);


	display.setControlText(label[0], "");
	display.setControlText(label[1], "");
	display.setControlText(label[2], "");
	display.setControlText(label[3], "");
	display.setControlText(label[4], "Change");
	display.setControlText(label[5], "");
	display.setControlText(label[6], "");
	display.setControlText(label[7], "");


	display.setControlHide(label[5]);
	display.setControlHide(label[6]);


	frameData.placesCount = 4;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlacesConfig[0][0];
	frameData.places[1] = &framesPlacesConfig[1][0];
	frameData.places[2] = &framesPlacesConfig[2][0];
	frameData.places[3] = &framesPlacesConfig[3][0];

	display.refreshControl(bgLabel);

	display.setControlHide(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	activateLabelsBorder();
	display.synchronizeRefresh();

}


//==============================================================================================================
void cConfigEditor::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

void cConfigEditor::selectSubmenu()
{
	selectedPlace = 1;
	activateLabelsBorder();
}

void cConfigEditor::selectSecondSubmenu()
{
	selectedPlace = 3;
	activateLabelsBorder();
}

void cConfigEditor::selectConfigList()
{
	selectedPlace = 2;
	activateLabelsBorder();
}





void cConfigEditor::changeLabelText(uint8_t labelIdx, const char *text)
{
	display.setControlText(label[labelIdx], text);
	display.setControlShow(label[labelIdx]);
	display.refreshControl(label[labelIdx]);
}





void cConfigEditor::showConfigList5(uint8_t start, uint8_t length, char** listText)
{
	configListShown = 1;
	selectedConfigListPosition = start;

	display.setControlPosition(configListControl, (800/8)*5+1,  -1);

	configList.linesCount = 13;
	configList.start = start;
	configList.length = length;
	configList.data = listText;

	display.setControlData(configListControl, &configList);
	display.setControlShow(configListControl);
	display.refreshControl(configListControl);


	//display.setControlText(label[7], "");
	display.setControlShow(label[5]);
	display.setControlShow(label[6]);
	display.setControlShow(label[7]);


	display.refreshControl(label[7]);
}

void cConfigEditor::hideConfigList()
{
	flashingState = 0;

	display.setControlHide(configListControl);


	//display.setControlText(label[7], "");

	display.setControlHide(label[5]);
	display.setControlHide(label[6]);
	display.setControlHide(label[7]);

	display.refreshControl(label[7]);

	configListShown = 0;
}


void cConfigEditor::showSubmenu()
{
	submenuShown = 1;

	display.setControlShow(configSubmenuListControl);
	display.setControlShow(label[2]);
	display.setControlShow(label[3]);
	display.setControlShow(label[4]);

	display.refreshControl(label[4]);

}

void cConfigEditor::hideSubmenu()
{
	display.setControlHide(configSubmenuListControl);
	display.setControlHide(label[2]);
	display.setControlHide(label[3]);
	display.setControlHide(label[4]);

	display.refreshControl(label[4]);

	submenuShown = 0;
}

void cConfigEditor::showSecondSubmenu()
{
	secondSubmenuShown = 1;

	display.setControlShow(configSecondSubmenuListControl);
	display.setControlShow(label[5]);
	display.setControlShow(label[6]);
	display.setControlShow(label[7]);

	display.refreshControl(label[7]);

}

void cConfigEditor::hideSecondSubmenu()
{
	display.setControlHide(configSecondSubmenuListControl);
	display.setControlHide(label[5]);
	display.setControlHide(label[6]);
	display.setControlHide(label[7]);

	display.refreshControl(label[7]);

	secondSubmenuShown = 0;
}

//==============================================================================================================
//==============================================================================================================
//==============================================================================================================


void cConfigEditor::showFirmwareUpdatePopout()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(label[i], "");
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.setControlText(label[6], "Cancel");
	display.setControlText(label[7], "Update");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	display.setControlText(popoutWindowLabel,"Do you want to flash new firmware?");
	display.setControlShow(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	display.synchronizeRefresh();
}


void cConfigEditor::hideFirmwareUpdatePopout()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(label[i], "");
		display.refreshControl(label[i]);
	}

	display.setControlShow(frameControl);
	display.refreshControl(frameControl);

	display.setControlHide(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	display.synchronizeRefresh();
}

void cConfigEditor::resizeLabelConfigDefault()
{
	for(int i=0;i<8;i++)
	{
		display.setControlText(label[i],"");
		display.setControlValue(label[i], 0);
		display.refreshControl(label[i]);
	}
}

void cConfigEditor::resizeLabelConfigMaster()
{
	for(int i=0;i<8;i++)
	{
		display.setControlValue(label[i], 1);
		display.refreshControl(label[i]);
	}
}

void cConfigEditor::resizeFirmwareLabel(uint8_t control)// 0 - revert to normal, 1 - rescale to double label
{
	if(control)
	{
		//display.setControlPosition(bottomLabel[6],  (800/8)*0+(800/8),  465);
		display.setControlPosition(label[0],  (800/8)*0+(800/8),  452);
		//display.setControlSize(bottomLabel[6],  800/4-6,  30);
		display.setControlSize(label[0],  800/4-6,  58);
	}
	else
	{
		display.setControlPosition(label[0],  (800/8)*0+(800/16),  452);
		display.setControlSize(label[0],  800/8-6,  59);
	}

	display.refreshControl(label[0]);

}






