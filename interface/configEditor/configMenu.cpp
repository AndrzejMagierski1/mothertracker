
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
cMenuGroup menuCCOut		(menuMidi, 		8, "CC out", 		10);

const strItemTypeValue setupCC1 		{ &mtConfig.midi.ccOut[0],  1, 1, 127 	 };
const strItemTypeValue setupCC2			{ &mtConfig.midi.ccOut[1],  1, 1, 127 	 };
const strItemTypeValue setupCC3 		{ &mtConfig.midi.ccOut[2],  1, 1, 127 	 };
const strItemTypeValue setupCC4 		{ &mtConfig.midi.ccOut[3],  1, 1, 127 	 };
const strItemTypeValue setupCC5 		{ &mtConfig.midi.ccOut[4],  1, 1, 127 	 };
const strItemTypeValue setupCC6 		{ &mtConfig.midi.ccOut[5],  1, 1, 127 	 };
const strItemTypeValue setupCC7 		{ &mtConfig.midi.ccOut[6],  1, 1, 127 	 };
const strItemTypeValue setupCC8 		{ &mtConfig.midi.ccOut[7],  1, 1, 127 	 };
const strItemTypeValue setupCC9 		{ &mtConfig.midi.ccOut[8],  1, 1, 127 	 };
const strItemTypeValue setupCC10		{ &mtConfig.midi.ccOut[9],  1, 1, 127 	 };

cMenuItem melCC1(menuCCOut, 		0, "CC 1", 	menuItemTypeValueU8, &setupCC1);
cMenuItem melCC2(menuCCOut, 		1, "CC 2", 	menuItemTypeValueU8, &setupCC2);
cMenuItem melCC3(menuCCOut, 		2, "CC 3", 	menuItemTypeValueU8, &setupCC3);
cMenuItem melCC4(menuCCOut, 		3, "CC 4", 	menuItemTypeValueU8, &setupCC4);
cMenuItem melCC5(menuCCOut, 		4, "CC 5", 	menuItemTypeValueU8, &setupCC5);
cMenuItem melCC6(menuCCOut, 		5, "CC 6", 	menuItemTypeValueU8, &setupCC6);
cMenuItem melCC7(menuCCOut, 		6, "CC 7", 	menuItemTypeValueU8, &setupCC7);
cMenuItem melCC8(menuCCOut, 		7, "CC 8", 	menuItemTypeValueU8, &setupCC8);
cMenuItem melCC9(menuCCOut, 		8, "CC 9", 	menuItemTypeValueU8, &setupCC9);
cMenuItem melCC10(menuCCOut, 		9, "CC 10", menuItemTypeValueU8, &setupCC10);

///////////
const strItemTypeActionButton updateFirmwareSetup 	{ firmwareUpgradeActivate, firmwareUpgradeDeactivate };
const strItemTypeLabel currentVerisonSetup 			{ interfaceGlobals.currentFirmwareVersion };

cMenuItem melUpdateFirmware	(menuFirmware, 	0, "Firmware Update", 		menuTypeItemActionButton, &updateFirmwareSetup);
cMenuItem melCurrentVersion	(menuFirmware, 	1, "Current Version", 		menuTypeItemLabel, &currentVerisonSetup);

/////////
const strItemTypeLabel helpSetup 			{ (char*)"" };

cMenuItem melOpenManual		(menuHelp, 		0, "Open Manual", 			menuTypeItemLabel, &helpSetup);

///////////
const strItemTypeLabel creditsSetup 		{ (char*)"Ziejas ;)" };

cMenuItem melOpenCredits	(menuCredits, 	0, "Credits", 				menuTypeItemLabel, &creditsSetup);



//=======================================================================

void cConfigEditor::createConfigMenu()
{
	createMenuBaseList();
	refreshConfigMenu(0);
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

}



void cConfigEditor::reloadSubmenu()
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

void cConfigEditor::reloadSecondSubmenu()
{
	if(menuBase.getSelChild()->getSelChild()->type == menuTypeGroup)
	{
		secondSubmenuList.linesCount = 13;
		secondSubmenuList.start = menuBase.getSelChild()->getSelChild()->getSelectedItem();
		secondSubmenuList.length = menuBase.getSelChild()->getSelChild()->getCount();
		secondSubmenuList.params = menuBase.getSelChild()->getSelChild()->getNames();
		secondSubmenuList.values = menuBase.getSelChild()->getSelChild()->getValues();

		display.setControlData(configSecondSubmenuListControl, &secondSubmenuList);
		display.refreshControl(configSecondSubmenuListControl);
		display.setControlShow(configSecondSubmenuListControl);
	}
}

void cConfigEditor::loadConfigTextList(strItemTypeListText* itemSetup)
{
	showConfigList5(*itemSetup->value, itemSetup->count, (char**)itemSetup->ptrText);
	changeLabelText(7, "Select");
}

void cConfigEditor::loadConfigValuesList(strItemTypeListValues* itemSetup)
{
//	configList.linesCount = 13;
//	configList.start = *itemSetup->value ;
//	configList.length = (itemSetup->max-itemSetup->min)/itemSetup->interval;
//
//	configList.data = (char**)itemSetup->ptrText ;
//
//	display.setControlData(configListControl, &configList);
//	display.refreshControl(configListControl);
//	display.setControlShow(configListControl);
}


void cConfigEditor::changeMenuListPosition(uint8_t list, int16_t value)
{
	//sprawdzanie czy taka lista wogole dostepna
	if(list == 2 && menuBase.getSelChild()->getSelChild()->type != menuTypeGroup) return;
	if(list == 1 && menuBase.getSelChild()->type != menuTypeGroup) return;

	// wykownywanie akcji na zjechanie/ukrycie itemu listy ActionButton
	uint8_t itemPositionChanged = 0;
	void (*action_funct)(void) = nullptr;
	setActualMenuAction(&action_funct);

	if(list == 0)
	{
		uint8_t start_position = menuBase.selectedItem;

		if(menuBase.selectedItem + value < 0) menuBase.selectedItem  = 0;
		else if(menuBase.selectedItem + value > menuBase.childsCount-1) menuBase.selectedItem = menuBase.childsCount-1;
		else menuBase.selectedItem += value;

		if( start_position != menuBase.selectedItem) itemPositionChanged++;

		display.setControlValue(configBasemenuListControl, menuBase.selectedItem);
		display.refreshControl(configBasemenuListControl);
	}
	else if(list == 1)
	{
		uint8_t start_position = menuBase.getSelChild()->selectedItem;

		uint8_t* item_selected = &menuBase.getSelChild()->selectedItem;
		uint8_t item_max = menuBase.getSelChild()->childsCount - 1;

		if(*item_selected + value < 0) *item_selected  = 0;
		else if(*item_selected + value > item_max) *item_selected = item_max;
		else *item_selected += value;

		display.setControlValue(configSubmenuListControl, menuBase.getSelChild()->selectedItem);
		display.refreshControl(configSubmenuListControl);

		if( start_position !=  menuBase.getSelChild()->selectedItem) itemPositionChanged++;
	}
	else if(list == 2)
	{
		uint8_t start_position = menuBase.getSelChild()->getSelChild()->selectedItem;

		uint8_t* item_selected = &menuBase.getSelChild()->getSelChild()->selectedItem;
		uint8_t item_max = menuBase.getSelChild()->getSelChild()->childsCount - 1;

		if(*item_selected + value < 0) *item_selected  = 0;
		else if(*item_selected + value > item_max) *item_selected = item_max;
		else *item_selected += value;

		display.setControlValue(configSecondSubmenuListControl, *item_selected);
		display.refreshControl(configSecondSubmenuListControl);

		if( start_position !=  menuBase.getSelChild()->getSelChild()->selectedItem) itemPositionChanged++;
	}

	if(itemPositionChanged)
	{
		// odswiezenie menu
		refreshConfigMenu(list);

		//schowanie listy wyboru wartosci
		hideConfigList();

		// sprawdzanie czy zmieniono element menu z wczesniejszego typu actionbutton
		if(action_funct != nullptr)
		{
			action_funct();
		}
	}
}



void cConfigEditor::refreshConfigMenu(uint8_t listChanged)
{
	switch(listChanged)
	{
	case 0:
		// 1 lista (base)
		if(menuBase.getSelChild()->type == menuTypeItem)
		{
			hideSecondSubmenu();
			hideSubmenu();
			return;
		}
		else if(menuBase.getSelChild()->type == menuTypeGroup)
		{
			reloadSubmenu();
			showSubmenu();
		}
	case 1:
		// 2 lista (submenu)
		if(menuBase.getSelChild()->getSelChild()->type == menuTypeItem)
		{
			setLabelByMenuItemType(4, ((cMenuItem*)menuBase.getSelChild()->getSelChild())->getItemType());
			hideSecondSubmenu();
			return;
		}
		else if(menuBase.getSelChild()->getSelChild()->type == menuTypeGroup)
		{
			reloadSecondSubmenu();
			showSecondSubmenu();
			changeLabelText(4, "");
		}
	case 2:
		// 3 lista (second submenu)
		if(menuBase.getSelChild()->getSelChild()->getSelChild()->type == menuTypeItem)
		{
			setLabelByMenuItemType(7, ((cMenuItem*)menuBase.getSelChild()->getSelChild()->getSelChild())->getItemType());
		}
		else if(menuBase.getSelChild()->getSelChild()->getSelChild()->type == menuTypeGroup)
		{
			changeLabelText(7,"");
		}
	}
}


void cConfigEditor::setLabelByMenuItemType(uint8_t label, menu_item_t type)
{
	switch(type)
	{
	case menuItemTypeValueU8:
		changeLabelText(label,"Change");
		break;
	case menuTypeItemListValues:
		changeLabelText(label,"Change");
		break;
	case menuTypeItemListText:
		changeLabelText(label,"Change");
		break;
	case menuTypeItemLabel:
		changeLabelText(label,"");
		break;
	case menuItemTypeEmpty:
		changeLabelText(label,"");
		break;
	case menuTypeItemActionButton:
		changeLabelText(label,"Run");
		break;
	}
}


void cConfigEditor::setActualMenuAction(void (**actionFunct)(void))
{

	if(menuBase.getSelChild()->type == menuTypeItem)
	{
		if(((cMenuItem*)menuBase.getSelChild())->getItemType() == menuTypeItemActionButton)
			*actionFunct = ((strItemTypeActionButton*)((cMenuItem*)menuBase.getSelChild())->getItemSetup())->funct2;
	}
	else if (menuBase.getSelChild()->type == menuTypeGroup)
	{
		if(menuBase.getSelChild()->getSelChild()->type == menuTypeItem)
		{
			if(((cMenuItem*)menuBase.getSelChild()->getSelChild())->getItemType() == menuTypeItemActionButton)
				*actionFunct = ((strItemTypeActionButton*)((cMenuItem*)menuBase.getSelChild()->getSelChild())->getItemSetup())->funct2;
		}
		else if (menuBase.getSelChild()->getSelChild()->type == menuTypeGroup)
		{
			if(menuBase.getSelChild()->getSelChild()->getSelChild()->type == menuTypeItem)
			{
				if(((cMenuItem*)menuBase.getSelChild()->getSelChild()->getSelChild())->getItemType() == menuTypeItemActionButton)
					*actionFunct = ((strItemTypeActionButton*)((cMenuItem*)menuBase.getSelChild()->getSelChild()->getSelChild())->getItemSetup())->funct2;
			}
		}
	}
}


void cConfigEditor::executeSelectedListItem(uint8_t list)
{
	if(list == 0)
	{
		menuBase.execute();
	}
	else if(list == 1)
	{
		menuBase.getSelChild()->execute();
	}
	else if(list == 2)
	{
		menuBase.getSelChild()->getSelChild()->execute();
	}
}




void cMenuGroup::execute()
{
	if(childs[selectedItem]->type != menuTypeItem) return;


	cMenuItem* selected_child = (cMenuItem*)childs[selectedItem];

	switch(selected_child->itemType)
	{
		case menuItemTypeValueU8:

			break;
		case menuTypeItemListValues:
			configEditor.loadConfigValuesList((strItemTypeListValues*)selected_child->itemSetup);
			break;
		case menuTypeItemListText:
			configEditor.loadConfigTextList((strItemTypeListText*)selected_child->itemSetup);
			break;
		case menuTypeItemLabel:

			break;
		case menuItemTypeEmpty:

			break;
		case menuTypeItemActionButton:
		{
			if(((strItemTypeActionButton*)selected_child->itemSetup)->funct1 != nullptr)
			{
				((strItemTypeActionButton*)selected_child->itemSetup)->funct1();
			}
			break;
		}


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
