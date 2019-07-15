#ifndef INTERFACE_SONGEDITOR_H_
#define INTERFACE_SONGEDITOR_H_


#include <modulesBase.h>

#include "mtStructs.h"


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

	hControl topLabel[4];
	hControl bottomLabel[4];

	hControl patternsListControl;
	hControl barControl[4];
	hControl frameControl;


	uint8_t selectedPlace = 0;

	void setDefaultScreenFunct();

	void changePatternsSelection(int16_t value);

//--------------------------------------------------------------
// patterns
	void listPatterns();

	uint16_t songLength = 0;
	char patternsNamesList[PATTERNS_COUNT][35];
	char *patternNames[PATTERNS_COUNT];
	uint8_t selectedPattern = 0;

//--------------------------------------------------------------
//reverb


//--------------------------------------------------------------
//limiter


//--------------------------------------------------------------

};





extern cSongEditor songEditor;




#endif /* INTERFACE_SONGEDITOR_H_ */
