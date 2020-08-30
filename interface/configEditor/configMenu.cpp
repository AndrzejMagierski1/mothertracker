
#include "configEditor/configEditor.h"
#include "configEditor/configMenu.h"

#include "configEditor/configMenuActions.h"

#include "imageViewer/imageViewerDefs.h"


// baza baz
cMenuGroup menuBase(menuBase, 0, 0, 6);
//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================

// grupy glowne
cMenuGroup menuGeneral	(menuBase, 0, "General", 	10);
cMenuGroup menuMidi		(menuBase, 1, "MIDI", 		9);
cMenuGroup menuMetro	(menuBase, 2, "Metronome", 	5);
cMenuGroup menuFirmware	(menuBase, 3, "Firmware", 	2);
cMenuGroup menuManual	(menuBase, 4, "Manual", 	1);
cMenuGroup menuCredits	(menuBase, 5, "Credits", 	1);


// elementy/////////

//general
const strItemTypeListValues setupPatternDiv 			{ &mtConfig.general.patternDiv,  		0, 15, 1, 1	 };
const strItemTypeListText setupRadioRegion				{ &mtConfig.general.radioRegion, 		3, ptrRadioRegion };
const strItemTypeListTextWithAction setupDispBrightness { &mtConfig.general.dispBrightness,  	3, ptrBrightness, setDisplayBrightness };
const strItemTypeListTextWithAction setupPadsBrightness { &mtConfig.general.padsBrightness, 	3, ptrBrightness, setPadsBrightness };
const strItemTypeListText setupMtpState 				{ &mtConfig.general.mtpState,  	 		2, ptrMtpState  };

const strItemTypeListText setupRecQuantization 			{ &mtConfig.general.recOptions,  		4,	ptrRecQuantization		};
const strItemTypeListText setupPerfSource 				{ &mtConfig.general.performanceSource,  2, 	ptrPerformanceSource	};
const strItemTypeListTextWithAction setupPadsScale		{ &mtConfig.general.padBoardScale,  	40, ptrScaleNames,		setPadBoardValues };
const strItemTypeListTextWithAction setupPadsRootNote	{ &mtConfig.general.padBoardRootNote,  	25,	&ptrAllNotes[24],	setPadBoardValues };
const strItemTypeListTextWithAction setupPadsLayout		{ &mtConfig.general.padBoardNoteOffset,	12, ptrPadsLayout,		setPadBoardValues };
const strItemTypeListTextWithAction setupAntialiasing 	{ &mtConfig.general.antialiasingEnable,	2,  ptrAntyaliasingEnable,	setAntyaliasingEnable };


cMenuItem melPatternDiv				(menuGeneral, 	0, "Pattern Divider", 		menuTypeItemListValues,  &setupPatternDiv);
cMenuItem melRadioReg				(menuGeneral, 	1, "Radio region",	 		menuTypeItemListText, &setupRadioRegion);
cMenuItem melDispBright				(menuGeneral, 	2, "Display Brightness", 	menuTypeItemListTextWithAction, &setupDispBrightness);
cMenuItem melPadsBright				(menuGeneral, 	3, "Pads Brightness", 		menuTypeItemListTextWithAction, &setupPadsBrightness);
cMenuItem melRecQuantization		(menuGeneral, 	4, "Recording options", 	menuTypeItemListText, &setupRecQuantization);
cMenuItem melPerformanceSource		(menuGeneral, 	5, "Performance presets", 	menuTypeItemListText, &setupPerfSource);
cMenuItem melPadsScale				(menuGeneral, 	6, "Pads Scale", 			menuTypeItemListTextWithAction, &setupPadsScale);
cMenuItem melPadsRootNote			(menuGeneral, 	7, "Pads Root Note", 		menuTypeItemListTextWithAction, &setupPadsRootNote);
cMenuItem melPadsLayout				(menuGeneral, 	8, "Pads Layout", 			menuTypeItemListTextWithAction, &setupPadsLayout);
cMenuItem melAntialiasingEnable		(menuGeneral, 	9, "Anti-aliasing", 		menuTypeItemListTextWithAction, &setupAntialiasing);
//cMenuItem melMtpState	(menuGeneral, 		3, "Files transfer", 		menuTypeItemListText, &setupMtpState);

///////////////////
// metronome
const strItemTypeListTextWithAction setupMetroState			{ &mtConfig.metronome.state,  			2,  ptrMetroState,		setMetronomValues };
const strItemTypeListTextWithAction setupMetroPreRoll		{ &mtConfig.metronome.preRoll,  		2,  ptrPreRollState,		setMetronomValues };
const strItemTypeListTextWithAction setupMetroTimeSigNum	{ &mtConfig.metronome.timeSignatureNumerator, 	12, ptrMetroTimeSig,	setMetronomValues };
const strItemTypeListTextWithAction setupMetroTimeSigDenum	{ &mtConfig.metronome.timeSignatureDenominator, 	12, ptrMetroTimeSig,	setMetronomValues };
const strItemTypeListValues setupMetroVolume				{ &mtConfig.metronome.volume,			0, 100, 1, 0	};

cMenuItem melMetroState			(menuMetro, 	0, "State", 				menuTypeItemListTextWithAction, &setupMetroState);
cMenuItem melMetroPreRoll		(menuMetro, 	1, "Pre Roll", 				menuTypeItemListTextWithAction, &setupMetroPreRoll);
cMenuItem melMetroTimeSigNum	(menuMetro, 	2, "Time signature num", 	menuTypeItemListTextWithAction, &setupMetroTimeSigNum);
cMenuItem melMetroTimeSigDenum	(menuMetro, 	3, "Time signature denum", 	menuTypeItemListTextWithAction, &setupMetroTimeSigDenum);
cMenuItem melMetroVolume		(menuMetro, 	4, "Volume", 				menuTypeItemListValues,			&setupMetroVolume);



///////////////////
// midi
const strItemTypeListText clockInSetup 			{ &mtConfig.midi.clkIn,  			3, ptrClockIn  	};
const strItemTypeListText clockOutSetup 		{ &mtConfig.midi.clkOut, 	 		4, ptrMidiOut  	};
const strItemTypeListText transportInSetup 		{ &mtConfig.midi.transportIn, 		3, ptrClockIn  	};
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

const strItemTypeListValues setupCC1 		{ &mtConfig.midi.ccOut[0],  0, 127, 1, 0	 };
const strItemTypeListValues setupCC2		{ &mtConfig.midi.ccOut[1],  0, 127, 1, 0	 };
const strItemTypeListValues setupCC3 		{ &mtConfig.midi.ccOut[2],  0, 127, 1, 0	 };
const strItemTypeListValues setupCC4 		{ &mtConfig.midi.ccOut[3],  0, 127, 1, 0	 };
const strItemTypeListValues setupCC5 		{ &mtConfig.midi.ccOut[4],  0, 127, 1, 0	 };
const strItemTypeListValues setupCC6 		{ &mtConfig.midi.ccOut[5],  0, 127, 1, 0	 };
const strItemTypeListValues setupCC7 		{ &mtConfig.midi.ccOut[6],  0, 127, 1, 0	 };
const strItemTypeListValues setupCC8 		{ &mtConfig.midi.ccOut[7],  0, 127, 1, 0	 };
const strItemTypeListValues setupCC9 		{ &mtConfig.midi.ccOut[8],  0, 127, 1, 0	 };
const strItemTypeListValues setupCC10		{ &mtConfig.midi.ccOut[9],  0, 127, 1, 0	 };

cMenuItem melCC1(menuCCOut, 		0, "CC A", 	menuTypeItemListValues, &setupCC1);
cMenuItem melCC2(menuCCOut, 		1, "CC B", 	menuTypeItemListValues, &setupCC2);
cMenuItem melCC3(menuCCOut, 		2, "CC C", 	menuTypeItemListValues, &setupCC3);
cMenuItem melCC4(menuCCOut, 		3, "CC D", 	menuTypeItemListValues, &setupCC4);
cMenuItem melCC5(menuCCOut, 		4, "CC E", 	menuTypeItemListValues, &setupCC5);
//cMenuItem melCC6(menuCCOut, 		5, "CC 6", 	menuTypeItemListValues, &setupCC6);
//cMenuItem melCC7(menuCCOut, 		6, "CC 7", 	menuTypeItemListValues, &setupCC7);
//cMenuItem melCC8(menuCCOut, 		7, "CC 8", 	menuTypeItemListValues, &setupCC8);
//cMenuItem melCC9(menuCCOut, 		8, "CC 9", 	menuTypeItemListValues, &setupCC9);
//cMenuItem melCC10(menuCCOut, 		9, "CC 10", menuTypeItemListValues, &setupCC10);

///////////
const strItemTypeActionButton updateFirmwareSetup 	{ firmwareUpgradeActivate, firmwareUpgradeDeactivate };
const strItemTypeLabel currentVerisonSetup 			{ interfaceGlobals.currentFirmwareVersion };

cMenuItem melUpdateFirmware	(menuFirmware, 	0, "Firmware Update", 	menuTypeItemActionButton, &updateFirmwareSetup);
cMenuItem melCurrentVersion	(menuFirmware, 	1, "Current Version", 	menuTypeItemLabel, &currentVerisonSetup);

/////////
const strItemTypeActionButton manual1Setup 			{ openManual1Action, nullptr};
const strItemTypeActionButton manual2Setup 			{ openManual2Action, nullptr};
const strItemTypeActionButton manual3Setup 			{ openManual3Action, nullptr};
const strItemTypeActionButton manual4Setup 			{ openManual4Action, nullptr};
const strItemTypeActionButton manual5Setup 			{ openManual5Action, nullptr};
const strItemTypeActionButton manual6Setup 			{ openManual6Action, nullptr};
const strItemTypeActionButton manual7Setup 			{ openManual7Action, nullptr};
const strItemTypeActionButton manual8Setup 			{ openManual8Action, nullptr};
const strItemTypeActionButton manual9Setup 			{ openManual9Action, nullptr};
const strItemTypeActionButton manual10Setup 		{ openManual10Action, nullptr};
const strItemTypeActionButton manual11Setup 		{ openManual11Action, nullptr};
const strItemTypeActionButton manual12Setup 		{ openManual12Action, nullptr};
const strItemTypeActionButton manual13Setup 		{ openManual13Action, nullptr};
const strItemTypeActionButton manual14Setup 		{ openManual14Action, nullptr};
const strItemTypeActionButton manual15Setup 		{ openManual15Action, nullptr};

cMenuItem melOpenManual1		(menuManual, 0,  &manualFolderNames[0][0], menuTypeItemActionButton, &manual1Setup);
//cMenuItem melOpenManual2		(menuManual, 1,  &manualFolderNames[1][0], menuTypeItemActionButton, &manual2Setup);
//cMenuItem melOpenManual3		(menuManual, 2,  &manualFolderNames[2][0], menuTypeItemActionButton, &manual3Setup);
//cMenuItem melOpenManual4		(menuManual, 3,  &manualFolderNames[3][0], menuTypeItemActionButton, &manual4Setup);
//cMenuItem melOpenManual5		(menuManual, 4,  &manualFolderNames[4][0], menuTypeItemActionButton, &manual5Setup);
//cMenuItem melOpenManual6		(menuManual, 5,  &manualFolderNames[5][0], menuTypeItemActionButton, &manual6Setup);
//cMenuItem melOpenManual7		(menuManual, 6,  &manualFolderNames[6][0], menuTypeItemActionButton, &manual7Setup);
//cMenuItem melOpenManual8		(menuManual, 7,  &manualFolderNames[7][0], menuTypeItemActionButton, &manual8Setup);
//cMenuItem melOpenManual9		(menuManual, 8,  &manualFolderNames[8][0], menuTypeItemActionButton, &manual9Setup);
//cMenuItem melOpenManual10		(menuManual, 9,  &manualFolderNames[9][0], menuTypeItemActionButton, &manual10Setup);
//cMenuItem melOpenManual11		(menuManual, 10, &manualFolderNames[10][0], menuTypeItemActionButton, &manual11Setup);
//cMenuItem melOpenManual12		(menuManual, 11, &manualFolderNames[11][0], menuTypeItemActionButton, &manual12Setup);
//cMenuItem melOpenManual13		(menuManual, 12, &manualFolderNames[12][0], menuTypeItemActionButton, &manual13Setup);
//cMenuItem melOpenManual14		(menuManual, 13, &manualFolderNames[13][0], menuTypeItemActionButton, &manual14Setup);
//cMenuItem melOpenManual15		(menuManual, 14, &manualFolderNames[14][0], menuTypeItemActionButton, &manual15Setup);

///////////
const strItemTypeActionButton creditsSetup 		{ openCreditsAction, nullptr};

cMenuItem melOpenCredits	(menuCredits, 	0, "Show Credits", 			menuTypeItemActionButton, &creditsSetup);



//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================
//=====================================================================================================================================

hMenuItem activeSubmenu;

void cConfigEditor::createConfigMenu()
{
	activeSubmenu = menuBase.getSelChild();

	createMenuBaseList();
	refreshConfigMenu();
}

void cConfigEditor::hideConfigMenu()
{

	display.setControlHide(configLabel);

	display.setControlHide(configListControl);
	display.setControlHide(configBasemenuListControl);
	display.setControlHide(configSubmenuListControl);
	display.setControlHide(frameControl);

	display.refreshControl(frameControl);
}

void cConfigEditor::createMenuBaseList()
{
	basemenuList.linesCount = 14;
	basemenuList.start = menuBase.getSelectedItem();
	basemenuList.length = menuBase.getCount();
	basemenuList.data = menuBase.getNames();

	display.setControlData(configBasemenuListControl, &basemenuList);
	display.refreshControl(configBasemenuListControl);
	display.setControlShow(configBasemenuListControl);
}

void cConfigEditor::reloadSubmenu()
{
	cMenuGroup* submenu = (cMenuGroup*)activeSubmenu;

	submenuList.linesCount = 14;
 	submenuList.start = submenu->getSelectedItem();
	submenuList.length = submenu->getCount();
	submenuList.params = submenu->getNames();
	submenuList.values = submenu->getValues();
	submenuList.valueEditActive = 0;

	if(submenu->parentMenu != &menuBase) 	changeLabelText(2, "Back");
	else 									changeLabelText(2, "");

	display.setControlData(configSubmenuListControl, &submenuList);
	display.refreshControl(configSubmenuListControl);
	display.setControlShow(configSubmenuListControl);
}


void cConfigEditor::loadConfigTextList(strItemTypeListText* itemSetup)
{
	changeLabelText(5, "Apply");
	changeLabelText(2, "Cancel");

	showConfigList4(2, *itemSetup->value, itemSetup->count, (char**)itemSetup->ptrText);

}

void cConfigEditor::loadConfigValuesList(strItemTypeListValues* itemSetup)
{
	changeLabelText(5, "Apply");
	changeLabelText(2, "Cancel");

	showConfigList4Val(itemSetup->value, itemSetup->min, itemSetup->max, itemSetup->interval, itemSetup->dev);
}

// wyjdz do menu wyzszego poziomu
bool cConfigEditor::menuGoOut()
{
	// tylko jesli nadrzede menu jest inne niz bazowe
	if(((cMenuGroup*)activeSubmenu)->parentMenu != &menuBase)
	{
		activeSubmenu = ((cMenuGroup*)activeSubmenu)->parentMenu;

		refreshConfigMenu();

		return true;
	}

	return false;
}

// wejdz do nizszego podmenu
void cConfigEditor::menuGoIn()
{
	// wykonaj akcje na zaznaczonej pozycji wybranej listy
	if(((cMenuGroup*)activeSubmenu)->type == menuTypeGroup)
	{
		// jesli to kolejna grupa to ustaw ja jako aktywna i odswiez
		if(((cMenuGroup*)activeSubmenu)->getSelChild()->type == menuTypeGroup)
		{
			activeSubmenu = ((cMenuGroup*)activeSubmenu)->getSelChild();

			refreshConfigMenu();
		}
		// jesli to element ostateczny przejdz do jego edycji/akcji
		else if(((cMenuGroup*)activeSubmenu)->getSelChild()->type == menuTypeItem)
		{
			((cMenuGroup*)activeSubmenu)->executeItem();
		}
	}
}


//source = 0 - encoder, = 1 - klawisze pod ekranem
void cConfigEditor::changeMenuListPosition(uint8_t list, int16_t value, uint8_t source)
{
	//sprawdzanie czy taka lista wogole dostepna
	if(list == 1 && menuBase.getSelChild()->type != menuTypeGroup) return;

	// wykownywanie akcji na zjechanie/ukrycie itemu listy ActionButton
	uint8_t itemPositionChanged = 0;
	void (*action_funct)(void) = nullptr;
	cMenuItem* selectedMenuItem = nullptr;

	if(itemEditorShown && list == 0)
	{
		itemEditorClose();

	}

	if(list == 0)
	{
		uint8_t start_position = menuBase.selectedItem;

		if(menuBase.selectedItem + value < 0) menuBase.selectedItem  = 0;
		else if(menuBase.selectedItem + value > menuBase.childsCount-1) menuBase.selectedItem = menuBase.childsCount-1;
		else menuBase.selectedItem += value;

		if(start_position != menuBase.selectedItem) itemPositionChanged++;

		activeSubmenu = menuBase.getSelChild();

		display.setControlValue(configBasemenuListControl, menuBase.selectedItem);
		display.refreshControl(configBasemenuListControl);
	}
	else if(list == 1)
	{
		cMenuGroup* submenu = (cMenuGroup*)activeSubmenu;

		uint8_t start_position = submenu->selectedItem;

		uint8_t* item_selected = &submenu->selectedItem;
		uint8_t item_max = submenu->childsCount - 1;

		if(*item_selected + value < 0) *item_selected  = 0;
		else if(*item_selected + value > item_max) *item_selected = item_max;
		else *item_selected += value;

		display.setControlValue(configSubmenuListControl, submenu->selectedItem);
		display.refreshControl(configSubmenuListControl);

		if(start_position !=  submenu->selectedItem) itemPositionChanged++;
	}


	if(itemPositionChanged)
	{
		// odswiezenie menu
		if(selectedMenuItem != nullptr) selectedMenuItem->resetValueEditState();

		refreshConfigMenu();

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

void cConfigEditor::refreshConfigMenu()
{
	if(menuBase.type == menuTypeItem)
	{
		hideSubmenu();
		return;
	}
	else if(menuBase.type == menuTypeGroup)
	{
		reloadSubmenu();
		showSubmenu();

		// ustawianie etykiety przycisku edycji/akcji
		if(((cMenuGroup*)activeSubmenu)->getSelChild()->type == menuTypeItem)
		{
			cMenuItem* selected_child = (cMenuItem*)((cMenuGroup*)activeSubmenu)->getSelChild();

			setLabelByMenuItemType(5, selected_child->getItemType(), selected_child->getValueEditState());
		}
		else
		{
			changeLabelText(5, "Change"); //xxx wejscie do podmenu
		}
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

// edytowanie wartosci elementu menu
void cConfigEditor::itemEditorOpen(menu_item_t type, const char* title, const void* setup)
{
	hideSubmenu();

	itemEditorShown = 1;

	showConfigItemLabel(title);

	switch(type)
	{
		case menuItemTypeValueU8:
		{
			configEditor.loadConfigValuesList((strItemTypeListValues*)setup);
			break;
		}
		case menuTypeItemListValues:
		{
			configEditor.loadConfigValuesList((strItemTypeListValues*)setup);
			break;
		}
		case menuTypeItemListText:
		{
			configEditor.loadConfigTextList((strItemTypeListText*)setup);
			break;
		}
		case menuTypeItemListTextWithAction:
		{
			configEditor.loadConfigTextList((strItemTypeListText*)setup);
			break;
		}
		case menuTypeItemActionButton:
		{

			break;
		}

		default:
			break;
	}

}

void cConfigEditor::itemEditorClose()
{
	itemEditorShown = 0;
	hideConfigList();
	hideConfigItemLabel();
	refreshConfigMenu();

	changeLabelText(5,"Change");

	cMenuItem* selected_child = (cMenuItem*)((cMenuGroup*)activeSubmenu)->getSelChild();
	switch(selected_child->getItemType())
	{
		case menuTypeItemActionButton:
		{
			if(((strItemTypeActionButton*)selected_child->getItemSetup())->funct2 != nullptr)
			{
				((strItemTypeActionButton*)selected_child->getItemSetup())->funct2();
			}
			break;
		}

		default:
			break;
	}

}

void cConfigEditor::itemEditorApply()
{
	configListConfirm(configEditor.selectedConfigListPosition);

	itemEditorClose();
}

void cConfigEditor::itemEditorChangeValue(int16_t value)
{
	changeConfigListPosition(value);
}


void cConfigEditor::configListConfirm(uint8_t list_pos)
{
	if(((cMenuGroup*)activeSubmenu)->getSelChild()->type != menuTypeItem)
	{
		return;
	}

	cMenuItem* selected_child = (cMenuItem*)((cMenuGroup*)activeSubmenu)->getSelChild();

	switch(selected_child->getItemType())
	{
		case menuItemTypeValueU8:
		{

			break;
		}
		case menuTypeItemListValues:
		{
			strItemTypeListValues* temp_str = (strItemTypeListValues*)selected_child->getItemSetup();
			if(list_pos <= temp_str->max-temp_str->min) *(temp_str->value) = temp_str->min+list_pos;
			break;
		}
		case menuTypeItemListText:
		{
			strItemTypeListText* temp_str = (strItemTypeListText*)selected_child->getItemSetup();
			if(list_pos < temp_str->count) *(temp_str->value) = list_pos;
			break;
		}
		case menuTypeItemListTextWithAction:
		{
			strItemTypeListTextWithAction* temp_str = (strItemTypeListTextWithAction*)selected_child->getItemSetup();
			if(list_pos < temp_str->count) *(temp_str->value) = list_pos;
			temp_str->funct1();
			break;
		}
		case menuTypeItemActionButton:
		{

			break;
		}
		default: break;
	}

	saveConfigToEeprom();
}

// wykonywanie akcji/edycji na elemencie menu
void cMenuGroup::executeItem()
{
	if(childs[selectedItem]->type != menuTypeItem) return;

	cMenuItem* selected_child = (cMenuItem*)childs[selectedItem];

	switch(selected_child->itemType)
	{
		case menuItemTypeValueU8:
		case menuTypeItemListValues:
		case menuTypeItemListText:
		case menuTypeItemListTextWithAction:
			configEditor.itemEditorOpen(selected_child->itemType, selected_child->itemName, selected_child->itemSetup);
			break;
		case menuTypeItemLabel:

			break;
		case menuItemTypeEmpty:

			break;
		case menuTypeItemActionButton:
		{
			configEditor.itemEditorOpen(selected_child->itemType, selected_child->itemName, selected_child->itemSetup);

			if(((strItemTypeActionButton*)selected_child->itemSetup)->funct1 != nullptr)
			{
				((strItemTypeActionButton*)selected_child->itemSetup)->funct1();
			}

			break;
		}

		default: break;
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
