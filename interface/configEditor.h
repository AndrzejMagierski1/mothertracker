#ifndef INTERFACE_CONFIGEDITOR_H_
#define INTERFACE_CONFIGEDITOR_H_


#include <modulesBase.h>
#include "SD.h"



#include "mtStructs.h"


enum mtConfigMode
{
	mtConfigModeDefault,
	mtConfigModeMaster,
	mtConfigModeMasterTracks,


};

enum mtConfigSpecific
{
	configDefaultGeneral,
	configDefaultAudio,
	configDefaultMIDI,
	configDefaultInterface,
	configDefaultSD,
	configDefaultFirmware,

};


const uint8_t firmware_list_max=10;
const uint8_t firmware_name_length=40;


const uint8_t groupCount = 6;
const char groupNamesLabels[groupCount][15] =
{
		"General",
		"Audio I/O",
		"MIDI",
		"Interface",
		"SD",
		"Firmware"

};




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
		selectedConfigGroup = 0;
		topLabel[8] = {nullptr};
		bottomLabel[8] = {nullptr};
		barControl[8] = {nullptr};
		configGroupsListControl = nullptr;
		editorInstrument = nullptr;
		frameControl = nullptr;
		firmwareListControl = nullptr;
		popoutWindowLabel = nullptr;

	}
	virtual ~cConfigEditor() {}


	//-----------------------------------------

	void showDefaultConfigScreen();
	void showMasterScreen();
	void showMasterTracksScreen();

	void changeSelectionInGroup(int16_t value);


	//config
	void showConfigGroupList();

	//master
	void showVolume();
	void showReverbSize();
	void showReverbDamping();
	void showLimiterAttack();
	void showLimiterRelease();
	void showLimiterTreshold();

	//master tracks



	//

	//-----------------------------------------

	void setConfigScreenFunct();
	void setMasterScreenFunct();
	void setMasterTracksScreenFunct();


	// config
	void changeConfigGroupSelection(int16_t value);

	//master
	void changeVolume(int16_t value);
	void changeReverbRoomSize(int16_t value);
	void changeReverbDamping(int16_t value);
	void changeLimiterAttack(int16_t value);
	void changeLimiterRelease(int16_t value);
	void changeLimiterTreshold(int16_t value);


	//master tracks



	//
	void activateLabelsBorder();

	// firmware
	uint8_t firmwareSelect;
	uint8_t firmwareFoundNum;
	FsFile sdLocation;
	uint8_t listInitFlag=0;
	char firmwareNamesList[firmware_list_max][firmware_name_length];
	char *firmwareNames[firmware_list_max];
	hControl firmwareListControl;
	hControl popoutWindowLabel;

	strList firmwareList;
	uint8_t selectionActive;
	void showFirmwareMenu();
	void hideFirmwareMenu();

	void showFirmwareUpdateLabels();
	void hideFirmwareUpdateLabels();

	void changeFirmwareSelection(int16_t value);

	void listAllFirmwares();

	void showWarning();
	void hideWarning();
	void showFirmwareUpdatePopout();
	void hideFirmwareUpdatePopout();



//----------------------------------

	strFrameData frameData;

	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl barControl[8];

	hControl configGroupsListControl;

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;

	hControl frameControl;


	uint8_t selectedPlace[3] = {0};

	// typ trybu/ekranu
	uint8_t mode = 0;

	strInstrument * editorInstrument;

//----------------------------------
// lista play mode
	strList configGroupList;

	void listConfigGroups();

	char *configGroupsNames[groupCount];

	uint8_t selectedConfigGroup;
	uint8_t previousSelectedConfigGroup=UINT8_MAX;

//----------------------------------

	char volumeVal[4];
	char reverbSizeVal[4];
	char reverbDampVal[4];
	char limitAttackVal[8];
	char limitReleaseVal[8];
	char limitThresholdVal[4];

//----------------------------------

	uint8_t exitOnButtonRelease = 0;
};

extern cConfigEditor configEditor;



#endif /* INTERFACE_CONFIGEDITOR_H_ */
