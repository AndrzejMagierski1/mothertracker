#ifndef INTERFACE_PERFORMANCEMODE_H_
#define INTERFACE_PERFORMANCEMODE_H_


#include <modulesBase.h>

#include "mtStructs.h"



enum mtPerformanceMode
{
	mtPerformanceMaster,
	mtPerformanceFxes,

};

const uint8_t trackMasterModeCount = 2;

const char trackMasterLabels[trackMasterModeCount][5] =
{
	"On",
	"Off",
};


const uint8_t trackFxesCount = 12;
const char trackFxesLabels[trackFxesCount][20] =
{
	"Cutoff",
	"Start",
	"Loop 1",
	"Loop 2",
	"asd",
	"asd",
	"asd",
	"sad",
	"asd",
	"asd",
	"asd",
	"asd",
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

	hControl textLabel[12] = {nullptr};


	hControl frameControl = nullptr;



	uint8_t selectedPlace[2] = {0};



	// typ trybu/ekranu
	uint8_t mode = mtPerformanceMaster;

	uint8_t refreshMaster = 1;






};

extern cPerformanceMode performanceMode;



#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
