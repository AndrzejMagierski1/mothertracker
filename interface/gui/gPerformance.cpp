



#include <performanceMode.h>


static uint16_t framesPlaces[12][4] =
{
	{0,			 31, 800/12, 387},
	{(800/12)*1, 31, 800/12, 387},
	{(800/12)*2, 31, 800/12, 387},
	{(800/12)*3, 31, 800/12, 387},
	{(800/12)*4, 31, 800/12, 387},
	{(800/12)*5, 31, 800/12, 387},
	{(800/12)*6, 31, 800/12, 387},
	{(800/12)*7, 31, 800/12, 387},
	{(800/12)*8, 31, 800/12, 387},
	{(800/12)*9, 31, 800/12, 387},
	{(800/12)*10, 31, 800/12, 387},
	{(800/12)*11, 31, 800/12, 387},
};

static uint32_t textLabelsColors[] =
{
	0xFFFFFF, // tekst
	0x111111, // tło
	0xFF0000, // ramka
};


static uint32_t activeLabelsColors[] =
{
	0x000000, // tekst
	0xFFFFFF, // tło
	0xFF0000, // ramka
};

static uint32_t inactiveLabelsColors[] =
{
	0x000000, // tekst
	0x555555, // tło
	0xFF0000, // ramka
};

void cPerformanceMode::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleRightX | controlStyleCenterY);
	prop2.x = 769;
	if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
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
		prop.text = (char*)"";
		prop.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY);
		prop.x = (800/8)*i+(800/16);
		prop.y = 465;
		prop.w = 800/8-6;
		prop.h = 30;
		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop);

		prop.y = 437;
		prop.h = 28;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop);
	}


	for(uint8_t i = 0; i<12; i++)
	{
		prop.data = nullptr;
		prop.style = 	(controlStyleCenterX | controlStyleCenterY | controlStyleFont2 | controlStyleVerticalText);
		prop.x = (800/12)*i+(800/24);
		prop.y = 220;
		prop.colors = textLabelsColors;
		if(textLabel[i] == nullptr) textLabel[i] = display.createControl<cLabel>(&prop);


		//textLabelData[i].bitmapIndex = 1;
		//textLabelData[i].xValue = (800/12)*i+(800/24);
		//textLabelData[i].yValue = 320;

		multiLabelData[i].textLinesCount = 4;
		multiLabelData[i].xOffset = 0;
		multiLabelData[i].yOffset = 240;
		//multiLabelData[i].wBg = (800/12);
		//multiLabelData[i].hBg = 400;
		multiLabelData[i].text[0] = &fxValuesText[i][0][0];
		multiLabelData[i].text[1] = &fxValuesText[i][1][0];
		multiLabelData[i].text[2] = &fxValuesText[i][2][0];
		multiLabelData[i].text[3] = &fxValuesText[i][3][0];

		prop.style = 	(controlStyleCenterX | controlStyleFont2);
		if(i%2 == 0) prop.style |= controlStyleBackground;
		prop.x = (800/12)*i+(800/24);
		prop.y = 30;
		prop.colors = textLabelsColors;
		prop.w = (800/12);
		prop.h = 388;
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
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;

		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}

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

void cPerformanceMode::showPerformanceMaster()
{
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	display.setControlText(titleLabel, "Performance Master");
	display.refreshControl(titleLabel);

	//showActualInstrument();


	display.setControlText(bottomLabel[0], "Track 1");
	display.setControlText(bottomLabel[1], "Track 2");
	display.setControlText(bottomLabel[2], "Track 3");
	display.setControlText(bottomLabel[3], "Track 4");
	display.setControlText(bottomLabel[4], "Track 5");
	display.setControlText(bottomLabel[5], "Track 6");
	display.setControlText(bottomLabel[6], "Track 7");
	display.setControlText(bottomLabel[7], "Track 8");


//-------------------------------------

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlPosition(bottomLabel[i], -1, 452);
		display.setControlSize(bottomLabel[i], -1, 59);

		display.setControlHide(topLabel[i]);
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
		//display.refreshControl(topLabel[i]);

		//TO DO: nie powinno tu tego być tylko tma gdzie zarzadzanie zmiennymi projektu
		if(mtProject.values.trackMute[i] >= trackMasterModeCount) mtProject.values.trackMute[i] = 0;

		display.setControlText(textLabel[i], &trackMasterLabels[mtProject.values.trackMute[i]][0]);
		display.setControlShow(textLabel[i]);
		display.refreshControl(textLabel[i]);
	}

	display.setControlHide(textLabel[8]);
	display.setControlHide(textLabel[9]);
	display.setControlHide(textLabel[10]);
	display.setControlHide(textLabel[11]);


	display.synchronizeRefresh();

}


void cPerformanceMode::showPerformanceFxes()
{
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	display.setControlText(titleLabel, "Performance Fxes");
	display.refreshControl(titleLabel);

	display.setControlText(bottomLabel[0], "Track 1");
	display.setControlText(bottomLabel[1], "Track 2");
	display.setControlText(bottomLabel[2], "Track 3");
	display.setControlText(bottomLabel[3], "Track 4");
	display.setControlText(bottomLabel[4], "Track 5");
	display.setControlText(bottomLabel[5], "Track 6");
	display.setControlText(bottomLabel[6], "Track 7");
	display.setControlText(bottomLabel[7], "Track 8");


	//-------------------------------------

	for(uint8_t i = 0; i<performanceFxesCount; i++)
	{
		display.setControlText(textLabel[i], &performanceFxesLabels[mtProject.values.perfFxPlaces[i]][0]);
		display.setControlShow(textLabel[i]);
		display.refreshControl(textLabel[i]);

		showPerformaceValue(i);
	}

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlPosition(bottomLabel[i], -1, 465);
		display.setControlSize(bottomLabel[i], -1, 30);

		//display.setControlShow(topLabel[i]);
		//display.setControlShow(bottomLabel[i]);
		//display.refreshControl(bottomLabel[i]);
		//display.refreshControl(topLabel[i]);
	}

	refreshTracksPatterns();
	refreshTracksState();


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


void cPerformanceMode::refreshFxNames(uint8_t place)
{
	if(place < 12)
	{
		display.setControlText(textLabel[place], &performanceFxesLabels[mtProject.values.perfFxPlaces[place]][0]);
		display.setControlShow(textLabel[place]);
		display.refreshControl(textLabel[place]);
		return;
	}

	for(uint8_t i = 0; i<performanceFxesCount; i++)
	{
		display.setControlText(textLabel[i], &performanceFxesLabels[mtProject.values.perfFxPlaces[i]][0]);
		display.setControlShow(textLabel[i]);
		display.refreshControl(textLabel[i]);
	}
}


void cPerformanceMode::refreshTracksState()
{
	for(uint8_t i = 0; i<8; i++)
	{
		if(tracksPerformanceState[i] == 1)
		{
			uint32_t* ptrColors = activeLabelsColors;
			if(mtProject.values.trackMute[i] > 0) ptrColors = inactiveLabelsColors;

			display.setControlColors(topLabel[i], ptrColors);
			display.setControlColors(bottomLabel[i], ptrColors);
			display.setControlStyle(bottomLabel[i], ( controlStyleBorder | controlStyleBackground | controlStyleCenterX | controlStyleCenterY));
		}
		else
		{
			uint32_t* ptrColors = activeLabelsColors;
			if(mtProject.values.trackMute[i] > 0) ptrColors = inactiveLabelsColors;

			display.setControlColors(topLabel[i], ptrColors);
			display.setControlColors(bottomLabel[i], ptrColors);
			display.setControlStyle(bottomLabel[i], ( controlStyleBackground | controlStyleCenterX | controlStyleCenterY));

		}

		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}


	display.synchronizeRefresh();
}

void cPerformanceMode::refreshTracksPatterns()
{
	for(uint8_t i = 0; i<8; i++)
	{
		sprintf(&trackPaternText[i][0],"Pattern %d", mtProject.values.perfTracksPatterns[i]);
		display.setControlText(topLabel[i], &trackPaternText[i][0]);

		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
		//display.setControlShow(bottomLabel[i]);
		//display.refreshControl(bottomLabel[i]);
	}

	display.synchronizeRefresh();
}

//=================================================================================
// zmiana danych performance w trybie edycji
//=================================================================================
//void changePerformanceData(int16_t value)
//{

//}



//=================================================================================
//
//=================================================================================
void cPerformanceMode::refreshActiveValueForFx(uint8_t fx)
{
	if(fx >= performanceFxesCount) return;

	for(uint8_t place = 0; place < 12; place++)
	{
		if(fx != mtProject.values.perfFxPlaces[place]) continue;

		multiLabelData[place].selected = activeFxValues[mtProject.values.perfFxPlaces[place]]+1;
		display.refreshControl(value1Label[place]);
	}
}

//=================================================================================
//
//=================================================================================
void cPerformanceMode::showPerformaceValue(uint8_t fx)
{
	if(fx >= performanceFxesCount) return;

	for(uint8_t place = 0; place < 12; place++)
	{
		if(fx != mtProject.values.perfFxPlaces[place]) continue;

		multiLabelData[place].selected = activeFxValues[mtProject.values.perfFxPlaces[place]]+1;

		for(uint8_t slot = 0; slot < 4; slot++)
		{
			int16_t fx_value  = slot==activeFxValues[fx] ?  fxTempValues[fx] : fxValues[fx][slot];

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
				else 								strcpy(&fxValuesText[place][slot][0], "Fwd");
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

			default:
				sprintf(&fxValuesText[place][slot][0],"%d", fx_value);
				break;
			}



		}

		display.setControlShow(value1Label[place]);
		display.refreshControl(value1Label[place]);
	}


}

//=================================================================================
//
//=================================================================================
void cPerformanceMode::showArrow(uint8_t place, uint8_t type)
{
	if(mtProject.values.perfFxPlaces[place] == mtPerfFxNone) return;

	//textLabelData[place].bitmapIndex = type;

	display.setAddControlStyle(value1Label[place], controlStyleShowBitmap);
	display.refreshControl(value1Label[place]);
}

//=================================================================================
//
//=================================================================================
void cPerformanceMode::hideArrow(uint8_t place)
{

	display.setRemoveControlStyle(value1Label[place], controlStyleShowBitmap);
	display.refreshControl(value1Label[place]);
}

