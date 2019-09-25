#ifndef INTERFACE_SONGEDITOR_H_
#define INTERFACE_SONGEDITOR_H_


#include <modulesBase.h>

#include "mtStructs.h"

#define CURR_PATTERN_MARK_POS		1

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
		topLabel[4] = {nullptr};
		bottomLabel[4] = {nullptr};

		patternsListControl = nullptr;
		barControl[4] = {nullptr};
		frameControl = nullptr;
	}
	virtual ~cSongEditor() {}

	void showDefaultScreen();

	void showPatternsList();


	void activateLabelsBorder();

	strList patternsList;

	strFrameData frameData;

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;
	hControl topLabel[7];
	hControl bottomLabel[7];

	hControl patternsListControl;
	hControl barControl[4];
	hControl frameControl;

	uint8_t slotToPattern[255];
	uint8_t selectedPlace = 0;
	uint8_t localSongPosition = UINT8_MAX;

	void setDefaultScreenFunct();

	void changePatternsSelection(int16_t value);

	void clearPatternMark();
	void markCurrentPattern();
	void readSong();

//--------------------------------------------------------------
// patterns
	void listPatterns();


	uint16_t songLength = 1;
	char patternsNamesList[SONG_MAX][28];
	char *patternNames[SONG_MAX];
	uint8_t selectedPattern = 0;




//--------------------------------------------------------------
//reverb


//--------------------------------------------------------------
//limiter


//--------------------------------------------------------------

};





extern cSongEditor songEditor;




#endif /* INTERFACE_SONGEDITOR_H_ */
