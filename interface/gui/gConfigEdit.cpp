

#include "configEditor.h"


static uint16_t framesPlaces[7][4] =
{
	{0, 		412, 800/8, 68},
	{(800/8)*1, 412, 800/8, 68},
	{(800/8)*2, 412, 800/8, 68},
	{(800/8)*3, 412, 800/8, 68},
	{(800/8)*4, 412, 800/8, 68},
	{(800/8)*5, 412, 800/8, 68},
	{(800/8)*6, 0, 800/4, 480},
};



void cConfigEditor::initDisplayControls()
{
	// inicjalizacja kontrolek
	strControlProperties prop2;

	for(uint8_t i = 0; i<6; i++)
	{
		prop2.text = (char*)"";
		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 450;
		prop2.w = 800/8-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.x = (800/8)*i+(800/16);
		prop2.y = 415;
		prop2.h = 26;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);
	}

	prop2.x = (800/4)*3+(800/8);
	prop2.y = 450;
	prop2.w = 800/4-10;
	prop2.h = 30;
	if(bottomLabel[6] == nullptr) bottomLabel[6] = display.createControl<cLabel>(&prop2);

	prop2.y = 415;
	prop2.h = 26;
	if(topLabel[6] == nullptr) topLabel[6] = display.createControl<cLabel>(&prop2);


	configGroupList.linesCount = groupCount;
	configGroupList.start = 0;
	configGroupList.length = groupCount;
	configGroupList.data = configGroupsNames;
	strControlProperties prop;
	prop.x = (800/8)*6+5;
	prop.y = 140;
	prop.w = 800/4-10;
	prop.h = 25;
	prop.data = &configGroupList;
	if(configGroupsListControl == nullptr)  configGroupsListControl = display.createControl<cList>(&prop);


	// ramka
	frameData.placesCount = 7;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	prop.style = 0;
	prop.value = 0;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);

}


void cConfigEditor::destroyDisplayControls()
{

	display.destroyControl(configGroupsListControl);
	configGroupsListControl = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;

		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}


	display.destroyControl(frameControl);
	frameControl = nullptr;
}

void cConfigEditor::showDefaultScreen()
{

	//lista
	//display.setControlShow(playModeListControl);
	//display.refreshControl(playModeListControl);

	// bottom labels
	display.setControlText(bottomLabel[0], "1");
	display.setControlText(bottomLabel[1], "2");
	display.setControlText(bottomLabel[2], "3");
	display.setControlText(bottomLabel[3], "4");
	display.setControlText(bottomLabel[4], "5");
	display.setControlText(bottomLabel[5], "6");
	display.setControlText(bottomLabel[6], " /\\\           \\\/ ");
	//display.setControlText(bottomLabel[7], "");


	display.setControlText(topLabel[6], "Config");





	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}


	display.synchronizeRefresh();

}


//==============================================================================================================
void cConfigEditor::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

//==============================================================================================================


void cConfigEditor::showConfigGroupList()
{
	configGroupList.start = selectedConfigGroup;
	configGroupList.length = groupCount;
	configGroupList.linesCount = groupCount;
	configGroupList.data = configGroupsNames;

	display.setControlData(configGroupsListControl,  &configGroupList);
	display.setControlShow(configGroupsListControl);
	display.refreshControl(configGroupsListControl);


}
