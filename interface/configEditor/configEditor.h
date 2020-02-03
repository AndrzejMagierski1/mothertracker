#ifndef INTERFACE_CONFIGEDITOR_H_
#define INTERFACE_CONFIGEDITOR_H_


#include "core/modulesBase.h"
#include "SD.h"



#include "mtStructs.h"



#include "configEditor/configMenuDefs.h"



#undef MAX_SELECT_NODES
#define MAX_SELECT_NODES	7

const uint8_t firmware_list_max=10;
const uint8_t firmware_name_length=32;


class cConfigEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cConfigEditor()
	{
		label[8] = {nullptr};
		barControl[8] = {nullptr};
		editorInstrument = nullptr;
		frameControl = nullptr;
		popoutWindowLabel = nullptr;

	}
	virtual ~cConfigEditor() {}


	//-----------------------------------------

	void showDefaultConfigScreen();
	void showMasterScreen();

	void turnOffPerformanceMode();
	void turnOffRadio();



	//menu
	void createConfigMenu();
	void createMenuBaseList();
	void reloadSubmenu();
	void loadConfigTextList(strItemTypeListText* itemSetup);
	void loadConfigValuesList(strItemTypeListValues* itemSetup);



	void changeMenuListPosition(uint8_t list, int16_t value);
	void executeSelectedListItem(uint8_t list);

	void configListConfirm(uint8_t list_pos);


	void refreshConfigMenu(uint8_t listChanged);
	void setLabelByMenuItemType(uint8_t label, menu_item_t type, uint8_t variant = 0);
	void setActualMenuAction(void (**actionFunct)(void), void** menu_item);

	void reloadSecondSubmenu();


	//

	//-----------------------------------------

	void setConfigScreenFunct();


	void activateLabelsBorder();

	//
	uint8_t flashingState = 0;
	uint8_t firmwareFoundNum;
	SdFile sdLocation;
	hControl popoutWindowLabel;



	uint8_t selectionActive;





	void showConfigList5(uint8_t start, uint8_t length, char** listText);
	void hideConfigList();

	void changeConfigListPosition(int16_t value);

	void showSubmenu();
	void hideSubmenu();
	void showSecondSubmenu();
	void hideSecondSubmenu();


	char* ptrfirmwareNamesList[firmware_list_max];
	char firmwareNamesList[firmware_list_max][firmware_name_length];
	void listAllFirmwares();
	void showFlashingWarning();
	void updateFirmware();;


	void showFirmwareUpdatePopout();
	void hideFirmwareUpdatePopout();

	void resizeLabelConfigDefault();
	void resizeLabelConfigMaster();
	void resizeFirmwareLabel(uint8_t control); // 0 - revert to normal, 1 - rescale to double label;

	uint8_t processUpdate = 0;

//----------------------------------

	strFrameData frameData;

	hControl label[8];
	hControl barControl[8];

	hControl configBasemenuListControl;
	hControl configSubmenuListControl;
	hControl configSecondSubmenuListControl;
	hControl configListControl;


	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;

	hControl bgLabel;
	hControl frameControl;


	uint8_t selectedPlace = 0;

	// typ trybu/ekranu
	uint8_t mode = 0;

	strInstrument * editorInstrument;

	strLabelData labelArrow[3];

//----------------------------------
// listy

	strList basemenuList;
	strParamValueList submenuList;
	strParamValueList secondSubmenuList;
	strList configList;



	uint8_t secondSubmenuShown = 0;


	uint8_t configListShown = 0;
	uint8_t selectedConfigListPosition = 0;


	uint8_t exitOnButtonRelease = 0;

	// MULTISEL
	select_node_t selectNodes[MAX_SELECT_NODES];




	void changeLabelText(uint8_t labelIdx, const char *text);





	void saveConfigToEeprom();


};

extern cConfigEditor configEditor;


//===========================================================================
//===========================================================================
//===========================================================================




void firmwareUpgradeActivate();
void firmwareUpgradeDeactivate();
















#endif /* INTERFACE_CONFIGEDITOR_H_ */
