#ifndef LIBS_NEWDISPLAY_LINEINDICATORCONTROL_H_
#define LIBS_NEWDISPLAY_LINEINDICATORCONTROL_H_
#include "progressCursorControl.h"



class cLineIndicator: public cProgressCursor
{
public:
	cLineIndicator(strControlProperties* properties);
	~cLineIndicator();
	virtual uint8_t update();
private:
};




#endif /* LIBS_NEWDISPLAY_LINEINDICATORCONTROL_H_ */
