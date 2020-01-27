
#include "configEditor/configEditor.h"

#include "configEditor/configMenuDefs.h"


// baza baz
cMenuGroup menuBase(menuBase, 0, 0, 6);

// grupy glowne
cMenuGroup menuGeneral	(menuBase, 0, "General", 	3);
cMenuGroup menuMidi		(menuBase, 1, "MIDI", 		7);
cMenuGroup menuFirmware	(menuBase, 2, "Firmware", 	3);
cMenuGroup menuHelp		(menuBase, 3, "Help", 		0);
cMenuGroup menuCredits	(menuBase, 4, "Credits", 	0);

// elementy
cMenuItem melPatternDiv	(menuGeneral, 0, "Pattern Divider");
cMenuItem melRadioReg	(menuGeneral, 1, "Radio region");
cMenuItem melDispBright	(menuGeneral, 2, "Display Brightness");



void cConfigEditor::createConfigMenu()
{


	createMenuBaseList(menuBase);






}

void cConfigEditor::createMenuBaseList(cMenuGroup& baseGroup)
{


	basemenuList.linesCount = baseGroup.childsCount;
	basemenuList.start = 0;
	basemenuList.length = baseGroup.childsCount;
	basemenuList.data = baseGroup.childsNames;

	display.setControlData(configBasemenuListControl, &basemenuList);
	display.refreshControl(configBasemenuListControl);
	display.setControlShow(configBasemenuListControl);





	submenuList.linesCount = baseGroup.getSelChildCount();
	submenuList.start = 0;
	submenuList.length = baseGroup.getSelChildCount();
	submenuList.params = baseGroup.getSelChildNames();
	submenuList.values = baseGroup.getSelChildNames();

	display.setControlData(configSubmenuListControl, &submenuList);
	display.refreshControl(configSubmenuListControl);
	display.setControlShow(configSubmenuListControl);


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
