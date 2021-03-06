
#include "core/interface.h"
#include "core/interfacePopups.h"


#include "mtPadBoard.h"
#include "mtPadsBacklight.h"
#include "core/songTimer.h"

#include "mtSequencer.h"

#include "patternEditor/patternEditor.h"

static  uint8_t functLeftPopup();
static  uint8_t functRightPopup();
static  uint8_t functUpPopup();
static  uint8_t functDownPopup();

static  uint8_t functEncoderPopup(int16_t value);

static  uint8_t functPadsPopup(uint8_t pad, uint8_t state, int16_t velo);


static uint8_t functDescriptionButton();

extern  uint8_t functInvert();
extern  cPatternEditor patternEditor;


//static uint32_t instrListColors[] =
//{
//	0xFF0000,	//	 listItemFrame
//	0x000000,	//	 listItemFrameBG
//	0xFFFFFF,	//	 listScrollBar
//	0x554A19,	//	 listBG
//	0xFFFFFF,	//	 fontList
//};
//
static uint32_t progressPopupColors[4] =
{
	0xffffff, // tekst
	0x222222, // tło
	one_true_red, // ramka
	0xFFFFFF, // tekst2
};

static uint32_t listBgLabelColors[] =
{
	0xFFFFFF, // tekst
	0x0a0a0a, // tło
	0xFF0000, // ramka
};

static uint32_t titleBgLabelColors[] =
{
	0x000000, // tekst
	0xFFFFFF, // tło
	0xFF0000, // ramka
};


cInterfacePopups mtPopups;
static  cInterfacePopups* POP = &mtPopups;

//=====================================================================================================
//
//=====================================================================================================

void cInterfacePopups::initPopupsDisplayControls()
{

	strControlProperties prop;
	popupList.start = 0;
	popupList.linesCount = 16;
	popupList.length = 16;
	popupList.data = interfaceGlobals.ptrIntrumentsNames;
	popupList.lineLengthMax = 22;
	//prop.style = controlStyleBackground;
	prop.x = (800/4)*(3)+5;
	prop.y = 40;
	prop.w = 800/4;
	prop.h = 430;
	//prop.colors = instrListColors;
	prop.data = &popupList;
	if(popupListControl == nullptr)  popupListControl = display.createControl<cList>(&prop);


	padNamesStruct.length = 5;
	padNamesStruct.name = interfaceGlobals.padNamesPointer;
	prop.style = 0;
	prop.x = 13;
	prop.y = 130;
	prop.w = 780;
	prop.h = 260;
	prop.colors = nullptr;
	prop.value = -1;
	prop.data = &padNamesStruct;
	if(keyboardControl == nullptr)  keyboardControl = display.createControl<cNotePopout>(&prop);


	strControlProperties prop1;
	prop1.x = 190;
	prop1.y = 170;
	prop1.style = controlStyleValue_0_100 | controlStyleCenterX;
	prop1.h = 100;
	prop1.w = 420;
	if(progressBar == nullptr)  progressBar = display.createControl<cHorizontalBar>(&prop1);


	strControlProperties prop3;
	prop3.style = 	(controlStyleBackground | controlStyleBottomShadow);
	prop3.x = 800/2-150;
	prop3.y = 480/2-50;
	prop3.w = 300;
	prop3.h = 70;
	prop3.data = nullptr;
	if(textPopup == nullptr) textPopup = display.createControl<cTextPopup>(&prop3);


	//textBoxData
	prop3.style = 0;
	prop3.x = 0;
	prop3.y = 0;
	prop3.w = 50;
	prop3.h = 50;
	prop3.data = nullptr; //&textBoxData;
	if(textBox == nullptr) textBox = display.createControl<cTextBox>(&prop3);


	strControlProperties prop2;
	prop2.style = 	(controlStyleCenterY | controlStyleBackground);
	prop2.x = 30;
	prop2.y = 0;
	if(textLabel1 == nullptr) textLabel1 = display.createControl<cLabel>(&prop2);
	prop2.x = 769;
	prop2.value = 0;
	prop2.colors = interfaceGlobals.activeLabelsColors;
	prop2.style = 	( controlStyleCenterX | controlStyleFont3 );
	//prop2.data = &labelDoubleArrow;
	if(textLabel2 == nullptr) textLabel2 = display.createControl<cLabel>(&prop2);


	prop2.style = 	(controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 795;
	prop2.h = 25;
	prop2.data = nullptr;
	prop2.colors = listBgLabelColors;
	if(bgLabel == nullptr) bgLabel = display.createControl<cLabel>(&prop2);


}

void cInterfacePopups::destroyPopupsDisplayControls()
{
	/*

	for(uint8_t i = 0; i<4; i++)
	{
		display.destroyControl(listControl[i]);
		listControl[i] = nullptr;
	}

	display.destroyControl(keyboardControl);
	keyboardControl = nullptr;

	 */
}

void cInterfacePopups::update()
{

	if(globalPopupDisplayTime > 0)
	{
		if(popupTimer > 1000)
		{
			popupTimer = 0;

			if(globalPopupDisplayTime == 1)
			{
				hide();
			}
			else globalPopupDisplayTime--;


		}
	}


}


//=====================================================================================================
// show
//=====================================================================================================

uint8_t cInterfacePopups::getStepPopupState()
{
	return stepPopupState;
}


void cInterfacePopups::showNotesPopup()
{
	display.hideAllControls();

	songTimer.show();

	display.setControlPosition(bgLabel, 2, 0);
	display.setControlSize(bgLabel, 795, 26);
	display.setControlColors(bgLabel, titleBgLabelColors);
	display.setControlStyle(bgLabel, controlStyleShow | controlStyleBackground);
	display.refreshControl(bgLabel);

	display.setControlText(textLabel1, " Notes");
	display.setControlPosition(textLabel1, 2, 0);
	display.setControlColors(textLabel1, titleBgLabelColors);//xxx
	display.setControlSize(textLabel1, 800, 26);
	display.setControlValue(textLabel1, 0);
	display.setControlStyle(textLabel1, controlStyleShow | controlStyleCenterY | controlStyleBackground | controlStyleFont4);
	display.refreshControl(textLabel1);

	showActualInstrument();

	for(uint8_t i = 0; i < 48; i++)
	{
		interfaceGlobals.padNamesPointer[i] = (char*)mtNotes[mtPadBoard.getNoteFromPad(i)];
	}

	display.setControlValue(keyboardControl, selectedActualItem);
	display.setControlShow(keyboardControl);
	display.refreshControl(keyboardControl);

	display.synchronizeRefresh();
}

void cInterfacePopups::showInstrumentsPopup()
{
	//display.hideAllControls();


	display.setControlPosition(bgLabel, 600+2, 29);
	display.setControlSize(bgLabel,  200, 450); //419
	display.setControlColors(bgLabel, listBgLabelColors);

	display.setControlStyle(bgLabel, controlStyleShow | controlStyleBackground);
	display.refreshControl(bgLabel);

	display.setControlText(textLabel1, "  Instruments");
	display.setControlColors(textLabel1, titleBgLabelColors);//xxx
	display.setControlPosition(textLabel1, 600+1, 0);
	display.setControlSize(textLabel1, 200, 26);
	display.setControlValue(textLabel1, 0);
	display.setControlStyle(textLabel1, controlStyleShow | controlStyleCenterY | controlStyleBackground | controlStyleFont4);
	display.refreshControl(textLabel1);


	//showActualInstrument();
	listInstruments();

	ptrActualItemsList = (char**)(interfaceGlobals.ptrIntrumentsNames);

	instrList.start = selectedActualItem;
	instrList.linesCount = 16;
	instrList.length = 64;
	instrList.data = interfaceGlobals.ptrIntrumentsNames;
	instrList.lineLengthMax = 22;

	display.setControlPosition(popupListControl, 600, 30);
	display.setControlData(popupListControl, &instrList);
	display.setControlShow(popupListControl);
	display.refreshControl(popupListControl);

	//refreshAllList();

	display.synchronizeRefresh();
}


void cInterfacePopups::showFxesPopup()
{
	//display.hideAllControls();
	labelDoubleArrow.bitmaps[0].xValue = 600 + 15;
	labelDoubleArrow.bitmaps[0].yValue = 451;
	labelDoubleArrow.bitmaps[1].bitmapIndex = 0;

	refreshStepPopupDescription();
	display.setControlColors(bgLabel, listBgLabelColors);
	display.setControlStyle(bgLabel, controlStyleShow | controlStyleBackground);
	display.refreshControl(bgLabel);

	display.setControlText(textLabel1, "  Fx");
	display.setControlValue(textLabel1, 0);
	display.setControlColors(textLabel1, titleBgLabelColors);//xxx
//	display.setControlPosition(textLabel1, 600+1, 0);
//	display.setControlSize(textLabel1, 200, 26);
	display.setControlStyle(textLabel1, controlStyleShow | controlStyleCenterY | controlStyleBackground | controlStyleFont4);
	display.refreshControl(textLabel1);





	//showActualInstrument();
	ptrActualItemsList = (char**)(interfaceGlobals.ptrFxNames);

	instrList.start = selectedActualItem;
	instrList.linesCount = 15;
	instrList.length = FX_COUNT-FX_COUNT_HIDDEN_FXes;
	instrList.data = (char**)interfaceGlobals.ptrFxNames;

	display.setControlPosition(popupListControl, 600, 30);
	display.setControlData(popupListControl, &instrList);
	display.setControlShow(popupListControl);
	display.refreshControl(popupListControl);

	//refreshAllList();

	display.synchronizeRefresh();
}



//=====================================================================================================
// show / hide
//=====================================================================================================
void cInterfacePopups::showStepPopup(uint8_t stepPopupType, int8_t initVal)
{
	if(stepPopupType == stepPopupNone) return;

	if(stepPopupState != stepPopupNone)
	{
		hideStepPopups();
	}

	stepPopupState = stepPopupType;
	selectedActualItem = initVal;

	setPopupFunct();

	switch(stepPopupState)
	{
	case stepPopupNone:		break;
	case stepPopupNote:		showNotesPopup();		break;
	case stepPopupInstr:	showInstrumentsPopup();	break;
	case stepPopupFx:		showFxesPopup();		break;
	default:	break;
	}

	lightUpPads();
}


void cInterfacePopups::hideStepPopups()
{
	if(stepPopupState)
	{
		stepPopupState = stepPopupNone;

		display.setControlHide(popupListControl);
		display.setControlHide(popupListControl);
		display.setControlHide(popupListControl);
		display.setControlHide(popupListControl);

		display.setControlHide(keyboardControl);

		display.setControlHide(bgLabel);
		display.setControlHide(textLabel1);
		display.setControlHide(textLabel2);

		display.setControlHide(textBox);

		mtInterface.FM->clearButton(interfaceButtonRec);
//		mtInterface.FM->clearButton(interfaceButtonShift);
//		mtInterface.FM->clearButton(interfaceButtonEnter);

		mtInterface.FM->clearButton(interfaceButtonLeft);
		mtInterface.FM->clearButton(interfaceButtonRight);
		mtInterface.FM->clearButton(interfaceButtonUp);
		mtInterface.FM->clearButton(interfaceButtonDown);

		mtInterface.FM->clearAllPots();
		mtInterface.FM->clearAllPads();
	}

	padsBacklight.clearAllPads(0, 1, 1);
}

void cInterfacePopups::setPopupFunct()
{
	switch(stepPopupState)
	{
	case stepPopupNote:
		mtInterface.FM->clearButton(interfaceButtonInstr);
		mtInterface.FM->clearButton(interfaceButtonFx1);
		mtInterface.FM->clearButton(interfaceButtonFx2);
		break;
	case stepPopupInstr:
		mtInterface.FM->clearButton(interfaceButtonNote);
		mtInterface.FM->clearButton(interfaceButtonFx1);
		mtInterface.FM->clearButton(interfaceButtonFx2);
		break;
	case stepPopupFx:
		mtInterface.FM->clearButton(interfaceButtonNote);
		mtInterface.FM->clearButton(interfaceButtonInstr);
		mtInterface.FM->setButtonObj(interfaceButton6, buttonPress, functDescriptionButton);
		mtInterface.FM->setButtonObj(interfaceButton7, buttonPress, functDescriptionButton);
		break;
	default:	break;
	}

	mtInterface.FM->clearButton(interfaceButtonRec);
//	mtInterface.FM->clearButton(interfaceButtonShift);
//	mtInterface.FM->clearButton(interfaceButtonEnter);

	mtInterface.FM->clearButtonsRange(interfaceButton0,interfaceButton5);

	mtInterface.FM->setPotObj(interfacePot0, functEncoderPopup, nullptr);




	mtInterface.FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeftPopup);
	//mtInterface.FM->setButtonObj(interfaceButton5, buttonPress, functInvert);
	mtInterface.FM->setButtonObj(interfaceButtonRight, buttonPress, functRightPopup);
	mtInterface.FM->setButtonObj(interfaceButtonUp, buttonPress, functUpPopup);
	mtInterface.FM->setButtonObj(interfaceButtonDown, buttonPress, functDownPopup);

	mtInterface.FM->setPadsGlobal(functPadsPopup);
}
//=====================================================================================================
//
//=====================================================================================================

void cInterfacePopups::setStepPopupValue(int16_t value)
{
	if(stepPopupState == stepPopupNone) return;

	selectedActualItem = value;

	switch(stepPopupState)
	{
	case stepPopupNote:
	{
		selectPadOnPopup(selectedActualItem);
		mtProject.values.lastUsedNote = mtPadBoard.convertPadToNote(selectedActualItem);
		break;
	}
	case stepPopupInstr:
	{
		mtProject.values.lastUsedInstrument = selectedActualItem;
		display.setControlValue(popupListControl, selectedActualItem);
		display.refreshControl(popupListControl);
		break;
	}
	case stepPopupFx:
	{
		//refreshAllList();

		selectedActualItem = constrain(selectedActualItem, 0, FX_COUNT-1-FX_COUNT_HIDDEN_FXes);
		mtProject.values.lastUsedFx = selectedActualItem;
		display.setControlValue(popupListControl, selectedActualItem);
		display.refreshControl(popupListControl);

		if(mtConfig.interface.fxPopupDescription)
		{
			mtPopups.refreshFxDescription();
		}

		break;
	}
	default:	break;
	}

	lightUpPads();
}


void cInterfacePopups::changeStepPopupValue(int16_t value, uint8_t dir)
{
	if(stepPopupState == stepPopupNone) return;

	switch(stepPopupState)
	{
	case stepPopupNote:
	{
		if(dir > 0)
		{
			value = (dir == 1 ? -12 : (dir == 2 ? 1 : (dir == 3 ? 12 : (-1))));
		}

		if(selectedActualItem + value < 0) selectedActualItem = 0;
		else if(selectedActualItem + value > 47) selectedActualItem = 47;
		else selectedActualItem += value;

		mtProject.values.lastUsedNote = mtPadBoard.convertPadToNote(selectedActualItem);
		selectPadOnPopup(selectedActualItem);
		break;
	}
	case stepPopupInstr:
	{
		if(dir > 0)
		{
			value = (dir == 1 ? -1 : (dir == 2 ? 12 : (dir == 3 ? 1 : (-12))));
		}

		if(selectedActualItem + value < 0) selectedActualItem = 0;
		else if(selectedActualItem + value > INSTRUMENTS_MAX+16) selectedActualItem = INSTRUMENTS_MAX+16;
		else selectedActualItem += value;

		mtProject.values.lastUsedInstrument = selectedActualItem;
		//showActualInstrument();

		display.setControlValue(popupListControl, selectedActualItem);
		display.refreshControl(popupListControl);

		break;
	}
	case stepPopupFx:
	{
		if(dir > 0)
		{
			value = (dir == 1 ? -1 : (dir == 2 ? 12 : (dir == 3 ? 1 : (-12))));
		}

		padsBacklight.setBackLayer(0, 0, selectedActualItem);


		selectedActualItem = constrain(selectedActualItem + value,0, FX_COUNT-1-FX_COUNT_HIDDEN_FXes);
//		if(selectedActualItem + value < 0) selectedActualItem = 0;
//		else if(selectedActualItem + value > FX_COUNT) selectedActualItem = FX_COUNT;
//		else selectedActualItem += value;

		mtProject.values.lastUsedFx = selectedActualItem;

		display.setControlValue(popupListControl, selectedActualItem);
		display.refreshControl(popupListControl);

		if(mtConfig.interface.fxPopupDescription)
		{
			mtPopups.refreshFxDescription();
		}

		padsBacklight.setBackLayer(1, 20, selectedActualItem);

		break;
	}

	default:	break;
	}

	lightUpPads();
}

void cInterfacePopups::refreshFxDescription()
{

	display.setControlValue(textBox, 10);
	display.setControlText(textBox, interfaceGlobals.fxDescPtr(selectedActualItem));
	//display.setControlShow(textBox);
	display.refreshControl(textBox);
}

int16_t cInterfacePopups::getStepPopupValue()
{
	return selectedActualItem;
}


void cInterfacePopups::lightUpPads()
{
	if(stepPopupState == stepPopupNone) return;

	switch(stepPopupState)
	{
	case stepPopupNote:
	{
		padsBacklight.clearAllPads(0, 1, 1);
		break;
	}
	case stepPopupInstr:
	{
		padsBacklight.clearAllPads(0, 1, 1);
		int8_t show_instr = mtProject.values.lastUsedInstrument ;

		if(show_instr >= 0 && show_instr <= 47)
		{
			padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, show_instr);
		}
		break;
	}
	case stepPopupFx:
	{
		padsBacklight.clearAllPads(0, 1, 1);
		if(selectedActualItem < 0 || selectedActualItem > FX_MAX) break;
		padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, selectedActualItem);
		break;
	}

	default: break;

	}
}


//=====================================================================================================
//
//=====================================================================================================


void cInterfacePopups::showActualInstrument()
{
	static char actualInstrName[SAMPLE_NAME_SIZE+4];

	uint8_t i = mtProject.values.lastUsedInstrument;

	sprintf(actualInstrName, "%d.", i+1);

	strncat(actualInstrName, mtProject.instrument[i].sample.file_name, 25);

	display.setControlText(textLabel2,  actualInstrName);
	display.setControlStyle(textLabel2, controlStyleShow  | controlStyleCenterY);
	display.setControlPosition(textLabel2, 585, 0);
	display.refreshControl(textLabel2);
}
/*
void cInterfacePopups::showActualInstrument()
{
	static char actualInstrName[SAMPLE_NAME_SIZE+4];

	uint8_t i = mtProject.values.lastUsedInstrument;

	sprintf(actualInstrName, "%d. ", i+1);

	strncat(&actualInstrName[0], mtProject.instrument[i].sample.file_name, SAMPLE_NAME_SIZE);

	display.setControlText(textLabel2,  actualInstrName);
	display.setControlStyle(textLabel2, controlStyleShow | controlStyleRightX | controlStyleCenterY);
	display.setControlPosition(textLabel2, 769, 12);
	display.refreshControl(textLabel2);
}
*/

void cInterfacePopups::listInstruments()
{
	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		sprintf(&interfaceGlobals.intrumentsNames[i][0], "%d. ", i+1);

		if(mtProject.instrument[i].isActive)
		{
			strncat(&interfaceGlobals.intrumentsNames[i][0], mtProject.instrument[i].sample.file_name, SAMPLE_NAME_SIZE);
		}

		interfaceGlobals.ptrIntrumentsNames[i] = &interfaceGlobals.intrumentsNames[i][0];
	}
	for(uint8_t i = 0; i < 16; i++)
	{
		sprintf(&interfaceGlobals.intrumentsNames[INSTRUMENTS_COUNT+i][0], "%d. MIDI Channel %d", INSTRUMENTS_COUNT+i+3, i+1);
		interfaceGlobals.ptrIntrumentsNames[INSTRUMENTS_COUNT+i] = &interfaceGlobals.intrumentsNames[INSTRUMENTS_COUNT+i][0];
	}
}

//void cInterfacePopups::refreshAllList()
//{
//	for(uint8_t i = 0; i<4; i++)
//	{
//		if(selectedActualItem >= i*12 && selectedActualItem < (i+1)*12)
//		{
//			popupList.start = selectedActualItem%12;
//		}
//		else
//		{
//			popupList.start = -1;
//		}
//
//		popupList.length = 12;
//		popupList.linesCount = 12;
//		popupList.data = ptrActualItemsList+(i*12);
//
//
//		display.setControlData(listControl, &popupList);
//		display.setControlShow(listControl);
//
//		refreshList(i);
//	}
//}
//
//
//void cInterfacePopups::refreshList(uint8_t n)
//{
//	int8_t position = -1;
//
//	if(selectedActualItem >= n*12 && selectedActualItem < (n+1)*12)
//	{
//		position = selectedActualItem%12;
//	}
//
//	display.setControlValue(listControl, position);
//	display.refreshControl(listControl);
//}


void cInterfacePopups::selectPadOnPopup(int8_t pad)
{
	display.setControlValue(keyboardControl, pad);
	display.refreshControl(keyboardControl);
}


void cInterfacePopups::toggleStepPopupDescription()
{
	mtConfig.interface.fxPopupDescription = !mtConfig.interface.fxPopupDescription;
	refreshStepPopupDescription();
}

void cInterfacePopups::refreshStepPopupDescription()
{
	labelDoubleArrow.bitmaps[0].xValue = 600 + 15;
	labelDoubleArrow.bitmaps[0].yValue = 451;
	labelDoubleArrow.bitmaps[1].bitmapIndex = 0;

	display.setControlData(textLabel2, &labelDoubleArrow);
	display.setControlText(textLabel2, "Description");
	display.setControlStyle(textLabel2, controlStyleShow | controlStyleCenterY  | controlStyleFont1  | controlStyleShowBitmap);
	display.setControlColors(textLabel2, interfaceGlobals.activeButtonLabelsColors);
	display.setControlPosition(textLabel2, 600+40, 445);
	display.setControlSize(textLabel2, 200, 26);

	if(mtConfig.interface.fxPopupDescription)
	{
		display.setControlPosition(textLabel1, 400+1, 0);
		display.setControlSize(textLabel1, 400, 26);
		display.setControlValue(textLabel1, 0);
		display.setControlColors(textLabel1, titleBgLabelColors);//xxx
		display.refreshControl(textLabel1);

		display.setControlSize(bgLabel, 400, 450);
		display.setControlPosition(bgLabel, 400, 29);
		display.refreshControl(bgLabel);

		labelDoubleArrow.bitmaps[0].bitmapIndex = displayDoubleArrowR;
		display.refreshControl(textLabel2);

		display.setControlSize(textBox, 200, 450);
		display.setControlPosition(textBox, 400, 29);
		display.setControlShow(textBox);


		mtPopups.refreshFxDescription();

		if(!(patternEditor.trackerPattern.popupMode & 4))
		{
			patternEditor.trackerPattern.popupMode |= 4;
			display.refreshControl(patternEditor.patternControl);
		}
	}
	else
	{
		display.setControlPosition(textLabel1, 600+1, 0);
		display.setControlSize(textLabel1, 200, 26);
		display.setControlValue(textLabel1, 0);
		display.refreshControl(textLabel1);
		display.setControlPosition(bgLabel, 600+2, 29);
		display.setControlSize(bgLabel, 200, 450); //419
		display.refreshControl(bgLabel);

		labelDoubleArrow.bitmaps[0].bitmapIndex = displayDoubleArrowL;
		display.refreshControl(textLabel2);

		display.setControlHide(textBox);

		if(patternEditor.trackerPattern.popupMode & 4)
		{
			patternEditor.trackerPattern.popupMode  &= ~(4);
			display.refreshControl(patternEditor.patternControl);
		}
	}
}


//=========================================================================================================
static  uint8_t functLeftPopup()
{
	mtPopups.changeStepPopupValue(0,4);

	return 1;
}

static  uint8_t functRightPopup()
{
	mtPopups.changeStepPopupValue(0,2);

	return 1;
}

static  uint8_t functUpPopup()
{
	mtPopups.changeStepPopupValue(0,1);

	return 1;
}

static  uint8_t functDownPopup()
{
	mtPopups.changeStepPopupValue(0,3);

	return 1;
}


//=========================================================================================================
static  uint8_t functEncoderPopup(int16_t value)
{
	mtPopups.changeStepPopupValue(value);

	return 1;
}

//=========================================================================================================
static uint8_t functDescriptionButton()
{
	mtPopups.toggleStepPopupDescription();


	return 1;
}


//=========================================================================================================
static  uint8_t functPadsPopup(uint8_t pad, uint8_t state, int16_t velo)
{
	// wprowadzanie danych
	if (state == buttonPress)
	{
			//Sequencer::strPattern::strTrack::strStep *step =
			//		&(sequencer.getPatternToUI())->track[PTE->trackerPattern.actualTrack].step[PTE->trackerPattern.actualStep];
			//uint8_t note =  mtPadBoard.getNoteFromPad(pad);

			switch(POP->getStepPopupState())
			{
			case stepPopupNone:
			{
				padsBacklight.setFrontLayer(1,mtConfig.values.padsLightFront, pad);
				break;
			}
			case stepPopupNote:
			{
				//if (note < 0) break;
				/*
				sequencer.blinkNote(mtProject.values.lastUsedInstrument,
									note,
									step->velocity,
									PTE->trackerPattern.actualTrack);
				 */
				//uint8_t note = mtPadBoard.convertPadToNote(pad);
				//if(note > 48) note = 48;
				//editorInstrument->tune = note;

				mtPopups.setStepPopupValue(pad);
				padsBacklight.setFrontLayer(1,mtConfig.values.padsLightFront, pad);
				uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
				sequencer.handleNoteOn(
									Sequencer::GRID_OUTSIDE_PATTERN,
									noteFromPad,
									-1,
									pad);
				break;
			}
			case stepPopupInstr:
			{
				//if (note < 0) break;
				/*
				sequencer.blinkNote(mtProject.values.lastUsedInstrument,
									note,
									step->velocity,
									PTE->trackerPattern.actualTrack);
				*/
				padsBacklight.setFrontLayer(1,mtConfig.values.padsLightFront, pad);
				mtPadBoard.startInstrument(pad, mtProject.values.lastUsedInstrument,-1);
				break;
			}
			case stepPopupFx:
			{
				//if(pad < 0 || pad > FX_MAX) break;
				padsBacklight.setBackLayer(0, 0, mtProject.values.lastUsedFx);
				padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, pad);
				mtPopups.setStepPopupValue(pad);
				break;
			}
			default:	break;
			}
	}
	else if(state == buttonRelease)
	{
		padsBacklight.setFrontLayer(0,0, pad);
		uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
		sequencer.handleNoteOff(Sequencer::GRID_OUTSIDE_PATTERN, noteFromPad, 0, pad);
	}

	return 1;
}
//=========================================================================================================
//=========================================================================================================
//=========================================================================================================

/*
//	wywolywanie popupu	- max 4 linie tekstu
void show(config_slot, linia_1_textu);
void show(config_slot, linia_1_textu,linia_2_textu);
void show(config_slot, linia_1_textu,linia_1_textu...);

//	 ukrywanie jesli potrzba szybciej niz ustawiony czas
void hide();

//	configuracja wygladu popupu
void config(slot, struktura_konfigutracji);
*/
void cInterfacePopups::show(uint8_t config_slot, const char* line1)
{
	textLines[0] = (char*)line1;

	show(config_slot, textLines, 1);
}

void cInterfacePopups::show(uint8_t config_slot, const char* line1, const char* line2)
{
	textLines[0] = (char*)line1;
	textLines[1] = (char*)line2;

	show(config_slot, textLines, 2);
}

void cInterfacePopups::show(uint8_t config_slot, const char* line1, const char* line2, const char* line3)
{
	textLines[0] = (char*)line1;
	textLines[1] = (char*)line2;
	textLines[2] = (char*)line3;

	show(config_slot, textLines, 3);
}

void cInterfacePopups::show(uint8_t config_slot, const char* line1, const char* line2, const char* line3, const char* line4)
{
	textLines[0] = (char*)line1;
	textLines[1] = (char*)line2;
	textLines[2] = (char*)line3;
	textLines[3] = (char*)line4;

	show(config_slot, textLines, 4);
}


void cInterfacePopups::show(uint8_t config_slot, char** multiLineText, uint8_t lines_count)
{
	if(config_slot >= configsCount) return;
	// ustawienie czasu
	popupTimer = 0;
	globalPopupDisplayTime = globalConfig[config_slot].time;

	// kopiowanie configu
	popupData.textLinesCount = lines_count;
	popupData.multiLineText = multiLineText;

	for(uint8_t i = 0; i < textLinesCount; i++)
	{
		textStyles[i] = &globalConfig[config_slot].lineStyle[i];
		textColors[i] = &globalConfig[config_slot].lineColor[i];
	}

	popupData.multiLineStyle = textStyles;
	popupData.multiLineColors = textColors;

	// odsiwezenie kontrolki
	display.setControlData(textPopup, &popupData);
	display.setControlPosition(textPopup, globalConfig[config_slot].x, globalConfig[config_slot].y);
	display.setControlSize(textPopup, globalConfig[config_slot].w, globalConfig[config_slot].h);
	display.setControlStyle(textPopup, controlStyleShow | controlStyleBackground | controlStyleBottomShadow | controlStyleNoTransparency);
	display.setControlShow(textPopup);
	display.refreshControl(textPopup);
}

void cInterfacePopups::hide()
{
	if(globalPopupDisplayTime)
	{
		globalPopupDisplayTime = 0;

		display.setControlHide(textPopup);
		display.refreshControl(textPopup);
	}
}

void cInterfacePopups::config(uint8_t slot, strPopupStyleConfig* config)
{
	if(slot >= configsCount) return;

	memcpy(&globalConfig[slot] ,config, sizeof(strPopupStyleConfig));
}


//##############################################################################################################
//##############################################################################################################
//##############################################################################################################
void cInterfacePopups::showProgressPopup(const char* text)
{
	textLines[0] = (char*)text;
	textLines[1] = nullptr;

	progressPopupTextStyle = controlStyleCenterX | controlStyleCenterY;
	progressPopupTextColor = 0xffffff;

	popupData.textLinesCount = 2;
	popupData.multiLineText = textLines;

	for(uint8_t i = 0; i < textLinesCount; i++)
	{
		textStyles[i] = &progressPopupTextStyle;
		textColors[i] = &progressPopupTextColor;
	}

	popupData.multiLineStyle = textStyles;
	popupData.multiLineColors = textColors;

	// odsiwezenie kontrolki
	display.setControlData(textPopup, &popupData);
	display.setControlPosition(textPopup, 800/2-150, 480/2-50);
	display.setControlSize(textPopup, 300, 70);
	display.setControlStyle(textPopup, controlStyleShow | controlStyleBackground | controlStyleBottomShadow | controlStyleNoTransparency);
	display.setControlShow(textPopup);
	display.refreshControl(textPopup);

	display.setControlValue(progressBar, 0);
	display.setControlPosition(progressBar, 250, 195);
	display.setControlSize(progressBar,  300, 70);
	display.setControlValue(progressBar, 0);

}


void cInterfacePopups::changePopupProgress(int8_t value)
{
	display.setControlShow(progressBar);
	display.setControlValue(progressBar, value);
	display.refreshControl(progressBar);
}


void cInterfacePopups::hideProgressPopup()
{
	display.setControlValue(progressBar, 0);
	display.setControlHide(textPopup);
	display.setControlHide(progressBar);
	display.refreshControl(progressBar);
}


void cInterfacePopups::showInfoPopup(const char* text1, const char* text2)
{

	textLines[0] = (char*)text1;
	if(text2 == nullptr)
	{
		textLines[1] = nullptr;
		popupData.textLinesCount = 1;
	}
	else
	{
		textLines[1] = (char*)text2;
		popupData.textLinesCount = 2;
	}

	progressPopupTextStyle = controlStyleCenterX | controlStyleCenterY;
	progressPopupTextColor = 0xffffff;

	popupData.multiLineText = textLines;

	for(uint8_t i = 0; i < textLinesCount; i++)
	{
		textStyles[i] = &progressPopupTextStyle;
		textColors[i] = &progressPopupTextColor;
	}

	popupData.multiLineStyle = textStyles;
	popupData.multiLineColors = textColors;

	// odsiwezenie kontrolki
	display.setControlData(textPopup, &popupData);
	display.setControlPosition(textPopup, 800/2-150, 480/2-50);
	display.setControlSize(textPopup, 400, 70);
	display.setControlStyle(textPopup, controlStyleShow | controlStyleBackground | controlStyleBottomShadow | controlStyleNoTransparency);
	display.setControlShow(textPopup);
	display.refreshControl(textPopup);

}


void cInterfacePopups::hideInfoPopup()
{
	display.setControlHide(textPopup);
	display.refreshControl(textPopup);
}

