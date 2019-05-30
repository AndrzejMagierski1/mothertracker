#ifndef NEWINTERFACE_INSTRUMENTEDITOR_H_
#define NEWINTERFACE_INSTRUMENTEDITOR_H_


#include "modulesBase.h"

#include "SD.h"

#include "mtSequencer.h"



class cInstrumentEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	cInstrumentEditor() {}
	~cInstrumentEditor() {}

	void showDefaultScreen();



//----------------------------------


//----------------------------------



	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl patternControl;








};

extern cInstrumentEditor instrumentEditor;



#endif /* NEWINTERFACE_INSTRUMENTEDITOR_H_ */
