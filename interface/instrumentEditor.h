#ifndef INTERFACE_INSTRUMENTEDITOR_H_
#define INTERFACE_INSTRUMENTEDITOR_H_


#include <modulesBase.h>
#include "SD.h"

#include "mtSequencer.h"



class cInstrumentEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cInstrumentEditor() {}
	~cInstrumentEditor() {}

	void showDefaultScreen();


	void setDefaultScreenFunct();

//----------------------------------


//----------------------------------



	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl patternControl;








};

extern cInstrumentEditor instrumentEditor;



#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
