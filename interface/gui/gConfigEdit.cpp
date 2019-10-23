

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
		prop2.style = 	(controlStyleBackground | controlStyleCenterX | controlStyleCenterY);
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 452;
		prop2.h = 58;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.y = 465;
		prop2.h = 30;
		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);


		prop2.x = (800/8)*i+5;
		prop2.y = 30;
		prop2.w = 800/8-10;
		prop2.style =  controlStyleValue_0_100;
		prop2.h = 388;
		if(barControl[i] == nullptr)  barControl[i] = display.createControl<cBar>(&prop2);

	}


	configGroupList.linesCount = groupCount;
	configGroupList.start = 0;
	configGroupList.length = groupCount;
	configGroupList.data = configGroupsNames;

	prop.x = (800/8)*6+8;
	prop.y = 140;
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
	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

		//display.setControlText(topLabel[i], "");
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);

		display.setControlHide(barControl[i]);
		display.refreshControl(barControl[i]);
	}

	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Config");
	display.refreshControl(titleLabel);


	listConfigGroups();
	//lista
	showConfigGroupList();

	// bottom labels
	display.setControlText(topLabel[0], "1");
	display.setControlText(topLabel[1], "2");
	display.setControlText(topLabel[2], "3");
	display.setControlText(topLabel[3], "4");
	display.setControlText(topLabel[4], "5");
	display.setControlText(topLabel[5], "6");
	display.setControlText(topLabel[6], " /\\           \\/ ");
	//display.setControlText(bottomLabel[7], "");

	display.setControlPosition(bottomLabel[6],  (800/8)*6+(800/8),  465);
	display.setControlPosition(topLabel[6],  (800/8)*6+(800/8),  452);
	display.setControlSize(bottomLabel[6],  800/4-6,  30);
	display.setControlSize(topLabel[6],  800/4-6,  58);


	display.setControlText(topLabel[6], "Config");


	display.setControlHide(bottomLabel[7]);
	display.setControlHide(topLabel[7]);


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

	// bottom labels
	display.setControlText(bottomLabel[0], "Volume");
	display.setControlText(bottomLabel[1], "Rev. Size");
	display.setControlText(bottomLabel[2], "Rev. Dump");
	display.setControlText(bottomLabel[3], "Bit Depth");
	display.setControlText(bottomLabel[4], "Limit. A");
	display.setControlText(bottomLabel[5], "Limit. R");
	display.setControlText(bottomLabel[6], "Limit. T");
	display.setControlText(bottomLabel[7], " ");

	display.setControlPosition(bottomLabel[6],  (800/8)*6+(800/16),  465);
	display.setControlPosition(topLabel[6],  (800/8)*6+(800/16),  437);
	display.setControlSize(bottomLabel[6],  800/8-6,  30);
	display.setControlSize(topLabel[6],  800/8-6,  28);


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
	showBitDepth();


	display.setControlHide(configGroupsListControl);


	framesPlaces[6][0] = (800/8)*6+2;
	framesPlaces[6][1] = 31;
	framesPlaces[6][2] = 800/8-5;
	framesPlaces[6][3] = 387;


	display.synchronizeRefresh();

}

void cConfigEditor::showMasterTracksScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Tracks");
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
	sprintf(volumeVal,"%d",mtProject.values.volume);

	display.setControlValue(barControl[0], mtProject.values.volume);
//	display.setControlValue(barControl[0], mtProject.values.volume);
//	display.setControlShow(barControl[0]);
	display.refreshControl(barControl[0]);

	display.setControlText(topLabel[0], volumeVal);
	display.refreshControl(topLabel[0]);
}

void cConfigEditor::showReverbSize()
{
	sprintf(reverbSizeVal,"%d",mtProject.values.reverbRoomSize);

	display.setControlValue(barControl[1], mtProject.values.reverbRoomSize);
	//display.setControlShow(barControl[0]);
	display.refreshControl(barControl[1]);

	display.setControlText(topLabel[1], reverbSizeVal);
	display.refreshControl(topLabel[1]);
}

void cConfigEditor::showReverbDamping()
{
	sprintf(reverbDampVal,"%d",mtProject.values.reverbDamping);

	display.setControlValue(barControl[2], mtProject.values.reverbDamping);
	//display.setControlShow(barControl[1]);
	display.refreshControl(barControl[2]);

	display.setControlText(topLabel[2], reverbDampVal);
	display.refreshControl(topLabel[2]);
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

	display.setControlText(topLabel[4], limitAttackVal);
	display.refreshControl(topLabel[4]);
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

	display.setControlText(topLabel[5], limitReleaseVal);
	display.refreshControl(topLabel[5]);
}

void cConfigEditor::showLimiterTreshold()
{
	sprintf(limitThresholdVal,"%d",(mtProject.values.limiterTreshold*100)/LIMITER_TRESHOLD_MAX);

	display.setControlValue(barControl[6], (mtProject.values.limiterTreshold*100)/LIMITER_TRESHOLD_MAX);
	display.setControlShow(barControl[6]);
	display.refreshControl(barControl[6]);

	display.setControlText(topLabel[6], limitThresholdVal);
	display.refreshControl(topLabel[6]);
}

void cConfigEditor::showBitDepth()
{
	sprintf(bitDepthVal,"%d",mtProject.values.bitDepth);

	uint8_t localVal = map(mtProject.values.bitDepth,BIT_DEPTH_MIN,BIT_DEPTH_MAX,0,100);
	display.setControlValue(barControl[3], localVal);
//	display.setControlShow(barControl[3]);
	display.refreshControl(barControl[3]);

	display.setControlText(topLabel[3], bitDepthVal);
	display.refreshControl(topLabel[3]);
}

void cConfigEditor::showFirmwareUpdateLabels()
{
	for(int i=0;i<6;i++)
	{
		display.setControlText(topLabel[i], "");
		display.refreshControl(topLabel[i]);
	}

	display.setControlText(topLabel[0], "Firmware");

	if(firmwareFoundNum)
	{
		display.setControlText(topLabel[2], "Update");
	}

	display.refreshControl(topLabel[0]);
	display.refreshControl(topLabel[2]);


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
	display.setControlText(topLabel[0], "1");
	display.setControlText(topLabel[1], "2");
	display.setControlText(topLabel[2], "3");
	display.setControlText(topLabel[3], "4");
	display.setControlText(topLabel[4], "5");
	display.setControlText(topLabel[5], "6");

	for(int i=0;i<6;i++)
	{
		display.refreshControl(topLabel[i]);
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
	for(uint8_t i = 0 ; i < 6; i++)
	{
		display.setControlText(bottomLabel[i], "");
		display.setControlText(topLabel[i], "");
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	display.setControlText(topLabel[0], "Yes");
	display.setControlText(topLabel[5], "No");

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

void cConfigEditor::resizeLabelConfigDefault()
{
	for(int i=0;i<8;i++)
	{
		display.setControlText(topLabel[i],"");
		display.setControlText(bottomLabel[i],"");
		display.setControlSize(topLabel[i], (800/8)-6, 58);
		display.setControlPosition(topLabel[i], (800/8)*i+(800/16), 452);
		display.refreshControl(topLabel[i]);
	}
}

void cConfigEditor::resizeLabelConfigMaster()
{
	for(int i=0;i<8;i++)
	{

		display.setControlSize(topLabel[i], (800/8)-6, 28);
		display.setControlPosition(topLabel[i], (800/8)*i+(800/16), 437);
		display.refreshControl(topLabel[i]);
	}

//	display.setControlText(topLabel[6],"");
//	display.refreshControl(topLabel[6]);
}

void cConfigEditor::resizeFirmwareLabel(uint8_t control)// 0 - revert to normal, 1 - rescale to double label
{
	if(control)
	{
		//display.setControlPosition(bottomLabel[6],  (800/8)*0+(800/8),  465);
		display.setControlPosition(topLabel[0],  (800/8)*0+(800/8),  452);
		//display.setControlSize(bottomLabel[6],  800/4-6,  30);
		display.setControlSize(topLabel[0],  800/4-6,  58);
	}
	else
	{
		display.setControlPosition(topLabel[0],  (800/8)*0+(800/16),  452);
		display.setControlSize(topLabel[0],  800/8-6,  58);
	}

	display.refreshControl(topLabel[0]);
}






