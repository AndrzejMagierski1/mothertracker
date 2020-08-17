#ifndef INTERFACE_MASTERPARAMS_H_
#define INTERFACE_MASTERPARAMS_H_


#include "core/modulesBase.h"
#include "SD.h"
#include "mtKeyboardManager.h"

#include "mtStructs.h"


#undef MAX_SELECT_NODES
#define MAX_SELECT_NODES	7


class cMasterParams: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cMasterParams()
	{
		label[8] = {nullptr};
		barControl[8] = {nullptr};
		editorInstrument = nullptr;
		frameControl = nullptr;

		delayPingpongEnableList = nullptr;
		delaySyncEnableList = nullptr;
		delaySyncRateList = nullptr;
	}
	virtual ~cMasterParams() {}


	//-----------------------------------------
	struct strTrackLevel;
	void showDefaultConfigScreen();
	void showMasterScreen();


	void changeSelectionInGroup(int16_t value, uint8_t groupNum);



	//master
	void showVolume();
	void showLimiterAttack();
	void showLimiterRelease();
	void showLimiterTreshold();
	void showBitDepth();

	//delay
	void showDelayScreen();
	void showDelayPingPongEnable();
	void showDelaySyncEnable();
	void showDelayRate();
	void showDelayFeedback();
	void showDelayTime();

	//reverb
	void showReverbScreen();
	void showReverbSize();
	void showReverbDamp();
	void showReverbPredelay();
	void showReverbDiffusion();

	enum struct display_t
	{
		masterValues,
		mixer,
		mixerDelayReverb
	} displayType;

	enum
	{
		delayVolumeScreenPosition,
		reverbVolumeScreenPosition,
		screenPositionCount
	};

	enum
	{
		delayIsSolo = 10,
		reverbIsSolo
	};

	uint8_t selectedPlaceDelay;
	uint8_t selectedPlaceReverb;
	bool isDelayScreen;
	bool isReverbScreen;
	//mixer
	void showMixerScreen();
	void showMixerDelayReverbScreen();
	void showLevelBar(uint8_t n, strTrackLevel * level );
	void showTrackVolumeBar(uint8_t n);
	void showDelayVolumeBar();
	void showReverbVolumeBar();
	void switchToMaster();
	void switchToMixer();
	void switchToMixerDelayReverb();
	void switchToDelayScreen();
	void switchToReverbScreen();
	void calcLevel(strTrackLevel * level, float value);
	void calcTrackLevel(uint8_t n);
	void calcDelayLevel();
	void calcReverbLevel();
	struct strTrackLevel
	{
		float measureSum = 0;
		uint8_t measureCounter = 0;
		elapsedMicros timer = 0;
		elapsedMillis redColorTimer = 0;
		uint8_t value;
		uint8_t lastValue;

	} trackLevel[8], reverbLevel, delayLevel;
	uint8_t isSolo = 0;
	int8_t soloTrack = -1;
	char mixerLabel[8][7];
	char mixerVolumeLabel[8][5];
	char delayVolumeLabel[5];
	char reverbVolumeLabel[5];

	bool trackIsEdited[8];
	bool ignoreMuteRelease[8];

	bool reverbIsEdited;
	bool ignoreMuteReleaseReverb;
	bool delayIsEdited;
	bool ignoreMuteReleaseDelay;

	//menu

	//

	//-----------------------------------------

	void setMasterScreenFunct();
	void setDelayScreenFunct();
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
// delay

	void changeDelayPingPongEnable(int16_t val, bool rollListOver = false);
	void changeDelaySyncEnable(int16_t val, bool rollListOver = false);
	void changeDelayRate(int16_t val, bool rollListOver = false);
	void changeDelayTime(int16_t val);
	void changeDelayFeedback(int16_t val);

	void setDefaultDelayPingPongEnable();
	void setDefaultDelaySyncEnable();
	void setDefaultDelayRate();
	void setDefaultDelayTime();
	void setDefaultDelayFeedback();

// reverb
	void changeReverbSize(int16_t val);
	void changeReverbDamp(int16_t val);
	void changeReverbPredelay(int16_t val);
	void changeReverbDiffusion(int16_t val);
// Mixer
	void changeTrackVolume(int16_t val, uint8_t track);
	void setDefaultTrackVolume(uint8_t track);
// Mixer Delay Reverb
	void changeReverbVolume(int16_t val);
	void changeDelayVolume(int16_t val);
	void setDefaultReverbVolume();
	void setDefaultDelayVolume();



	void setDefaultReverbSize();
	void setDefaultReverbDamp();
	void setDefaultReverbPredelay();
	void setDefaultReverbDiffusion();
	//
	void activateLabelsBorder();
	void refreshDelayFrame();
	void refreshReverbFrame();



	uint8_t processUpdate = 0;

//----------------------------------

	strFrameData frameData;

	strList pingpongEnableData;
	strList syncEnableData;
	strList syncRateData;

	const char * delayPingPongEnable[2] =
	{
			(char *) "On",
			(char *) "Off"
	};

	const char * delaySyncEnable[2] =
	{
			(char *) "On",
			(char *) "Off"
	};

	const char * delaySyncRates[19] =
	{
			(char *) "8",
			(char *) "6",
			(char *) "4",
			(char *) "3",
			(char *) "2",
			(char *) "3/2",
			(char *) "1",
			(char *) "3/4",
			(char *) "1/2",
			(char *) "3/8",
			(char *) "1/3",
			(char *) "1/4",
			(char *) "3/16",
			(char *) "1/6",
			(char *) "1/8",
			(char *) "1/12",
			(char *) "1/16",
			(char *) "1/24",
			(char *) "1/32",
	};

	hControl label[8];
	hControl barControl[8];

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;

	hControl bgLabel;
	hControl frameControl;

	hControl delayPingpongEnableList;
	hControl delaySyncEnableList;
	hControl delaySyncRateList;
	hControl keyboardControl;
	hControl editName;
	hControl trackNameLabel[8];
	hControl trackVolumeBar[8];

	uint8_t selectedPlace = 0;


	uint8_t sourceModuleButton = 0;

	// typ trybu/ekranu
	uint8_t mode = 0;

	strInstrument * editorInstrument;

	mtKeyboardManager keyboardManager;
	uint8_t editTrackNameMode = 0;
	uint8_t actualEditingTrackName = 0;
//----------------------------------
//----------------------------------

	char volumeVal[4];
	char delayFeedbackVal[4];
	char delayTimeVal[9];
	char limitAttackVal[8];
	char limitReleaseVal[8];
	char limitThresholdVal[4];
	char bitDepthVal[4];

	char reverbSizeVal[4];
	char reverbDampVal[4];
	char reverbPredelayVal[4];
	char reverbDiffusionVal[4];

//----------------------------------

	uint8_t exitOnButtonRelease = 0;

	uint8_t getListsActive();
	void setDataForLists(uint8_t listNum, uint8_t max);
	void listDataForLists(uint8_t listNum, uint8_t nameNum, const char *names);


	void processChangeInGroup0();
	void processChangeInGroup1();
	void processChangeInGroup2();
	void processChangeInGroup3();
	void chainProcessChange(uint8_t groupNum, uint8_t forceProcess);

	void resizeToDefaultConfig();
	void resizeToDefaultMaster();
	void resizeBarControl(display_t val);
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
	void showMidiNotesFrom(uint8_t listPosition);
	void showMidiNotesChannels(uint8_t listPosition);

	void changeLabelText(uint8_t labelIdx, const char *text);


	// nzawy trackow
	void editTrackName(uint8_t button);
	void setKeyboardExportFunctions();
	void showKeyboardExport();
	void showEditTracksNamesMode();
};

extern cMasterParams masterParams;



#endif /* INTERFACE_MASTERPARAMS_H_ */
