#ifndef INTERFACE_SONGEDITOR_H_
#define INTERFACE_SONGEDITOR_H_


#include "core/modulesBase.h"

#include "mtStructs.h"


typedef enum
{
	iconPlay,
	iconLoop,
}icon_t;

typedef enum
{
	playerUp,
	playerDown,
	playerLeft,
	playerRight,
}player_direction_t;

typedef enum
{
	selDirNone = 0x00U,
	selDirLeft = 0x01U,
	selDirRight = 0x02U,
	selDirUp = 0x04U,
	selDirDown = 0x08U,

}select_direction_t;

class cSongEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cSongEditor()
	{

	}
	virtual ~cSongEditor() {}

	void showDefaultScreen();

	void showPatternsList();


	void activateLabelsBorder();

	void showTempoValue();

	localList_t patternsList;

	strFrameData frameData;
	strLabelData doubleArrow[2];

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;
	hControl label[8] = {nullptr};
	hControl barControl[2] = {nullptr};
	hControl songPlayerControl = NULL;

	//hControl patternsListControl = nullptr;
	hControl frameControl = nullptr;
	hControl bgLabel;
	hControl loadHorizontalBarControl = nullptr;

	uint8_t slotToPattern[255];
	uint8_t selectedPlace = 0;
	uint8_t localSongPosition = UINT8_MAX;
	uint8_t localPatternToPlay;

	void setDefaultScreenFunct();

	void changePatternsSelection(int16_t value);

	void markCurrentPattern(uint8_t forceRefresh);
	void readSong();

	void changeGlobalTempo(int16_t value);

//--------------------------------------------------------------
// patterns
	void listPatterns();


	uint16_t songLength = 1;
	char patternsNamesList[SONG_MAX][28];
	char *patternNames[SONG_MAX];
	uint8_t selectedPattern = 0;





//--------------------------------------------------------------
//  global params
	char globalTempoVal[10];
	char patternLengthVal[6];



//--------------------------------------------------------------

	//Icons
	int16_t loopPosition = -1;
	void showIcon(icon_t iconType,uint8_t position);
	void hideIcon();
	void handleEntryIcon();
	int16_t findSlotWithPattern();
//--------------------------------------------------------------

	void switchToNewPattern();

	uint8_t exitOnButtonRelease;

	player_data_t songPlayerData;
	uint32_t *patternBitmask;

	//uint8_t patternUsageTable[14];
	void refreshSongPlayerControl();
	void startSongPlayer();
	void stopSongPlayer();
	void refreshSongPlayer(uint8_t source);



	void walkOnSongPlayer(player_direction_t dir);
	void selectSpecificTrack(uint8_t track, uint8_t pattern);

	elapsedMillis songPlayerRefreshTimer;

	song_selection_t copyCurrentData;
	uint8_t isSomethingCopied = 0;
	uint8_t isCopyingInProgress = 0;
	uint8_t copyElementMax;
	uint8_t currentCopyElement;
	uint8_t currSelDirection;
	uint8_t currSelDirection2;

	uint8_t isBusy = 0;

	void showHorizontalBar(const char *text, uint32_t max, uint32_t current);
	void hideHorizontalBar();

	uint8_t isDeletingInProgress = 0;
	uint8_t deleteElementMax;
	uint8_t currentDeleteElement;
};





extern cSongEditor songEditor;




#endif /* INTERFACE_SONGEDITOR_H_ */
