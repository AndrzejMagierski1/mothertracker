
#include "configEditor/configEditor.h"
#include "configEditor/configMenu.h"

#include "configEditor/configMenuActions.h"



// baza baz
cMenuGroup menuBase(menuBase, 0, 0, 5);
//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================

// grupy glowne
cMenuGroup menuGeneral	(menuBase, 0, "General", 	9);
cMenuGroup menuMidi		(menuBase, 1, "MIDI", 		9);
cMenuGroup menuMetro	(menuBase, 2, "Metronome", 	4);
cMenuGroup menuFirmware	(menuBase, 3, "Firmware", 	2);
//cMenuGroup menuHelp		(menuBase, 4, "Help", 		1);
cMenuGroup menuCredits	(menuBase, 4, "Credits", 	1);


// elementy/////////

//general
const strItemTypeValue8 setupPatternDiv 		{ &mtConfig.general.patternDiv,  	0, 15, 1, 1	 };
const strItemTypeListText setupRadioRegion		{ &mtConfig.general.radioRegion, 	3, ptrRadioRegion };
const strItemTypeListTextWithAction setupDispBrightness 	{ &mtConfig.general.dispBrightness,  3, ptrBrightness, setDisplayBrightness };
const strItemTypeListTextWithAction setupPadsBrightness 	{ &mtConfig.general.padsBrightness,  3, ptrBrightness, setPadsBrightness };
const strItemTypeListText setupMtpState 		{ &mtConfig.general.mtpState,  	 	2, ptrMtpState  };

const strItemTypeListText setupRecQuantization 	{ &mtConfig.general.recQuantization,  		2,		ptrRecQuantization		};
const strItemTypeListText setupPerfSource 		{ &mtConfig.general.performanceSource,  	2, 		ptrPerformanceSource	};
const strItemTypeListTextWithAction setupPadsScale		{ &mtConfig.general.padBoardScale,  	40, ptrScaleNames,		setPadBoardValues };
const strItemTypeListTextWithAction setupPadsRootNote	{ &mtConfig.general.padBoardRootNote,  	25,	&ptrAllNotes[24],	setPadBoardValues };
const strItemTypeListTextWithAction setupPadsLayout		{ &mtConfig.general.padBoardNoteOffset,	12, ptrPadsLayout,		setPadBoardValues };


cMenuItem melPatternDiv	(menuGeneral, 		0, "Pattern Divider", 		menuItemTypeValueU8,  &setupPatternDiv);
cMenuItem melRadioReg	(menuGeneral, 		1, "Radio region",	 		menuTypeItemListText, &setupRadioRegion);
cMenuItem melDispBright	(menuGeneral, 		2, "Display Brightness", 	menuTypeItemListTextWithAction, &setupDispBrightness);
cMenuItem melPadsBright	(menuGeneral, 		3, "Pads Brightness", 		menuTypeItemListTextWithAction, &setupPadsBrightness);
cMenuItem melRecQuantization	(menuGeneral, 	4, "Record microtiming", 	menuTypeItemListText, &setupRecQuantization);
cMenuItem melPerformanceSource	(menuGeneral, 	5, "Performance presets", 	menuTypeItemListText, &setupPerfSource);
cMenuItem melPadsScale			(menuGeneral, 	6, "Pads Scale", 			menuTypeItemListTextWithAction, &setupPadsScale);
cMenuItem melPadsRootNote		(menuGeneral, 	7, "Pads Root Note", 		menuTypeItemListTextWithAction, &setupPadsRootNote);
cMenuItem melPadsLayout			(menuGeneral, 	8, "Pads Layout", 			menuTypeItemListTextWithAction, &setupPadsLayout);

//cMenuItem melMtpState	(menuGeneral, 		3, "Files transfer", 		menuTypeItemListText, &setupMtpState);

///////////////////
// metronome
const strItemTypeListTextWithAction setupMetroState		{ &mtConfig.metronome.state,  			2,  ptrMetroState,		setMetronomValues };
const strItemTypeListTextWithAction setupMetroTimeSigNum	{ &mtConfig.metronome.timeSignatureNumerator, 	12, ptrMetroTimeSig,	setMetronomValues };
const strItemTypeListTextWithAction setupMetroTimeSigDenum	{ &mtConfig.metronome.timeSignatureDenominator, 	12, ptrMetroTimeSig,	setMetronomValues };
const strItemTypeValue8 setupMetroVolume				{ &mtConfig.metronome.volume,			0, 100,	};

cMenuItem melMetroState			(menuMetro, 	0, "State", 				menuTypeItemListTextWithAction, &setupMetroState);
cMenuItem melMetroTimeSigNum	(menuMetro, 	1, "Time signature num", 	menuTypeItemListTextWithAction, &setupMetroTimeSigNum);
cMenuItem melMetroTimeSigDenum	(menuMetro, 	2, "Time signature denum", 	menuTypeItemListTextWithAction, &setupMetroTimeSigDenum);
cMenuItem melMetroVolume		(menuMetro, 	3, "Volume", 				menuItemTypeValueU8, 			&setupMetroVolume);



///////////////////
// midi
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
cMenuItem melNotsInMode		(menuMidi, 		4, "Notes in", 				menuTypeItemListText, &notesInModeSetup);
cMenuItem melNotesInChannel	(menuMidi, 		5, "Notes in channnel", 	menuTypeItemListText, &notesInChanneletup);
cMenuItem melNotsOutMode	(menuMidi, 		6, "MIDI out", 				menuTypeItemListText, &notesOutModeSetup);
cMenuItem melNotesOutChannel(menuMidi, 		7, "CC/PC out channel", 	menuTypeItemListText, &notesOutChannelSetup);
cMenuGroup menuCCOut		(menuMidi, 		8, "CC out", 		5);

const strItemTypeValue8 setupCC1 		{ &mtConfig.midi.ccOut[0],  0, 127, 1, 0	 };
const strItemTypeValue8 setupCC2		{ &mtConfig.midi.ccOut[1],  0, 127, 1, 0	 };
const strItemTypeValue8 setupCC3 		{ &mtConfig.midi.ccOut[2],  0, 127, 1, 0	 };
const strItemTypeValue8 setupCC4 		{ &mtConfig.midi.ccOut[3],  0, 127, 1, 0	 };
const strItemTypeValue8 setupCC5 		{ &mtConfig.midi.ccOut[4],  0, 127, 1, 0	 };
const strItemTypeValue8 setupCC6 		{ &mtConfig.midi.ccOut[5],  0, 127, 1, 0	 };
const strItemTypeValue8 setupCC7 		{ &mtConfig.midi.ccOut[6],  0, 127, 1, 0	 };
const strItemTypeValue8 setupCC8 		{ &mtConfig.midi.ccOut[7],  0, 127, 1, 0	 };
const strItemTypeValue8 setupCC9 		{ &mtConfig.midi.ccOut[8],  0, 127, 1, 0	 };
const strItemTypeValue8 setupCC10		{ &mtConfig.midi.ccOut[9],  0, 127, 1, 0	 };

cMenuItem melCC1(menuCCOut, 		0, "CC A", 	menuItemTypeValueU8, &setupCC1);
cMenuItem melCC2(menuCCOut, 		1, "CC B", 	menuItemTypeValueU8, &setupCC2);
cMenuItem melCC3(menuCCOut, 		2, "CC C", 	menuItemTypeValueU8, &setupCC3);
cMenuItem melCC4(menuCCOut, 		3, "CC D", 	menuItemTypeValueU8, &setupCC4);
cMenuItem melCC5(menuCCOut, 		4, "CC E", 	menuItemTypeValueU8, &setupCC5);
//cMenuItem melCC6(menuCCOut, 		5, "CC 6", 	menuItemTypeValueU8, &setupCC6);
//cMenuItem melCC7(menuCCOut, 		6, "CC 7", 	menuItemTypeValueU8, &setupCC7);
//cMenuItem melCC8(menuCCOut, 		7, "CC 8", 	menuItemTypeValueU8, &setupCC8);
//cMenuItem melCC9(menuCCOut, 		8, "CC 9", 	menuItemTypeValueU8, &setupCC9);
//cMenuItem melCC10(menuCCOut, 		9, "CC 10", menuItemTypeValueU8, &setupCC10);

///////////
const strItemTypeActionButton updateFirmwareSetup 	{ firmwareUpgradeActivate, firmwareUpgradeDeactivate };
const strItemTypeLabel currentVerisonSetup 			{ interfaceGlobals.currentFirmwareVersion };

cMenuItem melUpdateFirmware	(menuFirmware, 	0, "Firmware Update", 		menuTypeItemActionButton, &updateFirmwareSetup);
cMenuItem melCurrentVersion	(menuFirmware, 	1, "Current Version", 		menuTypeItemLabel, &currentVerisonSetup);

/////////
//const strItemTypeLabel helpSetup 			{ (char*)"" };

//cMenuItem melOpenManual		(menuHelp, 		0, "Open Manual", 			menuTypeItemLabel, &helpSetup);

///////////
const strItemTypeActionButton creditsSetup 		{ openCreditsAction, nullptr};

cMenuItem melOpenCredits	(menuCredits, 	0, "Credits", 				menuTypeItemActionButton, &creditsSetup);



//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================

void cConfigEditor::createConfigMenu()
{
	createMenuBaseList();
	refreshConfigMenu(0);
}

void cConfigEditor::hideConfigMenu()
{
	display.setControlHide(configListControl);
	display.setControlHide(configBasemenuListControl);
	display.setControlHide(configSubmenuListControl);
	display.setControlHide(configSecondSubmenuListControl);
	display.setControlHide(frameControl);


	display.refreshControl(frameControl);

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
	submenuList.valueEditActive = 0;

	if(menuBase.getSelChild()->getSelChild()->type == menuTypeItem)
		submenuList.valueEditActive = ((cMenuItem*)menuBase.getSelChild()->getSelChild())->getValueEditState();

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
		secondSubmenuList.valueEditActive = 0;

		if(menuBase.getSelChild()->getSelChild()->getSelChild()->type == menuTypeItem)
			secondSubmenuList.valueEditActive = ((cMenuItem*)menuBase.getSelChild()->getSelChild()->getSelChild())->getValueEditState();

		display.setControlData(configSecondSubmenuListControl, &secondSubmenuList);
		display.refreshControl(configSecondSubmenuListControl);
		display.setControlShow(configSecondSubmenuListControl);
	}
}

void cConfigEditor::loadConfigTextList(strItemTypeListText* itemSetup)
{
	showConfigList5(2, *itemSetup->value, itemSetup->count, (char**)itemSetup->ptrText);
	changeLabelText(7, "Select");
	selectConfigList(2);
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

//source = 0 - encoder, = 1 - klawisze pod ekranem
void cConfigEditor::changeMenuListPosition(uint8_t list, int16_t value, uint8_t source)
{
	//sprawdzanie czy taka lista wogole dostepna
	if(list == 2 && menuBase.getSelChild()->getSelChild()->type != menuTypeGroup) return;
	if(list == 1 && menuBase.getSelChild()->type != menuTypeGroup) return;

	// wykownywanie akcji na zjechanie/ukrycie itemu listy ActionButton
	uint8_t itemPositionChanged = 0;
	void (*action_funct)(void) = nullptr;
	cMenuItem* selectedMenuItem = nullptr;
	uint8_t itemLevel = 0;
	getSelectedItemInfo(&action_funct, (void**)(&selectedMenuItem), &itemLevel);

	if(selectedMenuItem != nullptr && itemLevel == list && selectedMenuItem->getValueEditState())
	{
		if(source == 1) value = -value;
		chanegeItemValue((void*)(selectedMenuItem), value);
		refreshConfigMenu(0);
		return;
	}

	if(list == 0)
	{
		uint8_t start_position = menuBase.selectedItem;

		if(menuBase.selectedItem + value < 0) menuBase.selectedItem  = 0;
		else if(menuBase.selectedItem + value > menuBase.childsCount-1) menuBase.selectedItem = menuBase.childsCount-1;
		else menuBase.selectedItem += value;

		if(start_position != menuBase.selectedItem) itemPositionChanged++;

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

		if(start_position !=  menuBase.getSelChild()->selectedItem) itemPositionChanged++;
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

		if(start_position !=  menuBase.getSelChild()->getSelChild()->selectedItem) itemPositionChanged++;
	}

	if(itemPositionChanged)
	{
		// odswiezenie menu
		if(selectedMenuItem != nullptr) selectedMenuItem->resetValueEditState();
		//refreshConfigMenu(list);// to do obadać temat
		refreshConfigMenu(0);

		//schowanie listy wyboru wartosci
// 		hideConfigList();

		// sprawdzanie czy zmieniono element menu z wczesniejszego typu actionbutton
		if(action_funct != nullptr)
		{
			action_funct();
		}
	}
}

void cConfigEditor::chanegeItemValue(void* selectedMenuItem, int16_t value)
{
	if(selectedMenuItem == nullptr) return;
	if(((cMenuItem*)selectedMenuItem)->getItemType() != menuItemTypeValueU8) return; // narazie tylko dla U8

	strItemTypeValue8* itemSetup = (strItemTypeValue8*)((cMenuItem*)selectedMenuItem)->getItemSetup();

	if(*itemSetup->value + value < itemSetup->min) *itemSetup->value  = itemSetup->min;
	else if(*itemSetup->value + value > itemSetup->max) *itemSetup->value = itemSetup->max;
	else *itemSetup->value += value;
}

void cConfigEditor::refreshConfigMenu(uint8_t listChanged)
{
	if(listChanged <= 0)
	{// 1 lista (base)
		if(menuBase.getSelChild()->type == menuTypeItem)
		{
			//if(((cMenuItem*)menuBase.getSelChild())->valueEditState)
			hideSecondSubmenu();
			hideSubmenu();
			return;
		}
		else if(menuBase.getSelChild()->type == menuTypeGroup)
		{
			reloadSubmenu();
			showSubmenu();
		}
	}
	if(listChanged <= 1)
	{// 2 lista (submenu)
		if(menuBase.getSelChild()->getSelChild()->type == menuTypeItem)
		{
			hideConfigList();
			cMenuItem* temp_child = (cMenuItem*)menuBase.getSelChild()->getSelChild();
			setLabelByMenuItemType(4, temp_child->getItemType(), temp_child->getValueEditState());
			hideSecondSubmenu();
			return;
		}
		else if(menuBase.getSelChild()->getSelChild()->type == menuTypeGroup)
		{
			hideConfigList();
			reloadSecondSubmenu();
			showSecondSubmenu();
			changeLabelText(4, "");
			//if(menuBase.getSelChild()->getSelChild()->getSelChild() == nullptr) return;
			cMenuItem* item_child = (cMenuItem*)menuBase.getSelChild()->getSelChild()->getSelChild();
			setLabelByMenuItemType(7, item_child->getItemType(), item_child->getValueEditState());
			//setLabelByMenuItemType(7, menuItemTypeValueU8, 0);
		}
	}
	if(listChanged <= 2)
	{

		// 3 lista (second submenu)
		//if(menuBase.getSelChild()->getSelChild()->getSelChild() == nullptr) return;
/*		if(item_child->type == menuTypeItem)
		{
			//cMenuItem* temp_child = (cMenuItem*)menuBase.getSelChild()->getSelChild()->getSelChild();
			setLabelByMenuItemType(7, ((cMenuItem*)item_child)->getItemType(), ((cMenuItem*)item_child)->getValueEditState());
		}
		else if(item_child->type == menuTypeGroup)
		{
			changeLabelText(7,"");
		}
*/
	}
}


void cConfigEditor::setLabelByMenuItemType(uint8_t label, menu_item_t type, uint8_t variant)
{
	switch(type)
	{
	case menuItemTypeValueU8:
	{
		if(variant == 0) 		changeLabelText(label, "Change");
		else if(variant == 1) 	changeLabelText(label, "Confirm");

	}
	break;
	case menuTypeItemListValues:
		changeLabelText(label,"Change");
		break;
	case menuTypeItemListText:
		changeLabelText(label,"Change");
		break;
	case menuTypeItemListTextWithAction:
		changeLabelText(label,"Change");
		break;
	case menuTypeItemLabel:
		changeLabelText(label,"");
		break;
	case menuItemTypeEmpty:
		changeLabelText(label,"");
		break;
	case menuTypeItemActionButton:
		changeLabelText(label,"Enter");
		break;
	}
}


void cConfigEditor::getSelectedItemInfo(void (**actionFunct)(void), void** menu_item, uint8_t* item_level) // to do cos nie dizala
{

	if(menuBase.getSelChild()->type == menuTypeItem)
	{
		if(((cMenuItem*)menuBase.getSelChild())->getItemType() == menuTypeItemActionButton)
			*actionFunct = ((strItemTypeActionButton*)((cMenuItem*)menuBase.getSelChild())->getItemSetup())->funct2;

		*menu_item = menuBase.getSelChild();
		*item_level = 0;
	}
	else if (menuBase.getSelChild()->type == menuTypeGroup)
	{
		if(menuBase.getSelChild()->getSelChild()->type == menuTypeItem)
		{
			if(((cMenuItem*)menuBase.getSelChild()->getSelChild())->getItemType() == menuTypeItemActionButton)
				*actionFunct = ((strItemTypeActionButton*)((cMenuItem*)menuBase.getSelChild()->getSelChild())->getItemSetup())->funct2;

			*menu_item = menuBase.getSelChild()->getSelChild();
			*item_level = 1;
		}
		else if (menuBase.getSelChild()->getSelChild()->type == menuTypeGroup)
		{
			if(menuBase.getSelChild()->getSelChild()->getSelChild()->type == menuTypeItem)
			{
				if(((cMenuItem*)menuBase.getSelChild()->getSelChild()->getSelChild())->getItemType() == menuTypeItemActionButton)
					*actionFunct = ((strItemTypeActionButton*)((cMenuItem*)menuBase.getSelChild()->getSelChild()->getSelChild())->getItemSetup())->funct2;

				*menu_item = menuBase.getSelChild()->getSelChild()->getSelChild();
				*item_level = 2;
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


void cConfigEditor::configListConfirm(uint8_t list_pos)
{
	cMenuItem* selected_child = nullptr;

	// znajduje item który wywolal zatwierdzana liste
	if(menuBase.getSelChild()->type == menuTypeItem)
	{
		selected_child = (cMenuItem*)menuBase.getSelChild();
	}
	else if (menuBase.getSelChild()->type == menuTypeGroup)
	{
		if(menuBase.getSelChild()->getSelChild()->type == menuTypeItem)
		{
			selected_child = (cMenuItem*)menuBase.getSelChild()->getSelChild();
		}
		else if (menuBase.getSelChild()->getSelChild()->type == menuTypeGroup)
		{
			// po second submenu nie mozna wysietlac listy
			//if(menuBase.getSelChild()->getSelChild()->getSelChild()->type == menuTypeItem)
			//{
			//	selected_child = (cMenuItem*)menuBase.getSelChild()->getSelChild()->getSelChild();
			//}
		}
	}

	if(selected_child != nullptr)
	{
		switch(selected_child->getItemType())
		{
			case menuTypeItemListValues:
			{
				strItemTypeListValues* temp_str = (strItemTypeListValues*)selected_child->getItemSetup();
				if(list_pos <= temp_str->max-temp_str->min) *(temp_str->value) = temp_str->min+list_pos;
				selectSubmenu();
				break;
			}
			case menuTypeItemListText:
			{
				strItemTypeListText* temp_str = (strItemTypeListText*)selected_child->getItemSetup();
				if(list_pos < temp_str->count) *(temp_str->value) = list_pos;
				selectSubmenu();
				break;
			}
			case menuTypeItemListTextWithAction:
			{
				strItemTypeListTextWithAction* temp_str = (strItemTypeListTextWithAction*)selected_child->getItemSetup();
				if(list_pos < temp_str->count) *(temp_str->value) = list_pos;
				temp_str->funct1();
				selectSubmenu();
				break;
			}
			default: break;
		}

		saveConfigToEeprom();
	}
	//schowanie listy wyboru wartosci
	hideConfigList();
	//odswiezenie calego menu
	refreshConfigMenu(0);
}

void cMenuGroup::execute()
{
	if(childs[selectedItem]->type != menuTypeItem) return;

	cMenuItem* selected_child = (cMenuItem*)childs[selectedItem];

	switch(selected_child->itemType)
	{
		case menuItemTypeValueU8:
			selected_child->valueEditState = !selected_child->valueEditState;
			if(!selected_child->valueEditState) configEditor.saveConfigToEeprom();
			configEditor.refreshConfigMenu(0);
			break;
		case menuTypeItemListValues:
			configEditor.loadConfigValuesList((strItemTypeListValues*)selected_child->itemSetup);
			break;
		case menuTypeItemListText:
			configEditor.loadConfigTextList((strItemTypeListText*)selected_child->itemSetup);
			break;
		case menuTypeItemListTextWithAction:
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
