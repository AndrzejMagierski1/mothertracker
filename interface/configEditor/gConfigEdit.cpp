

#include "configEditor.h"
#include "mtGridEditor.h"
#include "mtPadsBacklight.h"

static uint16_t framesPlacesConfig[4][4]=
{
	{(800/8)*0+1, 29, 800/4-3, 391},
	{(800/8)*2+1, 29, 600/2-3, 391},
	{(800/8)*5+1, 29, 800/4-3, 391},
	{(800/8)*5+1, 29, 600/2-3, 391},
};

static uint16_t framesPlacesConfigPadScreen[3][4]=
{
	{(800/8)*0+1, 29, 800/8-3, 391},
	{(800/8)*1+1, 29, 800/8-3, 391},
	{(800/8)*2+1, 29, 800/8-3, 391}
};


static uint32_t popUpLabelColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	one_true_red, // ramka
};

const char creditsText[1500] =
{

/*
	"Polyend Team 2020:"
	"\nMarcin Bortkiewicz, Cezary Fila, Katarzyna Florczak, Andrzej Magierski, Maciej Pancer, Michal Pilat, Piotr Raczynski,"
	"\nDaniel Romasiuk, Krzysztof Szulc, Jacek Tworkowski, Krystian Zemanowicz, Krzysztof Ziejewski, Krzysztof Zwolinski."
	"\n"
	"\nSpecial thanks to the beta testers & artists for feedback, tests, demo tracks, tutorials,videos and reviews: "
	"\nSam Barker, Gui Boratto, Cuckoo, Mike Coakley, Alfred Darlington (Daedelus), Matthew Dear, Richard Devine,"
	"\nZiv Eliraz (Loopop), Darren Emerson (Underworld), Derrick Estrada (Baseck), Aaron Funk (Venetian Snares), "
	"\nPaul Geissinger (Starkey), John Greenwood (Radiohead), Julien Guillot (The Junglechrist), Scott Harper (Knobs),"
	"\nAndrew Huang, Richard D. James (Aphex Twin), Benn Jordan (The Flashbulb), Tim Koch, Jeremy Leaird-Koch "
	"\n(Red Means Recording), Jamie Lidell, Robert Lippok, Sarah Longfield,Ricky Martinez, Jonathan Miller, Bryan Noll"
	"\n(Lightbath), Terrence O'Brien (Endgadget), Bogdan Raczynski, Tom Rowlands (Chemical Brothers), Sam Shepherd"
	"\n(Floating Points), Cenk Sayinli (mr. Dataline), Lukasz Seliga (SLG), Jacek Sienkiewicz, Sam Slater, Travis Stewart"
	"\n(Machinedrum), Jimi Tenor, Si Truss (Future Music), Brain Waltzera, Axel Willner (The Field), Danny Wolfers (Legowelt)."
	"\n"
	"\nMany thanks for the sample packs to:"
	"\nAlex Cummings / ARC noise, Julien Guillot / Stazma, Jamie Lidell / Isolation Loops, Plughugger /"
	"\nEP16 Techno, Alex Restis / 8-Bit Essentials & Crude Drums, Scot Solida /"
*/

	"Polyend Team 2020:"
	"\nMarcin Bortkiewicz, Cezary Fila, Katarzyna Florczak, Andrzej Magierski, Maciej Pancer, Michal Pilat, Piotr Raczynski,"
	"\nDaniel Romasiuk, Krzysztof Szulc, Jacek Tworkowski, Krystian Zemanowicz, Krzysztof Ziejewski, Krzysztof Zwolinski."
	"\n"
	"\nSpecial thanks to the beta testers & artists for feedback, tests, demo tracks, tutorials, videos and reviews: "
	"\nSam Barker, Gui Boratto, Cuckoo, Mike Coakley, Alfred Darlington (Daedelus), Matthew Dear, Richard Devine,"
	"\nZiv Eliraz (Loopop), Darren Emerson (Underworld), Derrick Estrada (Baseck), Paul Geissinger (Starkey),"
	"\nJulien Guillot (The Junglechrist), Scott Harper (Knobs), Andrew Huang, Richard D. James (Aphex Twin),"
	"\nBenn Jordan (The Flashbulb), Tim Koch, Jeremy Leaird-Koch (Red Means Recording), Jamie Lidell, Robert Lippok,"
	"\nSarah Longfield, Ricky Martinez, Bryan Noll (Lightbath), Terrence O'Brien (Endgadget), Bogdan Raczynski,"
	"\nSam Shepherd (Floating Points), Cenk Sayinli (Dataline), Lukasz Seliga (SLG), Jacek Sienkiewicz, Sam Slater,"
	"\nTravis Stewart (Machinedrum), Jimi Tenor, Si Truss (Future Music), Brain Waltzera, Axel Willner (The Field),"
	"\nDanny Wolfers (Legowelt)."
	"\n"
	"\nMany thanks for the provided sample packs to:"
	"\nAlex Cummings, The Flashbulb, Jamie Lidell, Legowelt, Sarah Longfield, Plughugger, Alex Restis, Scot Solida, Stazma. "









};





void cConfigEditor::initDisplayControls()
{
	// inicjalizacja kontrolek
	strControlProperties gridNameLabelProp;
	gridNameLabelProp.style = ( controlStyleHide | controlStyleRightX | controlStyleCenterY | controlStyleFont4);
	gridNameLabelProp.x = 790;
	gridNameLabelProp.y = 13;
	if(gridNameLabel == nullptr) gridNameLabel = display.createControl<cLabel>(&gridNameLabelProp);


	strControlProperties prop9;
	prop9.x = 400;
	prop9.colors = popUpLabelColors;
	prop9.y = 300;
	prop9.h = 100;
	prop9.w = 800-10;
	prop9.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleFont2 | controlStyleRoundedBorder);
	prop9.text = (char*)"";
	if(popoutWindowLabel == nullptr)  popoutWindowLabel = display.createControl<cLabel>(&prop9);

	strControlProperties prop3;
	// ramka
	prop3.style = 0;
	prop3.value = 0;
	prop3.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop3);


	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY | controlStyleFont4);
	prop2.x = 9;
	prop2.y = 13;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 2;
	prop2.y = 0;
	prop2.w = 795;
	prop2.h = 26;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);


	labelArrow[0].bitmaps[0].bitmapIndex = displayArrowU;
	labelArrow[0].bitmaps[0].xValue =  (800/8)*0+(800/16);
	labelArrow[0].bitmaps[0].yValue = 447;
	labelArrow[0].bitmaps[1].bitmapIndex = displayArrowD;
	labelArrow[0].bitmaps[1].xValue =  (800/8)*1+(800/16);
	labelArrow[0].bitmaps[1].yValue = 447;

	labelArrow[1].bitmaps[0].bitmapIndex = displayArrowU;
	labelArrow[1].bitmaps[0].xValue =  (800/8)*2+(800/16);
	labelArrow[1].bitmaps[0].yValue = 447;
	labelArrow[1].bitmaps[1].bitmapIndex = displayArrowD;
	labelArrow[1].bitmaps[1].xValue =  (800/8)*3+(800/16);
	labelArrow[1].bitmaps[1].yValue = 447;

	labelArrow[2].bitmaps[0].bitmapIndex = displayArrowU;
	labelArrow[2].bitmaps[0].xValue =  (800/8)*5+(800/16);
	labelArrow[2].bitmaps[0].yValue = 447;
	labelArrow[2].bitmaps[1].bitmapIndex = displayArrowD;
	labelArrow[2].bitmaps[1].xValue =  (800/8)*6+(800/16);
	labelArrow[2].bitmaps[1].yValue = 447;

	for(uint8_t i = 0; i<8; i++)
	{
		prop2.value = 0;
		prop2.data = nullptr;
		prop2.colors = interfaceGlobals.activeLabelsColors;
		prop2.style = 	( controlStyleCenterX | controlStyleFont3 | controlStyleCenterY);
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 424;
		prop2.h =  55;
		if(i == 0) {prop2.style |= controlStyleShowBitmap; prop2.data = &labelArrow[0];}
		if(i == 2) {prop2.style |= controlStyleShowBitmap; prop2.data = &labelArrow[1];}
		if(i == 5) {prop2.style |= controlStyleShowBitmap; prop2.data = &labelArrow[2];}
		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);

	}

	prop2.text = nullptr;
	prop2.colors = interfaceGlobals.activeBgLabelsColors;
	prop2.value = 36; //36;
	prop2.style = controlStyleNoTransparency | controlStyleShow;
	prop2.x = 0;
	prop2.w = 800;
	prop2.y = 424;
	prop2.h =  55;
	if(bgLabel == nullptr) bgLabel = display.createControl<cBgLabel>(&prop2);

	strControlProperties prop;

	prop.style = controlStyleBackground;
	prop.x = (800/8)*0+1;
	prop.y = 29;
	prop.w = 800/4-3;
	prop.h = 394;
	prop.data = &basemenuList;
	if(configBasemenuListControl == nullptr)  configBasemenuListControl = display.createControl<cList>(&prop);

	prop.x = (800/8)*2+1;
	prop.y = 29;
	prop.w = 600/2-3;
	prop.h = 394;
	prop.data = &submenuList;
	if(configSubmenuListControl == nullptr) configSubmenuListControl = display.createControl<cParamValueList>(&prop);

	prop.x = (800/8)*5+1;
	prop.y = 29;
	prop.w = 600/2-3;
	prop.h = 394;
	prop.data = &secondSubmenuList;
	if(configSecondSubmenuListControl == nullptr) configSecondSubmenuListControl = display.createControl<cParamValueList>(&prop);

	prop.style = controlStyleBackground;
	prop.x = (800/8)*5+1;
	prop.y = 29;
	prop.w = 800/4-3;
	prop.h = 394;
	if(configListControl == nullptr)  configListControl = display.createControl<cList>(&prop);

	prop3.style = controlStyleFont5;
	prop3.value = 3;
	prop3.x = 0;
	prop3.y = 35;
	prop3.w = 780;
	prop3.h = 400;
	prop3.data = nullptr; //&textBoxData;
	if(textBox == nullptr) textBox = display.createControl<cTextBox>(&prop3);

	strControlProperties gridPadsProp;

	for(uint8_t i = 0; i < 48; i++)
	{
		interfaceGlobals.padNamesPointer[i] = (char*)mtNotes[mtGrid.pad[i].note];
		sprintf(interfaceGlobals.padFinetuneNames[i],"%d",mtGrid.pad[i].microtune);
	}

	padNamesStruct.length = 5;
	padNamesStruct.noteName = interfaceGlobals.padNamesPointer;
	padNamesStruct.finetuneName = interfaceGlobals.padFinetuneNames;

	gridPadsProp.style = 0;
	gridPadsProp.x = 13;
	gridPadsProp.y = 130;
	gridPadsProp.w = 780;
	gridPadsProp.h = 260;
	gridPadsProp.colors = nullptr;
	gridPadsProp.value = gridEditor.getSelectedPad();
	gridPadsProp.data = &padNamesStruct;
	if(gridPadsControl == nullptr)  gridPadsControl = display.createControl<cGridEditorPopup>(&gridPadsProp);


	strControlProperties gridPadBarProp;


	gridPadBarProp.x = (800/8)*0+1;
	gridPadBarProp.y = 29;
	gridPadBarProp.w = 800/8-3;
	gridPadBarProp.style =  controlStyleValue_0_100 | controlStyleBackground;
	gridPadBarProp.h = 394;
	if(padBar[0] == nullptr)  padBar[0] = display.createControl<cBar>(&gridPadBarProp);

	gridPadBarProp.x = (800/8)*1+1;
	gridPadBarProp.y = 29;
	gridPadBarProp.w = 800/8-3;
	gridPadBarProp.style =  controlStyleValueLeftRight_100_100 | controlStyleBackground;
	gridPadBarProp.h = 394;
	if(padBar[1] == nullptr)  padBar[1] = display.createControl<cBar>(&gridPadBarProp);


	strControlProperties gridPadListProp;

	padListData.linesCount = 5;
	padListData.start = 0;
	padListData.length = 2;
	padListData.data = (char**)padListNames;

	gridPadListProp.style = controlStyleBackground;
	gridPadListProp.x = (800/8)*(2)+1;
	gridPadListProp.y = 29;
	gridPadListProp.w = 800/8-3;
	gridPadListProp.h = 394;
	gridPadListProp.data = &padListData;
	if(padList == nullptr)  padList = display.createControl<cList>(&gridPadListProp);
}



void cConfigEditor::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(gridNameLabel);
	gridNameLabel = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(configBasemenuListControl);
	configBasemenuListControl = nullptr;

	display.destroyControl(configSubmenuListControl);
	configSubmenuListControl = nullptr;

	display.destroyControl(configSecondSubmenuListControl);
	configSecondSubmenuListControl = nullptr;

	display.destroyControl(configListControl);
	configListControl = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;
	}

	display.destroyControl(bgLabel);
	bgLabel = nullptr;

	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(popoutWindowLabel);
	popoutWindowLabel = nullptr;

	display.destroyControl(textBox);
	textBox = nullptr;

	display.destroyControl(gridPadsControl);
	gridPadsControl = nullptr;

	display.destroyControl(padList);
	padList = nullptr;

	for(uint8_t i = 0 ; i < 2 ; i++)
	{
		display.destroyControl(padBar[i]);
		padBar[i] = nullptr;
	}
}

void cConfigEditor::showDefaultConfigScreen()
{
	display.setControlHide(gridPadsControl);
	display.refreshControl(gridPadsControl);

	display.setControlStyle(label[0],( controlStyleCenterX | controlStyleFont3 | controlStyleCenterY | controlStyleShowBitmap));
	display.setControlStyle(label[2],( controlStyleCenterX | controlStyleFont3 | controlStyleCenterY | controlStyleShowBitmap));
	display.setControlStyle(label[5],( controlStyleCenterX | controlStyleFont3 | controlStyleCenterY | controlStyleShowBitmap));

	for(uint8_t i = 0; i<8; i++)
	{
		//display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlShow(label[i]);
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");
		display.refreshControl(label[i]);
		//display.setControlText2(label[i], "");
	}



//	if(i == 0) {prop2.style |= controlStyleShowBitmap; prop2.data = &labelArrow[0];}
//	if(i == 2) {prop2.style |= controlStyleShowBitmap; prop2.data = &labelArrow[1];}
//	if(i == 5) {prop2.style |= controlStyleShowBitmap; prop2.data = &labelArrow[2];}


	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Config");
	display.refreshControl(titleLabel);


	display.setControlText(label[4], "Change");
	display.setControlText(label[7], "Change");


	frameData.placesCount = 4;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlacesConfig[0][0];
	frameData.places[1] = &framesPlacesConfig[1][0];
	frameData.places[2] = &framesPlacesConfig[2][0];
	frameData.places[3] = &framesPlacesConfig[3][0];

	display.setControlValue(bgLabel, 36);
	display.refreshControl(bgLabel);

	display.setControlShow(frameControl);

	display.setControlHide(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	activateLabelsBorder();
	display.synchronizeRefresh();

}


//==============================================================================================================
void cConfigEditor::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	//display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

void cConfigEditor::selectSubmenu()
{
	selectedPlace = 1;
	activateLabelsBorder();
}

void cConfigEditor::selectSecondSubmenu()
{
	selectedPlace = 3;
	activateLabelsBorder();
}

void cConfigEditor::selectConfigList(uint8_t width)
{

	if(width == 3)
	{
		frameData.places[2] = &framesPlacesConfig[3][0];
	}
	else
	{
		frameData.places[2] = &framesPlacesConfig[2][0];
	}

	selectedPlace = 2;
	activateLabelsBorder();
}





void cConfigEditor::changeLabelText(uint8_t labelIdx, const char *text)
{
	display.setControlText(label[labelIdx], text);
	display.setControlShow(label[labelIdx]);
	display.refreshControl(label[labelIdx]);
}





void cConfigEditor::showConfigList5(uint8_t list_width, uint8_t start, uint8_t length, char** listText)
{
	configListShown = 1;
	selectedConfigListPosition = start;

	display.setControlPosition(configListControl, (800/8)*5+1,  -1);

	if(list_width == 3)
	{
		display.setControlSize(configListControl, ((800/8)*3)-3,  -1);
		frameData.places[2] =&framesPlacesConfig[3][0];
	}
	else //2
	{
		display.setControlSize(configListControl, ((800/8)*2)-3,  -1);
		frameData.places[2] = &framesPlacesConfig[2][0];
	}

	configList.linesCount = 13;
	configList.start = start;
	configList.length = length;
	configList.data = listText;

	display.setControlData(configListControl, &configList);
	display.setControlShow(configListControl);
	display.refreshControl(configListControl);

	//display.setControlText(label[7], "");
	display.setControlShow(label[5]);
	display.setControlShow(label[6]);
	display.setControlShow(label[7]);

	display.refreshControl(label[7]);
}



void cConfigEditor::hideConfigList()
{
	flashingState = 0;

	display.setControlHide(configListControl);

	//display.setControlText(label[7], "");
	display.setControlHide(label[5]);
	display.setControlHide(label[6]);
	display.setControlHide(label[7]);

	display.refreshControl(label[7]);

	configListShown = 0;
}


void cConfigEditor::showSubmenu()
{
	submenuShown = 1;

	display.setControlShow(configSubmenuListControl);
	display.setControlShow(label[2]);
	display.setControlShow(label[3]);
	display.setControlShow(label[4]);

	display.refreshControl(label[4]);

}

void cConfigEditor::hideSubmenu()
{
	display.setControlHide(configSubmenuListControl);
	display.setControlHide(label[2]);
	display.setControlHide(label[3]);
	display.setControlHide(label[4]);

	display.refreshControl(label[4]);

	submenuShown = 0;
}

void cConfigEditor::showSecondSubmenu()
{
	secondSubmenuShown = 1;

	display.setControlShow(configSecondSubmenuListControl);

	display.setControlShow(label[5]);
	display.setControlShow(label[6]);
	display.setControlShow(label[7]);

	display.refreshControl(label[5]);
	display.refreshControl(label[6]);
	display.refreshControl(label[7]);
}

void cConfigEditor::hideSecondSubmenu()
{
	display.setControlHide(configSecondSubmenuListControl);
	display.setControlHide(label[5]);
	display.setControlHide(label[6]);
	display.setControlHide(label[7]);

	display.refreshControl(label[7]);

	secondSubmenuShown = 0;
}

//==============================================================================================================
//==============================================================================================================
//==============================================================================================================


void cConfigEditor::showFirmwareUpdatePopout()
{
	updatePopupShown = 1;

	for(uint8_t i = 0 ; i < 8; i++)
	{
		//display.setControlText(label[i], "");
		if(i<6) display.setControlHide(label[i]);
		else display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.setControlText(label[6], "Update");
	display.setControlText(label[7], "Cancel");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	display.setControlText(popoutWindowLabel,"Do you want to flash new firmware?");
	display.setControlShow(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	display.synchronizeRefresh();
}


void cConfigEditor::hideFirmwareUpdatePopout()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(label[i], "");
		display.refreshControl(label[i]);
	}

	display.setControlShow(frameControl);
	display.refreshControl(frameControl);

	display.setControlHide(popoutWindowLabel);
	display.refreshControl(popoutWindowLabel);

	display.synchronizeRefresh();
}


void cConfigEditor::showCreditsControls()
{
	display.setControlText(titleLabel, "Credits");
	display.refreshControl(titleLabel);

	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlHide(label[i]);
		//display.setControlText(label[i], "");
		//display.refreshControl(label[i]);
	}

	display.setControlShow(label[7]);
	display.setControlText(label[7], "Close");
	display.refreshControl(label[7]);

	display.setControlHide(frameControl);
	//display.refreshControl(frameControl);

	display.setControlValue(bgLabel, 1);
	display.refreshControl(bgLabel);

	display.setControlShow(textBox);
	//display.setControlValue(textBox, 5);
	display.setControlText(textBox, creditsText);
	display.refreshControl(textBox);



}


void cConfigEditor::hideCreditsControls()
{
	display.setControlText(titleLabel, "Config");
	display.refreshControl(titleLabel);

	display.setControlHide(textBox);
	display.setControlText(label[7], "");
	display.refreshControl(label[7]);
}

void cConfigEditor::showGridScreen()
{
	display.setControlHide(configBasemenuListControl);
	display.refreshControl(configBasemenuListControl);
	display.setControlHide(configSubmenuListControl);
	display.refreshControl(configSubmenuListControl);

	hideSecondSubmenu();

	display.setControlText(titleLabel, "Grid Editor");
	display.refreshControl(titleLabel);

	display.setControlText(gridNameLabel, mtGrid.name);
	display.setControlShow(gridNameLabel);
	display.refreshControl(gridNameLabel);

	display.setControlValue(bgLabel, 0b11101111);
	display.refreshControl(bgLabel);

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	for(uint8_t i = 0; i < 48; i++)
	{
		interfaceGlobals.padNamesPointer[i] = (char*)mtNotes[mtGrid.pad[i].note];
		sprintf(interfaceGlobals.padFinetuneNames[i],"%d",mtGrid.pad[i].microtune);

		if(mtGrid.pad[i].ledEnable) padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, i);
		else padsBacklight.setBackLayer(0, 0, i);

	}

	display.setControlShow(gridPadsControl);
	display.setControlValue(gridPadsControl,gridEditor.getSelectedPad());
	display.refreshControl(gridPadsControl);

	for(uint8_t i = 0 ; i < 5; i++)
	{
		display.setControlHide(label[i]);
		display.refreshControl(label[i]);
	}

	display.setControlText(label[0],"Note");
	display.setControlText(label[1],"Microtune");
	display.setControlText(label[2],"LED");

	for(uint8_t i = 0 ; i < 3 ; i++)
	{
		display.setControlStyle(label[i], ( controlStyleCenterX | controlStyleFont3));
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlShow(label[i]);
		display.setControlValue(label[i],1);

		reloadPadScreenDisplayedValue(i);
		refreshPadValue(i, enScreenType::screenTypeGridScreen);
	}



	display.setControlStyle(label[5], ( controlStyleCenterX | controlStyleFont3 | controlStyleCenterY));
	display.setControlText(label[5], "Save");
	display.setControlShow(label[5]);
	display.refreshControl(label[5]);
	display.setControlStyle(label[6], ( controlStyleCenterX | controlStyleFont3 | controlStyleCenterY));
	display.setControlText(label[6], "Cancel");
	display.setControlShow(label[6]);
	display.refreshControl(label[6]);
	display.setControlText(label[7], "Select");
	display.setControlShow(label[7]);
	display.refreshControl(label[7]);


	display.synchronizeRefresh();
}

void cConfigEditor::refreshGridScreen()
{
	display.setControlValue(gridPadsControl,gridEditor.getSelectedPad());
	display.refreshControl(gridPadsControl);

	for(uint8_t i = 0 ; i < 3 ; i++)
	{
		reloadPadScreenDisplayedValue(i);
		refreshPadValue(i, enScreenType::screenTypeGridScreen);
	}
}

void cConfigEditor::hideGridScreen()
{
	display.setControlHide(gridPadsControl);
	display.refreshControl(gridPadsControl);
	display.setControlHide(gridNameLabel);
	display.refreshControl(gridNameLabel);
}
void cConfigEditor::showPadScreen()
{
	display.setControlShow(padBar[0]);
	display.setControlShow(padBar[1]);
	display.setControlShow(padList);

	display.setControlText(label[0],"Note");
	display.setControlText(label[1],"Microtune");
	display.setControlText(label[2],"LED");

	for(uint8_t i = 0 ; i < 3 ; i++)
	{
		display.setControlStyle(label[i], ( controlStyleCenterX | controlStyleFont3));
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlShow(label[i]);
		display.setControlValue(label[i],1);

		reloadPadScreenDisplayedValue(i);
		refreshPadValue(i,enScreenType::screenTypePadScreen);
	}

	for(uint8_t i = 3 ; i < 7; i++)
	{
		display.setControlText(label[i],"");
		display.setControlText2(label[i],"");
		display.setControlHide(label[i]);
	}

	display.setControlText(label[7],"Back");

	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.refreshControl(label[i]);
	}

	sprintf(padScreenTitleLabelName,"Grid Editor Pad %d", gridEditor.getSelectedPad() + 1);

	display.setControlText(titleLabel, padScreenTitleLabelName);
	display.refreshControl(titleLabel);



	display.setControlText(gridNameLabel, mtGrid.name);
	display.setControlShow(gridNameLabel);
	display.refreshControl(gridNameLabel);

	display.setControlValue(bgLabel, 0b10001111);
	display.refreshControl(bgLabel);

	display.refreshControl(padBar[0]);
	display.refreshControl(padBar[1]);
	display.refreshControl(padList);

	frameData.placesCount = 3;
	frameData.startPlace = selectedPlacePadScreen;
	frameData.places[0] = &framesPlacesConfigPadScreen[0][0];
	frameData.places[1] = &framesPlacesConfigPadScreen[1][0];
	frameData.places[2] = &framesPlacesConfigPadScreen[2][0];

	refreshPadScreenFrame();
	display.setControlShow(frameControl);
	display.synchronizeRefresh();
}
void cConfigEditor::hidePadScreen()
{
	display.setControlHide(padBar[0]);
	display.setControlHide(padBar[1]);
	display.setControlHide(padList);

	display.refreshControl(padBar[0]);
	display.refreshControl(padBar[1]);
	display.refreshControl(padList);
}

void cConfigEditor::refreshPadScreenFrame()
{
	display.setControlValue(frameControl,selectedPlacePadScreen);
	display.refreshControl(frameControl);
}

void cConfigEditor::refreshPadValue(uint8_t value, enScreenType screen)
{
	switch(value)
	{
	case 0 :
		if(screen == screenTypePadScreen)
		{
			display.setControlValue(padBar[value],padScreenDisplayedValue[value]);
			display.refreshControl(padBar[value]);
		}
		display.setControlText2(label[value],mtNotes[mtGrid.pad[gridEditor.getSelectedPad()].note]);
		display.refreshControl(label[value]);
		break;
	case 1:
		if(screen == screenTypePadScreen)
		{
			display.setControlValue(padBar[value],padScreenDisplayedValue[value]);
			display.refreshControl(padBar[value]);
		}
		sprintf(microtuneValue, "%d", mtGrid.pad[gridEditor.getSelectedPad()].microtune);
		display.setControlText2(label[value], microtuneValue);
		display.refreshControl(label[value]);
		break;
	case 2:
		if(screen == screenTypePadScreen)
		{
			display.setControlValue(padList,padScreenDisplayedValue[value]);
			display.refreshControl(padList);
		}
		display.setControlText2(label[value], padListNames[padScreenDisplayedValue[value]]);
		display.refreshControl(label[value]);
		if(mtGrid.pad[gridEditor.getSelectedPad()].ledEnable) padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, gridEditor.getSelectedPad());
		else padsBacklight.setBackLayer(0, 0, gridEditor.getSelectedPad());
		break;
	default: break;
	}

}

