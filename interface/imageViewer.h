#ifndef INTERFACE_IMAGEVIEWER_H_
#define INTERFACE_IMAGEVIEWER_H_


#include <modulesBase.h>



class cImageViewer: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cImageViewer()
	{

	}
	~cImageViewer() {}


	//----------------------------------
	void setDefaultScreenFunct();
	void showDefaultScreen();

	//----------------------------------


	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;


	hControl topLabel[8] = {nullptr};
	hControl bottomLabel[8] = {nullptr};

	hControl image = nullptr;

};

extern cImageViewer imageViewer;



#endif /* INTERFACE_IMAGEVIEWER_H_ */
