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


	//config


	//menu
	void createConfigMenu();
	void createMenuBaseList();
	void ReloadSubmenu();
	void changeMenuListPosition(uint8_t list, int16_t value);
	void executeSelectedListItem(uint8_t list);
	//

	//-----------------------------------------

	void setConfigScreenFunct();


	void activateLabelsBorder();

	//
	uint8_t firmwareFoundNum;
	SdFile sdLocation;
	uint8_t listInitFlag=0;
	hControl popoutWindowLabel;

	uint8_t selectionActive;





	void showConfigList5(uint8_t start, uint8_t length, char** listText);
	void hideConfigList();
	void changeConfigListPosition(int16_t value);





	char* ptrfirmwareNamesList[firmware_list_max];
	char firmwareNamesList[firmware_list_max][firmware_name_length];
	void listAllFirmwares();
	void showFlashingWarning();
	void updateFirmware();;


	void showFirmwareUpdatePopout();
	void hideFirmwareUpdatePopout();

	void resizeLabelConfigDefault();
	void resizeLabelConfigMaster();
	void resizeFirmwareLabel(uint8_t control);// 0 - revert to normal, 1 - rescale to double label;

	uint8_t processUpdate = 0;

//----------------------------------

	strFrameData frameData;

	hControl label[8];
	hControl barControl[8];

	hControl configBasemenuListControl;
	hControl configSubmenuListControl;
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
	strList configList;


	uint8_t configListShown = 0;
	uint8_t selectedConfigListPosition = 0;


	uint8_t exitOnButtonRelease = 0;

	// MULTISEL
	select_node_t selectNodes[MAX_SELECT_NODES];

	void addNode(editFunct_t funct , uint8_t nodeNum);
	void removeNode(uint8_t nodeNum);
	void stepThroughNodes(int16_t value);
	void clearAllNodes();
	void cancelMultiFrame();

	uint8_t getListsActive();
	void setDataForLists(uint8_t listNum, uint8_t max);
	void listDataForLists(uint8_t listNum, uint8_t nameNum, const char *names);





	void processChangeInGroup0();
	void processChangeInGroup1();
	void processChangeInGroup2();
	void processChangeInGroup3();
	void chainProcessChange(uint8_t groupNum, uint8_t forceProcess);

	void resizeToDefaultConfig();
	void resizeToSmallConfig(uint8_t labelIdx);

	void showGeneralSubmenus(uint8_t listPosition);
	void showFirmwares(uint8_t listPosition);
	void showMidiSubmenus(uint8_t listPosition);
	void showFirmwareFlashButton();


	void showPatternDividers(uint8_t listPosition);
	void showRadioRegions(uint8_t listPosition);
	void showBrightnessLevels(uint8_t listPosition);

	void showClockIn(uint8_t listPosition);
	void showClockOut(uint8_t listPosition);
	void showTransportIn(uint8_t listPosition);
	void showTransportOut(uint8_t listPosition);
	void showCCouts(uint8_t listPosition);
	void showCCnumber(uint8_t listPosition);
	void showMidiNotesIn_mode(uint8_t listPosition);
	void showMidiNotesInChannels(uint8_t listPosition);
	void showMidiNotesOutChannels(uint8_t listPosition);

	void showMidiNotesOut_mode(uint8_t);




	//setters
	uint8_t setPatternDivider(uint32_t val);
	uint8_t setBrightness(uint32_t val);
	uint8_t setRadioRegion(uint32_t val);
	uint8_t setClockIn(uint32_t val);
	uint8_t setClockOut(uint32_t val);
	uint8_t setTransportIn(uint32_t val);
	uint8_t setTransportOut(uint32_t val);
	uint8_t setCCout(uint8_t ccNum, uint32_t val);

	uint8_t setNotesIn(uint32_t val);
	uint8_t setNotesInChannel(uint32_t val);
	uint8_t setNotesOutMode(uint32_t val);
	uint8_t setNotesOutChannel(uint32_t val);

	uint8_t hasConfigChanged = 0;


	void showExecute();



	void changeLabelText(uint8_t labelIdx, const char *text);

};

extern cConfigEditor configEditor;


//===========================================================================
//===========================================================================
//===========================================================================




void firmwareUpgradeActivate();
void firmwareUpgradeDeactivate();
















#endif /* INTERFACE_CONFIGEDITOR_H_ */
