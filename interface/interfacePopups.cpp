

#include <interface.h>

#include <projectEditor.h>

#include "mtFileManager.h"
#include <display.h>

//=====================================================================================================
//
//=====================================================================================================

void cInterfacePopups::initPopupsDisplayControls()
{

	strControlProperties prop;
	// inicjalizacja list instrumentow
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

	prop.style = 0;
	prop.x = 16;
	prop.y = 130;
	prop.w = 780;
	prop.h = 280;
	prop.colors = nullptr;
	prop.value = -1;
	prop.data=&padNamesStruct;

	if(keyboardControl == nullptr)  keyboardControl = display.createControl<cNotePopout>(&prop);

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
//
//=====================================================================================================

void cInterfacePopups::showNotesPopup(int8_t startVal)
{
	notePopupState = 1;

	display.hideAllControls();

	display.setControlText(titleLabel, "Notes");
	display.setControlShow(titleLabel);
	display.refreshControl(titleLabel);

	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	showActualInstrument();

	selectedActualItem = startVal;
	display.setControlShow(keyboardControl);
	display.refreshControl(keyboardControl);

	display.synchronizeRefresh();
}

void cInterfacePopups::showInstrumentsPopup(int8_t startVal)
{
	intrPopupState = 1;

	display.hideAllControls();

	display.setControlText(titleLabel, "Instruments");
	display.setControlShow(titleLabel);
	display.refreshControl(titleLabel);

	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	showActualInstrument();
	listInstruments();

	selectedActualItem = startVal;
	ptrActualItemsList = (char**)(interfaceGlobals.ptrIntrumentsNames);
	refreshAllList();

	display.synchronizeRefresh();
}

void cInterfacePopups::showVolumesPopup(int8_t startVal)
{
	volPopupState = 1;

	display.hideAllControls();

	display.setControlText(titleLabel, "Volume");
	display.setControlShow(titleLabel);
	display.refreshControl(titleLabel);

	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	showActualInstrument();
}

void cInterfacePopups::showFxesPopup(int8_t startVal)
{
	fxPopupState = 1;

	display.hideAllControls();

	display.setControlText(titleLabel, "Fx");
	display.setControlShow(titleLabel);
	display.refreshControl(titleLabel);

	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	showActualInstrument();

	selectedActualItem = startVal;
	ptrActualItemsList = (char**)(interfaceGlobals.ptrFxNames);
	refreshAllList();

	display.synchronizeRefresh();
}

void cInterfacePopups::showGlobalPopup()
{

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

	display.setControlText(instrumentLabel,  actualInstrName);
	display.setControlShow(instrumentLabel);
	display.refreshControl(instrumentLabel);
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










