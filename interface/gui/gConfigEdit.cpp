

#include "configEditor.h"


static uint16_t framesPlaces[8][4] =
{
	{0, 		30, 800/8, 380},
	{(800/8)*1, 30, 800/8, 380},
	{(800/8)*2, 30, 800/8, 380},
	{(800/8)*3, 30, 800/8, 380},
	{(800/8)*4, 30, 800/8, 380},
	{(800/8)*5, 30, 800/8, 380},
	{(800/8)*6, 30, 800/8, 380},
	{(800/8)*7, 30, 800/8, 380},
};

static uint16_t firmwareFramesPlaces[6][4] =
{
	{0, 30, 200, 380},
	{200, 30, 100, 380},
	{300, 30, 100, 380},
	{400, 30, 100, 380},
	{500, 30, 100, 380},
	{600, 30, 200, 380},
};


static uint32_t popUpLabelColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFF0000, // ramka
};


void cConfigEditor::initDisplayControls()
{
	// inicjalizacja kontrolek
	strControlProperties prop2;
	prop2.text = (char*)"";
	prop2.style = 	( controlStyleShow | controlStyleBackground | controlStyleCenterX | controlStyleCenterY);
	prop2.x = 400;
	prop2.y = 12;
	prop2.w = 800;
	prop2.h = 25;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);


	for(uint8_t i = 0; i<8; i++)
	{
		prop2.text = (char*)"";
		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 450;
		prop2.w = 800/8-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.x = (800/8)*i+(800/16);
		prop2.y = 415;
		prop2.h = 26;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.x = (800/8)*i+5;
		prop2.y = 30;
		prop2.w = 800/8-10;
		prop2.style =  controlStyleValue_0_100;
		prop2.h = 380;
		if(barControl[i] == nullptr)  barControl[i] = display.createControl<cBar>(&prop2);
	}


	configGroupList.linesCount = groupCount;
	configGroupList.start = 0;
	configGroupList.length = groupCount;
	configGroupList.data = configGroupsNames;
	strControlProperties prop;
	prop.x = (800/8)*6+5;
	prop.y = 140;
	prop.w = 800/4-10;
	prop.h = 25;
	prop.data = &configGroupList;
	if(configGroupsListControl == nullptr)  configGroupsListControl = display.createControl<cList>(&prop);


	// ramka
	frameData.placesCount = 7;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	frameData.places[7] = &framesPlaces[7][0];
	prop.style = 0;
	prop.value = 0;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);


	strControlProperties prop9;
	prop9.x = 300;
	prop9.colors = popUpLabelColors;
	prop9.y = 350;
	prop9.h = 100;
	prop9.w = 600-(10);
	prop9.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleFont2 | controlStyleRoundedBorder);
	prop9.text = (char*)"";
	if(popoutWindowLabel == nullptr)  popoutWindowLabel = display.createControl<cLabel>(&prop9);

	firmwareList.linesCount = 20;
	firmwareList.start = 0;
	firmwareList.length = 255;
	firmwareList.data = firmwareNames;

	strControlProperties prop10;
	prop10.x = 0+5;
	prop10.y = 30;
	prop10.w = (800/4-10);
	prop10.h = 25;
	prop10.data = &firmwareList;
	if(firmwareListControl == nullptr)  firmwareListControl = display.createControl<cList>(&prop10);
}


void cConfigEditor::destroyDisplayControls()
{
	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(configGroupsListControl);
	configGroupsListControl = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;

		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;

		display.destroyControl(barControl[i]);
		barControl[i] = nullptr;
	}


	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(firmwareListControl);
	firmwareListControl = nullptr;

	display.destroyControl(popoutWindowLabel);
	popoutWindowLabel = nullptr;
}

void cConfigEditor::showDefaultConfigScreen()
{
	display.setControlText(titleLabel, "Config");
	display.refreshControl(titleLabel);

	listConfigGroups();
	//lista
	showConfigGroupList();

	// bottom labels
	display.setControlText(bottomLabel[0], "1");
	display.setControlText(bottomLabel[1], "2");
	display.setControlText(bottomLabel[2], "3");
	display.setControlText(bottomLabel[3], "4");
	display.setControlText(bottomLabel[4], "5");
	display.setControlText(bottomLabel[5], "6");
	display.setControlText(bottomLabel[6], " /\\           \\/ ");
	//display.setControlText(bottomLabel[7], "");



	display.setControlPosition(bottomLabel[6],  (800/8)*6+(800/8),  450);
	display.setControlPosition(topLabel[6],  (800/8)*6+(800/8),  415);
	display.setControlSize(bottomLabel[6],  800/4-10,  30);
	display.setControlSize(topLabel[6],  800/4-10,  26);


	display.setControlText(topLabel[6], "Config");


	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);

		display.setControlHide(barControl[i]);
		display.refreshControl(barControl[i]);
	}

	display.setControlHide(bottomLabel[7]);
	display.setControlHide(topLabel[7]);


	framesPlaces[6][0] = (800/8)*6;
	framesPlaces[6][1] = 30;
	framesPlaces[6][2] = 800/4;
	framesPlaces[6][3] = 380;

	display.setControlHide(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);


	display.synchronizeRefresh();

}

void cConfigEditor::showMasterScreen()
{
	display.setControlText(titleLabel, "Master");
	display.refreshControl(titleLabel);

	// bottom labels
	display.setControlText(bottomLabel[0], "Volume");
	display.setControlText(bottomLabel[1], "Rev. Size");
	display.setControlText(bottomLabel[2], "Rev. Dump");
	display.setControlText(bottomLabel[3], "Limit. A");
	display.setControlText(bottomLabel[4], "Limit. R");
	display.setControlText(bottomLabel[5], "Limit. T");
	display.setControlText(bottomLabel[6], " ");
	display.setControlText(bottomLabel[7], " ");

	display.setControlPosition(bottomLabel[6],  (800/8)*6+(800/16),  450);
	display.setControlPosition(topLabel[6],  (800/8)*6+(800/16),  415);
	display.setControlSize(bottomLabel[6],  800/8-10,  30);
	display.setControlSize(topLabel[6],  800/8-10,  26);


	display.setControlText(topLabel[6], " ");
	display.setControlText(topLabel[6], " ");
	display.setControlText(topLabel[6], " ");
	display.setControlText(topLabel[6], " ");
	display.setControlText(topLabel[6], " ");
	display.setControlText(topLabel[6], " ");
	display.setControlText(topLabel[6], " ");
	display.setControlText(topLabel[6], " ");


	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);

		if(i<6) display.setControlShow(barControl[i]);
		//display.refreshControl(barControl[i]);
	}


	showVolume();
	showReverbSize();
	showReverbDamping();
	showLimiterAttack();
	showLimiterRelease();
	showLimiterTreshold();


	display.setControlHide(configGroupsListControl);


	framesPlaces[6][0] = (800/8)*6;
	framesPlaces[6][1] = 30;
	framesPlaces[6][2] = 800/8;
	framesPlaces[6][3] = 380;


	display.synchronizeRefresh();

}

void cConfigEditor::showMasterTracksScreen()
{
	display.setControlText(titleLabel, "");
	display.refreshControl(titleLabel);

	// bottom labels
	display.setControlText(bottomLabel[0], "On");
	display.setControlText(bottomLabel[1], "On");
	display.setControlText(bottomLabel[2], "On");
	display.setControlText(bottomLabel[3], "On");
	display.setControlText(bottomLabel[4], "On");
	display.setControlText(bottomLabel[5], "On");
	display.setControlText(bottomLabel[6], "On");
	display.setControlText(bottomLabel[7], "On");

	display.setControlPosition(bottomLabel[6],  (800/8)*6+(800/16),  450);
	display.setControlPosition(topLabel[6],  (800/8)*6+(800/16),  415);
	display.setControlSize(bottomLabel[6],  800/8-10,  30);
	display.setControlSize(topLabel[6],  800/8-10,  26);

	display.setControlText(topLabel[0], "Solo");
	display.setControlText(topLabel[1], "Solo");
	display.setControlText(topLabel[2], "Solo");
	display.setControlText(topLabel[3], "Solo");
	display.setControlText(topLabel[4], "Solo");
	display.setControlText(topLabel[5], "Solo");
	display.setControlText(topLabel[6], "Solo");
	display.setControlText(topLabel[7], "Solo");


	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);

		display.setControlShow(barControl[i]);
		display.refreshControl(barControl[i]);
	}

	display.setControlHide(configGroupsListControl);

	display.synchronizeRefresh();

}

//==============================================================================================================
void cConfigEditor::activateLabelsBorder()
{
	if(selectedPlace[mode] > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace[mode]);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

void cConfigEditor::listConfigGroups()
{

	for(uint8_t i = 0; i < groupCount; i++)
	{
		configGroupsNames[i] = (char*)&groupNamesLabels[i][0];
	}
}
//==============================================================================================================


void cConfigEditor::showConfigGroupList()
{
	configGroupList.start = selectedConfigGroup;
	configGroupList.length = groupCount;
	configGroupList.linesCount = groupCount;
	configGroupList.data = configGroupsNames;

	display.setControlData(configGroupsListControl,  &configGroupList);
	display.setControlShow(configGroupsListControl);
	display.refreshControl(configGroupsListControl);


}

void cConfigEditor::showVolume()
{
	display.setControlValue(barControl[0], mtConfig.audioCodecConfig.headphoneVolume*100);
//	display.setControlValue(barControl[0], mtProject.values.volume);
	display.setControlShow(barControl[0]);
	display.refreshControl(barControl[0]);
}

void cConfigEditor::showReverbSize()
{
	display.setControlValue(barControl[1], mtProject.values.reverbRoomSize);
	//display.setControlShow(barControl[0]);
	display.refreshControl(barControl[1]);
}

void cConfigEditor::showReverbDamping()
{
	display.setControlValue(barControl[2], mtProject.values.reverbDamping);
	//display.setControlShow(barControl[1]);
	display.refreshControl(barControl[2]);
}

void cConfigEditor::showLimiterAttack()
{
	display.setControlValue(barControl[3], (mtProject.values.limiterAttack*100)/LIMITER_ATTACK_MAX);
	//display.setControlShow(barControl[2]);
	display.refreshControl(barControl[3]);
}

void cConfigEditor::showLimiterRelease()
{
	//display.setControlValue(barControl[4], (mtProject.values.limiterRelease*100)/LIMITER_RELEASE_MAX);
	//display.setControlShow(barControl[2]);
	display.refreshControl(barControl[4]);
}

void cConfigEditor::showLimiterTreshold()
{
	display.setControlValue(barControl[5], (mtProject.values.limiterTreshold*100)/LIMITER_TRESHOLD_MAX);
	//display.setControlShow(barControl[3]);
	display.refreshControl(barControl[5]);
}

void cConfigEditor::showFirmwareUpdateLabels()
{
	display.setControlText(topLabel[0], "Firmware");
	display.setControlText(topLabel[2], "Update");

	display.refreshControl(topLabel[0]);
	display.refreshControl(topLabel[2]);

	for(int i=0;i<6;i++)
	{
		display.setControlText(bottomLabel[i], "");
		display.refreshControl(bottomLabel[i]);
	}

	// zmiana ramkowania
	frameData.placesCount = 6;
	frameData.startPlace = 0;
	frameData.places[0] = &firmwareFramesPlaces[0][0];
	frameData.places[1] = &firmwareFramesPlaces[1][0];
	frameData.places[2] = &firmwareFramesPlaces[2][0];
	frameData.places[3] = &firmwareFramesPlaces[3][0];
	frameData.places[4] = &firmwareFramesPlaces[4][0];
	frameData.places[5] = &firmwareFramesPlaces[5][0];

	display.setControlHide(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	firmwareList.length = firmwareFoundNum;
	firmwareList.linesCount = 15;
	firmwareList.data = firmwareNames;
	firmwareList.start= firmwareSelect;

	display.setControlData(firmwareListControl,  &firmwareList);
	display.setControlShow(firmwareListControl);
	display.refreshControl(firmwareListControl);
}

void cConfigEditor::hideFirmwareUpdateLabels()
{
	display.setControlText(topLabel[0], "");
	display.setControlText(topLabel[2], "");

	display.refreshControl(topLabel[0]);
	display.refreshControl(topLabel[2]);

	display.setControlText(bottomLabel[0], "1");
	display.setControlText(bottomLabel[1], "2");
	display.setControlText(bottomLabel[2], "3");
	display.setControlText(bottomLabel[3], "4");
	display.setControlText(bottomLabel[4], "5");
	display.setControlText(bottomLabel[5], "6");

	for(int i=0;i<6;i++)
	{
		display.refreshControl(bottomLabel[i]);
	}

	//przywrocenie ramki
	// ramka
	frameData.placesCount = 7;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	frameData.places[7] = &framesPlaces[7][0];

	display.setControlHide(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	display.setControlHide(firmwareListControl);
	display.refreshControl(firmwareListControl);
}

void cConfigEditor::showFirmwareUpdatePopout()
{
	for(uint8_t i = 0 ; i < 6; i++)
	{
		display.setControlText(bottomLabel[i], "");
		display.setControlText(topLabel[i], "");
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	display.setControlText(bottomLabel[0], "Yes");
	display.setControlText(bottomLabel[5], "No");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	display.setControlText(popoutWindowLabel,"Do you want to flash new firmware?");
	display.setControlShow(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	display.synchronizeRefresh();
}

void cConfigEditor::hideFirmwareUpdatePopout()
{
	for(uint8_t i = 0 ; i < 6; i++)
	{
		display.setControlText(bottomLabel[i], "");
		display.setControlText(topLabel[i], "");
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	display.setControlText(topLabel[0], "Firmware");
	display.setControlText(topLabel[2], "Update");

	display.setControlShow(frameControl);
	display.refreshControl(frameControl);

	display.setControlHide(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	display.synchronizeRefresh();
}





