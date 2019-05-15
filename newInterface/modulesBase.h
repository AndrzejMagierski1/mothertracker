#ifndef NEWINTERFACE_MODULESBASE_H_
#define NEWINTERFACE_MODULESBASE_H_


#include <stdint.h>





class cModuleBase
{
public:

	virtual void update() = 0;
	virtual void start(uint32_t options) = 0;
	virtual void stop() = 0;

	void setEventFunct(void (*func)(uint8_t, void*, void*, void*)) { eventFunct = func; };



private:

	void (*eventFunct)(uint8_t, void*, void*, void*);




};














#endif /* NEWINTERFACE_MODULESBASE_H_ */
