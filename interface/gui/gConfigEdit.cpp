

#include "configEditor.h"


static uint16_t framesPlaces[8][4] =
{
	{0+2, 		31, 800/8-5, 387},
	{(800/8)*1+2, 31, 800/8-5, 387},
	{(800/8)*2+2, 31, 800/8-5, 387},
	{(800/8)*3+2, 31, 800/8-5, 387},
	{(800/8)*4+2, 31, 800/8-5, 387},
	{(800/8)*5+2, 31, 800/8-5, 387},
	{(800/8)*6+2, 31, 800/8-5, 387},
	{(800/8)*7+2, 31, 800/8-5, 387},
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

	strControlProperties prop;
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


	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);


	for(uint8_t i = 0; i<8; i++)
	{
		prop2.text = (char*)"";
		//prop2.data =  &bottomValuesConfig;
		prop2.colors = interfaceGlobals.activeLabelsColors;

		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY );
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 452;
		prop2.h =  59;

		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);

		prop2.x = (800/8)*i+5;
		prop2.y = 30;
		prop2.w = 800/8-10;
		prop2.style =  controlStyleValue_0_100;
		prop2.h = 388;
		if(barControl[i] == nullptr)  barControl[i] = display.createControl<cBar>(&prop2);

	}


	configGroupList.linesCount = mtConfigGroupsCount;
	configGroupList.start = 0;
	configGroupList.length = mtConfigGroupsCount;
	configGroupList.data = configGroupsNames;

	prop.x = (800/8)*6+8;
	prop.y = 37;
	prop.w = 800/4-16;
	prop.h = 25;
	prop.data = &configGroupList;
	if(configGroupsListControl == nullptr)  configGroupsListControl = display.createControl<cList>(&prop);



	firmwareList.linesCount = 20;
	firmwareList.start = 0;
	firmwareList.length = 255;
	firmwareList.data = firmwareNames;

	strControlProperties prop10;
	prop10.x = 8;
	prop10.y = 38;
	prop10.w = (800/4-16);
	prop10.h = 25;
	prop10.data = &firmwareList;
	if(firmwareListControl == nullptr)  firmwareListControl = display.createControl<cList>(&prop10);
}


void cConfigEditor::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(configGroupsListControl);
	configGroupsListControl = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;

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
	for(uint8_t i = 0; i<7; i++)
	{
		//display.setControlText(topLabel[i], "");
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);

		display.setControlHide(barControl[i]);
		display.refreshControl(barControl[i]);
	}

	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Config");
	display.refreshControl(titleLabel);


	listConfigGroups();
	//lista
	showConfigGroupList();

	display.setControlValue(label[0], 0);
	display.setControlValue(label[1], 0);
	display.setControlValue(label[2], 0);
	display.setControlValue(label[3], 0);
	display.setControlValue(label[4], 0);
	display.setControlValue(label[5], 0);
	display.setControlValue(label[6], 0);
	display.setControlValue(label[7], 0);

	display.setControlText(label[0], "1");
	display.setControlText(label[1], "2");
	display.setControlText(label[2], "3");
	display.setControlText(label[3], "4");
	display.setControlText(label[4], "5");
	display.setControlText(label[5], "Help");
	display.setControlText(label[6], "Config");
	//display.setControlText(bottomLabel[7], "");

	display.setControlPosition(label[6],  (800/8)*6+(800/8),  452);
	display.setControlSize(label[6],  800/4-6,  59);

	display.setControlHide(label[7]);


	framesPlaces[6][0] = (800/8)*6+2;
	framesPlaces[6][1] = 31;
	framesPlaces[6][2] = 800/4-5;
	framesPlaces[6][3] = 387;

	display.setControlHide(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);


	display.synchronizeRefresh();

}

void cConfigEditor::showMasterScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Master");
	display.refreshControl(titleLabel);

	display.setControlValue(label[0], 1);
	display.setControlValue(label[1], 1);
	display.setControlValue(label[2], 1);
	display.setControlValue(label[3], 1);
	display.setControlValue(label[4], 1);
	display.setControlValue(label[5], 1);
	display.setControlValue(label[6], 1);
	display.setControlValue(label[7], 1);

	display.setControlText2(label[0], "Volume");
	display.setControlText2(label[1], "Rev. Size");
	display.setControlText2(label[2], "Rev. Dump");
	display.setControlText2(label[3], "Bit Depth");
	display.setControlText2(label[4], "Limit. A");
	display.setControlText2(label[5], "Limit. R");
	display.setControlText2(label[6], "Limit. T");
	display.setControlText2(label[7], " ");

	display.setControlPosition(label[6],  (800/8)*6+(800/16),  452);
	display.setControlSize(label[6],  800/8-6,  59);

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);

		if(i<6) display.setControlShow(barControl[i]);
		//display.refreshControl(barControl[i]);
	}


	showVolume();
	showReverbSize();
	showReverbDamping();
	showLimiterAttack();
	showLimiterRelease();
	showLimiterTreshold();
	showBitDepth();


	display.setControlHide(configGroupsListControl);


	framesPlaces[6][0] = (800/8)*6+2;
	framesPlaces[6][1] = 31;
	framesPlaces[6][2] = 800/8-5;
	framesPlaces[6][3] = 387;


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

	for(uint8_t i = 0; i < mtConfigGroupsCount; i++)
	{
		configGroupsNames[i] = (char*)&groupNamesLabels[i][0];
	}
}
//==============================================================================================================

void cConfigEditor::showConfigGroupList()
{
	configGroupList.start = selectedConfigGroup;
	configGroupList.length = mtConfigGroupsCount;
	configGroupList.linesCount = mtConfigGroupsCount;
	configGroupList.data = configGroupsNames;

	display.setControlData(configGroupsListControl,  &configGroupList);
	display.setControlShow(configGroupsListControl);
	display.refreshControl(configGroupsListControl);


}

void cConfigEditor::showVolume()
{
	sprintf(volumeVal,"%d", mtProject.values.volume);

	display.setControlValue(barControl[0], mtProject.values.volume);
//	display.setControlValue(barControl[0], mtProject.values.volume);
//	display.setControlShow(barControl[0]);
	display.refreshControl(barControl[0]);

	display.setControlText(label[0], volumeVal);
	display.refreshControl(label[0]);
}

void cConfigEditor::showReverbSize()
{
	sprintf(reverbSizeVal,"%d",mtProject.values.reverbRoomSize);

	display.setControlValue(barControl[1], mtProject.values.reverbRoomSize);
	//display.setControlShow(barControl[0]);
	display.refreshControl(barControl[1]);

	display.setControlText(label[1], reverbSizeVal);
	display.refreshControl(label[1]);
}

void cConfigEditor::showReverbDamping()
{
	sprintf(reverbDampVal,"%d",mtProject.values.reverbDamping);

	display.setControlValue(barControl[2], mtProject.values.reverbDamping);
	//display.setControlShow(barControl[1]);
	display.refreshControl(barControl[2]);

	display.setControlText(label[2], reverbDampVal);
	display.refreshControl(label[2]);
}

void cConfigEditor::showLimiterAttack()
{
	uint8_t length;

	sprintf(limitAttackVal,"%.3f",(float)(mtProject.values.limiterAttack/1000.0f));
	length=strlen(limitAttackVal);
	limitAttackVal[length]='s';
	limitAttackVal[length+1]=0;

	display.setControlValue(barControl[4], (mtProject.values.limiterAttack*100)/LIMITER_ATTACK_MAX);
	//display.setControlShow(barControl[2]);
	display.refreshControl(barControl[4]);

	display.setControlText(label[4], limitAttackVal);
	display.refreshControl(label[4]);
}

void cConfigEditor::showLimiterRelease()
{
	uint8_t length;

	sprintf(limitReleaseVal,"%.3f",(float)(mtProject.values.limiterRelease/1000.0f));
	length=strlen(limitReleaseVal);
	limitReleaseVal[length]='s';
	limitReleaseVal[length+1]=0;

	display.setControlValue(barControl[5], (mtProject.values.limiterRelease*100)/LIMITER_RELEASE_MAX);
	//display.setControlShow(barControl[2]);
	display.refreshControl(barControl[5]);

	display.setControlText(label[5], limitReleaseVal);
	display.refreshControl(label[5]);
}

void cConfigEditor::showLimiterTreshold()
{
	sprintf(limitThresholdVal,"%d",(mtProject.values.limiterTreshold*100)/LIMITER_TRESHOLD_MAX);

	display.setControlValue(barControl[6], (mtProject.values.limiterTreshold*100)/LIMITER_TRESHOLD_MAX);
	display.setControlShow(barControl[6]);
	display.refreshControl(barControl[6]);

	display.setControlText(label[6], limitThresholdVal);
	display.refreshControl(label[6]);
}

void cConfigEditor::showBitDepth()
{
	sprintf(bitDepthVal,"%d",mtProject.values.bitDepth);

	uint8_t localVal = map(mtProject.values.bitDepth,BIT_DEPTH_MIN,BIT_DEPTH_MAX,0,100);
	display.setControlValue(barControl[3], localVal);
//	display.setControlShow(barControl[3]);
	display.refreshControl(barControl[3]);

	display.setControlText(label[3], bitDepthVal);
	display.refreshControl(label[3]);
}

void cConfigEditor::showFirmwareUpdateLabels()
{
	for(int i=0;i<6;i++)
	{
		display.setControlText(label[i], "");
		display.refreshControl(label[i]);
	}

	display.setControlText(label[0], "Firmware");

	uint32_t *colors = interfaceGlobals.activeLabelsColors;
	if(firmwareFoundNum == 0)
	{
		colors = interfaceGlobals.inactiveLabelsColors;
	}

	display.setControlColors(label[2], colors);
	display.setControlText(label[2], "Update");

	display.refreshControl(label[0]);
	display.refreshControl(label[2]);


	framesPlaces[0][0] = 0+2;
	framesPlaces[0][1] = 31;
	framesPlaces[0][2] = 800/4-5;
	framesPlaces[0][3] = 387;

	framesPlaces[1][0] = 0+2;
	framesPlaces[1][1] = 31;
	framesPlaces[1][2] = 800/4-5;
	framesPlaces[1][3] = 387;

	display.setControlHide(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	firmwareList.length = firmwareFoundNum;
	firmwareList.linesCount = 15;
	firmwareList.data = firmwareNames;
	firmwareList.start= firmwareSelect;

	display.setControlData(firmwareListControl,  &firmwareList);
	display.setControlShow(firmwareListControl);
	display.refreshControl(firmwareListControl);

	resizeFirmwareLabel(1);
}

void cConfigEditor::hideFirmwareUpdateLabels()
{
	display.setControlText(label[0], "1");
	display.setControlText(label[1], "2");
	display.setControlText(label[2], "3");
	display.setControlText(label[3], "4");
	display.setControlText(label[4], "5");
	display.setControlText(label[5], "Tutorial");

	for(int i=0;i<6;i++)
	{
		display.refreshControl(label[i]);
	}

	//przywrocenie ramki

	framesPlaces[0][0] = 0+2;
	framesPlaces[0][1] = 31;
	framesPlaces[0][2] = 800/8-5;
	framesPlaces[0][3] = 387;

	framesPlaces[1][0] = (800/8)*1+2;
	framesPlaces[1][1] = 31;
	framesPlaces[1][2] = 800/8-5;
	framesPlaces[1][3] = 387;

	display.setControlHide(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	display.setControlHide(firmwareListControl);
	display.refreshControl(firmwareListControl);

	resizeFirmwareLabel(0);

}

void cConfigEditor::showFirmwareUpdatePopout()
{
	resizeFirmwareLabel(0);

	for(uint8_t i = 0 ; i < 6; i++)
	{
		display.setControlText(label[i], "");
		display.refreshControl(label[i]);
	}

	display.setControlText(label[0], "Yes");
	display.setControlText(label[5], "No");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	display.setControlText(popoutWindowLabel,"Do you want to flash new firmware?");
	display.setControlShow(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	display.synchronizeRefresh();
}

void cConfigEditor::hideFirmwareUpdatePopout()
{
	resizeFirmwareLabel(1);

	for(uint8_t i = 0 ; i < 6; i++)
	{
		display.setControlText(label[i], "");
		display.refreshControl(label[i]);
	}

	display.setControlText(label[0], "Firmware");
	display.setControlText(label[2], "Update");

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






