#ifndef INTERFACE_CONFIGEDITOR_H_
#define INTERFACE_CONFIGEDITOR_H_


#include "core/modulesBase.h"
#include "SD.h"



#include "mtStructs.h"



#include "configEditor/configMenuDefs.h"
#include "mtGridEditor.h"


#undef MAX_SELECT_NODES
#define MAX_SELECT_NODES	7

const uint8_t firmware_list_max=100;
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
		editorInstrument = nullptr;
		frameControl = nullptr;
		popoutWindowLabel = nullptr;
	}
	virtual ~cConfigEditor() {}


	//-----------------------------------------

	void showDefaultConfigScreen();
	void showMasterScreen();

	//void turnOffPerformanceMode();
	//void turnOffRadio();



	//menu
	void createConfigMenu();
	void hideConfigMenu();

	void createMenuBaseList();
	void reloadSubmenu();
	void loadConfigTextList(strItemTypeListText* itemSetup);
	void loadConfigValuesList(strItemTypeListValues* itemSetup);



	void changeMenuListPosition(uint8_t list, int16_t value, uint8_t source = 0);
	void executeSelectedListItem(uint8_t list);
	void chanegeItemValue(void* selectedMenuItem, int16_t value);

	void configListConfirm(uint8_t list_pos);


	void refreshConfigMenu(uint8_t listChanged);
	void setLabelByMenuItemType(uint8_t label, menu_item_t type, uint8_t variant = 0);
	void getSelectedItemInfo(void (**actionFunct)(void), void** menu_item, uint8_t* item_level);

	void reloadSecondSubmenu();

	void setGridScreenFunction();
	void setPadScreenFunction();
	void reloadPadScreenDisplayedValue(uint8_t value);

	void showGridScreen();
	void refreshGridScreen();
	void hideGridScreen();
	void showPadScreen();
	void hidePadScreen();


	void refreshPadScreenValue(uint8_t value);


	//

	//-----------------------------------------

	void setConfigScreenFunct();


	void activateLabelsBorder();
	void selectSubmenu();
	void selectSecondSubmenu();
	void selectConfigList(uint8_t width);

	//
	uint8_t flashingState = 0;
	uint8_t firmwareFoundNum;
	SdDir sdLocation;
	hControl popoutWindowLabel;


	uint8_t configPopupActive = 0;



	void showConfigList5(uint8_t list_width, uint8_t start, uint8_t length, char** listText);
	void hideConfigList();

	void changeConfigListPosition(int16_t value);

	void showSubmenu();
	void hideSubmenu();
	void showSecondSubmenu();
	void hideSecondSubmenu();


	char** ptrfirmwareNamesList;
	//char firmwareNamesList[firmware_list_max][firmware_name_length];
	void listAllFirmwares();
	void showFlashingWarning();
	void updateFirmware();
	void cancelUpdateFirmware();

	void showFirmwareUpdatePopout();
	void hideFirmwareUpdatePopout();

	void resizeLabelConfigDefault();
	void resizeLabelConfigMaster();
	void resizeFirmwareLabel(uint8_t control); // 0 - revert to normal, 1 - rescale to double label;





	uint8_t processUpdate = 0;

//----------------------------------

	strFrameData frameData;

	hControl label[8];

	hControl configBasemenuListControl;
	hControl configSubmenuListControl;
	hControl configSecondSubmenuListControl;
	hControl configListControl;
	hControl textBox;

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl gridNameLabel = nullptr;
	hControl instrumentLabel = nullptr;

	hControl bgLabel;
	hControl frameControl;
	hControl gridPadsControl = nullptr;

	hControl padBar[2] = {nullptr, nullptr};
	hControl padList = nullptr;

	uint8_t selectedPlace = 0;
	uint8_t selectedPlacePadScreen = 0;
	// typ trybu/ekranu
	uint8_t mode = 0;

	strInstrument * editorInstrument;

	strLabelData labelArrow[3];

	strTextBoxData textBoxData;

	strPadNames padNamesStruct;

	strList padListData;

	uint8_t padScreenDisplayedValue[3];
	char microtuneValue[6];
	char padScreenTitleLabelName[20];

	const char* padListNames[2] =
	{
			(char*)"On",
			(char*)"Off"
	};

//----------------------------------
// listy

	strList basemenuList;
	strParamValueList submenuList;
	strParamValueList secondSubmenuList;
	strList configList;



	uint8_t submenuShown = 0;
	uint8_t secondSubmenuShown = 0;


	uint8_t configListShown = 0;
	uint8_t selectedConfigListPosition = 0;

	uint8_t updatePopupShown = 0;


	uint8_t exitOnButtonRelease = 0;

	// MULTISEL
	select_node_t selectNodes[MAX_SELECT_NODES];




	void changeLabelText(uint8_t labelIdx, const char *text);


	//---------- credits ------------

	void openCredits();
	void closeCredits();
	void showCreditsControls();
	void hideCreditsControls();



	///-------------------------------




	void saveConfigToEeprom();


};

extern cConfigEditor configEditor;


//===========================================================================
//===========================================================================
//===========================================================================




void firmwareUpgradeActivate();
void firmwareUpgradeDeactivate();
















#endif /* INTERFACE_CONFIGEDITOR_H_ */
