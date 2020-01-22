

#include "imageViewer/imageViewer.h"


void cImageViewer::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY | controlStyleFont4);
	prop2.x = 9;
	prop2.y = 13;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	//prop2.style = 	( controlStyleShow | controlStyleRightX | controlStyleCenterY);
	//prop2.x = 769;
	//if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 2;
	prop2.y = 0;
	prop2.w = 795;
	prop2.h = 25;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);


	strControlProperties prop;
	// inicjalizacja kontrolek
	for(uint8_t i = 0; i<8; i++)
	{
		prop.text = (char*)"";
		prop.style = 	( controlStyleShow | controlStyleCenterX | controlStyleCenterY);
		prop.x = (800/8)*i+(800/16);
		prop.y = 452;
		prop.w = 800/8-6;
		prop.h = 59;
		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop);

		//prop.y = 437;
		//prop.h = 28;
		//if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop);
	}

	prop2.text = nullptr;
	prop2.colors = interfaceGlobals.activeBgLabelsColors;
	prop2.value = 255;
	prop2.style = controlStyleNoTransparency | controlStyleShow;
	prop2.x = 0;
	prop2.w = 800;
	prop2.y = 424;
	prop2.h =  55;
	if(bgLabel == nullptr) bgLabel = display.createControl<cBgLabel>(&prop2);



	prop.text = nullptr;
	prop.style = 0;
	prop.value = 300000;
	prop.x = 0;
	prop.y = 29;
	prop.w = 800;
	prop.h = 390;
	if(image == nullptr) image = display.createControl<cImg>(&prop);

}


void cImageViewer::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(instrumentLabel);
	instrumentLabel = nullptr;


	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;
	}

	display.destroyControl(bgLabel);
	bgLabel = nullptr;


	display.destroyControl(image);
	image = nullptr;
}


void cImageViewer::showDefaultScreen()
{
	// bottom labels
	display.setControlText(label[0], "<<");
	display.setControlText(label[1], "");
	display.setControlText(label[2], "");
	display.setControlText(label[3], "");
	display.setControlText(label[4], "");
	display.setControlText(label[5], "");
	display.setControlText(label[6], "");
	display.setControlText(label[7], ">>");

	display.setControlText(titleLabel, "Manual");
	display.refreshControl(titleLabel);
	display.refreshControl(titleBar);


	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(label[i]);
		//display.refreshControl(topLabel[i]);
	}

	display.refreshControl(bgLabel);


	display.synchronizeRefresh();

}




//====================================================================================================


