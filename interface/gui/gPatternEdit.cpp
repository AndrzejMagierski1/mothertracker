
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
	0xFF0000, // zaznaczenie
	0x111111, // podzialka
	0x222222, // nieaktywny
	0xFFFFFF, // playhead
};

uint32_t patternTrackerSelectionColor = 0xff0000;


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
static const uint8_t fillStepCount = 35;
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
		"11",
		"12",
		"13",
		"14",
		"15",
		"16",
		"17",
		"18",
		"19",
		"20",
		"21",
		"22",
		"23",
		"24",
		"25",
		"26",
		"27",
		"28",
		"29",
		"30",
		"31",
		"32",

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
		&fillStepLabels[13][0],
		&fillStepLabels[14][0],
		&fillStepLabels[15][0],
		&fillStepLabels[16][0],
		&fillStepLabels[17][0],
		&fillStepLabels[18][0],
		&fillStepLabels[19][0],
		&fillStepLabels[20][0],
		&fillStepLabels[21][0],
		&fillStepLabels[22][0],
		&fillStepLabels[23][0],
		&fillStepLabels[24][0],
		&fillStepLabels[25][0],
		&fillStepLabels[26][0],
		&fillStepLabels[27][0],
		&fillStepLabels[28][0],
		&fillStepLabels[29][0],
		&fillStepLabels[30][0],
		&fillStepLabels[31][0],
		&fillStepLabels[32][0],
		&fillStepLabels[33][0],
		&fillStepLabels[34][0],

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
		//prop2.data =  &bottomValuesConfig;
		prop2.colors = interfaceGlobals.activeLabelsColors;

		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY );
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 452;
		prop2.h =  59;

		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);
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
	prop.style = (controlStyleCenterX | controlStyleCenterY | controlStyleFont2);
	prop.colors = patternLabelColors;
	prop.x = (800/8)*(2)+(800/16);
	prop.y = 8*28 + 130;
	prop.w = 800/8-10;
	prop.h = 28;
	if(val1PopupLabel == nullptr)  val1PopupLabel = display.createControl<cLabel>(&prop);

	// label val2
	prop.style = (controlStyleCenterX | controlStyleCenterY | controlStyleFont2);
	prop.x = (800/8)*(3)+(800/16);
	prop.y = 8*28 + 130;
	prop.w = 800/8-10;
	prop.h = 28;
	if(val2PopupLabel == nullptr) val2PopupLabel = display.createControl<cLabel>(&prop);

	// label tla
	prop.style = (controlStyleBackground | controlStyleCenterX | controlStyleNoTransparency );
	prop.colors = patternLabelColors;
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
		display.destroyControl(label[i]);
		label[i] = nullptr;
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

	display.setControlValue(label[0], 1);
	display.setControlValue(label[1], 1);
	display.setControlValue(label[2], 1);
	display.setControlValue(label[3], 0);
	display.setControlValue(label[4], 0);
	display.setControlValue(label[5], 0);
	display.setControlValue(label[6], 0);
	display.setControlValue(label[7], 0);

	display.setControlText2(label[0], "Pattern");
	display.setControlText2(label[1], "Length");
	display.setControlText2(label[2], "Step");
	display.setControlText(label[3], "Fill");
	//display.setControlText(topLabel[4], "Randomise");
	display.setControlText(label[4], "");
	display.setControlText(label[5], "Invert");
	display.setControlText(label[6], "Transpose");
	display.setControlText(label[7], "Undo");


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


	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlPosition(label[i], -1, 452);
		display.setControlSize(label[i], -1, 59);

		display.setControlColors(label[i], interfaceGlobals.activeLabelsColors);

		display.setControlShow(label[i]);

		display.refreshControl(label[i]);
	}


	display.setControlShow(patternControl);
	display.refreshControl(patternControl);

	display.synchronizeRefresh();
}





void cPatternEditor::showEditModeLabels()
{
	display.setControlColors(label[3], interfaceGlobals.activeLabelsColors);
	display.setControlColors(label[4], interfaceGlobals.activeLabelsColors);
	display.setControlColors(label[5], interfaceGlobals.activeLabelsColors);
	display.setControlColors(label[6], interfaceGlobals.activeLabelsColors);
	display.setControlColors(label[7], interfaceGlobals.activeLabelsColors);

	display.refreshControl(label[3]);
	display.refreshControl(label[4]);
	display.refreshControl(label[5]);
	display.refreshControl(label[6]);
	display.refreshControl(label[7]);

	display.synchronizeRefresh();
}

void cPatternEditor::hideEditModeLabels()
{
	display.setControlColors(label[3], interfaceGlobals.inactiveLabelsColors);
	display.setControlColors(label[4], interfaceGlobals.inactiveLabelsColors);
	display.setControlColors(label[5], interfaceGlobals.inactiveLabelsColors);
	display.setControlColors(label[6], interfaceGlobals.inactiveLabelsColors);
	display.setControlColors(label[7], interfaceGlobals.inactiveLabelsColors);

	display.refreshControl(label[3]);
	display.refreshControl(label[4]);
	display.refreshControl(label[5]);
	display.refreshControl(label[6]);
	display.refreshControl(label[7]);

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
	display.setControlText(label[0], pattern);
	display.refreshControl(label[0]);
}

void cPatternEditor::showLength()
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	sprintf(length, "%d",  pattern->track[0].length+1);

	display.setControlText(label[1], length);
	display.refreshControl(label[1]);
}

void cPatternEditor::showStep()
{
//	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	sprintf(step, "%d",  mtProject.values.patternEditStep);

	display.setControlText(label[2], step);
	display.refreshControl(label[2]);
}

void cPatternEditor::refreshPatternParams()
{
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

	display.setControlText(label[1], "Type");

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
		display.setControlText(label[2], "Value");
	else
		display.setControlText(label[2], "From");

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
		display.setControlText(label[3], "");
		display.setControlHide(val2PopupLabel);
	}
	else
	{
		display.setControlText(label[3], "To");
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


		display.setControlPosition(label[4],  (800/8)*4+(800/16), -1);
		display.setControlSize(label[4], 800/8-6, -1);
		display.setControlText(label[4], "In Scale");

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

		display.setControlPosition(label[4],  (800/8)*5, -1);
		display.setControlSize(label[4], 800/4-6, -1);
		display.setControlText(label[4], "Fx Type");

		frameData.places[4] = &framesPopupPlaces[5][0];
	}
	else
	{
		display.setControlHide(param2PopupListControl);
		display.setControlPosition(label[4],  (800/8)*4+(800/16), -1);
		display.setControlSize(label[4], 800/8-6, -1);
		display.setControlText(label[4], "");
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


	display.setControlText(label[0], "Step");
	display.setControlText(label[5], "");



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


	display.setControlText(label[6], "Cancel");
	display.setControlText(label[7], "Fill");

	display.refreshControl(param1PopupListControl);
	display.refreshControl(val1PopupLabel);
	display.refreshControl(val2PopupLabel);
	display.refreshControl(param3PopupListControl);
	display.refreshControl(param2PopupListControl);

	display.setControlShow(patternPopupLabel);
	display.refreshControl(patternPopupLabel);

	display.setControlValue(label[0], 0);
	display.setControlValue(label[1], 0);
	display.setControlValue(label[2], 0);


	for(uint8_t i = 0; i < 8; i++)
	{
		display.setControlPosition(label[i], -1, 463);
		display.setControlSize(label[i], -1, 30);
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
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
		display.setControlText(label[2], "Value");
		display.setControlText(label[3], "");

		display.setControlHide(val2PopupLabel);
	}
	else
	{
		display.setControlText(label[2], "From");
		display.setControlText(label[3], "To");

		display.setControlShow(val2PopupLabel);
	}

	display.refreshControl(label[2]);
	display.refreshControl(label[3]);
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
		display.setControlText(label[4], "In Scale");
	}
	else if(editParam == 3 || editParam == 2)
	{
		display.setControlValue(param2PopupListControl, fillData[editParam].param+1);
		display.setControlText(label[4], "Fx Type");
	}
	else
	{
		display.setControlText(label[4], " ");
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

//##############################################################################################
//###############################                              #################################
//##############################################################################################
void cPatternEditor::activateSelection()
{
	patternTrackerColors[6] = patternTrackerSelectionColor;
}

void cPatternEditor::deactivateSelection()
{
	patternTrackerColors[6] = 0xffffff;
}

void cPatternEditor::playheadRecMode()
{
	patternTrackerColors[9] = patternTrackerSelectionColor;
}

void cPatternEditor::playheadNormalMode()
{
	patternTrackerColors[9] = 0xffffff;
}

