
#include "configEditor/configEditor.h"
#include "configEditor/configMenu.h"


// baza baz
cMenuGroup menuBase(menuBase, 0, 0, 5);

// grupy glowne
cMenuGroup menuGeneral	(menuBase, 0, "General", 	3);
cMenuGroup menuMidi		(menuBase, 1, "MIDI", 		9);
cMenuGroup menuFirmware	(menuBase, 2, "Firmware", 	2);
cMenuGroup menuHelp		(menuBase, 3, "Help", 		1);
cMenuGroup menuCredits	(menuBase, 4, "Credits", 	1);


// elementy/////////
const strItemTypeValue setupPatternDiv 		{ &mtConfig.general.patternDiv,  1, 1, 16 	 };
const strItemTypeListText setupRadioRegion	{ &mtConfig.general.radioRegion, 3, ptrRadioRegion };
const strItemTypeListText setupBrightness 	{ &mtConfig.general.brightness,  3, ptrBrightness  };

cMenuItem melPatternDiv	(menuGeneral, 		0, "Pattern Divider", 		menuItemTypeValueU8,  &setupPatternDiv);
cMenuItem melRadioReg	(menuGeneral, 		1, "Radio region",	 		menuTypeItemListText, &setupRadioRegion);
cMenuItem melDispBright	(menuGeneral, 		2, "Display Brightness", 	menuTypeItemListText, &setupBrightness);

///////////////////
const strItemTypeListText clockInSetup 			{ &mtConfig.midi.clkIn,  			3, ptrClockIn  	};
const strItemTypeListText clockOutSetup 		{ &mtConfig.midi.clkOut, 	 		4, ptrMidiOut  	};
const strItemTypeListText transportInSetup 		{ &mtConfig.midi.transportIn, 		4, ptrMidiIn  	};
const strItemTypeListText transportOutSetup 	{ &mtConfig.midi.transportOut, 		4, ptrMidiOut  	};
const strItemTypeListText notesInModeSetup		{ &mtConfig.midi.notesInMode,		4, ptrMidiIn  		};
const strItemTypeListText notesOutModeSetup 	{ &mtConfig.midi.notesOutMode, 		4, ptrMidiOut  		};
const strItemTypeListText notesInChanneletup	{ &mtConfig.midi.notesInChannel, 	17, ptrInChannel  	};
const strItemTypeListText notesOutChannelSetup 	{ &mtConfig.midi.notesOutChannel, 	16, ptrOutChannel  	};


cMenuItem melClockIn		(menuMidi, 		0, "Clock in", 				menuTypeItemListText, &clockInSetup);
cMenuItem melClockOut		(menuMidi, 		1, "Clock out",				menuTypeItemListText, &clockOutSetup);
cMenuItem melTransportIn	(menuMidi, 		2, "Transport in", 			menuTypeItemListText, &transportInSetup);
cMenuItem melTransportOut	(menuMidi, 		3, "Transport out", 		menuTypeItemListText, &transportOutSetup);
cMenuItem melNotsInMode		(menuMidi, 		4, "Notes in mode", 		menuTypeItemListText, &notesInModeSetup);
cMenuItem melNotesInChannel	(menuMidi, 		5, "Notes in channnel", 	menuTypeItemListText, &notesOutModeSetup);
cMenuItem melNotsOutMode	(menuMidi, 		6, "Notes out mode", 		menuTypeItemListText, &notesInChanneletup);
cMenuItem melNotesOutChannel(menuMidi, 		7, "Notes out channel", 	menuTypeItemListText, &notesOutChannelSetup);
cMenuGroup melCCOut			(menuMidi, 		8, "CC out", 		10);


///////////
const strItemTypeActionButton updateFirmwareSetup 	{ firmwareUpgradeActivate, firmwareUpgradeDeactivate };
const strItemTypeLabel currentVerisonSetup 			{ interfaceGlobals.currentFirmwareVersion };

cMenuItem melUpdateFirmware	(menuFirmware, 	0, "Update Firmware", 		menuTypeItemActionButton, &updateFirmwareSetup);
cMenuItem melCurrentVersion	(menuFirmware, 	1, "Current Version", 		menuTypeItemLabel, &currentVerisonSetup);

/////////
cMenuItem melOpenManual		(menuHelp, 		0, "Open Manual", 			menuTypeItemListText, &clockInSetup);

///////////
cMenuItem melOpenCredits	(menuCredits, 	0, "Credits", 				menuTypeItemListText, &clockInSetup);



//=======================================================================

void cConfigEditor::createConfigMenu()
{

	createMenuBaseList();






}

void cConfigEditor::createMenuBaseList()
{
	basemenuList.linesCount = 13;
	basemenuList.start = menuBase.getSelectedItem();
	basemenuList.length = menuBase.getCount();
	basemenuList.data = menuBase.getNames();

	display.setControlData(configBasemenuListControl, &basemenuList);
	display.refreshControl(configBasemenuListControl);
	display.setControlShow(configBasemenuListControl);


	ReloadSubmenu();


}



void cConfigEditor::ReloadSubmenu()
{
	submenuList.linesCount = 13;
 	submenuList.start = menuBase.getSelChild()->getSelectedItem();
	submenuList.length = menuBase.getSelChild()->getCount();
	submenuList.params = menuBase.getSelChild()->getNames();
	submenuList.values = menuBase.getSelChild()->getValues();

	display.setControlData(configSubmenuListControl, &submenuList);
	display.refreshControl(configSubmenuListControl);
	display.setControlShow(configSubmenuListControl);
}





void cConfigEditor::changeMenuListPosition(uint8_t list, int16_t value)
{
	if(list == 0)
	{
		if(menuBase.selectedItem + value < 0) menuBase.selectedItem  = 0;
		else if(menuBase.selectedItem + value > menuBase.childsCount-1) menuBase.selectedItem = menuBase.childsCount-1;
		else menuBase.selectedItem += value;

		display.setControlValue(configBasemenuListControl, menuBase.selectedItem);
		display.refreshControl(configBasemenuListControl);

		ReloadSubmenu();
	}
	else if(list == 1)
	{
		if(menuBase.getSelChild()->selectedItem + value < 0) menuBase.getSelChild()->selectedItem  = 0;
		else if(menuBase.getSelChild()->selectedItem + value > menuBase.getSelChild()->childsCount -1) menuBase.getSelChild()->selectedItem = menuBase.getSelChild()->childsCount-1;
		else menuBase.getSelChild()->selectedItem += value;

		display.setControlValue(configSubmenuListControl, menuBase.getSelChild()->selectedItem);
		display.refreshControl(configSubmenuListControl);


	}



}

void cConfigEditor::executeSelectedListItem(uint8_t list)
{
	if(list == 0)
	{

	}
	else if(list == 1)
	{
		menuBase.getSelChild()->execute();
	}
}


void cMenuGroup::execute()
{
	if(childs[selectedItem]->type == menuTypeItem)
	{
		((cMenuItem*)childs[selectedItem])->execute1();
	}
}

void cMenuGroup::reloadValues()
{
	for(uint8_t i = 0; i < childsCount; i++)
	{
		if(childs[i]->type == menuTypeItem)
		{
			childsValues[i] = ((cMenuItem*)childs[i])->getValue();
		}
		else
		{
			childsValues[i] = (char*)ptrEmptyName;
		}

	}
}
