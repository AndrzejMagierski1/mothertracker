
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
	{0+1, 		  256, 800/8-3, 165},
	{(800/8)*1+1, 256, 800/8-3, 165},
	{(800/8)*2+1, 256, 800/8-3, 165},
	{(800/8)*3+1, 256, 800/8-3, 165},
	{(800/8)*4+1, 256, 800/8-3, 165},
	{(800/8)*5+1, 256, 800/8-3, 165},
	{(800/8)*6+1, 256, 800/8-3, 165},
	{(800/8)*7+1, 256, 800/8-3, 165},

	{(800/8)*1+1, 256, 800/4-3, 165},
};


static uint32_t patternLabelColors[] =
{
	0xFFFFFF, // tekst
	0x000000, // tło
	0xFF0000, // ramka
};

volatile uint32_t patternTrackerColors[] =
{
	0x000000, // 0 linie
	0x0a0a0a, // 1 background  /numery wierszy juz nie bo nie ma/

	0x00e8be, // 2 nuta
	0xfef749, // 3 instrument
	0xbb58f1, // 4 effekt1
	0x57f1ff, // 5 effekt2

	0x003933, // 6 nuta nieaktywna
	0x453b1b, // 7 instrument nieaktywny
	0x321f3a, // 8 effekt1 nieaktywny
	0x183744, // 9 effekt2 nieaktywny

	one_true_red,//0xf13c3c, // 10 zaznaczenie
	0x0e0e0e, // 11 podzialka
	0x333333, // 12 nieaktywny
	0x000000, // 13 playhead

	0xffffff, // 14 tekst zanzaczenia
	one_true_red, // 15 playhed rec mode
};

uint32_t patternTrackerSelectionColor = one_true_red;


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
		"No",
		"Yes",
};
const char* ptrfillScaleFilterNames[fillScaleFilterCount] =
{
		&fillScaleFilterLabels[0][0],
		&fillScaleFilterLabels[1][0],
};
//------------------------------------------------------------
static const uint8_t fillStepCount = 37;
const char fillStepLabels[fillStepCount][11] =
{
		"Occ. Note",
		"Empty Note",
		"Occ. Fx",
		"Empty Fx",
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
		&fillStepLabels[35][0],
		&fillStepLabels[36][0],
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

		prop2.style = 	( controlStyleCenterX | controlStyleFont3);
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 424;
		prop2.h =  55;

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

	//=====================================================================================================
	// POPUP
	//=====================================================================================================


	// lista 1
	prop.x = (800/8)*(1)+1;
	prop.y = 8*28 + 32;
	prop.w = 800/8-3;
	prop.h = 168;
	prop.colors = nullptr;
	prop.style = controlStyleBackground;
	prop.data = &fillTypeList;
	if(param1PopupListControl == nullptr)  param1PopupListControl = display.createControl<cList>(&prop);

	// lista 2
	prop.x = (800/8)*(4)+1;
	prop.data = &fillScaleFilterList;
	if(param2PopupListControl == nullptr)  param2PopupListControl = display.createControl<cList>(&prop);

	// lista 3
	prop.x = (800/8)*(0)+1;
	prop.data = &fillStepList;
	if(param3PopupListControl == nullptr)  param3PopupListControl = display.createControl<cList>(&prop);

	// bar val1
	prop.x = (800/8)*(2)+1;
	prop.style =  controlStyleValue_0_100 | controlStyleBackground;
	if(val1PopupBar == nullptr)  val1PopupBar = display.createControl<cBar>(&prop);

	// bar val1
	prop.x = (800/8)*(3)+1;
	if(val2PopupBar == nullptr)  val2PopupBar = display.createControl<cBar>(&prop);

	// label tytulowy popupu
	prop.style = ( controlStyleCenterY | controlStyleFont4 | controlStyleBackground);
	prop.colors = nullptr;
	prop.x = 2;
	prop.y = 8*28 + 15;
	prop.w = 795;
	prop.h = 27;
	if(patternPopupTitleLabel == nullptr)  patternPopupTitleLabel = display.createControl<cLabel>(&prop);

	// label tla
	prop.style = (controlStyleBackground | controlStyleCenterX | controlStyleNoTransparency );
	prop.colors = patternLabelColors;
	prop.x = 400;
	prop.y = 8*28 + 4;
	prop.w = 800;
	prop.h = 199;
	if(patternPopupLabel == nullptr)  patternPopupLabel = display.createControl<cLabel>(&prop);

	//=====================================================================================================
	//=====================================================================================================
	//=====================================================================================================



	//prop.text = (char*)"";
	prop.style = 	(controlStyleShow );//| controlStyleFont2 | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
	prop.x = 0;
	prop.y = 0;
	prop.w = 800;
	prop.h = 423;
	patternTrackerColors[10] = patternTrackerSelectionColor;
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

	display.destroyControl(bgLabel);
	bgLabel = nullptr;

	display.destroyControl(param1PopupListControl);
	param1PopupListControl = nullptr;

	display.destroyControl(val1PopupBar);
	val1PopupBar = nullptr;

	display.destroyControl(val2PopupBar);
	val2PopupBar = nullptr;

	display.destroyControl(param3PopupListControl);
	param3PopupListControl = nullptr;

	display.destroyControl(param2PopupListControl);
	param2PopupListControl = nullptr;

	display.destroyControl(patternPopupLabel);
	patternPopupLabel = nullptr;

	display.destroyControl(patternPopupTitleLabel);
	patternPopupTitleLabel = nullptr;

	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(notePopoutControl);
	notePopoutControl = nullptr;

}


void cPatternEditor::showDefaultScreen()
{


//	display.setControlHide(titleLabel);
//	display.setControlHide(titleBar);
//	display.setControlHide(instrumentLabel);

	display.setControlHide(notePopoutControl);

	// bottom labels

	display.setControlValue(label[0], 1);
	display.setControlValue(label[1], 1);
	display.setControlValue(label[2], 1);
	display.setControlValue(label[3], 1);
	display.setControlValue(label[4], 1);
	display.setControlValue(label[5], 1);
	display.setControlValue(label[6], 1);
	display.setControlValue(label[7], 1);

	display.setControlText(label[0], "Pattern");
	display.setControlText(label[1], "Length");
	display.setControlText(label[2], "Step");
	display.setControlText(label[3], "Fill");
	display.setControlText(label[4], "Preview");
	display.setControlText(label[5], "Invert");
	display.setControlText(label[6], "");
	display.setControlText(label[7], "Undo");

	display.setControlText2(label[3], "");
	display.setControlText2(label[4], "");
	display.setControlText2(label[5], "");
	display.setControlText2(label[6], "");
	display.setControlText2(label[7], "");

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
		//display.setControlPosition(label[i], -1, 424);
		//display.setControlSize(label[i], -1, 55);


		display.setControlColors(label[i], interfaceGlobals.activeLabelsColors);

		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);

		display.setControlShow(label[i]);

		display.refreshControl(label[i]);
	}




	display.setControlShow(bgLabel);
	display.refreshControl(bgLabel);

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
	display.setControlText2(label[0], pattern);
	display.refreshControl(label[0]);
}

void cPatternEditor::showLength()
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	sprintf(length, "%d",  pattern->track[0].length+1);

	display.setControlText2(label[1], length);
	display.refreshControl(label[1]);
}

void cPatternEditor::showStep()
{
//	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	sprintf(step, "%d",  mtProject.values.patternEditStep);

	display.setControlText2(label[2], step);
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

void cPatternEditor::showFillNote()
{
	// typ
	fillTypeList.linesCount = fillTypeListCount;
	fillTypeList.start = fillData[editParam].type;
	fillTypeList.length = fillTypeListCount;
	fillTypeList.data = (char**)(&ptrfillTypeListNames);

	display.setControlPosition(param1PopupListControl, (800/8)*(3)+1, -1);
	display.setControlData(param1PopupListControl, &fillTypeList);
	display.setControlValue(param1PopupListControl, fillData[editParam].type);
	display.setControlShow(param1PopupListControl);

	display.setControlText(label[3], "Fill Type");

	// value/from
	display.setControlPosition(val1PopupBar, (800/8)*(4)+1, -1);
	display.setControlValue(val1PopupBar, (fillData[editParam].from*100)/(127));
	display.setControlShow(val1PopupBar);

	display.setControlText2(label[4], (char*)&mtNotes[fillData[editParam].from][0]);

	if(fillData[editParam].type == 0)
		display.setControlText(label[4], "Note");
	else
		display.setControlText(label[4], "From");

	// to
	display.setControlPosition(val2PopupBar, (800/8)*(5)+1, -1);

	if(fillData[editParam].type == 0)
	{
		display.setControlText(label[5], "");
		display.setControlText2(label[5], "");
		display.setControlValue(val2PopupBar, -1);
	}
	else
	{
		display.setControlText2(label[5], (char*)&mtNotes[fillData[editParam].to][0]);
		display.setControlValue(val2PopupBar, (fillData[editParam].to*100)/(127));
		display.setControlShow(val2PopupBar);
		display.setControlText(label[5], "To");
	}

	//scale
	fillScaleFilterList.linesCount = fillScaleFilterCount;
	fillScaleFilterList.start = fillData[editParam].param;
	fillScaleFilterList.length = fillScaleFilterCount;
	fillScaleFilterList.data = (char**)(&ptrfillScaleFilterNames);

	display.setControlPosition(param2PopupListControl, (800/8)*(1)+1, -1);
	display.setControlData(param2PopupListControl, &fillScaleFilterList);
	display.setControlValue(param2PopupListControl, fillData[editParam].param);
	display.setControlShow(param2PopupListControl);
	display.setControlSize(param2PopupListControl, 800/4-3, -1);
	display.setControlPosition(param2PopupListControl, (800/8)*(1)+1, -1);

	display.setControlPosition(label[1],  (800/8)*2, -1);
	display.setControlSize(label[1], 800/8-6, -1);
	display.setControlText(label[1], "In Scale");

	display.setControlText(label[2], "");
	display.setControlText2(label[2], "");

	frameData.places[1] = &framesPopupPlaces[8][0];

	display.setControlValue(bgLabel, 250);

	for(uint8_t i = 0; i < 6; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}
}

void cPatternEditor::showFillInstr()
{
	// typ
	fillTypeList.linesCount = fillTypeListCount;
	fillTypeList.start = fillData[editParam].type;
	fillTypeList.length = fillTypeListCount;
	fillTypeList.data = (char**)(&ptrfillTypeListNames);

	display.setControlPosition(param1PopupListControl, (800/8)*(1)+1, -1);
	display.setControlData(param1PopupListControl, &fillTypeList);
	display.setControlValue(param1PopupListControl, fillData[editParam].type);
	display.setControlShow(param1PopupListControl);
	display.setControlPosition(label[1],  (800/8)*1+800/16, -1);
	display.setControlText(label[1], "Fill Type");

	// value/from
	fillText1[0]  = 0;
	display.setControlPosition(val1PopupBar, (800/8)*(2)+1, -1);
	display.setControlShow(val1PopupBar);
	display.setControlValue(val1PopupBar, (fillData[editParam].from*100)/(47+16));

	if(fillData[editParam].from < INSTRUMENTS_COUNT)
		sprintf(fillText1, "%d", (fillData[editParam].from+1));
	else
		sprintf(fillText1, "%d", (fillData[editParam].from+3));
	display.setControlText2(label[2], fillText1);

	if(fillData[editParam].type == 0)
		display.setControlText(label[2], "Instrument");
	else
		display.setControlText(label[2], "From");

	// to
	fillText2[0]  = 0;
	display.setControlPosition(val2PopupBar, (800/8)*(3)+1, -1);
	display.setControlShow(val2PopupBar);

	if(fillData[editParam].type == 0)
	{
		display.setControlText(label[3], "");
		display.setControlText2(label[3], "");
		display.setControlValue(val2PopupBar, -1);
	}
	else
	{
		display.setControlValue(val2PopupBar, (fillData[editParam].from*100)/(47+16));
		if(fillData[editParam].to < INSTRUMENTS_COUNT)
			sprintf(fillText2, "%d", (fillData[editParam].to+1));
		else
			sprintf(fillText2, "%d", (fillData[editParam].to+3));
		display.setControlText2(label[3], fillText2);
		display.setControlText(label[3], "To");
	}


	display.setControlHide(param2PopupListControl);
	display.setControlHide(label[4]);
	display.setControlHide(label[5]);
	display.setControlValue(bgLabel, 255);

	for(uint8_t i = 0; i < 4; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}
}

void cPatternEditor::showFillFx()
{
	// typ
	fillTypeList.linesCount = fillTypeListCount;
	fillTypeList.start = fillData[editParam].type;
	fillTypeList.length = fillTypeListCount;
	fillTypeList.data = (char**)(&ptrfillTypeListNames);

	display.setControlPosition(param1PopupListControl, (800/8)*(3)+1, -1);
	display.setControlData(param1PopupListControl, &fillTypeList);
	display.setControlValue(param1PopupListControl, fillData[editParam].type);
	display.setControlShow(param1PopupListControl);
	display.setControlText(label[3], "Fill Type");
	display.setControlText2(label[3], "");

	// fx
	fillFxTypeList.linesCount = 6;
	fillFxTypeList.start = fillData[editParam].param+1;
	fillFxTypeList.length = FX_COUNT+1;
	fillFxTypeList.data = (char**)(&interfaceGlobals.ptrAllFxNames);

	display.setControlData(param2PopupListControl, &fillFxTypeList);
	display.setControlValue(param2PopupListControl, fillData[editParam].param+1);
	display.setControlShow(param2PopupListControl);
	display.setControlSize(param2PopupListControl, 800/4-6, -1);
	display.setControlPosition(param2PopupListControl, (800/8)*(1)+1, -1);

	display.setControlPosition(label[1],  (800/8)*2, -1);
	display.setControlSize(label[1], 800/4-3, -1);
	display.setControlText(label[1], "Fx Type");
	display.setControlText(label[2], "");
	display.setControlText2(label[2], "");

	frameData.places[1] = &framesPopupPlaces[8][0];

	// value/from
	fillText1[0]  = 0;
	display.setControlPosition(val1PopupBar, (800/8)*(4)+1, -1);
	display.setControlValue(val1PopupBar, (fillData[editParam].from*100)/(255));
	display.setControlShow(val1PopupBar);
	sprintf(fillText1, "%d", (fillData[editParam].from));
	display.setControlText2(label[4], fillText1);

	if(fillData[editParam].type == 0)
		display.setControlText(label[4], "Fx Value");
	else
		display.setControlText(label[4], "From");

	// to
	fillText2[0]  = 0;
	display.setControlPosition(val2PopupBar, (800/8)*(5)+1, -1);
	display.setControlShow(val2PopupBar);

	if(fillData[editParam].type == 0)
	{
		display.setControlText(label[5], "");
		display.setControlText2(label[5], "");
		display.setControlValue(val2PopupBar, -1);
	}
	else
	{
		sprintf(fillText2, "%d", (fillData[editParam].to));
		display.setControlText2(label[5], fillText2);
		display.setControlValue(val2PopupBar, (fillData[editParam].from*100)/(255));
		display.setControlText(label[5], "To");
	}

	for(uint8_t i = 0; i < 6; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}
}

void cPatternEditor::showFillPopup()
{
	frameData.places[0] = &framesPopupPlaces[0][0];
	frameData.places[1] = &framesPopupPlaces[1][0];
	frameData.places[2] = &framesPopupPlaces[2][0];
	frameData.places[3] = &framesPopupPlaces[3][0];
	frameData.places[4] = &framesPopupPlaces[4][0];
	frameData.places[5] = &framesPopupPlaces[5][0];
	frameData.places[6] = &framesPopupPlaces[6][0];
	frameData.places[7] = &framesPopupPlaces[7][0];

	// step
	fillStepList.linesCount = 6;
	fillStepList.start = fillStep+4;
	fillStepList.length = fillStepCount;
	fillStepList.data = (char**)(&ptrfillStepNames);

	display.setControlPosition(param3PopupListControl, (800/8)*(0)+1, -1);
	display.setControlData(param3PopupListControl, &fillStepList);
	display.setControlShow(param3PopupListControl);

	display.setControlText(label[0], "Step");
	display.setControlText2(label[0], "");

	// pozostale
	switch(editParam)
	{
	case 0:
		showFillNote();
		display.setControlText(patternPopupTitleLabel, " Fill Notes");
		break;
	case 1:
		showFillInstr();
		display.setControlText(patternPopupTitleLabel, " Fill Instruments");
		break;
	case 2:
		showFillFx();
		display.setControlText(patternPopupTitleLabel, " Fill Fx 1");
		break;
	case 3:
		showFillFx();
		display.setControlText(patternPopupTitleLabel, " Fill Fx 2");
		break;

	default:
		display.setControlText(patternPopupTitleLabel, " Fill");
		break;
	}

	display.setControlText(label[6], "Cancel");
	display.setControlText(label[7], "Fill");
	display.setControlShow(label[6]);
	display.refreshControl(label[6]);
	display.setControlShow(label[7]);
	display.refreshControl(label[7]);

	display.refreshControl(param1PopupListControl);
	display.refreshControl(val1PopupBar);
	display.refreshControl(val2PopupBar);
	display.refreshControl(param3PopupListControl);
	display.refreshControl(param2PopupListControl);
	display.refreshControl(param1PopupListControl);
	display.setControlShow(patternPopupLabel);
	display.refreshControl(patternPopupLabel);
	display.setControlShow(patternPopupTitleLabel);
	display.refreshControl(patternPopupTitleLabel);

	display.refreshControl(bgLabel);

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
		if(editParam == 1)
		{
			if(fillData[editParam].from < INSTRUMENTS_COUNT)
				sprintf(fillText1, "%d", (fillData[editParam].from+1));
			else
				sprintf(fillText1, "%d", (fillData[editParam].from+3));
			display.setControlText2(label[2], fillText1);

			if(fillData[editParam].type == 0)
				display.setControlText(label[2], "Instrument");
			else
				display.setControlText(label[2], "From");

			display.setControlValue(val2PopupBar,-1);
		}
		else if(editParam == 0)
		{

		}

	}
	else
	{
		display.setControlText(label[2], "From");
		display.setControlText(label[3], "To");

		display.setControlShow(val2PopupBar);
		display.setControlShow(label[3]);
	}

	display.refreshControl(label[2]);
	display.refreshControl(label[3]);
}


void cPatternEditor::refreshFillFrom()
{
	fillText1[0]  = 0;
	if(editParam == 0)
	{
		display.setControlText2(label[2], (char*)&mtNotes[fillData[editParam].from][0]);
		display.setControlValue(val1PopupBar, (fillData[editParam].from*100)/(127));
	}
	else if(editParam == 1)
	{
		if(fillData[editParam].from < INSTRUMENTS_COUNT)
			sprintf(fillText1, "%d", (fillData[editParam].from+1));
		else
			sprintf(fillText1, "%d", (fillData[editParam].from+3));

		display.setControlText2(label[2], fillText1);
		display.setControlValue(val1PopupBar, (fillData[editParam].from*100)/(47+16));
	}
	else if(editParam == 2 || editParam == 3)
	{
		sprintf(fillText1, "%d", (fillData[editParam].from));
		display.setControlText2(label[2], fillText1);
		display.setControlValue(val1PopupBar, (fillData[editParam].from*100)/(255));
	}

	display.refreshControl(label[2]);
	display.refreshControl(val1PopupBar);
}

void cPatternEditor::refreshFillTo()
{
	fillText2[0]  = 0;
	if(editParam == 0)
	{
		display.setControlText2(label[3], (char*)&mtNotes[fillData[editParam].to][0]);
		display.setControlValue(val2PopupBar, (fillData[editParam].to*100)/(127));
	}
	else if (editParam == 1)
	{
		if(fillData[editParam].to < INSTRUMENTS_COUNT)
			sprintf(fillText2, "%d", (fillData[editParam].to+1));
		else
			sprintf(fillText2, "%d", (fillData[editParam].to+3));

		display.setControlText2(label[3], fillText2);
		display.setControlValue(val2PopupBar, (fillData[editParam].to*100)/(47+16));
	}
	else if(editParam == 2 || editParam == 3)
	{
		sprintf(fillText2, "%d", (fillData[editParam].to));
		display.setControlText2(label[3], fillText2);
		display.setControlValue(val2PopupBar, (fillData[editParam].to*100)/(255));
	}

	display.refreshControl(label[3]);
	display.refreshControl(val2PopupBar);
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
	display.setControlValue(param3PopupListControl, fillStep+4);
	//display.setControlText(bottomLabel[5], "Step");

	display.refreshControl(param3PopupListControl);
}

void cPatternEditor::hideFillPopup()
{
	display.setControlHide(param1PopupListControl);
//	display.setControlHide(val1PopupLabel);
//	display.setControlHide(val2PopupLabel);
	display.setControlHide(val1PopupBar);
	display.setControlHide(val2PopupBar);
	display.setControlHide(param3PopupListControl);
	display.setControlHide(param2PopupListControl);

//	display.refreshControl(param1PopupListControl);
//	display.refreshControl(val1PopupLabel);
//	display.refreshControl(val2PopupLabel);
//	display.refreshControl(val3PopupLabel);
//	display.refreshControl(param2PopupListControl);


	display.setControlPosition(label[1],  (800/8)*1+800/16, -1);


	display.setControlValue(bgLabel, 255);

	display.setControlHide(patternPopupTitleLabel);
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
	patternTrackerColors[10] = patternTrackerSelectionColor;
}

void cPatternEditor::deactivateSelection()
{
	patternTrackerColors[10] = 0xffffff;
}
