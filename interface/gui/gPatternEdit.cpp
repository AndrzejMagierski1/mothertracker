
#include <patternEditor.h>
#include "mtStructs.h"



static uint16_t framesPlaces[8][4] =
{
	{0+1, 		  421, 800/8-1, 65},
	{(800/8)*1+1, 421, 800/8-1, 65},
	{(800/8)*2+1, 421, 800/8-1, 65},
	{(800/8)*3+1, 421, 800/8-1, 65},
	{(800/8)*4+1, 421, 800/8-1, 65},
	{(800/8)*5+1, 421, 800/8-1, 65},
	{(800/8)*6+1, 421, 800/8-1, 65},
	{(800/8)*7+1, 421, 800/8-1, 65},
};

static uint16_t framesPopupPlaces[9][4] =
{
	{0, 		255, 800/8, 187},
	{(800/8)*1, 255, 800/8, 187},
	{(800/8)*2, 255, 800/8, 187},
	{(800/8)*3, 255, 800/8, 187},

	{(800/8)*4, 255, 800/8, 187},
	{(800/8)*4, 255, 800/4, 187},

	{(800/8)*5, 255, 800/8, 187},
	{(800/8)*6, 255, 800/8, 187},
	{(800/8)*7, 255, 800/8, 187},
};


static uint32_t patternLabelColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFF0000, // ramka
};


volatile uint32_t patternTrackerColors[] =
{
	0xFFFFFF, // linie
	0xFFFFFF, // numery wierszy
	0xFFFFFF, // nuta
	0xFFFFFF, // instrument
	0xFFFFFF, // volume
	0xFFFFFF, // effekt
	0xff0000, // zaznaczenie
	0x111111, // podzialka
	0x222222, // nieaktywny
};

uint32_t patternTrackerSelectionColor = 0xff0000;

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
//------------------------------------------------------------
static const uint8_t trackMasterModeCount = 2;
static const char trackMasterLabels[trackMasterModeCount][5] =
{
	"On",
	"Off",
};
//------------------------------------------------------------
static const uint8_t fillTypeListCount = 3;
const char fillTypeListLabels[fillTypeListCount][11] =
{
		"Constant",
		"From-To",
		"Random",
};
const char* ptrfillTypeListNames[fillTypeListCount] =
{
		&fillTypeListLabels[0][0],
		&fillTypeListLabels[1][0],
		&fillTypeListLabels[2][0],
};
//------------------------------------------------------------
static const uint8_t fillScaleFilterCount = 2;
const char fillScaleFilterLabels[fillScaleFilterCount][5] =
{
		"Yes",
		"No",
};
const char* ptrfillScaleFilterNames[fillScaleFilterCount] =
{
		&fillScaleFilterLabels[0][0],
		&fillScaleFilterLabels[1][0],
};
//------------------------------------------------------------
static const uint8_t fillStepCount = 13;
const char fillStepLabels[fillStepCount][11] =
{
		"Empty",
		"Occupied",
		"Random",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
};

const char* ptrfillStepNames[fillStepCount] =
{
		&fillStepLabels[0][0],
		&fillStepLabels[1][0],
		&fillStepLabels[2][0],
		&fillStepLabels[3][0],
		&fillStepLabels[4][0],
		&fillStepLabels[5][0],
		&fillStepLabels[6][0],
		&fillStepLabels[7][0],
		&fillStepLabels[8][0],
		&fillStepLabels[9][0],
		&fillStepLabels[10][0],
		&fillStepLabels[11][0],
		&fillStepLabels[12][0],
};
//------------------------------------------------------------


void cPatternEditor::initDisplayControls()
{
	// inicjalizacja kontrolek
	strControlProperties prop;

	// ramka
	frameData.placesCount = 8;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	frameData.places[7] = &framesPlaces[7][0];
	prop.x = 0;
	prop.y = 0;
	prop.w = 0;
	prop.h = 0;
	prop.style = 0;
	prop.value = 0;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);


	strControlProperties prop2;

	for(uint8_t i = 0; i<8; i++)
	{
		prop2.text = (char*)"";
		prop2.data =  &bottomValuesConfig;
		prop2.colors = activeLabelsColors;

		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY );
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;

		prop2.y = i>3 ? 452 : 437;
		prop2.h = i>3 ? 59 : 28;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.y = 465;
		prop2.h = 30;
		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);
	}



	//=====================================================================================================
	// POPUP
	//=====================================================================================================


	// lista 1
	prop.x = (800/8)*(1)+5;
	prop.y = 8*28 + 122;
	prop.w = 800/8-10;
	prop.h = 25;
	prop.colors = nullptr;
	prop.style = controlStyleCenterY;
	prop.data = &fillTypeList;
	if(param1PopupListControl == nullptr)  param1PopupListControl = display.createControl<cList>(&prop);

	// lista 2
	prop.x = (800/8)*(4)+5;
	prop.y = 8*28 + 122;
	prop.w = 800/8-10;
	prop.h = 25;
	prop.data = &fillScaleFilterList;
	if(param2PopupListControl == nullptr)  param2PopupListControl = display.createControl<cList>(&prop);

	// lista 3
	prop.x = (800/8)*(0)+5;
	prop.y = 8*28 + 122;
	prop.w = 800/8-10;
	prop.h = 25;
	prop.data = &fillStepList;
	if(param3PopupListControl == nullptr)  param3PopupListControl = display.createControl<cList>(&prop);

	// label val1
	prop.style = 	(controlStyleCenterX | controlStyleCenterY | controlStyleFont2);
	prop.colors =  patternLabelColors;
	prop.x = (800/8)*(2)+(800/16);
	prop.y = 8*28 + 130;
	prop.w = 800/8-10;
	prop.h = 28;
	if(val1PopupLabel == nullptr)  val1PopupLabel = display.createControl<cLabel>(&prop);

	// label val2
	prop.style = 	(controlStyleCenterX | controlStyleCenterY | controlStyleFont2);
	prop.x = (800/8)*(3)+(800/16);
	prop.y = 8*28 + 130;
	prop.w = 800/8-10;
	prop.h = 28;
	if(val2PopupLabel == nullptr)  val2PopupLabel = display.createControl<cLabel>(&prop);

	// label tla
	prop.style = 	(controlStyleBackground | controlStyleCenterX | controlStyleNoTransparency );
	prop.colors =  patternLabelColors;
	prop.x = 400;
	prop.y = 8*28 + 4;
	prop.w = 800;
	prop.h = 214;
	if(patternPopupLabel == nullptr)  patternPopupLabel = display.createControl<cLabel>(&prop);


	//=====================================================================================================
	//=====================================================================================================
	//=====================================================================================================



	//prop.text = (char*)"";
	prop.style = 	(controlStyleShow );//| controlStyleFont2 | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
	prop.x = 0;
	prop.y = 0;
	prop.w = 50;
	prop.h = 25;
	patternTrackerColors[6] = patternTrackerSelectionColor;
	prop.colors = (uint32_t*)patternTrackerColors;
	prop.data = &trackerPattern;
	prop.value = patternViewMode;
	if(patternControl == nullptr)  patternControl = display.createControl<cTracker>(&prop);
	//hTrackControl = display.createControl<cLabel>(&prop);
	//display.refreshControl(hTrackControl);

}


void cPatternEditor::destroyDisplayControls()
{
	display.destroyControl(patternControl);
	patternControl = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;


		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}

	display.destroyControl(param1PopupListControl);
	param1PopupListControl = nullptr;

	display.destroyControl(val1PopupLabel);
	val1PopupLabel = nullptr;

	display.destroyControl(val2PopupLabel);
	val2PopupLabel = nullptr;

	display.destroyControl(param3PopupListControl);
	param3PopupListControl = nullptr;

	display.destroyControl(param2PopupListControl);
	param2PopupListControl = nullptr;


	display.destroyControl(patternPopupLabel);
	patternPopupLabel = nullptr;

	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(notePopoutControl);
	notePopoutControl = nullptr;


	for(uint8_t i = 0; i<4; i++)
	{
		display.destroyControl(fxListControl[i]);
		fxListControl[i] = nullptr;
	}

/*
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(instrumentLabel);
	instrumentLabel = nullptr;

*/
}


void cPatternEditor::showDefaultScreen()
{


//	display.setControlHide(titleLabel);
//	display.setControlHide(titleBar);
//	display.setControlHide(instrumentLabel);

	display.setControlHide(notePopoutControl);

	for(uint8_t i = 0; i<4; i++)
	{
		display.setControlHide(fxListControl[i]);
	}

	// bottom labels
	display.setControlText(bottomLabel[0], "Pattern");
	display.setControlText(bottomLabel[1], "Length");
	display.setControlText(bottomLabel[2], "Step");
	display.setControlText(bottomLabel[3], "");
	display.setControlText(bottomLabel[4], "");
	display.setControlText(bottomLabel[5], "");
	display.setControlText(bottomLabel[6], "");
	display.setControlText(bottomLabel[7], "");

	display.setControlText(topLabel[3], "");
	display.setControlText(topLabel[4], "");
	display.setControlText(topLabel[5], "");
	display.setControlText(topLabel[6], "");
	display.setControlText(topLabel[7], "");

//	showTempo();
	showPattern();
	showLength();
	showStep();

	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	frameData.places[7] = &framesPlaces[7][0];

	activateLabelsBorder();

	display.setControlPosition(topLabel[3], -1, 452);
	display.setControlPosition(topLabel[4], -1, 452);
	display.setControlPosition(topLabel[5], -1, 452);
	display.setControlPosition(topLabel[6], -1, 452);
	display.setControlPosition(topLabel[7], -1, 452);
	display.setControlSize(topLabel[3], -1, 59);
	display.setControlSize(topLabel[4], -1, 59);
	display.setControlSize(topLabel[5], -1, 59);
	display.setControlSize(topLabel[6], -1, 59);
	display.setControlSize(topLabel[7], -1, 59);

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlColors(topLabel[i], activeLabelsColors);
		display.setControlColors(bottomLabel[i], activeLabelsColors);

		display.setControlShow(topLabel[i]);

		if(i>2)	display.setControlHide(bottomLabel[i]);
		else 	display.setControlShow(bottomLabel[i]);

		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
	}


	display.setControlShow(patternControl);
	display.refreshControl(patternControl);

	display.synchronizeRefresh();
}





void cPatternEditor::showEditModeLabels()
{
	display.setControlText(topLabel[3], "Fill");
	//display.setControlText(topLabel[4], "Randomise");
	display.setControlText(topLabel[4], "");
	display.setControlText(topLabel[5], "Invert");
	display.setControlText(topLabel[6], "Transpose");
	display.setControlText(topLabel[7], "Undo");


	display.refreshControl(topLabel[3]);
	display.refreshControl(topLabel[4]);
	display.refreshControl(topLabel[5]);
	display.refreshControl(topLabel[6]);
	display.refreshControl(topLabel[7]);

	display.synchronizeRefresh();

}

void cPatternEditor::hideEditModeLabels()
{
	display.setControlText(topLabel[3], "");
	display.setControlText(topLabel[4], "");
	display.setControlText(topLabel[5], "");
	display.setControlText(topLabel[6], "");
	display.setControlText(topLabel[7], "");

	display.refreshControl(topLabel[3]);
	display.refreshControl(topLabel[4]);
	display.refreshControl(topLabel[5]);
	display.refreshControl(topLabel[6]);
	display.refreshControl(topLabel[7]);

	display.synchronizeRefresh();
}


//void cPatternEditor::showTempo()
//{
//	Sequencer::strPattern * pattern = sequencer.getPatternToUI();
//
//	//itoa(pattern->tempo, tempo, 10);
//	sprintf(tempo,"%.1f",  pattern->tempo);
//
//	display.setControlText(topLabel[0], tempo);
//	display.refreshControl(topLabel[0]);
//}

void cPatternEditor::showPattern()
{
	sprintf(pattern,"%d", mtProject.values.actualPattern);
	display.setControlText(topLabel[0], pattern);
	display.refreshControl(topLabel[0]);
}

void cPatternEditor::showLength()
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	sprintf(length, "%d",  pattern->track[0].length+1);

	display.setControlText(topLabel[1], length);
	display.refreshControl(topLabel[1]);
}

void cPatternEditor::showStep()
{
//	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	sprintf(step, "%d",  mtProject.values.patternEditStep);

	display.setControlText(topLabel[2],step);
	display.refreshControl(topLabel[2]);
}

void cPatternEditor::refreshPatternParams()
{
	if(masterTrackState) return;
//	showTempo();
	showPattern();
	showLength();
}


//##############################################################################################
//###############################             FILL             #################################
//##############################################################################################


void cPatternEditor::showFillPopup()
{
	//------------------------------
	// typ
	fillTypeList.linesCount = fillTypeListCount;
	fillTypeList.start = fillData[editParam].type;
	fillTypeList.length = fillTypeListCount;
	fillTypeList.data = (char**)(&ptrfillTypeListNames);

	display.setControlData(param1PopupListControl, &fillTypeList);
	display.setControlValue(param1PopupListControl, fillData[editParam].type);
	display.setControlShow(param1PopupListControl);

	display.setControlText(bottomLabel[1], "Type");

	//------------------------------
	// value/from
	fillText1[0]  = 0;
	if(editParam == 0)
	{
		display.setControlText(val1PopupLabel, (char*)&mtNotes[fillData[editParam].from][0]);
	}
	else if(editParam == 1)
	{
		if(fillData[editParam].from < INSTRUMENTS_COUNT)
			sprintf(fillText1, "%d", (fillData[editParam].from+1));
		else
			sprintf(fillText1, "%d", (fillData[editParam].from+3));

		display.setControlText(val1PopupLabel, fillText1);
	}
	else if(editParam == 2 || editParam == 3)
	{
		sprintf(fillText1, "%d", (fillData[editParam].from));
		display.setControlText(val1PopupLabel, fillText1);
	}
	display.setControlShow(val1PopupLabel);

	if(fillData[editParam].type == 0)
		display.setControlText(bottomLabel[2], "Value");
	else
		display.setControlText(bottomLabel[2], "From");

	//------------------------------
	// To
	fillText2[0]  = 0;
	if(editParam == 0)
	{
		display.setControlText(val2PopupLabel, (char*)&mtNotes[fillData[editParam].to][0]);
	}
	else if (editParam == 1)
	{
		if(fillData[editParam].to < INSTRUMENTS_COUNT)
			sprintf(fillText2, "%d", (fillData[editParam].to+1));
		else
			sprintf(fillText2, "%d", (fillData[editParam].to+3));

		display.setControlText(val2PopupLabel, fillText2);
	}
	else if(editParam == 2 || editParam == 3)
	{
		sprintf(fillText2, "%d", (fillData[editParam].to));
		display.setControlText(val2PopupLabel, fillText2);
	}

	if(fillData[editParam].type == 0)
	{
		display.setControlText(bottomLabel[3], "");
		display.setControlHide(val2PopupLabel);
	}
	else
	{
		display.setControlText(bottomLabel[3], "To");
		display.setControlShow(val2PopupLabel);
	}

	//------------------------------
	// skala
	if(editParam == 0)
	{
		fillScaleFilterList.linesCount = fillScaleFilterCount;
		fillScaleFilterList.start = fillData[editParam].param;
		fillScaleFilterList.length = fillScaleFilterCount;
		fillScaleFilterList.data = (char**)(&ptrfillScaleFilterNames);

		display.setControlData(param2PopupListControl, &fillScaleFilterList);
		display.setControlValue(param2PopupListControl, fillData[editParam].param);
		display.setControlShow(param2PopupListControl);
		display.setControlSize(param2PopupListControl, 800/8-10, 25);


		display.setControlPosition(bottomLabel[4],  (800/8)*4+(800/16), -1);
		display.setControlSize(bottomLabel[4], 800/8-6, -1);
		display.setControlText(bottomLabel[4], "In Scale");

		frameData.places[4] = &framesPopupPlaces[4][0];
	}
	//------------------------------
	// fx
	else if(editParam == 3 || editParam == 2)
	{
		fillFxTypeList.linesCount = 7;
		fillFxTypeList.start = fillData[editParam].param+1;
		fillFxTypeList.length = FX_COUNT+1;
		fillFxTypeList.data = (char**)(&interfaceGlobals.ptrAllFxNames);

		display.setControlData(param2PopupListControl, &fillFxTypeList);
		display.setControlValue(param2PopupListControl, fillData[editParam].param+1);
		display.setControlShow(param2PopupListControl);
		display.setControlSize(param2PopupListControl, 800/4-10, 25);

		display.setControlPosition(bottomLabel[4],  (800/8)*5, -1);
		display.setControlSize(bottomLabel[4], 800/4-6, -1);
		display.setControlText(bottomLabel[4], "Fx Type");

		frameData.places[4] = &framesPopupPlaces[5][0];
	}
	else
	{
		display.setControlHide(param2PopupListControl);
		display.setControlText(bottomLabel[4], "");
	}

	//------------------------------
	// step

	fillStepList.linesCount = 7;
	fillStepList.start = fillStep+2;
	fillStepList.length = fillStepCount;
	fillStepList.data = (char**)(&ptrfillStepNames);

	display.setControlData(param3PopupListControl, &fillStepList);
	//display.setControlValue(param3PopupListControl, fillStep+2);
	display.setControlShow(param3PopupListControl);
	//display.setControlSize(param3PopupListControl, 800/8-10, 25);


	display.setControlText(bottomLabel[0], "Step");
	display.setControlText(bottomLabel[5], "");



	switch(editParam)
	{
	case 0:
		display.setControlText(patternPopupLabel, "Fill Notes");
		break;
	case 1:
		display.setControlText(patternPopupLabel, "Fill Instruments");
		break;
	case 2:
		display.setControlText(patternPopupLabel, "Fill Fx 1");
		break;
	case 3:
		display.setControlText(patternPopupLabel, "Fill Fx 2");
		break;

	default:
		display.setControlText(patternPopupLabel, "Fill");
		break;
	}


	display.setControlText(bottomLabel[6], "Cancel");
	display.setControlText(bottomLabel[7], "Fill");

	display.refreshControl(param1PopupListControl);
	display.refreshControl(val1PopupLabel);
	display.refreshControl(val2PopupLabel);
	display.refreshControl(param3PopupListControl);
	display.refreshControl(param2PopupListControl);

	display.setControlShow(patternPopupLabel);
	display.refreshControl(patternPopupLabel);

	for(uint8_t i = 0; i < 8; i++)
	{
		//display.setControlText(topLabel[i], "");
		display.setControlHide(topLabel[i]);
		display.setControlShow(bottomLabel[i]);
		//display.refreshControl(topLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}

	frameData.places[0] = &framesPopupPlaces[0][0];
	frameData.places[1] = &framesPopupPlaces[1][0];
	frameData.places[2] = &framesPopupPlaces[2][0];
	frameData.places[3] = &framesPopupPlaces[3][0];

	// 4 zmienna
	//frameData.places[4] = &framesPopupPlaces[5][0];
	frameData.places[5] = &framesPopupPlaces[6][0];
	frameData.places[6] = &framesPopupPlaces[7][0];
	frameData.places[7] = &framesPopupPlaces[8][0];


	setFillPlace(fillPlace);
	activateFillPopupBorder();

	display.synchronizeRefresh();
}


void cPatternEditor::refreshFillType()
{
	display.setControlValue(param1PopupListControl, fillData[editParam].type);
	display.refreshControl(param1PopupListControl);

	if(fillData[editParam].type == 0)
	{
		display.setControlText(bottomLabel[2], "Value");
		display.setControlText(bottomLabel[3], "");

		display.setControlHide(val2PopupLabel);
	}
	else
	{
		display.setControlText(bottomLabel[2], "From");
		display.setControlText(bottomLabel[3], "To");

		display.setControlShow(val2PopupLabel);
	}

	display.refreshControl(bottomLabel[2]);
	display.refreshControl(bottomLabel[3]);
}


void cPatternEditor::refreshFillFrom()
{
	fillText1[0]  = 0;
	if(editParam == 0)
	{
		display.setControlText(val1PopupLabel, (char*)&mtNotes[fillData[editParam].from][0]);
	}
	else if(editParam == 1)
	{
		if(fillData[editParam].from < INSTRUMENTS_COUNT)
			sprintf(fillText1, "%d", (fillData[editParam].from+1));
		else
			sprintf(fillText1, "%d", (fillData[editParam].from+3));

		display.setControlText(val1PopupLabel, fillText1);
	}
	else if(editParam == 2 || editParam == 3)
	{
		sprintf(fillText1, "%d", (fillData[editParam].from));
		display.setControlText(val1PopupLabel, fillText1);
	}

	display.refreshControl(val1PopupLabel);
}

void cPatternEditor::refreshFillTo()
{
	fillText2[0]  = 0;
	if(editParam == 0)
	{
		display.setControlText(val2PopupLabel, (char*)&mtNotes[fillData[editParam].to][0]);
	}
	else if (editParam == 1)
	{
		if(fillData[editParam].to < INSTRUMENTS_COUNT)
			sprintf(fillText2, "%d", (fillData[editParam].to+1));
		else
			sprintf(fillText2, "%d", (fillData[editParam].to+3));

		display.setControlText(val2PopupLabel, fillText2);
	}
	else if(editParam == 2 || editParam == 3)
	{
		sprintf(fillText2, "%d", (fillData[editParam].to));
		display.setControlText(val2PopupLabel, fillText2);
	}

	display.refreshControl(val2PopupLabel);
}

void cPatternEditor::refreshFillParam()
{

	if(editParam == 0)
	{
		display.setControlValue(param2PopupListControl, fillData[editParam].param);
		display.setControlText(bottomLabel[4], "In Scale");
	}
	else if(editParam == 3 || editParam == 2)
	{
		display.setControlValue(param2PopupListControl, fillData[editParam].param+1);
		display.setControlText(bottomLabel[4], "Fx Type");
	}
	else
	{
		display.setControlText(bottomLabel[4], " ");
	}

	display.refreshControl(param2PopupListControl);
}

void cPatternEditor::refreshFillStep()
{
	//------------------------------
	// step
	//Serial.println(fillStep+2);
	display.setControlValue(param3PopupListControl, fillStep+2);
	//display.setControlText(bottomLabel[5], "Step");

	display.refreshControl(param3PopupListControl);
}

void cPatternEditor::hideFillPopup()
{
	display.setControlHide(param1PopupListControl);
	display.setControlHide(val1PopupLabel);
	display.setControlHide(val2PopupLabel);
	display.setControlHide(param3PopupListControl);
	display.setControlHide(param2PopupListControl);

//	display.refreshControl(param1PopupListControl);
//	display.refreshControl(val1PopupLabel);
//	display.refreshControl(val2PopupLabel);
//	display.refreshControl(val3PopupLabel);
//	display.refreshControl(param2PopupListControl);

	display.setControlHide(patternPopupLabel);
//	display.refreshControl(patternPopupLabel);
}
//


//##############################################################################################
//###############################                              #################################
//##############################################################################################


void cPatternEditor::activateFillPopupBorder()
{
	if(fillPlace < 0 || fillPlace > frameData.placesCount-1)
	{
		display.setControlHide(frameControl);
		display.refreshControl(frameControl);
		return;
	}

	display.setControlValue(frameControl, fillPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}




void cPatternEditor::activateLabelsBorder()
{
	if(selectedPlace < 0 || selectedPlace > frameData.placesCount-1)
	{
		display.setControlHide(frameControl);
		display.refreshControl(frameControl);
		return;
	}

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}


//-----------------------------------------------------------
void cPatternEditor::showTracksMaster()
{

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
		uint32_t* ptrColors = activeLabelsColors;
		if(mtProject.values.trackMute[i]) ptrColors = inactiveLabelsColors;

		display.setControlColors(topLabel[i], ptrColors);
		display.setControlColors(bottomLabel[i], ptrColors);

		display.setControlPosition(topLabel[i], -1, 437);
		display.setControlSize(topLabel[i], -1, 28);

		display.setControlPosition(bottomLabel[i], -1, 465);
		display.setControlSize(bottomLabel[i], -1, 30);

		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

		display.setControlText(topLabel[i], &trackMasterLabels[mtProject.values.trackMute[i]][0]);
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	display.synchronizeRefresh();

}

void cPatternEditor::refreshTracksMaster()
{
	for(uint8_t i = 0; i<8; i++)
	{
		uint32_t* ptrColors = activeLabelsColors;

		if(mtProject.values.trackMute[i]) ptrColors = inactiveLabelsColors;


		display.setControlText(topLabel[i], &trackMasterLabels[mtProject.values.trackMute[i]][0]);
		display.setControlColors(topLabel[i], ptrColors);
		display.setControlColors(bottomLabel[i], ptrColors);

		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

	}

	display.synchronizeRefresh();
}



