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

	void refreshImage();
	void countImages();
	void nextImage();
	void previouseImage();
	void setImage(uint8_t number);

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;

	char titleText[32];


	strFrameData frameData;
	hControl label[8] = {nullptr};
	hControl bgLabel;


	hControl image = nullptr;


	uint8_t imageNumber = 1;
	uint8_t imagesCount = 0;

	uint8_t refreshImageState = 0;
};

extern cImageViewer imageViewer;



#endif /* INTERFACE_IMAGEVIEWER_H_ */
