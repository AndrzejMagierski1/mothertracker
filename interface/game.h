
#ifndef GAME_H_
#define GAME_H_


#include <modulesBase.h>
#include "SD.h"



class cGameModule: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls() {}
	virtual void destroyDisplayControls() {}

	cGameModule()
	{



	}
	virtual ~cGameModule() {}

//----------------------------------

	void setDefaultScreenFunct();


//----------------------------------


};

extern cGameModule gameModule;





#endif /* GAME_H_ */
