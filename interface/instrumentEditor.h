#ifndef INTERFACE_INSTRUMENTEDITOR_H_
#define INTERFACE_INSTRUMENTEDITOR_H_


#include <modulesBase.h>
#include "SD.h"

#include "mtSequencer.h"

enum mtInstrumentEditorMode
{
	mtInstEditModeVolume,
	mtInstEditModeFilter,
	mtInstEditModeParams,

};

class cInstrumentEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cInstrumentEditor()
	{

	}
	~cInstrumentEditor() {}

	void showDefaultScreen();

	void showInstrumentVolume();
	void showInstrumentFilter();
	void showInstrumentParams();

	void activateLabelsBorder();


//----------------------------------

	void setDefaultScreenFunct();

	void setInstrumentVolumeFunct();
	void setInstrumentFilterFunct();
	void setInstrumentParamsFunct();


//----------------------------------

	strFrameData frameData;


	hControl titleLabel;
	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl barControl[8];

	hControl listControl;

	hControl frameControl;


	uint8_t selectedPlace = 0;


	uint8_t mode;

//----------------------------------


//----------------------------------






};

extern cInstrumentEditor instrumentEditor;



#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
