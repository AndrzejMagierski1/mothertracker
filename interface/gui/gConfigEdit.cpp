

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

void cConfigEditor::createFirmwareList()
{
	firmwareList.linesCount = 20;
	firmwareList.start = 0;
	firmwareList.length = 255;
	firmwareList.data = firmwareNames;
	strControlProperties prop;
	prop.x = 0+5;
	prop.y = 30;
	prop.w = (800/4-10);
	prop.h = 25;
	prop.data = &firmwareList;
	if(firmwareListControl == nullptr)  firmwareListControl = display.createControl<cList>(&prop);

	framesPlaces[0][0] = 0;
	framesPlaces[0][1] = 30;
	framesPlaces[0][2] = 800/4;
	framesPlaces[0][3] = 380;
}

void cConfigEditor::showFirmwareList()
{
	firmwareList.length = 5;
	firmwareList.linesCount = 15;
	firmwareList.data = firmwareNames;
	firmwareList.start= 0;

	display.setControlData(firmwareListControl,  &firmwareList);
	display.setControlShow(firmwareListControl);
	display.refreshControl(firmwareListControl);
}

void cConfigEditor:: hideFirmwareList()
{
	display.setControlHide(firmwareListControl);
	display.refreshControl(firmwareListControl);
}





