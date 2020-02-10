



#include "performanceMode/performanceMode.h"


static uint16_t framesPlaces[12][4] =
{
	{11,		 29,  62, 391},
	{11 + 65*1,  29,  62, 391},
	{11 + 65*2,  29,  62, 391},
	{11 + 65*3,  29,  62, 391},
	{11 + 65*4,  29,  62, 391},
	{11 + 65*5,  29,  62, 391},
	{11 + 65*6,  29,  62, 391},
	{11 + 65*7,  29,  62, 391},
	{11 + 65*8,  29,  62, 391},
	{11 + 65*9,  29,  62, 391},
	{11 + 65*10, 29,  62, 391},
	{11 + 65*11, 29,  62, 391},
};

static uint32_t textLabelsColors[] =
{
	0xFFFFFF, // tekst
	0x111111, // tło
	0xFF0000, // ramka
};

static uint32_t valuesLabelColors[] =
{
	0xFFFFFF, // tekst
	0x0a0a0a, // tło
	one_true_red, // zanzaczone
};


uint32_t redBlinkLabelColors[4] =
{
	one_true_red, // tekst
	0x323132, // tło
	one_true_red, // ramka
	one_true_red, // tekst2
};


void cPerformanceMode::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY | controlStyleFont4);
	prop2.x = 9;
	prop2.y = 13;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleRightX | controlStyleCenterY);
	prop2.x = 769;
	if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 2;
	prop2.y = 0;
	prop2.w = 795;
	prop2.h = 26;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);


	strControlProperties prop;
	// ramka
	//strControlProperties prop;
	frameData.placesCount = 12;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	frameData.places[7] = &framesPlaces[7][0];
	frameData.places[8] = &framesPlaces[8][0];
	frameData.places[9] = &framesPlaces[9][0];
	frameData.places[10] = &framesPlaces[10][0];
	frameData.places[11] = &framesPlaces[11][0];
	prop.style = 0;
	prop.value = 0;
	prop.colors = nullptr;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);


	//strControlProperties prop;

	// inicjalizacja kontrolek
	for(uint8_t i = 0; i<8; i++)
	{
		prop2.text = (char*)"";
		prop2.colors = interfaceGlobals.activeLabelsColors;
		prop2.style = 	(controlStyleCenterX  | controlStyleFont3);
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 424;
		prop2.h =  55;
		prop2.value =  1;

		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);
	}

	prop2.text = nullptr;
	prop2.colors = interfaceGlobals.activeBgLabelsColors;
	prop2.value = 255;
	prop2.style = controlStyleNoTransparency | controlStyleShow;
	prop2.x = 0;
	prop2.w = 800;
	prop2.y = 424;
	prop2.h =  55;
	if(bgLabel == nullptr) bgLabel = display.createControl<cBgLabel>(&prop2);


	for(uint8_t i = 0; i<12; i++)
	{
		prop.data = nullptr;
		prop.style = 	(controlStyleCenterX | controlStyleCenterY | controlStyleFont2 | controlStyleVerticalText);
		prop.x = 42  + 65 * i;
		prop.y = 210;
		prop.colors = textLabelsColors;
		if(textLabel[i] == nullptr) textLabel[i] = display.createControl<cLabel>(&prop);


		//textLabelData[i].bitmapIndex = 1;
		//textLabelData[i].xValue = (800/12)*i+(800/24);
		//textLabelData[i].yValue = 320;

		multiLabelData[i].textLinesCount = 4;
		multiLabelData[i].xOffset = 0;
		multiLabelData[i].yOffset = 20;
		//multiLabelData[i].wBg = (800/12);
		//multiLabelData[i].hBg = 400;
		multiLabelData[i].text[0] = &fxValuesText[i][0][0];
		multiLabelData[i].text[1] = &fxValuesText[i][1][0];
		multiLabelData[i].text[2] = &fxValuesText[i][2][0];
		multiLabelData[i].text[3] = &fxValuesText[i][3][0];

		prop.style = 	(controlStyleCenterX | controlStyleFont2 | controlStyleBackground | controlStyleBottomShadow);
		//prop.style |= controlStyleBackground;
		prop.x = 42  + 65*i;
		prop.y = 249;
		prop.colors = valuesLabelColors;
		prop.w = 62;
		prop.h = 174;
		prop.data = &multiLabelData[i];
		if(value1Label[i] == nullptr) value1Label[i] = display.createControl<cMultiLabel>(&prop);
	}


}


void cPerformanceMode::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(instrumentLabel);
	instrumentLabel = nullptr;


	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;
	}

	display.destroyControl(bgLabel);
	bgLabel = nullptr;


	for(uint8_t i = 0; i<12; i++)
	{
		display.destroyControl(textLabel[i]);
		textLabel[i] = nullptr;

		display.destroyControl(value1Label[i]);
		value1Label[i] = nullptr;
	}


	display.destroyControl(frameControl);
	frameControl = nullptr;

}

/*
void cInstrumentEditor::showDefaultScreen()
{
	// bottom labels
	display.setControlText(bottomLabel[0], "Start");
	display.setControlText(bottomLabel[1], "Loop Start");
	display.setControlText(bottomLabel[2], "Loop End");
	display.setControlText(bottomLabel[3], "End");
	display.setControlText(bottomLabel[4], "- Zoom");
	display.setControlText(bottomLabel[5], "+ Zoom");
	display.setControlText(bottomLabel[6], "Play Mode");
	//display.setControlText(bottomLabel[7], "End");

	display.refreshControl(titleLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
		display.refreshControl(barControl[i]);
	}

	display.synchronizeRefresh();

}
*/



void cPerformanceMode::showPerformanceFxes()
{
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	display.setControlText(titleLabel, "Performance FX");
	display.refreshControl(titleLabel);

	display.setControlText(label[0], "Track 1");
	display.setControlText(label[1], "Track 2");
	display.setControlText(label[2], "Track 3");
	display.setControlText(label[3], "Track 4");
	display.setControlText(label[4], "Track 5");
	display.setControlText(label[5], "Track 6");
	display.setControlText(label[6], "Track 7");
	display.setControlText(label[7], "Track 8");

//	display.setControlText2(label[0], "Pattern");
//	display.setControlText2(label[1], "Pattern");
//	display.setControlText2(label[2], "Pattern");
//	display.setControlText2(label[3], "Pattern");
//	display.setControlText2(label[4], "Pattern");
//	display.setControlText2(label[5], "Pattern");
//	display.setControlText2(label[6], "Pattern");
//	display.setControlText2(label[7], "Pattern");

	//-------------------------------------

	for(uint8_t i = 0; i < 12; i++)
	{
		display.setControlText(textLabel[i], &performanceFxesLabels[mtProject.values.perfFxPlaces[i]][0]);
		display.setControlShow(textLabel[i]);
		display.refreshControl(textLabel[i]);

		showPerformaceValue(i);
	}

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	showTracksPatterns();
	showTracksState();


	display.refreshControl(bgLabel);

	display.synchronizeRefresh();
}


//====================================================================================================

void cPerformanceMode::showEditFrame(uint8_t place)
{
	display.setControlValue(frameControl, place);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

void cPerformanceMode::hideEditFrame()
{
	display.setControlHide(frameControl);
	display.refreshControl(frameControl);
}


void cPerformanceMode::showFxNames(uint8_t place)
{
	if(place < 12)
	{
		display.setControlText(textLabel[place], &performanceFxesLabels[mtProject.values.perfFxPlaces[place]][0]);
		display.setControlShow(textLabel[place]);
		display.refreshControl(textLabel[place]);
		return;
	}

	for(uint8_t i = 0; i < 12; i++)
	{
		display.setControlText(textLabel[i], &performanceFxesLabels[mtProject.values.perfFxPlaces[i]][0]);
		display.setControlShow(textLabel[i]);
		display.refreshControl(textLabel[i]);
	}
}


void cPerformanceMode::showTracksState()
{
	for(uint8_t i = 0; i<8; i++)
	{
		if(tracksPerformanceState[i] == 1)
		{
			uint32_t* ptrColors = interfaceGlobals.activeLabelsColors;
			if(mtProject.values.trackMute[i] > 0) ptrColors = interfaceGlobals.disabledLabelsColors;

			display.setControlColors(label[i], ptrColors);
			//display.setControlColors(bottomLabel[i], ptrColors);
			display.setControlStyle(label[i], controlStyleCenterX | controlStyleFont3 | controlStyleBorder );
		}
		else
		{
			uint32_t* ptrColors = interfaceGlobals.activeLabelsColors;
			if(mtProject.values.trackMute[i] > 0) ptrColors = interfaceGlobals.disabledLabelsColors;

			display.setControlColors(label[i], ptrColors);
			//display.setControlColors(bottomLabel[i], ptrColors);
			display.setControlStyle(label[i], controlStyleCenterX | controlStyleFont3);

		}

		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
		//display.setControlShow(bottomLabel[i]);
		//display.refreshControl(bottomLabel[i]);
	}


	display.synchronizeRefresh();
}

void cPerformanceMode::showTracksPatterns()
{
	for(uint8_t i = 0; i<8; i++)
	{
		sprintf(&trackPaternText[i][0],"Patt: %d", mtProject.values.perfTracksPatterns[i]);
		//sprintf(&trackPaternText[i][0],"%d", mtProject.values.perfTracksPatterns[i]);
		display.setControlText2(label[i], &trackPaternText[i][0]);

		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
		//display.setControlShow(bottomLabel[i]);
		//display.refreshControl(bottomLabel[i]);
	}

	display.synchronizeRefresh();
}

//=================================================================================
//
//=================================================================================
void cPerformanceMode::showPerformaceValue(uint8_t place)
{
	if(place >= 12) return;

	multiLabelData[place].selected = mtProject.values.perfSelectedValues[place]+1;

	for(uint8_t slot = 0; slot < 4; slot++)
	{

		int16_t fx_value  = (slot == mtProject.values.perfSelectedValues[place])  	// ten zapis ustawial na start zaznaczone wartosci  fxow z tempValues
				? placesTempValues[place]											// zamiast poprawnych; naprawione w start()
				: mtProject.values.perfFxValues[place][slot];

		// wyjątkowe efejkty (nie liczbowe) obslużyc wyjątkowo
		switch(mtProject.values.perfFxPlaces[place])
		{
		case mtPerfFxNone:
		{
			strcpy(&fxValuesText[place][slot][0], "");
			break;
		}
		case mtPerfSamplePlayback:
		{
			if(fx_value == 1) 	strcpy(&fxValuesText[place][slot][0], "<<<");
			else 				strcpy(&fxValuesText[place][slot][0], ">>>");
			break;
		}
		case mtPerfStepStutter:
		{
			strcpy(&fxValuesText[place][slot][0], &performanceStutterLabels[fx_value][0]);
			break;
		}
		case mtPerfPatternPlayMode:
		{
			if(fx_value == 1) 		strcpy(&fxValuesText[place][slot][0], "Back");
			else if(fx_value == 2) 	strcpy(&fxValuesText[place][slot][0], "Rnd");
			else 					strcpy(&fxValuesText[place][slot][0], "Fwd");
			break;
		}

		case mtPerfPatternLength:
		{
				if (fx_value <= 0) strcpy(&fxValuesText[place][slot][0], "---");
				else
				{
					sprintf(&fxValuesText[place][slot][0], "%d", performancePatternLengthValues[fx_value-1]);

				}
			break;
		}

		// nie wyjatkowe (liczbowe) tu
		default:
			if(fx_value == 0) strcpy(&fxValuesText[place][slot][0], "---");
			else if(fx_value > 0) sprintf(&fxValuesText[place][slot][0],"+%d", fx_value);
			else sprintf(&fxValuesText[place][slot][0],"%d", fx_value);
			break;
		}
	}


	display.setControlShow(value1Label[place]);
	display.refreshControl(value1Label[place]);

}




void cPerformanceMode::colorTracksLabel(uint8_t track, uint8_t state)
{
	if(state) display.setControlColors(label[track], redBlinkLabelColors);
	else 	display.setControlColors(label[track], interfaceGlobals.activeLabelsColors);

	display.refreshControl(label[track]);
}
////=================================================================================
////
////=================================================================================
//void cPerformanceMode::showArrow(uint8_t place, uint8_t type)
//{
//	if(mtProject.values.perfFxPlaces[place] == mtPerfFxNone) return;
//
//	//textLabelData[place].bitmapIndex = type;
//
//	display.setAddControlStyle(value1Label[place], controlStyleShowBitmap);
//	display.refreshControl(value1Label[place]);
//}
//
////=================================================================================
////
////=================================================================================
//void cPerformanceMode::hideArrow(uint8_t place)
//{
//
//	display.setRemoveControlStyle(value1Label[place], controlStyleShowBitmap);
//	display.refreshControl(value1Label[place]);
//}

