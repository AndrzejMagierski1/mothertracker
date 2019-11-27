

#include <imageViewer.h>


void cImageViewer::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	//prop2.style = 	( controlStyleShow | controlStyleRightX | controlStyleCenterY);
	//prop2.x = 769;
	//if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
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
		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop);

		//prop.y = 437;
		//prop.h = 28;
		//if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop);
	}



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
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;

		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}

	display.destroyControl(image);
	image = nullptr;
}


void cImageViewer::showDefaultScreen()
{
	// bottom labels
	display.setControlText(bottomLabel[0], "<<");
	display.setControlText(bottomLabel[1], "");
	display.setControlText(bottomLabel[2], "");
	display.setControlText(bottomLabel[3], "");
	display.setControlText(bottomLabel[4], "");
	display.setControlText(bottomLabel[5], "");
	display.setControlText(bottomLabel[6], "");
	display.setControlText(bottomLabel[7], ">>");

	display.setControlText(titleLabel, "Manual");
	display.refreshControl(titleLabel);
	display.refreshControl(titleBar);


	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(bottomLabel[i]);
		//display.refreshControl(topLabel[i]);
	}

	display.synchronizeRefresh();

}




//====================================================================================================


