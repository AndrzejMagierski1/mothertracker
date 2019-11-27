
#include "interface.h"
#include "interfacePopups.h"


#include "mtPadBoard.h"
#include "mtPadsBacklight.h"

#include "mtSequencer.h"



static  uint8_t functLeftPopup();
static  uint8_t functRightPopup();
static  uint8_t functUpPopup();
static  uint8_t functDownPopup();

static  uint8_t functEncoderPopup(int16_t value);

static  uint8_t functPadsPopup(uint8_t pad, uint8_t state, int16_t velo);



static uint32_t instrListColors[] =
{
	0xFF0000,	//	 listItemFrame
	0x000000,	//	 listItemFrameBG
	0xFFFFFF,	//	 listScrollBar
	0x554A19,	//	 listBG
	0xFFFFFF,	//	 fontList
};

static uint32_t listBgLabelColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
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
	popupList.linesCount = 12;
	popupList.length = 12;
	popupList.data = interfaceGlobals.ptrIntrumentsNames;
	prop.style = controlStyleCenterY;
	prop.x = (800/4)*(3)+5;
	prop.y = 240;
	prop.w = 800/4-10;
	prop.h = 25;
	prop.colors = instrListColors;
	prop.data = &popupList;
	if(listControl == nullptr)  listControl = display.createControl<cList>(&prop);


	padNamesStruct.length = 5;
	padNamesStruct.name = interfaceGlobals.padNamesPointer;
	prop.style = 0;
	prop.x = 16;
	prop.y = 130;
	prop.w = 780;
	prop.h = 280;
	prop.colors = nullptr;
	prop.value = -1;
	prop.data=&padNamesStruct;
	if(keyboardControl == nullptr)  keyboardControl = display.createControl<cNotePopout>(&prop);


	strControlProperties prop2;
	prop2.style = 	(controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	prop2.colors = listBgLabelColors;
	if(textLabel1 == nullptr) textLabel1 = display.createControl<cLabel>(&prop2);
	prop2.style = 	(controlStyleRightX | controlStyleCenterY);
	prop2.x = 769;
	if(textLabel2 == nullptr) textLabel2 = display.createControl<cLabel>(&prop2);
	prop2.style = 	(controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(bgLabel == nullptr) bgLabel = display.createControl<cLabel>(&prop2);

	strControlProperties prop3;
	prop3.style = 	(controlStyleBackground);
	prop3.x = 0;
	prop3.y = 0;
	prop3.w = 50;
	prop3.h = 50;
	prop3.data = &popupData;
	if(textPopup == nullptr) textPopup = display.createControl<cTextPopup>(&prop3);


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

	display.setControlPosition(bgLabel, 0, 0);
	display.setControlSize(bgLabel, 800, 25);
	display.setControlColors(bgLabel, titleBgLabelColors);
	display.setControlStyle(bgLabel, controlStyleShow | controlStyleBackground);
	display.refreshControl(bgLabel);

	display.setControlText(textLabel1, "Notes");
	display.setControlPosition(textLabel1, 30, 12);
	display.setControlStyle(textLabel1, controlStyleShow | controlStyleCenterY);
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

	display.setControlPosition(bgLabel, 585, 0);
	display.setControlSize(bgLabel, 799-585, 479); //419
	display.setControlColors(bgLabel, listBgLabelColors);

	//display.setControlPosition(bgLabel, 0, 0);
	//display.setControlSize(bgLabel, 800, 25);
	display.setControlStyle(bgLabel, controlStyleShow | controlStyleBackground);
	display.refreshControl(bgLabel);

	display.setControlText(textLabel1, "Instruments");
	display.setControlPosition(textLabel1, 600, 12);
	//display.setControlPosition(textLabel1, 30, 12);
	display.setControlStyle(textLabel1, controlStyleShow | controlStyleCenterY);
	display.refreshControl(textLabel1);


	//showActualInstrument();
	listInstruments();

	ptrActualItemsList = (char**)(interfaceGlobals.ptrIntrumentsNames);

	instrList.start = selectedActualItem;
	instrList.linesCount = 17;
	instrList.length = 64;
	instrList.data = interfaceGlobals.ptrIntrumentsNames;

	display.setControlPosition(listControl, 600, 245);
	display.setControlData(listControl, &instrList);
	display.setControlShow(listControl);
	display.refreshControl(listControl);

	//refreshAllList();

	display.synchronizeRefresh();
}


void cInterfacePopups::showFxesPopup()
{
	//display.hideAllControls();

	display.setControlPosition(bgLabel, 585, 0);
	display.setControlSize(bgLabel, 799-585, 479); //419
	display.setControlColors(bgLabel, listBgLabelColors);

	//display.setControlPosition(bgLabel, 0, 0);
	//display.setControlSize(bgLabel, 800, 25);
	display.setControlStyle(bgLabel, controlStyleShow | controlStyleBackground);
	display.refreshControl(bgLabel);

	display.setControlText(textLabel1, "Fx");
	//display.setControlPosition(textLabel1, 30, 12);
	display.setControlPosition(textLabel1, 600, 12);
	display.setControlStyle(textLabel1, controlStyleShow | controlStyleCenterY);
	display.refreshControl(textLabel1);

	//showActualInstrument();
	ptrActualItemsList = (char**)(interfaceGlobals.ptrFxNames);

	instrList.start = selectedActualItem;
	instrList.linesCount = 17;
	instrList.length = 48;
	instrList.data = (char**)interfaceGlobals.ptrFxNames;

	display.setControlPosition(listControl, 600, 245);
	display.setControlData(listControl, &instrList);
	display.setControlShow(listControl);
	display.refreshControl(listControl);

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

		display.setControlHide(listControl);
		display.setControlHide(listControl);
		display.setControlHide(listControl);
		display.setControlHide(listControl);

		display.setControlHide(keyboardControl);

		display.setControlHide(bgLabel);
		display.setControlHide(textLabel1);
		display.setControlHide(textLabel2);

		mtInterface.uiFM.clearButton(interfaceButtonRec);
//		mtInterface.uiFM.clearButton(interfaceButtonShift);
//		mtInterface.uiFM.clearButton(interfaceButtonEnter);

		mtInterface.uiFM.clearButton(interfaceButtonLeft);
		mtInterface.uiFM.clearButton(interfaceButtonRight);
		mtInterface.uiFM.clearButton(interfaceButtonUp);
		mtInterface.uiFM.clearButton(interfaceButtonDown);

		mtInterface.uiFM.clearAllPots();
		mtInterface.uiFM.clearAllPads();
	}

	padsBacklight.clearAllPads(0, 1, 1);
}

void cInterfacePopups::setPopupFunct()
{
	switch(stepPopupState)
	{
	case stepPopupNote:
		mtInterface.uiFM.clearButton(interfaceButtonInstr);
		mtInterface.uiFM.clearButton(interfaceButtonFx1);
		mtInterface.uiFM.clearButton(interfaceButtonFx2);
		break;
	case stepPopupInstr:
		mtInterface.uiFM.clearButton(interfaceButtonNote);
		mtInterface.uiFM.clearButton(interfaceButtonFx1);
		mtInterface.uiFM.clearButton(interfaceButtonFx2);
		break;
	case stepPopupFx:
		mtInterface.uiFM.clearButton(interfaceButtonNote);
		mtInterface.uiFM.clearButton(interfaceButtonInstr);
		break;
	default:	break;
	}

	mtInterface.uiFM.clearButton(interfaceButtonRec);
//	mtInterface.uiFM.clearButton(interfaceButtonShift);
//	mtInterface.uiFM.clearButton(interfaceButtonEnter);

	mtInterface.uiFM.clearButtonsRange(interfaceButton0,interfaceButton7);

	mtInterface.uiFM.setPotObj(interfacePot0, functEncoderPopup, nullptr);

	mtInterface.uiFM.setButtonObj(interfaceButtonLeft, buttonPress, functLeftPopup);
	mtInterface.uiFM.setButtonObj(interfaceButtonRight, buttonPress, functRightPopup);
	mtInterface.uiFM.setButtonObj(interfaceButtonUp, buttonPress, functUpPopup);
	mtInterface.uiFM.setButtonObj(interfaceButtonDown, buttonPress, functDownPopup);

	mtInterface.uiFM.setPadsGlobal(functPadsPopup);
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
		refreshAllList();
		mtProject.values.lastUsedInstrument = selectedActualItem;
		break;
	}
	case stepPopupFx:
	{
		//refreshAllList();
		mtProject.values.lastUsedFx = selectedActualItem;
		display.setControlValue(listControl, selectedActualItem);
		display.refreshControl(listControl);

		break;
	}
	default:	break;
	}

	lightUpPads();
}


void cInterfacePopups::changeStepPopupValue(int16_t value, uint8_t dir)
{
	if(stepPopupState == stepPopupNone) return;

	uint8_t oldList = selectedActualItem/12;

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

		display.setControlValue(listControl, selectedActualItem);
		display.refreshControl(listControl);

		break;
	}
	case stepPopupFx:
	{
		if(dir > 0)
		{
			value = (dir == 1 ? -1 : (dir == 2 ? 12 : (dir == 3 ? 1 : (-12))));
		}

		padsBacklight.setBackLayer(0, 0, selectedActualItem);

		if(selectedActualItem + value < 0) selectedActualItem = 0;
		else if(selectedActualItem + value > FX_MAX) selectedActualItem = FX_MAX;
		else selectedActualItem += value;

		mtProject.values.lastUsedFx = selectedActualItem;

		display.setControlValue(listControl, selectedActualItem);
		display.refreshControl(listControl);

		padsBacklight.setBackLayer(1, 20, selectedActualItem);

		break;
	}

	default:	break;
	}

	lightUpPads();
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

	sprintf(actualInstrName, "%d. ", i+1);

	strncat(&actualInstrName[0], mtProject.instrument[i].sample.file_name, SAMPLE_NAME_SIZE);

	display.setControlText(textLabel2,  actualInstrName);
	display.setControlStyle(textLabel2, controlStyleShow  | controlStyleCenterY);
	display.setControlPosition(textLabel2, 585, 12);
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

void cInterfacePopups::refreshAllList()
{
	for(uint8_t i = 0; i<4; i++)
	{
		if(selectedActualItem >= i*12 && selectedActualItem < (i+1)*12)
		{
			popupList.start = selectedActualItem%12;
		}
		else
		{
			popupList.start = -1;
		}

		popupList.length = 12;
		popupList.linesCount = 12;
		popupList.data = ptrActualItemsList+(i*12);


		display.setControlData(listControl, &popupList);
		display.setControlShow(listControl);

		refreshList(i);
	}
}


void cInterfacePopups::refreshList(uint8_t n)
{
	int8_t position = -1;

	if(selectedActualItem >= n*12 && selectedActualItem < (n+1)*12)
	{
		position = selectedActualItem%12;
	}

	display.setControlValue(listControl, position);
	display.refreshControl(listControl);
}


void cInterfacePopups::selectPadOnPopup(int8_t pad)
{
	display.setControlValue(keyboardControl, pad);
	display.refreshControl(keyboardControl);
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
				padsBacklight.setFrontLayer(1,20, pad);
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

				padsBacklight.setFrontLayer(1,20, pad);
				mtPopups.setStepPopupValue(pad);
				mtPadBoard.startInstrument(pad, mtProject.values.lastUsedInstrument,-1);
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
				padsBacklight.setFrontLayer(1,20, pad);
				mtPadBoard.startInstrument(pad, mtProject.values.lastUsedInstrument,-1);
				break;
			}
			case stepPopupFx:
			{
				//if(pad < 0 || pad > FX_MAX) break;
				padsBacklight.setBackLayer(0, 0, mtProject.values.lastUsedFx);
				padsBacklight.setBackLayer(1, 20, pad);
				mtPopups.setStepPopupValue(pad);
				break;
			}
			default:	break;
			}
	}
	else if(state == buttonRelease)
	{
		padsBacklight.setFrontLayer(0,0, pad);
		mtPadBoard.stopInstrument(pad);
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
void cInterfacePopups::show(uint8_t config_slot, char* line1)
{
	textLines[0] = line1;

	show(config_slot, textLines, 1);
}

void cInterfacePopups::show(uint8_t config_slot, char* line1, char* line2)
{
	textLines[0] = line1;
	textLines[1] = line2;

	show(config_slot, textLines, 2);
}

void cInterfacePopups::show(uint8_t config_slot, char* line1, char* line2, char* line3)
{
	textLines[0] = line1;
	textLines[1] = line2;
	textLines[2] = line3;

	show(config_slot, textLines, 3);
}

void cInterfacePopups::show(uint8_t config_slot, char* line1, char* line2, char* line3, char* line4)
{
	textLines[0] = line1;
	textLines[1] = line2;
	textLines[2] = line3;
	textLines[3] = line4;

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
	//display.setControlData(textPopup, &popupData);
	display.setControlPosition(textPopup, globalConfig[config_slot].x, globalConfig[config_slot].y);
	display.setControlSize(textPopup, globalConfig[config_slot].w, globalConfig[config_slot].h);
	display.setControlStyle(textPopup, controlStyleShow | controlStyleBackground);
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



