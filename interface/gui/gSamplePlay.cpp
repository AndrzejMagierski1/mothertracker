

#include "samplePlayback.h"








void cSamplePlayback::initDisplayControls()
{
	// inicjalizacja kontrolek

	for(uint8_t i = 0; i<8; i++)
	{
		strControlProperties prop2;
		prop2.text = (char*)"";
		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 450;
		prop2.w = 800/8-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);
	}


	strControlProperties prop;
	prop.x = 0;
	prop.y = 75;
	prop.w = 800;
	prop.h = 300;
	prop.data = &spectrum;
	if(spectrumControl == nullptr)  spectrumControl = display.createControl<cSpectrum>(&prop);

	// te same wpolrzedne
	prop.data = &points;
	if(pointsControl == nullptr)  pointsControl = display.createControl<cPoints>(&prop);

}


void cSamplePlayback::destroyDisplayControls()
{
	display.destroyControl(spectrumControl);
	spectrumControl = nullptr;

	display.destroyControl(pointsControl);
	pointsControl = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;
	}

}

void cSamplePlayback::showDefaultScreen()
{
	processSpectrum();
	processPoints();

	//spectrum
	display.setControlShow(spectrumControl);
	display.refreshControl(spectrumControl);

	//points
	display.setControlShow(pointsControl);
	display.refreshControl(pointsControl);

	// bottom labels
	display.setControlText(bottomLabel[0], "Start");
	display.setControlText(bottomLabel[1], "Loop Start");
	display.setControlText(bottomLabel[2], "Loop End");
	display.setControlText(bottomLabel[3], "End");
	display.setControlText(bottomLabel[4], "Play Mode");
	display.setControlText(bottomLabel[5], "- Zoom");
	display.setControlText(bottomLabel[6], "+ Zoom");
	display.setControlText(bottomLabel[7], "");

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}

	display.synchronizeRefresh();

}


//==============================================================================================================

void cSamplePlayback::clearLabelsBorder()
{
	for(uint8_t i = 0; i<8; i++)
	{
		display.setRemoveControlStyle(bottomLabel[i], controlStyleBorder);
		display.refreshControl(bottomLabel[i]);
	}
}

