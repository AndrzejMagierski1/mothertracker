

#include "configEditor.h"



static uint16_t framesPlacesConfig[4][4]=
{
	{(800/8)*0+1, 29, 800/4-3, 391},
	{(800/8)*2+1, 29, 800/4-3, 391},
	{(800/8)*4+1, 29, 800/4-3, 391},
	{(800/8)*6+1, 29, 800/4-3, 391},
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


	for(uint8_t i = 0; i<8; i++)
	{
		prop2.value = 1;
		prop2.colors = interfaceGlobals.activeLabelsColors;
		prop2.style = 	( controlStyleCenterX | controlStyleFont3);
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 424;
		prop2.h =  55;
		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);

	}

	prop2.text = nullptr;
	prop2.colors = interfaceGlobals.activeBgLabelsColors;
	prop2.value = 84;
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
	if(configBasemenuListControl == nullptr)  configBasemenuListControl= display.createControl<cList>(&prop);

	prop.x = (800/8)*2+1;
	prop.y = 29;
	prop.w = 800/4-3;
	prop.h = 394;
	if(configSubmenuListControl == nullptr) configSubmenuListControl = display.createControl<cParamValueList>(&prop);


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

	resizeLabelConfigDefault();

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



	display.setControlText(label[0], "Config");
	display.setControlText(label[1], "");
	display.setControlText(label[2], "");
	display.setControlText(label[3], "");
	display.setControlText(label[4], "");
	display.setControlText(label[5], "");
	display.setControlText(label[6], "");
	//display.setControlText(bottomLabel[7], "");

	resizeToDefaultConfig();


	frameData.placesCount = 4;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlacesConfig[0][0];
	frameData.places[1] = &framesPlacesConfig[1][0];
	frameData.places[2] = &framesPlacesConfig[2][0];
	frameData.places[3] = &framesPlacesConfig[3][0];

	display.refreshControl(bgLabel);
	display.setControlValue(bgLabel, 84);

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

void cConfigEditor::changeLabelText(uint8_t labelIdx, const char *text)
{
	display.setControlText(label[labelIdx], text);
	display.refreshControl(label[labelIdx]);
}

//==============================================================================================================

void cConfigEditor::resizeToDefaultConfig()
{
	display.setControlPosition(label[0],  (800/8)*0+(800/8),  -1);
	display.setControlSize(label[0],  800/4-6,  -1);

	display.setControlHide(label[1]);

	display.setControlPosition(label[2],  (800/8)*2+(800/8),  -1);
	display.setControlSize(label[2],  800/4-6,  -1);

	display.setControlHide(label[3]);

	display.setControlPosition(label[4],  (800/8)*4+(800/8),  -1);
	display.setControlSize(label[4],  800/4-6,  -1);

	display.setControlHide(label[5]);

	display.setControlPosition(label[6],  (800/8)*6+(800/8),  -1);
	display.setControlSize(label[6],  800/4-6,  -1);

	display.setControlHide(label[7]);

	for(uint8_t i = 0; i < 8; i++)
	{
		display.setControlColors(label[i], interfaceGlobals.activeLabelsColors);
	}
}


void cConfigEditor::resizeToSmallConfig(uint8_t labelIdx)
{
	display.setControlPosition(label[labelIdx],  (800/8)*labelIdx+(800/16),  452);
	display.setControlSize(label[labelIdx],  800/8-6,  59);

	display.setControlShow(label[labelIdx+1]);
}


void cConfigEditor::showFirmwareUpdatePopout()
{
	resizeToDefaultConfig();

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

void cConfigEditor::showExecute()
{
	uint32_t *colors = interfaceGlobals.activeLabelsColors;

	if(firmwareFoundNum)
	{
		display.setControlColors(label[4], colors);
	}
	else
	{
		uint32_t *colors = interfaceGlobals.inactiveLabelsColors;
		display.setControlColors(label[4], colors);
	}

	changeLabelText(4, "Update");
}

void cConfigEditor::hideFirmwareUpdatePopout()
{
	resizeToDefaultConfig();

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






