#ifndef INTERFACE_PERFORMANCEMODE_H_
#define INTERFACE_PERFORMANCEMODE_H_


#include <modulesBase.h>

#include "mtStructs.h"



enum mtPerformanceMode
{
	mtPerformanceMaster,
	mtPerformanceFxes,

};






class cPerformanceMode: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cPerformanceMode()
	{

	}
	~cPerformanceMode() {}





	void showDefaultScreen();

	void showPerformanceMaster();
	void showPerformanceFxes();







	void setDefaultScreenFunct();

	void setPerformanceMaster();
	void setPerformanceFxes();




	void lightUpPadBoard();


	void activateLabelsBorder();

//----------------------------------


	strFrameData frameData;

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;




	hControl topLabel[8] = {nullptr};
	hControl bottomLabel[8] = {nullptr};



	hControl frameControl = nullptr;



	uint8_t selectedPlace[2] = {0};



	// typ trybu/ekranu
	uint8_t mode = mtPerformanceMaster;








};

extern cPerformanceMode performanceMode;



#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
