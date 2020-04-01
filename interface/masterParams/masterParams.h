#ifndef INTERFACE_MASTERPARAMS_H_
#define INTERFACE_MASTERPARAMS_H_


#include "core/modulesBase.h"
#include "SD.h"

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

	void showDefaultConfigScreen();
	void showMasterScreen();

	void turnOffPerformanceMode();
	void turnOffRadio();

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

	enum struct display_t
	{
		masterValues,
		mixer
	} displayType;

	uint8_t selectedPlaceDelay;
	bool isDelayScreen;
	//mixer
	void showMixerScreen();
	void showLevelBar(uint8_t n);
	void switchToMaster();
	void switchToMixer();
	void switchToDelayScreen();
	void calcTrackLevel(uint8_t n);

	struct strTrackLevel
	{
		float measureSum = 0;
		uint8_t measureCounter = 0;
		elapsedMicros timer = 0;
		elapsedMillis redColorTimer = 0;
		uint8_t value;
		uint8_t lastValue;

	} trackLevel[8];
	uint8_t isSolo = 0;
	int8_t soloTrack = -1;
	char mixerLabel[8][7];
	char * const mixerTrackLabel[8] =
	{
			(char*)"Track 1",
			(char*)"Track 2",
			(char*)"Track 3",
			(char*)"Track 4",
			(char*)"Track 5",
			(char*)"Track 6",
			(char*)"Track 7",
			(char*)"Track 8"
	};
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

	void changeDelayPingPongEnable(int16_t val);
	void changeDelaySyncEnable(int16_t val);
	void changeDelayRate(int16_t val);
	void changeDelayTime(int16_t val);
	void changeDelayFeedback(int16_t val);



	//
	void activateLabelsBorder();
	void refreshDelayFrame();



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

	const char * delaySyncRates[15] =
	{
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

	uint8_t selectedPlace = 0;

	// typ trybu/ekranu
	uint8_t mode = 0;

	strInstrument * editorInstrument;

//----------------------------------
//----------------------------------

	char volumeVal[4];
	char delayFeedbackVal[4];
	char delayTimeVal[9];
	char limitAttackVal[8];
	char limitReleaseVal[8];
	char limitThresholdVal[4];
	char bitDepthVal[4];

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

};

extern cMasterParams masterParams;



#endif /* INTERFACE_MASTERPARAMS_H_ */
