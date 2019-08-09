
#include <patternEditor.h>
#include "mtStructs.h"


static uint16_t framesPlaces[8][4] =
{
	{0, 		412, 800/8, 68},
	{(800/8)*1, 412, 800/8, 68},
	{(800/8)*2, 412, 800/8, 68},
	{(800/8)*3, 412, 800/8, 68},
	{(800/8)*4, 412, 800/8, 68},
	{(800/8)*5, 412, 800/8, 68},
	{(800/8)*6, 412, 800/8, 68},
	{(800/8)*7, 412, 800/8, 68},
};

static uint16_t framesPopupPlaces[8][4] =
{
	{0, 		260, 800/8, 408-228},
	{(800/8)*1, 260, 800/8, 408-228},
	{(800/8)*2, 260, 800/8, 408-228},
	{(800/8)*3, 260, 800/8, 408-228},
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

		prop2.style = 	( controlStyleShow | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 450;
		prop2.w = 800/8-10;
		prop2.h = 30;
		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.y = 415;
		prop2.h = 26;
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
	display.setControlText(bottomLabel[4], "Fill");
	display.setControlText(bottomLabel[5], "Random");
	display.setControlText(bottomLabel[6], "Invert");
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

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
	}

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
	display.setControlText(topLabel[1], "1");
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


void cPatternEditor::showFillPopup()
{
	//------------------------------
	// typ
	for(uint8_t i = 0; i<fillTypeListCount ; i++)
	{
		fillTypeListNames[i] = (char*)&fillTypeListLabels[i][0];
	}

	fillTypeList.linesCount = fillTypeListCount;
	fillTypeList.start = lastFillValues[editParam][0];
	fillTypeList.length = fillTypeListCount;
	fillTypeList.data = fillTypeListNames;

	display.setControlData(param1PopupListControl, &fillTypeList);
	display.setControlValue(param1PopupListControl, lastFillValues[editParam][0]);
	display.setControlShow(param1PopupListControl);

	display.setControlText(bottomLabel[0], "Type");

	//------------------------------
	// value/from
	if(editParam == 0)
	{
		display.setControlText(val1PopupLabel, (char*)&mtNotes[lastFillValues[editParam][1]][0]);

	}
	else if (editParam == 1 || editParam == 2 || editParam == 3)
	{
		uint8_t offs = (editParam == 1 ? 1 : 0);
		fillText1[0]  = 0;

		sprintf(fillText1, "%d", (lastFillValues[editParam][1] + offs));

		display.setControlText(val1PopupLabel, fillText1);
	}
	display.setControlShow(val1PopupLabel);

	if(lastFillValues[editParam][0] == 0)
		display.setControlText(bottomLabel[1], "Value");
	else
		display.setControlText(bottomLabel[1], "From");

	//------------------------------
	// To
	if(editParam == 0)
	{
		display.setControlText(val2PopupLabel, (char*)&mtNotes[lastFillValues[editParam][2]][0]);
	}
	else if (editParam == 1 || editParam == 2 || editParam == 3)
	{
		uint8_t offs = (editParam == 1 ? 1 : 0);
		fillText2[0]  = 0;

		sprintf(fillText2, "%d", (lastFillValues[editParam][2] + offs));

		display.setControlText(val2PopupLabel, fillText2);
	}
	display.setControlShow(val2PopupLabel);

	if(lastFillValues[editParam][0] == 0)
	{
		display.setControlText(bottomLabel[2], "");
		display.setControlText(val2PopupLabel, "");
	}
	else
	{
		display.setControlText(bottomLabel[2], "To");
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
		fillScaleFilterList.start = lastFillValues[editParam][3];
		fillScaleFilterList.length = fillScaleFilterCount;
		fillScaleFilterList.data = fillScaleFilterNames;

		display.setControlData(param2PopupListControl, &fillScaleFilterList);
		display.setControlValue(param2PopupListControl, lastFillValues[editParam][3]);
		display.setControlShow(param2PopupListControl);

		display.setControlText(bottomLabel[3], "In Scale");
	}
	//------------------------------
	// fx
	else if(trackerPattern.selectedParam == 3)
	{
		for(uint8_t i = 0; i<fillFxTypeCount ; i++)
		{
			fillFxTypeNames[i] = (char*)&fillFxTypeLabels[i][0];
		}

		fillFxTypeList.linesCount = 7;
		fillFxTypeList.start = lastFillValues[editParam][3];
		fillFxTypeList.length = fillFxTypeCount;
		fillFxTypeList.data = fillFxTypeNames;

		display.setControlData(param2PopupListControl, &fillFxTypeList);
		display.setControlValue(param2PopupListControl, lastFillValues[editParam][3]);
		display.setControlShow(param2PopupListControl);

		display.setControlText(bottomLabel[3], "Fx Type");
	}
	else
	{
		display.setControlHide(param2PopupListControl);
	}

	//------------------------------
	// step

	fillText3[0]  = 0;
	sprintf(fillText3, "%d", lastFillStep);

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
		display.refreshControl(topLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}

	frameData.places[0] = &framesPopupPlaces[0][0];
	frameData.places[1] = &framesPopupPlaces[1][0];
	frameData.places[2] = &framesPopupPlaces[2][0];
	frameData.places[3] = &framesPopupPlaces[3][0];
	frameData.places[4] = &framesPopupPlaces[4][0];
	frameData.places[5] = &framesPopupPlaces[5][0];
	frameData.places[6] = &framesPopupPlaces[6][0];
	frameData.places[7] = &framesPopupPlaces[7][0];

	activatePopupLabelsBorder();

	display.synchronizeRefresh();
}


void cPatternEditor::refreshFillPopup()
{
	display.setControlValue(param1PopupListControl, lastFillValues[editParam][0]);

	if(editParam == 0)
	{
		display.setControlText(val1PopupLabel, (char*)&mtNotes[lastFillValues[editParam][1]][0]);
		display.setControlText(val2PopupLabel, (char*)&mtNotes[lastFillValues[editParam][2]][0]);
	}
	if (editParam == 1 || editParam == 2 || editParam == 3)
	{
		uint8_t offs = (editParam == 1 ? 1 : 0);

		fillText1[0]  = 0;
		sprintf(fillText1, "%d", (lastFillValues[editParam][1] + offs));
		display.setControlText(val1PopupLabel, fillText1);

		fillText2[0]  = 0;
		sprintf(fillText2, "%d", (lastFillValues[editParam][2] + offs));
		display.setControlText(val2PopupLabel, fillText2);
	}

	display.setControlValue(param2PopupListControl, lastFillValues[editParam][3]);

	if(editParam == 1)
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

	//------------------------------
	// step
	fillText3[0]  = 0;
	sprintf(fillText3, "%d", lastFillStep);
	display.setControlText(val3PopupLabel, fillText3);
	display.setControlText(bottomLabel[5], "Step");



	if(lastFillValues[editParam][0] == 0)
	{
		display.setControlText(bottomLabel[1], "Value");
		display.setControlText(bottomLabel[2], "");

		display.setControlText(val2PopupLabel, "");
	}
	else
	{

		display.setControlText(bottomLabel[1], "From");
		display.setControlText(bottomLabel[2], "To");
	}


	display.refreshControl(param1PopupListControl);
	display.refreshControl(val1PopupLabel);
	display.refreshControl(val2PopupLabel);
	display.refreshControl(val3PopupLabel);
	display.refreshControl(param2PopupListControl);


	for(uint8_t i = 0; i < 8; i++)
	{
		display.refreshControl(bottomLabel[i]);
	}

	//display.synchronizeRefresh();

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


void cPatternEditor::showRandomPopup()
{


}

void cPatternEditor::activatePopupLabelsBorder()
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
