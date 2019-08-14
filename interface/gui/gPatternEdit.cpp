
#include <patternEditor.h>
#include "mtStructs.h"


static uint16_t framesPlaces[8][4] =
{
	{0, 		417, 800/8, 68},
	{(800/8)*1, 417, 800/8, 68},
	{(800/8)*2, 417, 800/8, 68},
	{(800/8)*3, 417, 800/8, 68},
	{(800/8)*4, 417, 800/8, 68},
	{(800/8)*5, 417, 800/8, 68},
	{(800/8)*6, 417, 800/8, 68},
	{(800/8)*7, 417, 800/8, 68},
};

static uint16_t framesPopupPlaces[9][4] =
{
	{0, 		260, 800/8, 408-228},
	{(800/8)*1, 260, 800/8, 408-228},
	{(800/8)*2, 260, 800/8, 408-228},

	{(800/8)*3, 260, 800/8, 408-228},
	{(800/8)*3, 260, 800/4, 408-228},

	{(800/8)*4, 260, 800/8, 408-228},
	{(800/8)*5, 260, 800/8, 408-228},
	{(800/8)*6, 260, 800/8, 408-228},
	{(800/8)*7, 260, 800/8, 408-228},
};


static uint32_t patternLabelColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFF0000, // ramka
};

void cPatternEditor::initDisplayControls()
{
	// inicjalizacja kontrolek

	for(uint8_t i = 0; i<8; i++)
	{
		strControlProperties prop2;
		prop2.text = (char*)"";
		prop2.data =  &bottomValuesConfig;

		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleRoundedBorder);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 465;
		prop2.w = 800/8-10;
		prop2.h = 30;
		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.y = i>3 ? 450 : 435;
		prop2.h = i>3 ? 60 : 30;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);

	}



	strControlProperties prop;
	//prop.text = (char*)"";
	prop.style = 	(controlStyleShow );//| controlStyleFont2 | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
	prop.x = 0;
	prop.y = 0;
	prop.w = 50;
	prop.h = 25;
	prop.data = &trackerPattern;
	if(patternControl == nullptr)  patternControl = display.createControl<cTracker>(&prop);
	//hTrackControl = display.createControl<cLabel>(&prop);
	//display.refreshControl(hTrackControl);




	//=====================================================================================================
	// POPUP
	//=====================================================================================================

	// label tla
	prop.style = 	(controlStyleBackground | controlStyleCenterX | controlStyleNoTransparency );
	prop.colors =  patternLabelColors;
	prop.x = 400;
	prop.y = 8*28 + 4;
	prop.w = 800; // jedna kolumna
	prop.h = 214;
	if(patternPopupLabel == nullptr)  patternPopupLabel = display.createControl<cLabel>(&prop);

	// lista 1
	prop.x = (800/8)*(0)+5;
	prop.y = 8*28 + 130;
	prop.w = 800/8-10;
	prop.h = 25;
	prop.colors = nullptr;
	prop.style = controlStyleCenterY;
	prop.data = &fillTypeList;
	if(param1PopupListControl == nullptr)  param1PopupListControl = display.createControl<cList>(&prop);

	// lista 2
	prop.x = (800/8)*(3)+5;
	prop.y = 8*28 + 130;
	prop.w = 800/8-10;
	prop.h = 25;
	prop.data = &fillScaleFilterList;
	if(param2PopupListControl == nullptr)  param2PopupListControl = display.createControl<cList>(&prop);

	// label val1
	prop.style = 	(controlStyleCenterX | controlStyleCenterY | controlStyleFont2);
	prop.colors =  patternLabelColors;
	prop.x = (800/8)*(1)+(800/16);
	prop.y = 8*28 + 130;
	prop.w = 800/8-10;
	prop.h = 28;
	if(val1PopupLabel == nullptr)  val1PopupLabel = display.createControl<cLabel>(&prop);

	// label val2
	prop.style = 	(controlStyleCenterX | controlStyleCenterY | controlStyleFont2);
	prop.x = (800/8)*(2)+(800/16);
	prop.y = 8*28 + 130;
	prop.w = 800/8-10;
	prop.h = 28;
	if(val2PopupLabel == nullptr)  val2PopupLabel = display.createControl<cLabel>(&prop);

	// label val3
	prop.style = 	(controlStyleCenterX | controlStyleCenterY | controlStyleFont2);
	prop.x = (800/8)*(5)+(800/16);
	prop.y = 8*28 + 130;
	prop.w = 800/8-10;
	prop.h = 28;
	if(val3PopupLabel == nullptr)  val3PopupLabel = display.createControl<cLabel>(&prop);



	//=====================================================================================================


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

	display.destroyControl(val3PopupLabel);
	val3PopupLabel = nullptr;

	display.destroyControl(param2PopupListControl);
	param2PopupListControl = nullptr;


	display.destroyControl(patternPopupLabel);
	patternPopupLabel = nullptr;

	display.destroyControl(frameControl);
	frameControl = nullptr;
}


void cPatternEditor::showDefaultScreen()
{

//	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	//display.setControlShow(patternControl);
	display.refreshControl(patternControl);

	// bottom labels
	display.setControlText(bottomLabel[0], "Tempo");
	display.setControlText(bottomLabel[1], "Pattern");
	display.setControlText(bottomLabel[2], "Length");
	display.setControlText(bottomLabel[3], "Step");
	display.setControlText(bottomLabel[4], "");
	display.setControlText(bottomLabel[5], "");
	display.setControlText(bottomLabel[6], "");
	display.setControlText(bottomLabel[7], "");


	showTempo();
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

	for(uint8_t i = 0; i<4; i++)
	{
		display.setControlShow(topLabel[i]);
		display.setControlShow(topLabel[i+4]);

		display.setControlShow(bottomLabel[i]);
		display.setControlHide(bottomLabel[i+4]);

		display.refreshControl(bottomLabel[i]);
		display.refreshControl(bottomLabel[i+4]);
		display.refreshControl(topLabel[i]);
		display.refreshControl(topLabel[i+4]);
	}


	display.synchronizeRefresh();
}

void cPatternEditor::showEditModeLabels()
{
	display.setControlText(topLabel[4], "Fill");
	display.setControlText(topLabel[5], "Randomise");
	display.setControlText(topLabel[6], "Invert");

	display.refreshControl(topLabel[4]);
	display.refreshControl(topLabel[5]);
	display.refreshControl(topLabel[6]);

	display.synchronizeRefresh();

}

void cPatternEditor::hideEditModeLabels()
{
	display.setControlText(topLabel[4], "");
	display.setControlText(topLabel[5], "");
	display.setControlText(topLabel[6], "");

	display.refreshControl(topLabel[4]);
	display.refreshControl(topLabel[5]);
	display.refreshControl(topLabel[6]);

	display.synchronizeRefresh();
}


void cPatternEditor::showTempo()
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	//itoa(pattern->tempo, tempo, 10);
	sprintf(tempo,"%.1f",  pattern->tempo);

	display.setControlText(topLabel[0], tempo);
	display.refreshControl(topLabel[0]);
}

void cPatternEditor::showPattern()
{
	sprintf(pattern,"%d", mtProject.values.actualPattern);
	display.setControlText(topLabel[1], pattern);
	display.refreshControl(topLabel[1]);
}

void cPatternEditor::showLength()
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	sprintf(length, "%d",  pattern->track[0].length+1);

	display.setControlText(topLabel[2], length);
	display.refreshControl(topLabel[2]);
}

void cPatternEditor::showStep()
{
//	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	sprintf(step, "%d",  mtProject.values.patternEditStep);

	display.setControlText(topLabel[3],step);
	display.refreshControl(topLabel[3]);
}


//##############################################################################################
//###############################             FILL             #################################
//##############################################################################################


void cPatternEditor::showFillPopup()
{
	//------------------------------
	// typ
	for(uint8_t i = 0; i<fillTypeListCount ; i++)
	{
		fillTypeListNames[i] = (char*)&fillTypeListLabels[i][0];
	}

	fillTypeList.linesCount = fillTypeListCount;
	fillTypeList.start = fillData[editParam].type;
	fillTypeList.length = fillTypeListCount;
	fillTypeList.data = fillTypeListNames;

	display.setControlData(param1PopupListControl, &fillTypeList);
	display.setControlValue(param1PopupListControl, fillData[editParam].type);
	display.setControlShow(param1PopupListControl);

	display.setControlText(bottomLabel[0], "Type");

	//------------------------------
	// value/from
	if(editParam == 0)
	{
		display.setControlText(val1PopupLabel, (char*)&mtNotes[fillData[editParam].from][0]);

	}
	else if (editParam == 1 || editParam == 2 || editParam == 3)
	{
		uint8_t offs = (editParam == 1 ? 1 : 0);
		fillText1[0]  = 0;

		sprintf(fillText1, "%d", (fillData[editParam].from + offs));

		display.setControlText(val1PopupLabel, fillText1);
	}
	display.setControlShow(val1PopupLabel);

	if(fillData[editParam].type == 0)
		display.setControlText(bottomLabel[1], "Value");
	else
		display.setControlText(bottomLabel[1], "From");

	//------------------------------
	// To
	if(editParam == 0)
	{
		display.setControlText(val2PopupLabel, (char*)&mtNotes[fillData[editParam].to][0]);
	}
	else if (editParam == 1 || editParam == 2 || editParam == 3)
	{
		uint8_t offs = (editParam == 1 ? 1 : 0);
		fillText2[0]  = 0;

		sprintf(fillText2, "%d", (fillData[editParam].to + offs));

		display.setControlText(val2PopupLabel, fillText2);
	}

	if(fillData[editParam].type == 0)
	{
		display.setControlText(bottomLabel[2], "");
		display.setControlHide(val2PopupLabel);
	}
	else
	{
		display.setControlText(bottomLabel[2], "To");
		display.setControlShow(val2PopupLabel);
	}

	//------------------------------
	// skala
	if(editParam == 0)
	{
		for(uint8_t i = 0; i<fillScaleFilterCount ; i++)
		{
			fillScaleFilterNames[i] = (char*)&fillScaleFilterLabels[i][0];
		}

		fillScaleFilterList.linesCount = fillScaleFilterCount;
		fillScaleFilterList.start = fillData[editParam].param;
		fillScaleFilterList.length = fillScaleFilterCount;
		fillScaleFilterList.data = fillScaleFilterNames;

		display.setControlData(param2PopupListControl, &fillScaleFilterList);
		display.setControlValue(param2PopupListControl, fillData[editParam].param);
		display.setControlShow(param2PopupListControl);
		display.setControlSize(param2PopupListControl, 800/8-10, 25);

		display.setControlText(bottomLabel[3], "In Scale");

		frameData.places[3] = &framesPopupPlaces[3][0];
	}
	//------------------------------
	// fx
	else if(editParam == 3)
	{
		for(uint8_t i = 0; i<fillFxTypeCount ; i++)
		{
			fillFxTypeNames[i] = (char*)&fillFxTypeLabels[i][0];
		}

		fillFxTypeList.linesCount = 7;
		fillFxTypeList.start = fillData[editParam].param;
		fillFxTypeList.length = fillFxTypeCount;
		fillFxTypeList.data = fillFxTypeNames;

		display.setControlData(param2PopupListControl, &fillFxTypeList);
		display.setControlValue(param2PopupListControl, fillData[editParam].param);
		display.setControlShow(param2PopupListControl);
		display.setControlSize(param2PopupListControl, 800/4-10, 25);

		display.setControlText(bottomLabel[3], "Fx Type");

		frameData.places[3] = &framesPopupPlaces[4][0];
	}
	else
	{
		display.setControlHide(param2PopupListControl);
		display.setControlText(bottomLabel[3], "");
	}

	//------------------------------
	// step

	fillText3[0]  = 0;
	sprintf(fillText3, "%d", fillStep);

	display.setControlText(val3PopupLabel, fillText3);
	display.setControlShow(val3PopupLabel);

	display.setControlText(bottomLabel[5], "Step");
	display.setControlText(bottomLabel[4], "");



	switch(editParam)
	{
	case 0:
		display.setControlText(patternPopupLabel, "Fill Notes");
		break;
	case 1:
		display.setControlText(patternPopupLabel, "Fill Instruments");
		break;
	case 2:
		display.setControlText(patternPopupLabel, "Fill Volumes");
		break;
	case 3:
		display.setControlText(patternPopupLabel, "Fill Fx");
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
	display.refreshControl(val3PopupLabel);
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
	// 3 zmienna
	frameData.places[4] = &framesPopupPlaces[5][0];
	frameData.places[5] = &framesPopupPlaces[6][0];
	frameData.places[6] = &framesPopupPlaces[7][0];
	frameData.places[7] = &framesPopupPlaces[8][0];

	activateFillPopupBorder();

	display.synchronizeRefresh();
}


void cPatternEditor::refreshFillType()
{
	display.setControlValue(param1PopupListControl, fillData[editParam].type);
	display.refreshControl(param1PopupListControl);

	if(fillData[editParam].type == 0)
	{
		display.setControlText(bottomLabel[1], "Value");
		display.setControlText(bottomLabel[2], "");

		display.setControlHide(val2PopupLabel);
	}
	else
	{
		display.setControlText(bottomLabel[1], "From");
		display.setControlText(bottomLabel[2], "To");

		display.setControlShow(val2PopupLabel);
	}

	display.refreshControl(bottomLabel[1]);
	display.refreshControl(bottomLabel[2]);
}


void cPatternEditor::refreshFillFrom()
{
	if(editParam == 0)
	{
		display.setControlText(val1PopupLabel, (char*)&mtNotes[fillData[editParam].from][0]);
	}
	else if (editParam == 1 || editParam == 2 || editParam == 3)
	{
		uint8_t offs = (editParam == 1 ? 1 : 0);

		fillText1[0]  = 0;
		sprintf(fillText1, "%d", (fillData[editParam].from + offs));
		display.setControlText(val1PopupLabel, fillText1);
	}

	display.refreshControl(val1PopupLabel);
	//display.refreshControl(bottomLabel[1]);
}

void cPatternEditor::refreshFillTo()
{
	if(editParam == 0)
	{
		display.setControlText(val2PopupLabel, (char*)&mtNotes[fillData[editParam].to][0]);
	}
	else if (editParam == 1 || editParam == 2 || editParam == 3)
	{
		uint8_t offs = (editParam == 1 ? 1 : 0);

		fillText2[0]  = 0;
		sprintf(fillText2, "%d", (fillData[editParam].to + offs));
		display.setControlText(val2PopupLabel, fillText2);
	}

	display.refreshControl(val2PopupLabel);
	//display.refreshControl(bottomLabel[2]);
}

void cPatternEditor::refreshFillParam()
{
	display.setControlValue(param2PopupListControl, fillData[editParam].param);

	if(editParam == 0)
	{
		display.setControlText(bottomLabel[3], "In Scale");
	}
	else if(editParam == 3)
	{
		display.setControlText(bottomLabel[3], "Fx Type");
	}
	else
	{
		display.setControlText(bottomLabel[3], " ");
	}

	//display.refreshControl(bottomLabel[3]);
	display.refreshControl(param2PopupListControl);

}

void cPatternEditor::refreshFillStep()
{
	//------------------------------
	// step
	fillText3[0]  = 0;
	sprintf(fillText3, "%d", fillStep);
	display.setControlText(val3PopupLabel, fillText3);
	display.setControlText(bottomLabel[5], "Step");

	display.refreshControl(val3PopupLabel);
}

void cPatternEditor::hideFillPopup()
{
	display.setControlHide(param1PopupListControl);
	display.setControlHide(val1PopupLabel);
	display.setControlHide(val2PopupLabel);
	display.setControlHide(val3PopupLabel);
	display.setControlHide(param2PopupListControl);

	display.refreshControl(param1PopupListControl);
	display.refreshControl(val1PopupLabel);
	display.refreshControl(val2PopupLabel);
	display.refreshControl(val3PopupLabel);
	display.refreshControl(param2PopupListControl);

	display.setControlHide(patternPopupLabel);
	display.refreshControl(patternPopupLabel);
}
//


//##############################################################################################
//###############################            RANDOM            #################################
//##############################################################################################

void cPatternEditor::showRandomisePopup()
{
	//------------------------------
	// from
	if(editParam == 0)
	{
		display.setControlText(val1PopupLabel, (char*)&mtNotes[randomiseData[editParam].from][0]);

	}
	else if (editParam == 1 || editParam == 2 || editParam == 3)
	{
		uint8_t offs = (editParam == 1 ? 1 : 0);
		fillText1[0]  = 0;

		sprintf(fillText1, "%d", (randomiseData[editParam].from + offs));

		display.setControlText(val1PopupLabel, fillText1);
	}
	display.setControlShow(val1PopupLabel);

	display.setControlText(bottomLabel[1], "From");

	//------------------------------
	// to
	if(editParam == 0)
	{
		display.setControlText(val2PopupLabel, (char*)&mtNotes[randomiseData[editParam].to][0]);
	}
	else if (editParam == 1 || editParam == 2 || editParam == 3)
	{
		uint8_t offs = (editParam == 1 ? 1 : 0);
		fillText2[0]  = 0;

		sprintf(fillText2, "%d", (randomiseData[editParam].to + offs));

		display.setControlText(val2PopupLabel, fillText2);
	}

	display.setControlText(bottomLabel[2], "To");
	display.setControlShow(val2PopupLabel);

	//------------------------------
	// skala
	if(editParam == 0)
	{
		for(uint8_t i = 0; i<fillScaleFilterCount ; i++)
		{
			fillScaleFilterNames[i] = (char*)&fillScaleFilterLabels[i][0];
		}

		fillScaleFilterList.linesCount = fillScaleFilterCount;
		fillScaleFilterList.start = randomiseData[editParam].param;
		fillScaleFilterList.length = fillScaleFilterCount;
		fillScaleFilterList.data = fillScaleFilterNames;

		display.setControlData(param2PopupListControl, &fillScaleFilterList);
		display.setControlValue(param2PopupListControl, randomiseData[editParam].param);
		display.setControlShow(param2PopupListControl);
		display.setControlSize(param2PopupListControl, 800/8-10, 25);

		display.setControlText(bottomLabel[3], "In Scale");

		frameData.places[3] = &framesPopupPlaces[3][0];
	}
	//------------------------------
	// fx
	else if(editParam == 3)
	{
		for(uint8_t i = 0; i<fillFxTypeCount ; i++)
		{
			fillFxTypeNames[i] = (char*)&fillFxTypeLabels[i][0];
		}

		fillFxTypeList.linesCount = 7;
		fillFxTypeList.start = randomiseData[editParam].param;
		fillFxTypeList.length = fillFxTypeCount;
		fillFxTypeList.data = fillFxTypeNames;

		display.setControlData(param2PopupListControl, &fillFxTypeList);
		display.setControlValue(param2PopupListControl, randomiseData[editParam].param);
		display.setControlShow(param2PopupListControl);
		display.setControlSize(param2PopupListControl, 800/4-10, 25);

		display.setControlText(bottomLabel[3], "Fx Type");

		frameData.places[3] = &framesPopupPlaces[4][0];
	}
	else
	{
		display.setControlHide(param2PopupListControl);
		display.setControlText(bottomLabel[3], "");
	}

	//------------------------------
	// step

	fillText3[0]  = 0;
	sprintf(fillText3, "%d", randomiseStep);

	display.setControlText(val3PopupLabel, fillText3);
	display.setControlShow(val3PopupLabel);

	display.setControlText(bottomLabel[5], "Step");
	display.setControlText(bottomLabel[4], "");



	switch(editParam)
	{
	case 0:
		display.setControlText(patternPopupLabel, "Randomise Notes");
		break;
	case 1:
		display.setControlText(patternPopupLabel, "Randomise Instruments");
		break;
	case 2:
		display.setControlText(patternPopupLabel, "Randomise Volumes");
		break;
	case 3:
		display.setControlText(patternPopupLabel, "Randomise Fx");
		break;

	default:
		display.setControlText(patternPopupLabel, "Randomise");
		break;
	}


	display.setControlText(bottomLabel[0], "");
	display.setControlText(bottomLabel[6], "Cancel");
	display.setControlText(bottomLabel[7], "Randomise");

	display.refreshControl(param1PopupListControl);
	display.refreshControl(val1PopupLabel);
	display.refreshControl(val2PopupLabel);
	display.refreshControl(val3PopupLabel);
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
	// 3 zmienna
	frameData.places[4] = &framesPopupPlaces[5][0];
	frameData.places[5] = &framesPopupPlaces[6][0];
	frameData.places[6] = &framesPopupPlaces[7][0];
	frameData.places[7] = &framesPopupPlaces[8][0];

	activateRandomisePopupBorder();

	display.synchronizeRefresh();
}

void cPatternEditor::refreshRandomiseFrom()
{
	if(editParam == 0)
	{
		display.setControlText(val1PopupLabel, (char*)&mtNotes[randomiseData[editParam].from][0]);
	}
	else if (editParam == 1 || editParam == 2 || editParam == 3)
	{
		uint8_t offs = (editParam == 1 ? 1 : 0);

		fillText1[0]  = 0;
		sprintf(fillText1, "%d", (randomiseData[editParam].from + offs));
		display.setControlText(val1PopupLabel, fillText1);
	}

	display.refreshControl(val1PopupLabel);
	//display.refreshControl(bottomLabel[1]);
}

void cPatternEditor::refreshRandomiseTo()
{
	if(editParam == 0)
	{
		display.setControlText(val2PopupLabel, (char*)&mtNotes[randomiseData[editParam].to][0]);
	}
	else if (editParam == 1 || editParam == 2 || editParam == 3)
	{
		uint8_t offs = (editParam == 1 ? 1 : 0);

		fillText2[0]  = 0;
		sprintf(fillText2, "%d", (randomiseData[editParam].to + offs));
		display.setControlText(val2PopupLabel, fillText2);
	}

	display.refreshControl(val2PopupLabel);
	//display.refreshControl(bottomLabel[2]);
}

void cPatternEditor::refreshRandomiseParam()
{
	display.setControlValue(param2PopupListControl, randomiseData[editParam].param);

	if(editParam == 0)
	{
		display.setControlText(bottomLabel[3], "In Scale");
	}
	else if(editParam == 3)
	{
		display.setControlText(bottomLabel[3], "Fx Type");
	}
	else
	{
		display.setControlText(bottomLabel[3], " ");
	}

	//display.refreshControl(bottomLabel[3]);
	display.refreshControl(param2PopupListControl);
}

void cPatternEditor::refreshRandomiseStep()
{
	//------------------------------
	// step
	fillText3[0]  = 0;
	sprintf(fillText3, "%d", randomiseStep);
	display.setControlText(val3PopupLabel, fillText3);
	display.setControlText(bottomLabel[5], "Step");

	display.refreshControl(val3PopupLabel);
}

void cPatternEditor::hideRandomisePopup()
{
//	display.setControlHide(param1PopupListControl);
	display.setControlHide(val1PopupLabel);
	display.setControlHide(val2PopupLabel);
	display.setControlHide(val3PopupLabel);
	display.setControlHide(param2PopupListControl);

//	display.refreshControl(param1PopupListControl);
	display.refreshControl(val1PopupLabel);
	display.refreshControl(val2PopupLabel);
	display.refreshControl(val3PopupLabel);
	display.refreshControl(param2PopupListControl);

	display.setControlHide(patternPopupLabel);
	display.refreshControl(patternPopupLabel);
}


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


void cPatternEditor::activateRandomisePopupBorder()
{
	if(randomisePlace < 0 || randomisePlace > frameData.placesCount-1)
	{
		display.setControlHide(frameControl);
		display.refreshControl(frameControl);
		return;
	}

	display.setControlValue(frameControl, randomisePlace);
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
