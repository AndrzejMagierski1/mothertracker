
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





	prop.style = 	(controlStyleCenterX | controlStyleCenterY );
	prop.x = 800/2;
	prop.y = 480/2;
	prop.w = 100; // jedna kolumna
	prop.h = 250;
	prop.data = &fillPopup;
	if(patternPopupControl == nullptr)  patternPopupControl = display.createControl<cPatternPopup>(&prop);



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


	display.destroyControl(patternPopupControl);
	patternPopupControl = nullptr;

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

	//if(fillState > 0) showFillPopup();


	for(uint8_t i = 0; i<8; i++)
	{
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


void cPatternEditor::showFillPopup()
{
	fillPopup.column[0].type = 0;
	fillPopup.column[1].type = 0;
	fillPopup.column[2].type = 0;
	fillPopup.column[3].type = 0;
	fillPopup.column[4].type = 0;


	uint8_t col = 0;
	//------------------------------
	// kolumna 1
	fillPopup.column[col].value = lastFillValues[trackerPattern.selectedParam][col];

	for(uint8_t i = 0; i<fillTypeListCount ; i++)
	{
		fillTypeListNames[i] = (char*)&fillTypeListLabels[i][0];
	}

	fillTypeList.linesCount = fillTypeListCount;
	fillTypeList.start = fillPopup.column[col].value;
	fillTypeList.length = fillTypeListCount;
	fillTypeList.data = fillTypeListNames;

	fillPopup.column[col].type = 1;
	fillPopup.column[col].data = &fillTypeList;
	fillPopup.column[col].title = (char*)"Type";


	//------------------------------
	col = 1;

	fillPopup.column[col].value = lastFillValues[trackerPattern.selectedParam][col];
	fillPopup.column[col].type = 2;

	if(trackerPattern.selectedParam == 0)
	{
		fillPopup.column[col].text = (char*)&mtNotes[lastFillValues[trackerPattern.selectedParam][col]][0];
	}
	else if (trackerPattern.selectedParam == 1 || trackerPattern.selectedParam == 2 || trackerPattern.selectedParam == 3)
	{
		char inst0 = (lastFillValues[trackerPattern.selectedParam][col] + 1) / 10;
		char inst1 = (lastFillValues[trackerPattern.selectedParam][col] + 1) % 10;

		fillText1[0] = inst0 + 48;
		fillText1[1] = inst1 + 48;
		fillText1[2] = 0;

		fillPopup.column[col].text = fillText1;
	}

	if(fillPopup.column[0].value == 0)
		fillPopup.column[col].title = (char*)"Value";
	else
		fillPopup.column[col].title = (char*)"From";

	//------------------------------


	col = 2;

	fillPopup.column[col].value = lastFillValues[trackerPattern.selectedParam][col];
	fillPopup.column[col].type = 2;

	if(trackerPattern.selectedParam == 0)
	{
		fillPopup.column[col].text = (char*)&mtNotes[lastFillValues[trackerPattern.selectedParam][col]][0];
	}
	else if (trackerPattern.selectedParam == 1 || trackerPattern.selectedParam == 2 || trackerPattern.selectedParam == 3)
	{
		char inst0 = (lastFillValues[trackerPattern.selectedParam][col] + 1) / 10;
		char inst1 = (lastFillValues[trackerPattern.selectedParam][col] + 1) % 10;

		fillText2[0] = inst0 + 48;
		fillText2[1] = inst1 + 48;
		fillText2[2] = 0;

		fillPopup.column[col].text = fillText2;
	}

	if(lastFillValues[trackerPattern.selectedParam][0] != 0)
	{
		//fillPopup.column[col].text = fillText2;
		fillPopup.column[col].title = (char*)"To";
	}
	else
	{
		fillPopup.column[col].text = (char*)"";
		fillPopup.column[col].title = (char*)"";
	}





	//------------------------------
	if(trackerPattern.selectedParam == 0)
	{
		col=3;

		fillPopup.column[col].value = lastFillValues[trackerPattern.selectedParam][col];

		for(uint8_t i = 0; i<fillTypeListCount ; i++)
		{
			fillScaleFilterNames[i] = (char*)&fillScaleFilterLabels[i][0];
		}

		fillScaleFilterList.linesCount = fillScaleFilterCount;
		fillScaleFilterList.start = fillPopup.column[col].value;
		fillScaleFilterList.length = fillScaleFilterCount;
		fillScaleFilterList.data = fillScaleFilterNames;

		fillPopup.column[col].type = 1;
		fillPopup.column[col].data = &fillScaleFilterList;
		fillPopup.column[col].title = "In Scale";
		fillPopup.column[col].options = 0;
	}
	//------------------------------
	else if(trackerPattern.selectedParam == 3)
	{
		col=3;

		fillPopup.column[col].value = lastFillValues[trackerPattern.selectedParam][col];

		for(uint8_t i = 0; i<fillFxTypeCount ; i++)
		{
			fillFxTypeNames[i] = (char*)&fillFxTypeLabels[i][0];
		}

		fillFxTypeList.linesCount = 7;
		fillFxTypeList.start = fillPopup.column[col].value;
		fillFxTypeList.length = fillFxTypeCount;
		fillFxTypeList.data = fillFxTypeNames;

		fillPopup.column[col].type = 1;
		fillPopup.column[col].title = (char*)"Fx Type";
		fillPopup.column[col].data = &fillFxTypeList;
		fillPopup.column[col].options = columnOptionDoubleWide;
	}



	switch(trackerPattern.selectedParam)
	{
	case 0:
		display.setControlText(patternPopupControl, "Fill Notes");
		fillPopup.columnsCount = 4;
		break;
	case 1:
		display.setControlText(patternPopupControl, "Fill Instruments");
		fillPopup.columnsCount = 3;
		break;
	case 2:
		display.setControlText(patternPopupControl, "Fill Volumes");
		fillPopup.columnsCount = 3;
		break;
	case 3:
		display.setControlText(patternPopupControl, "Fill Fx");
		fillPopup.columnsCount = 4;
		break;

	default:
		hideFillPopup();
		selectedPlace = -1;
		activateLabelsBorder();
		return;
	}

	if(fillPlace > fillPopup.columnsCount-1) fillPlace = fillPopup.columnsCount-1;

	display.setAddControlStyle(patternPopupControl, controlStyleCreateSubcontrols);

	display.setControlValue(patternPopupControl, fillPlace);

	display.setControlData(patternPopupControl,  &fillPopup);
	display.setControlShow(patternPopupControl);
	display.refreshControl(patternPopupControl);

}


void cPatternEditor::refreshFillPopup()
{

	fillPopup.column[0].value = lastFillValues[trackerPattern.selectedParam][0];

	if(trackerPattern.selectedParam == 0)
	{
		fillPopup.column[1].text = (char*)&mtNotes[lastFillValues[trackerPattern.selectedParam][1]][0];
		fillPopup.column[2].text = (char*)&mtNotes[lastFillValues[trackerPattern.selectedParam][2]][0];
	}
	if (trackerPattern.selectedParam == 1 || trackerPattern.selectedParam == 2 || trackerPattern.selectedParam == 3)
	{
		char inst0 = (lastFillValues[trackerPattern.selectedParam][1] + 1) / 10;
		char inst1 = (lastFillValues[trackerPattern.selectedParam][1] + 1) % 10;

		fillText1[0] = inst0 + 48;
		fillText1[1] = inst1 + 48;
		fillText1[2] = 0;

		fillPopup.column[1].text = fillText1;

		inst0 = (lastFillValues[trackerPattern.selectedParam][2] + 1) / 10;
		inst1 = (lastFillValues[trackerPattern.selectedParam][2] + 1) % 10;

		fillText2[0] = inst0 + 48;
		fillText2[1] = inst1 + 48;
		fillText2[2] = 0;

		fillPopup.column[2].text = fillText2;
	}

	fillPopup.column[3].value = lastFillValues[trackerPattern.selectedParam][3];


	if(lastFillValues[trackerPattern.selectedParam][0] == 0)
	{
		fillPopup.column[1].title = (char*)"Value";

		fillPopup.column[2].title = (char*)"";
		fillPopup.column[2].text = (char*)"";
	}
	else
	{

		fillPopup.column[1].title = (char*)"From";
		fillPopup.column[2].title = (char*)"To";
	}



//	lastFillValues[trackerPattern.selectedParam][fillPlace] = fillPopup.column[fillPlace].value;


	display.setControlData(patternPopupControl,  &fillPopup);
	display.refreshControl(patternPopupControl);
}

void cPatternEditor::hideFillPopup()
{
	display.setControlHide(patternPopupControl);
	display.refreshControl(patternPopupControl);
}


void cPatternEditor::showRandomPopup()
{

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
