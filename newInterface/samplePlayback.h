#ifndef NEWINTERFACE_SAMPLEPLAYBACK_H_
#define NEWINTERFACE_SAMPLEPLAYBACK_H_


#include "modulesBase.h"

#include "SD.h"

#include "mtSequencer.h"



class cSamplePlayback: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	cSamplePlayback() {}
	~cSamplePlayback() {}

	void showDefaultScreen();



//----------------------------------


//----------------------------------



	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl patternControl;








};

extern cSamplePlayback samplePlayback;



#endif /* NEWINTERFACE_SAMPLEPLAYBACK_H_ */
