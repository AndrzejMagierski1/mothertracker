#ifndef INTERFACE_SONGEDITOR_H_
#define INTERFACE_SONGEDITOR_H_


#include <modulesBase.h>

#include "mtStructs.h"


typedef enum
{
	iconPlay,
	iconLoop,
}icon_t;

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
	void showPatternLengthValue();

	strList patternsList;

	strFrameData frameData;
	strBgLabelData bgLabelData;

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;
	hControl label[7] = {nullptr};
	hControl barControl[2] = {nullptr};

	hControl patternsListControl = nullptr;
	hControl frameControl = nullptr;
	hControl bgLabel;

	uint8_t slotToPattern[255];
	uint8_t selectedPlace = 0;
	uint8_t localSongPosition = UINT8_MAX;
	uint8_t localPatternToPlay;

	void setDefaultScreenFunct();

	void changePatternsSelection(int16_t value);

	void markCurrentPattern(uint8_t forceRefresh);
	void readSong();

	void changeGlobalTempo(int16_t value);
	void changeGlobalPatternLength(int16_t value);

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



};





extern cSongEditor songEditor;




#endif /* INTERFACE_SONGEDITOR_H_ */
