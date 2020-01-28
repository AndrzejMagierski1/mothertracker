
#include "configEditor/configEditor.h"




// baza baz
cMenuGroup menuBase(menuBase, 0, 0, 5);

// grupy glowne
cMenuGroup menuGeneral	(menuBase, 0, "General", 	3);
cMenuGroup menuMidi		(menuBase, 1, "MIDI", 		9);
cMenuGroup menuFirmware	(menuBase, 2, "Firmware", 	2);
cMenuGroup menuHelp		(menuBase, 3, "Help", 		1);
cMenuGroup menuCredits	(menuBase, 4, "Credits", 	1);


// elementy
const strItemTypeValue setupPatternDiv 		{ &mtConfig.general.patternDiv,  1, 1, 16 	 };
const strItemTypeListText setupRadioRegion	{ &mtConfig.general.radioRegion, 3, ptrRadioRegion };
const strItemTypeListText setupBrightness 	{ &mtConfig.general.brightness,  3, ptrBrightness  };

cMenuItem melPatternDiv	(menuGeneral, 		0, "Pattern Divider", 		menuItemTypeValue, 	  &setupPatternDiv);
cMenuItem melRadioReg	(menuGeneral, 		1, "Radio region",	 		menuTypeItemListText, &setupRadioRegion);
cMenuItem melDispBright	(menuGeneral, 		2, "Display Brightness", 	menuTypeItemListText, &setupBrightness);


const strItemTypeListText clockInSetup { &mtConfig.general.brightness,  3, ptrBrightness  };

cMenuItem melClockIn		(menuMidi, 		0, "Clock in", 				menuTypeItemListText, &clockInSetup);
cMenuItem melClockOut		(menuMidi, 		1, "Clock out",				menuTypeItemListText, &clockInSetup);
cMenuItem melTransportIn	(menuMidi, 		2, "Transport in", 			menuTypeItemListText, &clockInSetup);
cMenuItem melTransportOut	(menuMidi, 		3, "Transport out", 		menuTypeItemListText, &clockInSetup);
cMenuItem melCCOut			(menuMidi, 		4, "CC out", 				menuTypeItemListText, &clockInSetup);
cMenuItem melNotsInMode		(menuMidi, 		5, "Notes in mode", 		menuTypeItemListText, &clockInSetup);
cMenuItem melNotesInChannel	(menuMidi, 		6, "Notes in channnel", 	menuTypeItemListText, &clockInSetup);
cMenuItem melNotsOutMode	(menuMidi, 		7, "Notes out mode", 		menuTypeItemListText, &clockInSetup);
cMenuItem melNotesOutChannel(menuMidi, 		8, "Notes out channel", 	menuTypeItemListText, &clockInSetup);

cMenuItem melUpdateFirmware	(menuFirmware, 	0, "Update Firmware", 		menuTypeItemListText, &clockInSetup);
cMenuItem melCurrentVersion	(menuFirmware, 	1, "Current Version", 		menuTypeItemListText, &clockInSetup);

cMenuItem melOpenManual		(menuHelp, 		0, "Open Manual", 			menuTypeItemListText, &clockInSetup);

cMenuItem melOpenCredits	(menuCredits, 	0, "Credits", 				menuTypeItemListText, &clockInSetup);


//=======================================================================

void cConfigEditor::createConfigMenu()
{

	createMenuBaseList();






}

void cConfigEditor::createMenuBaseList()
{
	basemenuList.linesCount = menuBase.getCount();
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
	submenuList.linesCount = menuBase.getSelChild()->getCount();
	submenuList.start = menuBase.getSelChild()->getSelectedItem();
	submenuList.length = menuBase.getSelChild()->getCount();
	submenuList.params = menuBase.getSelChild()->getNames();
	submenuList.values = menuBase.getSelChild()->getNames();

	display.setControlData(configSubmenuListControl, &submenuList);
	display.refreshControl(configSubmenuListControl);
	display.setControlShow(configSubmenuListControl);
}





void cConfigEditor::chaangeListPosition(uint8_t list, int16_t value)
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








//
//cMenuBase::configMenuBase(uint8_t childsCount)
//{
//	if(childsCount > 0 && childs == nullptr)
//	{
//		childs = new hMenuUnit[childsCount];
//	}
//}
//
//cMenuBase::~configMenuBase()
//{
//	delete[] childs;
//};
