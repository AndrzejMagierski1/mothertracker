

#include "interfacePopups.h"
#include "mtPadBoard.h"



cInterfacePopups mtPopups;

//=====================================================================================================
//
//=====================================================================================================

void cInterfacePopups::initPopupsDisplayControls()
{

	strControlProperties prop;
	for(uint8_t i = 0; i<4; i++)
	{
		popupList[i].start = 0;
		popupList[i].linesCount = 12;
		popupList[i].length = 12;
		popupList[i].data = &interfaceGlobals.ptrIntrumentsNames[i*12];
		prop.style = controlStyleCenterY;
		prop.x = (800/4)*(i)+5;
		prop.y = 240;
		prop.w = 800/4-10;
		prop.h = 25;
		prop.colors = nullptr;
		prop.data = &popupList[i];
		if(listControl[i] == nullptr)  listControl[i] = display.createControl<cList>(&prop);
	}

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
	prop2.style = 	(controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(bgLabel == nullptr) bgLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	(controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	if(textLabel1 == nullptr) textLabel1 = display.createControl<cLabel>(&prop2);
	prop2.style = 	(controlStyleRightX | controlStyleCenterY);
	prop2.x = 769;
	if(textLabel2 == nullptr) textLabel2 = display.createControl<cLabel>(&prop2);


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

//=====================================================================================================
// show
//=====================================================================================================

uint8_t cInterfacePopups::getStepPopupState()
{
	return stepPopupState;
}

void cInterfacePopups::showStepPopup(uint8_t stepPopupType, int8_t initVal)
{
	if(stepPopupState != stepPopupNone)
	{
		hideStepPopups();
	}

	stepPopupState = stepPopupType;
	selectedActualItem = initVal;

	switch(stepPopupState)
	{
	case stepPopupNone:		break;
	case stepPopupNote:		showNotesPopup();		break;
	case stepPopupInstr:	showInstrumentsPopup();	break;
	case stepPopupVol:		showVolumesPopup();		break;
	case stepPopupFx:		showFxesPopup();		break;
	default:	break;
	}
}

void cInterfacePopups::showNotesPopup()
{
	display.hideAllControls();

	display.setControlPosition(bgLabel, 0, 0);
	display.setControlSize(bgLabel, 800, 25);
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
	display.hideAllControls();

	display.setControlPosition(bgLabel, 0, 0);
	display.setControlSize(bgLabel, 800, 25);
	display.setControlStyle(bgLabel, controlStyleShow | controlStyleBackground);
	display.refreshControl(bgLabel);

	display.setControlText(textLabel1, "Instruments");
	display.setControlPosition(textLabel1, 30, 12);
	display.setControlStyle(textLabel1, controlStyleShow | controlStyleCenterY);
	display.refreshControl(textLabel1);


	showActualInstrument();
	listInstruments();

	ptrActualItemsList = (char**)(interfaceGlobals.ptrIntrumentsNames);
	refreshAllList();

	display.synchronizeRefresh();
}

void cInterfacePopups::showVolumesPopup()
{
	display.hideAllControls();

	display.setControlPosition(bgLabel, 0, 0);
	display.setControlSize(bgLabel, 800, 25);
	display.setControlStyle(bgLabel, controlStyleShow | controlStyleBackground);
	display.refreshControl(bgLabel);

	display.setControlText(textLabel1, "Volumes");
	display.setControlPosition(textLabel1, 30, 12);
	display.setControlStyle(textLabel1, controlStyleShow | controlStyleCenterY);
	display.refreshControl(textLabel1);

	showActualInstrument();

	for(uint8_t i = 0; i < 48; i++)
	{
		interfaceGlobals.padNamesPointer[i] = (char*)mtVolumes[i];
	}

	display.setControlValue(keyboardControl, selectedActualItem);
	display.setControlShow(keyboardControl);
	display.refreshControl(keyboardControl);

	display.synchronizeRefresh();
}

void cInterfacePopups::showFxesPopup()
{
	display.hideAllControls();

	display.setControlPosition(bgLabel, 0, 0);
	display.setControlSize(bgLabel, 800, 25);
	display.setControlStyle(bgLabel, controlStyleShow | controlStyleBackground);
	display.refreshControl(bgLabel);

	display.setControlText(textLabel1, "Fx");
	display.setControlPosition(textLabel1, 30, 12);
	display.setControlStyle(textLabel1, controlStyleShow | controlStyleCenterY);
	display.refreshControl(textLabel1);

	showActualInstrument();

	ptrActualItemsList = (char**)(interfaceGlobals.ptrFxNames);
	refreshAllList();

	display.synchronizeRefresh();
}

void cInterfacePopups::showGlobalPopup()
{

}

//=====================================================================================================
// hide
//=====================================================================================================
void cInterfacePopups::hideStepPopups()
{
	if(stepPopupState)
	{
		display.setControlHide(listControl[0]);
		display.setControlHide(listControl[1]);
		display.setControlHide(listControl[2]);
		display.setControlHide(listControl[3]);

		display.setControlHide(keyboardControl);

		display.setControlHide(bgLabel);
		display.setControlHide(textLabel1);
		display.setControlHide(textLabel2);

	}
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
	case stepPopupNone:		break;
	case stepPopupNote:		selectPadOnPopup(selectedActualItem);		return;
	case stepPopupInstr:	refreshAllList();	break;
	case stepPopupVol:		selectPadOnPopup(selectedActualItem);		return;
	case stepPopupFx:		refreshAllList();		break;
	default:	break;
	}
}


void cInterfacePopups::changeStepPopupValue(int16_t value)
{
	if(stepPopupState == stepPopupNone) return;

	uint8_t oldList = selectedActualItem/12;

	switch(stepPopupState)
	{
	case stepPopupNote:
	case stepPopupVol:
	{
		if(selectedActualItem + value < 0) selectedActualItem = 0;
		else if(selectedActualItem + value > 47) selectedActualItem = 47;
		else selectedActualItem += value;
		return;
	}


	case stepPopupInstr:
	{
		if(selectedActualItem + value < 0) selectedActualItem = 0;
		else if(selectedActualItem + value > INSTRUMENTS_MAX) selectedActualItem = INSTRUMENTS_MAX;
		else selectedActualItem += value;
		break;
	}
	case stepPopupFx:
	{
		if(selectedActualItem + value < 0) selectedActualItem = 0;
		else if(selectedActualItem + value > FX_MAX) selectedActualItem = FX_MAX;
		else selectedActualItem += value;
		break;
	}

	default:	return;
	}


	uint8_t newList = selectedActualItem/12;

	mtProject.values.lastUsedFx = selectedActualItem;

	if(oldList != newList)
	{
		if(oldList < newList)
		{
			popupList[newList].start = 0;
			//if(type) popupList[newList].start = selectedFx%12;
			//intrumentsList[oldList].start = 11;
			popupList[oldList].start = -1;
		}
		else
		{
			//intrumentsList[oldList].start = 0;
			popupList[oldList].start = -1;

			popupList[newList].start = 11;
			//if(type) popupList[newList].start = selectedFx%12;
		}

		display.setControlData(listControl[newList], &popupList[newList]);
		display.setControlData(listControl[oldList], &popupList[oldList]);

		refreshList(oldList);
	}

	refreshList(newList);

	//showActualInstrument();
}

int16_t cInterfacePopups::getStepPopupValue()
{
	return selectedActualItem;
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
	display.setControlStyle(textLabel2, controlStyleShow | controlStyleRightX | controlStyleCenterY);
	display.setControlPosition(textLabel2, 769, 12);
	display.refreshControl(textLabel2);
}

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
}

void cInterfacePopups::refreshAllList()
{
	for(uint8_t i = 0; i<4; i++)
	{
		if(selectedActualItem >= i*12 && selectedActualItem < (i+1)*12)
		{
			popupList[i].start = selectedActualItem%12;
		}
		else
		{
			popupList[i].start = -1;
		}

		popupList[i].length = 12;
		popupList[i].linesCount = 12;
		popupList[i].data = ptrActualItemsList+(i*12);


		display.setControlData(listControl[i], &popupList[i]);
		display.setControlShow(listControl[i]);

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

	display.setControlValue(listControl[n], position);
	display.refreshControl(listControl[n]);
}


void cInterfacePopups::selectPadOnPopup(int8_t pad)
{
	display.setControlValue(keyboardControl, pad);
	display.refreshControl(keyboardControl);
}







