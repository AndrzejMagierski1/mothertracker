#ifndef INTERFACE_CONFIGEDITOR_H_
#define INTERFACE_CONFIGEDITOR_H_


#include <modulesBase.h>
#include "SD.h"



#include "mtStructs.h"






const uint8_t groupCount = 5;
const char groupNamesLabels[groupCount][15] =
{
		"General",
		"Audio I/O",
		"MIDI",
		"Interface",
		"SD",

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

	}
	virtual ~cConfigEditor() {}

	void showDefaultScreen();

	void showConfigGroupList();

	void setDefaultScreenFunct();

	void changeConfigGroupSelection(int16_t value);

	void activateLabelsBorder();


//----------------------------------

	strFrameData frameData;

	hControl topLabel[8];
	hControl bottomLabel[8];

	hControl configGroupsListControl;

	hControl frameControl;


	uint8_t selectedPlace = 0;




//----------------------------------
// lista play mode
	strList configGroupList;

	void listConfigGroups();

	char *configGroupsNames[groupCount];

	uint8_t selectedConfigGroup;

//----------------------------------

//----------------------------------



};

extern cConfigEditor configEditor;



#endif /* INTERFACE_CONFIGEDITOR_H_ */
