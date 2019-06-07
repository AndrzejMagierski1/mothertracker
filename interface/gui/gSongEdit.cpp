

#include <songEditor.h>


static uint16_t framesPlaces[5][4] =
{
	{0, 0, 800/2, 480},
	{(800/8)*4, 0, 800/8, 480},
	{(800/8)*5, 0, 800/8, 480},
	{(800/8)*6, 0, 800/8, 480},
	{(800/8)*7, 0, 800/8, 480},
};


void cSongEditor::initDisplayControls()
{
	strControlProperties prop2;
	prop2.text = (char*)"";
	prop2.style = 	( controlStyleBackground | controlStyleCenterX /*| controlStyleRoundedBorder*/);

	// inicjalizacja kontrolek
	for(uint8_t i = 0; i<4; i++)
	{
		prop2.x = (800/4)*i+(800/8);
		prop2.y = 420;
		prop2.w = 800/4-10;
		prop2.h = 26;

		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);

		//prop2.x = (800/4)*i+(800/8);
		prop2.y = 450;
		//prop2.w = 800/4-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);
	}


	patternsList.linesCount = 5;
	patternsList.start = 0;
	patternsList.length = songLength;
	patternsList.data = patternNames;
	strControlProperties prop;
	prop.x = 0+5;
	prop.y = 10;
	prop.w = 800/2-10;
	prop.h = 25;
	prop.data = &patternsList;
	if(patternsListControl == nullptr)  patternsListControl = display.createControl<cList>(&prop);


	for(uint8_t i = 0; i<4; i++)
	{
		prop.x = (800/8)*(4+i) + 5;
		prop.y = 10;
		prop.w = 800/8-10;
		prop.style = controlStyleValue_0_100 | controlStyleShow;
		prop.h = 400;
		prop.value = 0;
		if(barControl[i] == nullptr)  barControl[i] = display.createControl<cBar>(&prop);
	}

	frameData.placesCount = 5;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	prop.style = 0;
	prop.value = 0;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);
}


void cSongEditor::destroyDisplayControls()
{
	display.destroyControl(patternsListControl);
	patternsListControl = nullptr;

	for(uint8_t i = 0; i<4; i++)
	{
		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;

		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;

		display.destroyControl(barControl[i]);
		barControl[i] = nullptr;
	}

	display.destroyControl(frameControl);
	frameControl = nullptr;
}

void cSongEditor::showDefaultScreen()
{

	display.setControlText(topLabel[0], "Song Length");
	display.setControlText(topLabel[1], "Pattern");
	display.setControlText(topLabel[2], "Reverb");
	display.setControlText(topLabel[3], "Limiter");

	display.setControlText(bottomLabel[0], "  -             +  ");
	display.setControlText(bottomLabel[1], " /\\\           \\\/ ");
	display.setControlText(bottomLabel[2], "  Size      Dumping");
	display.setControlText(bottomLabel[3], " Attack    Treshold");

	for(uint8_t i = 0; i<4; i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);

		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}

	display.synchronizeRefresh();

}


//==============================================================================================================
void cSongEditor::showPatternsList()
{
	patternsList.start = 0;
	patternsList.length = songLength;
	patternsList.linesCount = 16;
	patternsList.data = patternNames;

	display.setControlData(patternsListControl,  &patternsList);
	display.setControlShow(patternsListControl);
	display.refreshControl(patternsListControl);


}

void cSongEditor::showReverbSize()
{
	display.setControlValue(barControl[0], mtProject.values.reverbRoomSize);
	//display.setControlShow(barControl[0]);
	display.refreshControl(barControl[0]);
}

void cSongEditor::showReverbDamping()
{
	display.setControlValue(barControl[1], mtProject.values.reverbDamping);
	//display.setControlShow(barControl[1]);
	display.refreshControl(barControl[1]);
}

void cSongEditor::showLimiterAttack()
{
	display.setControlValue(barControl[2], mtProject.values.limiterAttak);
	//display.setControlShow(barControl[2]);
	display.refreshControl(barControl[2]);
}

void cSongEditor::showLimiterTreshold()
{
	display.setControlValue(barControl[3], mtProject.values.limiterTreshold);
	//display.setControlShow(barControl[3]);
	display.refreshControl(barControl[3]);
}

//==============================================================================================================

void cSongEditor::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}
