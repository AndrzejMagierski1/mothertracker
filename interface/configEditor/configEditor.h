#ifndef INTERFACE_CONFIGEDITOR_H_
#define INTERFACE_CONFIGEDITOR_H_


#include "core/modulesBase.h"
#include "SD.h"



#include "mtStructs.h"



#include "configEditor/configMenuDefs.h"



class cMenuGroup;
class cMenuItem;

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
		selectedConfigGroup[4] = {0};
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
	void showConfigGroupList(strList *data , uint8_t listNum);



	//menu

	void createConfigMenu();

	void createMenuBaseList(cMenuGroup& baseGroup);

	//

	//-----------------------------------------

	void setConfigScreenFunct();


	// config
	void changeConfigGroupSelection(int16_t value);

/*	//master
	void changeVolume(int16_t value);
	void changeReverbRoomSize(int16_t value);
	void changeReverbDamping(int16_t value);
	void changeLimiterAttack(int16_t value);
	void changeLimiterRelease(int16_t value);
	void changeLimiterTreshold(int16_t value);
	void changeBitDepth(int16_t value);*/


	//master tracks



	//
	void activateLabelsBorder();

	// firmware
	uint8_t firmwareFoundNum;
	SdFile sdLocation;
	uint8_t listInitFlag=0;
	hControl popoutWindowLabel;

	strList firmwareList;
	uint8_t selectionActive;
	void showFirmwareMenu();
	void hideFirmwareMenu();

	void listAllFirmwares();

	void showFlashingWarning();

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



	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;

	hControl bgLabel;
	hControl frameControl;


	uint8_t selectedPlace = 0;

	// typ trybu/ekranu
	uint8_t mode = 0;

	strInstrument * editorInstrument;

//----------------------------------
// listy

	strList basemenuList;
	strParamValueList submenuList;


	char *ptrBaseListNames;


	char firmwareNamesList[firmware_list_max][firmware_name_length];

	void listConfigGroups();

	char *configGroupsNames[4][255];

	uint8_t selectedConfigGroup[4];
	uint8_t selectedConfigGroupMax[4];
	uint8_t listsActive[4];
	uint8_t previousSelectedConfigGroup=UINT8_MAX;

//----------------------------------

	char volumeVal[4];
	char reverbSizeVal[4];
	char reverbDampVal[4];
	char limitAttackVal[8];
	char limitReleaseVal[8];
	char limitThresholdVal[4];
	char bitDepthVal[4];

//----------------------------------

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

typedef enum enMenuType
{
	menuTypeEmpty,
	menuTypeGroup,
	menuTypeItem,

} menu_t;

typedef enum enMenuItemType
{
	menuItemTypeEmpty,
	menuItemTypeValue,
	menuTypeItemList,
	menuTypeItemLabel,
	menuTypeItemButton,

} menu_item_t;














class cMenuBase
{
protected:
	cMenuBase(menu_t menu_type) : type(menu_type) {}
	menu_t type;

	friend cMenuGroup;
	friend cMenuItem;
};

typedef cMenuBase* hMenuItem;


class cMenuGroup : public cMenuBase
{
public:
	cMenuGroup(cMenuGroup& parent, uint8_t slot, const char* name, uint8_t childs_count) :
		cMenuBase(menuTypeGroup),
		childsCount(childs_count),
		childs(new hMenuItem[childs_count]),
		childsNames(new char*[childs_count]),
		groupName(name)
		{
			if(slot < parent.childsCount)
			{
				parent.childs[slot] = this;
				parent.childsNames[slot] = (char*)name;
			}
		}

	~cMenuGroup() { delete[] childs; }

	uint8_t getSelChildCount()
	{
		if(this->childs[selectedItem]->type == menuTypeGroup)
			return ((cMenuGroup*)childs[selectedItem])->childsCount;
		else return 0;
	}

	char** getSelChildNames()
	{
		if(this->childs[selectedItem]->type == menuTypeGroup)
			return ((cMenuGroup*)childs[selectedItem])->childsNames;
		else return nullptr;
	}


private:
	const uint8_t childsCount;
	hMenuItem* childs;
	char** childsNames;
	const char* groupName;


	uint8_t selectedItem = 0;

	friend class cMenuItem;
	friend class cConfigEditor;
};



class cMenuItem : public cMenuBase
{
public:
	cMenuItem(cMenuGroup& parent, uint8_t slot, const char* name) :
		cMenuBase(menuTypeItem),
		groupName(name)
		{
			if(slot < parent.childsCount)
			{
				parent.childs[slot] = this;
				parent.childsNames[slot] = (char*)name;
			}
		}

	~cMenuItem() {}

private:
	const char* groupName;
	//
	menu_item_t itemType;

	//value
	int value;
	int interval;
	int min;
	int max;

	//list
	uint8_t position;
	uint8_t itemsCount;
	char** ptrItems;

	//


};











#endif /* INTERFACE_CONFIGEDITOR_H_ */
