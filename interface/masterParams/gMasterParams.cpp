

#include <masterParams/masterParams.h>


static uint16_t framesPlaces[8][4] =
{
	{0+1, 		  29, 800/8-3, 391},
	{(800/8)*1+1, 29, 800/8-3, 391},
	{(800/8)*2+1, 29, 800/8-3, 391},
	{(800/8)*3+1, 29, 800/8-3, 391},
	{(800/8)*4+1, 29, 800/8-3, 391},
	{(800/8)*5+1, 29, 800/8-3, 391},
	{(800/8)*6+1, 29, 800/8-3, 391},
	{(800/8)*7+1, 29, 800/8-3, 391},
};

static uint16_t framesPlacesConfig[4][4]=
{
	{(800/8)*0+1, 29, 800/4-3, 391},
	{(800/8)*2+1, 29, 800/4-3, 391},
	{(800/8)*4+1, 29, 800/4-3, 391},
	{(800/8)*6+1, 29, 800/4-3, 391},
};


static uint32_t levelBarColors[][8] =
{
	{
	0xFFFFFF, // kolor glowny
	0xff0000, // kolor dodatkowy 1 czer
	0x00ff00, // kolor dodatkowy 2 ziel
	0x080808, // kontener
	0x0a0a0a, // tlo
	},
	{
	0xFFFFFF, // kolor glowny
	0xff0000, // kolor dodatkowy 1 czer
	0x00ff00, // kolor dodatkowy 2 ziel
	0x080808, // kontener
	0x0a0a0a, // tlo
	},
	{
	0xFFFFFF, // kolor glowny
	0xff0000, // kolor dodatkowy 1 czer
	0x00ff00, // kolor dodatkowy 2 ziel
	0x080808, // kontener
	0x0a0a0a, // tlo
	},
	{
	0xFFFFFF, // kolor glowny
	0xff0000, // kolor dodatkowy 1 czer
	0x00ff00, // kolor dodatkowy 2 ziel
	0x080808, // kontener
	0x0a0a0a, // tlo
	},
	{
	0xFFFFFF, // kolor glowny
	0xff0000, // kolor dodatkowy 1 czer
	0x00ff00, // kolor dodatkowy 2 ziel
	0x080808, // kontener
	0x0a0a0a, // tlo
	},
	{
	0xFFFFFF, // kolor glowny
	0xff0000, // kolor dodatkowy 1 czer
	0x00ff00, // kolor dodatkowy 2 ziel
	0x080808, // kontener
	0x0a0a0a, // tlo
	},
	{
	0xFFFFFF, // kolor glowny
	0xff0000, // kolor dodatkowy 1 czer
	0x00ff00, // kolor dodatkowy 2 ziel
	0x080808, // kontener
	0x0a0a0a, // tlo
	},
	{
	0xFFFFFF, // kolor glowny
	0xff0000, // kolor dodatkowy 1 czer
	0x00ff00, // kolor dodatkowy 2 ziel
	0x080808, // kontener
	0x0a0a0a, // tlo
	},
};

static uint32_t editModeLabelsColors[4] =
{
	0x777777, // tekst
	0x323132, // tło
	one_true_red, // ramka
	one_true_red, // tekst2
};

void cMasterParams::initDisplayControls()
{
	// inicjalizacja kontrolek
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

	strControlProperties prop;
	// ramka
	prop.style = 0;
	prop.value = 0;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);

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

		prop2.colors = levelBarColors[i];
		prop2.value = 0;
		prop2.x = (800/8)*i+1;
		prop2.y = 29;
		prop2.w = 800/8-3;
		prop2.style =  controlStyleValue_0_100 | controlStyleBackground;
		prop2.h = 394;
		if(barControl[i] == nullptr)  barControl[i] = display.createControl<cBar>(&prop2);

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

	strControlProperties propPingPong;

	pingpongEnableData.linesCount = 2;
	pingpongEnableData.start = 0;
	pingpongEnableData.length = 2;
	pingpongEnableData.data = (char**)delayPingPongEnable;

	propPingPong.style = controlStyleBackground;
	propPingPong.x = (800/8)*(0)+1;
	propPingPong.y = 29;
	propPingPong.w = 800/8-3;
	propPingPong.h = 394;
	propPingPong.data = &pingpongEnableData;
	propPingPong.colors = nullptr;

	if(delayPingpongEnableList == nullptr) delayPingpongEnableList = display.createControl<cList>(&propPingPong);
	strControlProperties propSyncEnable;

	syncEnableData.linesCount = 2;
	syncEnableData.start = 0;
	syncEnableData.length = 2;
	syncEnableData.data = (char**)delaySyncEnable;

	propSyncEnable.style = controlStyleBackground;
	propSyncEnable.x = (800/8)*(1)+1;
	propSyncEnable.y = 29;
	propSyncEnable.w = 800/8-3;
	propSyncEnable.h = 394;
	propSyncEnable.data = &syncEnableData;
	propSyncEnable.colors = nullptr;

	if(delaySyncEnableList == nullptr) delaySyncEnableList = display.createControl<cList>(&propSyncEnable);

	strControlProperties propSyncRate;

	syncRateData.linesCount = 14;
	syncRateData.start = 0;
	syncRateData.length = 19;
	syncRateData.data = (char**)delaySyncRates;

	propSyncRate.style = controlStyleBackground;
	propSyncRate.x = (800/8)*(2)+1;
	propSyncRate.y = 29;
	propSyncRate.w = 800/8-3;
	propSyncRate.h = 394;
	propSyncRate.data = &syncRateData;
	propSyncRate.colors = nullptr;

	if(delaySyncRateList == nullptr) delaySyncRateList = display.createControl<cList>(&propSyncRate);

}


void cMasterParams::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;


	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;

		display.destroyControl(barControl[i]);
		barControl[i] = nullptr;

	}

	display.destroyControl(bgLabel);
	bgLabel = nullptr;

	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(delayPingpongEnableList);
	delayPingpongEnableList = nullptr;

	display.destroyControl(delaySyncEnableList);
	delaySyncEnableList = nullptr;

	display.destroyControl(delaySyncRateList);
	delaySyncRateList = nullptr;

	display.destroyControl(keyboardControl);
	keyboardControl = nullptr;

	display.destroyControl(editName);
	editName = nullptr;


}

void cMasterParams::showDefaultConfigScreen()
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
	display.setControlValue(bgLabel, 255);


	activateLabelsBorder();
	display.synchronizeRefresh();

}

void cMasterParams::showMasterScreen()
{

	display.setControlHide(delayPingpongEnableList);
	display.setControlHide(delaySyncEnableList);
	display.setControlHide(delaySyncRateList);

	display.refreshControl(delayPingpongEnableList);
	display.refreshControl(delaySyncEnableList);
	display.refreshControl(delaySyncRateList);

	for(uint8_t i = 0; i < 8; i++)
	{
		levelBarColors[i][0] = 0xFFFFFFFF;
	}

	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Master 1/2");
	display.refreshControl(titleLabel);

	display.setControlText(label[0], "Volume");
	display.setControlText(label[1], "Reverb");
	display.setControlText2(label[1], " ");
	display.setControlText(label[2], "Delay");
	display.setControlText2(label[2], " ");
	display.setControlText(label[3], "Bit Depth");
	display.setControlText(label[4], "LimiterAtt");
	display.setControlText(label[5], "LimiterRls");
	display.setControlText(label[6], "LimiterThr");
	display.setControlText(label[7], " ");
	display.setControlText2(label[7], " ");

	resizeToDefaultMaster();

	display.setControlColors(barControl[3], interfaceGlobals.activeBarColors);

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);

		if((i != 7) && ( i != 1) && (i != 2))
		{
			display.setControlShow(barControl[i]);
		}
		else
		{
			display.setControlHide(barControl[i]);
		}
		display.refreshControl(barControl[i]);
	}


	display.refreshControl(bgLabel);
	display.setControlValue(bgLabel, 255);



	showVolume();
	showLimiterAttack();
	showLimiterRelease();
	showLimiterTreshold();
	showBitDepth();


	frameData.placesCount = 7;
	frameData.startPlace = selectedPlace;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	frameData.places[7] = &framesPlaces[7][0];

	activateLabelsBorder();
	display.synchronizeRefresh();

}

void cMasterParams::showReverbScreen()
{
	for(uint8_t i = 0; i < 8; i++)
	{
		levelBarColors[i][0] = 0xFFFFFFFF;
	}

	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Master Reverb");
	display.refreshControl(titleLabel);

	display.setControlText(label[0], "Size");
	display.setControlText(label[1], "Damping");
	display.setControlText(label[2], "Predelay");
	display.setControlText(label[3], "Diffusion");
	display.setControlText(label[4], "");
	display.setControlText2(label[4], " ");
	display.setControlText(label[5], " ");
	display.setControlText2(label[5], " ");
	display.setControlText(label[6], " ");
	display.setControlText2(label[6], " ");
	display.setControlText(label[7], "Back");
	display.setControlText2(label[7], " ");

	resizeToDefaultMaster();



	for(uint8_t i = 0; i < 8; i++)
	{
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);

		if(i < 4)
		{
			display.setControlShow(barControl[i]);
		}
		else
		{
			display.setControlHide(barControl[i]);
		}
		display.refreshControl(barControl[i]);
	}


	display.refreshControl(bgLabel);
	display.setControlValue(bgLabel, 255);

	showReverbSize();
	showReverbDamp();
	showReverbPredelay();
	showReverbDiffusion();

	frameData.placesCount = 4;
	frameData.startPlace = selectedPlaceReverb;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	frameData.places[7] = &framesPlaces[7][0];

	refreshReverbFrame();
	display.synchronizeRefresh();
}

void cMasterParams::showDelayScreen()
{
	display.setControlShow(delayPingpongEnableList);
		display.setControlShow(delaySyncEnableList);
		display.setControlShow(delaySyncRateList);

		display.refreshControl(delayPingpongEnableList);
		display.refreshControl(delaySyncEnableList);
		display.refreshControl(delaySyncRateList);


		for(uint8_t i = 0; i < 8; i++)
		{
			levelBarColors[i][0] = 0xFFFFFFFF;
		}

		display.refreshControl(titleBar);

		display.setControlText(titleLabel, "Master Delay");
		display.refreshControl(titleLabel);

		display.setControlText(label[0], "PingPong");
		display.setControlText(label[1], "Sync");
		display.setControlText(label[2], "Rate");
		display.setControlText(label[3], "Time");
		display.setControlText(label[4], "Feedback");
		display.setControlText(label[5], " ");
		display.setControlText2(label[5], " ");
		display.setControlText(label[6], " ");
		display.setControlText2(label[6], " ");
		display.setControlText(label[7], "Back");
		display.setControlText2(label[7], " ");

		resizeToDefaultMaster();

		if(mtProject.values.delayParams & 0b01000000)
		{
			display.setControlColors(label[2], interfaceGlobals.activeLabelsColors);
			display.setControlColors(delaySyncRateList, interfaceGlobals.activeListColors);
			display.setControlColors(label[3], interfaceGlobals.inactiveLabelsColors);
			levelBarColors[3][0] = 0x222222;
			display.setControlColors(barControl[3], levelBarColors[3]);
		}
		else
		{
			display.setControlColors(label[3], interfaceGlobals.activeLabelsColors);
			levelBarColors[3][0] = 0xFFFFFF;
			display.setControlColors(barControl[3], levelBarColors[3]);
			display.setControlColors(label[2], interfaceGlobals.inactiveLabelsColors);
			display.setControlColors(delaySyncRateList, interfaceGlobals.inactiveListColors);
		}

		for(uint8_t i = 0; i < 8; i++)
		{
			display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
			display.setControlShow(label[i]);
			display.refreshControl(label[i]);

			if((i == 3) || ( i == 4))
			{
				display.setControlShow(barControl[i]);
			}
			else
			{
				display.setControlHide(barControl[i]);
			}
			display.refreshControl(barControl[i]);
		}


		display.refreshControl(bgLabel);
		display.setControlValue(bgLabel, 255);

		showDelayFeedback();
		showDelayTime();
		showDelayPingPongEnable();
		showDelaySyncEnable();
		showDelayRate();


		frameData.placesCount = 5;
		frameData.startPlace = selectedPlaceDelay;
		frameData.places[0] = &framesPlaces[0][0];
		frameData.places[1] = &framesPlaces[1][0];
		frameData.places[2] = &framesPlaces[2][0];
		frameData.places[3] = &framesPlaces[3][0];
		frameData.places[4] = &framesPlaces[4][0];
		frameData.places[5] = &framesPlaces[5][0];
		frameData.places[6] = &framesPlaces[6][0];
		frameData.places[7] = &framesPlaces[7][0];

		refreshDelayFrame();
		display.synchronizeRefresh();
}

void cMasterParams::showReverbSize()
{
	sprintf(reverbSizeVal,"%d", (int)(mtProject.values.reverb.size * 100));

	uint8_t displaySizeValue = mtProject.values.reverb.size * 100;

	display.setControlValue(barControl[0], displaySizeValue);
	display.refreshControl(barControl[0]);

	display.setControlText2(label[0], reverbSizeVal);
	display.refreshControl(label[0]);
}
void cMasterParams::showReverbDamp()
{
	sprintf(reverbDampVal,"%d", (int)(mtProject.values.reverb.damp * 100));

	uint8_t displayDampValue = mtProject.values.reverb.damp * 100;

	display.setControlValue(barControl[1], displayDampValue);
	display.refreshControl(barControl[1]);

	display.setControlText2(label[1], reverbDampVal);
	display.refreshControl(label[1]);
}
void cMasterParams::showReverbPredelay()
{
	sprintf(reverbPredelayVal,"%d", (int)(mtProject.values.reverb.predelay * 100));

	uint8_t displayPredelayValue = mtProject.values.reverb.predelay * 100;

	display.setControlValue(barControl[2], displayPredelayValue);
	display.refreshControl(barControl[2]);

	display.setControlText2(label[2], reverbPredelayVal);
	display.refreshControl(label[2]);
}
void cMasterParams::showReverbDiffusion()
{
	uint8_t displayDiffusionValue = map(mtProject.values.reverb.diffusion * 100, 0 , REVERB_DIFFUSION_MAX * 100, 0, 100);

	sprintf(reverbDiffusionVal,"%d", (int)(displayDiffusionValue));

	display.setControlValue(barControl[3], displayDiffusionValue);
	display.refreshControl(barControl[3]);

	display.setControlText2(label[3], reverbDiffusionVal);
	display.refreshControl(label[3]);
}
//==============================================================================================================
void cMasterParams::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

void cMasterParams::refreshDelayFrame()
{
	if(selectedPlaceDelay > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlaceDelay);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

void cMasterParams::refreshReverbFrame()
{
	if(selectedPlaceReverb > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlaceReverb);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

void cMasterParams::changeLabelText(uint8_t labelIdx, const char *text)
{
	display.setControlText(label[labelIdx], text);
	display.refreshControl(label[labelIdx]);
}

//==============================================================================================================

void cMasterParams::resizeToDefaultConfig()
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

void cMasterParams::resizeToDefaultMaster()
{
	display.setControlPosition(label[0],  (800/8)*0+(800/16),  -1);
	display.setControlSize(label[0],  800/8-6,  -1);

	display.setControlPosition(label[2],  (800/8)*2+(800/16),  -1);
	display.setControlSize(label[2],  800/8-6,  -1);

	display.setControlPosition(label[4],  (800/8)*4+(800/16),  -1);
	display.setControlSize(label[4],  800/8-6,  -1
						   );

	display.setControlPosition(label[6],  (800/8)*6+(800/16),  -1);
	display.setControlSize(label[6],  800/8-6,  -1);


	for(uint8_t i = 0; i < 8; i++)
	{
		display.setControlColors(label[i], interfaceGlobals.activeLabelsColors);
	}
}

void cMasterParams::resizeToSmallConfig(uint8_t labelIdx)
{
	display.setControlPosition(label[labelIdx],  (800/8)*labelIdx+(800/16),  452);
	display.setControlSize(label[labelIdx],  800/8-6,  59);

	display.setControlShow(label[labelIdx+1]);
}

void cMasterParams::showVolume()
{
	sprintf(volumeVal,"%d", mtConfig.audioCodecConfig.volume);

	display.setControlValue(barControl[0], mtConfig.audioCodecConfig.volume);
//	display.setControlValue(barControl[0], mtProject.values.volume);
//	display.setControlShow(barControl[0]);
	display.refreshControl(barControl[0]);

	display.setControlText2(label[0], volumeVal);
	display.refreshControl(label[0]);
}

void cMasterParams::showLimiterAttack()
{
	uint8_t length;

	sprintf(limitAttackVal,"%.3f",(float)(mtProject.values.limiterAttack/1000.0f));
	length=strlen(limitAttackVal);
	limitAttackVal[length]='s';
	limitAttackVal[length+1]=0;

	display.setControlValue(barControl[4], (mtProject.values.limiterAttack*100)/LIMITER_ATTACK_MAX);
	//display.setControlShow(barControl[2]);
	display.refreshControl(barControl[4]);

	display.setControlText2(label[4], limitAttackVal);
	display.refreshControl(label[4]);
}

void cMasterParams::showLimiterRelease()
{
	uint8_t length;

	sprintf(limitReleaseVal,"%.3f",(float)(mtProject.values.limiterRelease/1000.0f));
	length=strlen(limitReleaseVal);
	limitReleaseVal[length]='s';
	limitReleaseVal[length+1]=0;

	display.setControlValue(barControl[5], (mtProject.values.limiterRelease*100)/LIMITER_RELEASE_MAX);
	//display.setControlShow(barControl[2]);
	display.refreshControl(barControl[5]);

	display.setControlText2(label[5], limitReleaseVal);
	display.refreshControl(label[5]);
}

void cMasterParams::showLimiterTreshold()
{
	sprintf(limitThresholdVal,"%d",(mtProject.values.limiterTreshold*100)/LIMITER_TRESHOLD_MAX);

	display.setControlValue(barControl[6], (mtProject.values.limiterTreshold*100)/LIMITER_TRESHOLD_MAX);
	display.setControlShow(barControl[6]);
	display.refreshControl(barControl[6]);

	display.setControlText2(label[6], limitThresholdVal);
	display.refreshControl(label[6]);
}

void cMasterParams::showBitDepth()
{
	sprintf(bitDepthVal,"%d",mtProject.values.bitDepth);

	uint8_t localVal = map(mtProject.values.bitDepth,BIT_DEPTH_MIN,BIT_DEPTH_MAX,0,100);
	display.setControlValue(barControl[3], localVal);
//	display.setControlShow(barControl[3]);
	display.refreshControl(barControl[3]);

	display.setControlText2(label[3], bitDepthVal);
	display.refreshControl(label[3]);
}


//delay

void cMasterParams::showDelayPingPongEnable()
{
	bool enable = (mtProject.values.delayParams & (0b10000000));

	if(enable)
	{
		display.setControlValue(delayPingpongEnableList, 0);
		display.refreshControl(delayPingpongEnableList);

		display.setControlText2(label[0], delayPingPongEnable[0]);
		display.refreshControl(label[0]);
	}
	else
	{
		display.setControlValue(delayPingpongEnableList, 1);
		display.refreshControl(delayPingpongEnableList);

		display.setControlText2(label[0], delayPingPongEnable[1]);
		display.refreshControl(label[0]);
	}
}
void cMasterParams::showDelaySyncEnable()
{
	bool enable = (mtProject.values.delayParams & (0b01000000));

	if(enable)
	{
		display.setControlValue(delaySyncEnableList, 0);
		display.refreshControl(delaySyncEnableList);

		display.setControlText2(label[1], delaySyncEnable[0]);
		display.refreshControl(label[1]);


		display.setControlColors(label[2], interfaceGlobals.activeLabelsColors);
		display.setControlColors(delaySyncRateList, interfaceGlobals.activeListColors);
		display.setControlColors(label[3], interfaceGlobals.inactiveLabelsColors);
		levelBarColors[3][0] = 0x222222;
		display.setControlColors(barControl[3], levelBarColors[3]);

		display.refreshControl(label[2]);
		display.refreshControl(delaySyncRateList);
		display.refreshControl(label[3]);
		display.refreshControl(barControl[3]);

	}
	else
	{
		display.setControlValue(delaySyncEnableList, 1);
		display.refreshControl(delaySyncEnableList);

		display.setControlText2(label[1], delaySyncEnable[1]);
		display.refreshControl(label[1]);


		display.setControlColors(label[3], interfaceGlobals.activeLabelsColors);
		levelBarColors[3][0] = 0xFFFFFF;
		display.setControlColors(barControl[3], levelBarColors[3]);
		display.setControlColors(label[2], interfaceGlobals.inactiveLabelsColors);
		display.setControlColors(delaySyncRateList, interfaceGlobals.inactiveListColors);

		display.refreshControl(label[2]);
		display.refreshControl(delaySyncRateList);
		display.refreshControl(label[3]);
		display.refreshControl(barControl[3]);
	}

	display.synchronizeRefresh();
}
void cMasterParams::showDelayRate()
{
	uint8_t rateIdx = (mtProject.values.delayParams & (0b00111111));

	display.setControlValue(delaySyncRateList, rateIdx);
	display.refreshControl(delaySyncRateList);

	display.setControlText2(label[2], delaySyncRates[rateIdx]);
	display.refreshControl(label[2]);
}
void cMasterParams::showDelayFeedback()
{
	sprintf(delayFeedbackVal,"%d",mtProject.values.delayFeedback);

	uint8_t displayFeedbackTime = map(mtProject.values.delayFeedback,DELAY_FEEDBACK_MIN,DELAY_FEEDBACK_MAX,0,100);

	display.setControlValue(barControl[4], displayFeedbackTime);
	display.refreshControl(barControl[4]);

	display.setControlText2(label[4], delayFeedbackVal);
	display.refreshControl(label[4]);
}

void cMasterParams::showDelayTime()
{
	sprintf(delayTimeVal,"%d ms",mtProject.values.delayTime);

	uint8_t displayDelayTime = 0 ;

	if(mtProject.values.delayTime > 500) displayDelayTime = map(mtProject.values.delayTime,500,DELAY_TIME_MAX,50,100);
	else displayDelayTime = map(mtProject.values.delayTime,0,500,0,50);


	display.setControlValue(barControl[3], displayDelayTime);
	display.refreshControl(barControl[3]);

	display.setControlText2(label[3], delayTimeVal);
	display.refreshControl(label[3]);
}


//mixer
void cMasterParams::showMixerScreen()
{

	display.setControlHide(delayPingpongEnableList);
	display.setControlHide(delaySyncEnableList);
	display.setControlHide(delaySyncRateList);

	display.refreshControl(delayPingpongEnableList);
	display.refreshControl(delaySyncEnableList);
	display.refreshControl(delaySyncRateList);

	display.setControlText(titleLabel, "Master 2/2");
	display.refreshControl(titleLabel);

	if(isSolo)
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			sprintf(mixerLabel[i],"%s", soloTrack == i ? (char*)"Unsolo" : (char*)"Solo");
		}
	}
	else
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			sprintf(mixerLabel[i],"%s", mtProject.values.trackMute[i] ? (char*)"Unmute" :(char*)"Mute");
		}
	}

	if(displayType == display_t::mixer)
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			mtProject.values.TrackNames[i][7] = 0;
			display.setControlText(label[i], &mtProject.values.TrackNames[i][0]);
			showLevelBar(i);
		}
	}

	for(uint8_t i = 0; i < 8; i++)
	{
		display.setControlColors(label[i], mtProject.values.trackMute[i] ? interfaceGlobals.inactiveLabelsColors: interfaceGlobals.activeLabelsColors);
	}

	for(uint8_t i = 0; i < 8; i++)
	{
		display.setControlText2(label[i],mixerLabel[i]);
		display.refreshControl(label[i]);
	}
//	for(uint8_t i = 0; i < 8; i++)
//	{
//		display.setControlHide(barControl[i]);
//		display.refreshControl(barControl[i]);
//	}

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	display.synchronizeRefresh();
}

void cMasterParams::showLevelBar(uint8_t n)
{
	constexpr uint8_t RED_COLOR_LEVEL = 95;
	constexpr uint8_t LEVEL_TRESHOLD_UP = 80;
	constexpr uint8_t LEVEL_TRESHOLD_LOW = 60;

	if(trackLevel[n].value < RED_COLOR_LEVEL)
	{
		if(trackLevel[n].redColorTimer < 350)
		{
			levelBarColors[n][0] = one_true_red;
		}
		else
		{
			if(trackLevel[n].value > LEVEL_TRESHOLD_UP)
			{
				uint8_t green = map(trackLevel[n].value,LEVEL_TRESHOLD_UP,RED_COLOR_LEVEL,((one_true_green&0xff00)>>8),0);
				levelBarColors[n][0] = (one_true_red&0xff0000) | (green << 8);
			}
			else if(trackLevel[n].value > LEVEL_TRESHOLD_LOW)
			{
				uint8_t red = map(trackLevel[n].value,LEVEL_TRESHOLD_LOW,LEVEL_TRESHOLD_UP,0,((one_true_red&0xff0000)>>16));
				levelBarColors[n][0] = (red << 16) | (one_true_green&0xff00);
			}
			else
			{
				levelBarColors[n][0] = one_true_green;
			}
		}

		if(trackLevel[n].redColorTimer > 3000000) trackLevel[n].redColorTimer = 201;
	}
	else
	{
		trackLevel[n].redColorTimer = 0;
		levelBarColors[n][0] = one_true_red;

	}

	if(mtProject.values.trackMute[n]) levelBarColors[n][0] = interfaceGlobals.disabledLabelsColors[1];


	display.setControlShow(barControl[n]);
	display.setControlColors(barControl[n], levelBarColors[n]);
	display.setControlValue(barControl[n], trackLevel[n].value);
	display.refreshControl(barControl[n]);
}


void cMasterParams::showEditTracksNamesMode()
{

	if(editTrackNameMode)
	{
//		if(displayType == display_t::masterValues)
//		{
//			for(uint8_t i = 0; i < 8; i++)
//			{
//				display.setControlText(label[i], &mtProject.values.TrackNames[i][0]); //mixerTrackLabel[i]);
//				showLevelBar(i);
//			}
//		}

		for(uint8_t i = 0; i < 8; i++)
		{
			display.setControlColors(label[i], editModeLabelsColors);
			display.setControlText2(label[i], "Edit");
			display.refreshControl(label[i]);
		}

		display.synchronizeRefresh();
	}
	else
	{
		showMixerScreen();
	}


}

void cMasterParams::showKeyboardExport()
{

	for(uint8_t i = 0; i < 8; i++)
	{
		display.setControlHide(barControl[i]);

		display.setControlText(label[i],"");
		display.setControlText2(label[i],"");
	}

	display.setControlValue(bgLabel, 255);
	display.refreshControl(bgLabel);

	display.setControlText(label[0], "Enter");
	//display.setControlText(label[4], "Auto Name");
	display.setControlText(label[6], "Cancel");
	display.setControlText(label[7], "Save");

	for(uint8_t i = 0; i < 8; i++)
	{
		display.refreshControl(label[i]);
	}

	display.synchronizeRefresh();
}



