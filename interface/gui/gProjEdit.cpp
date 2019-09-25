#include <projectEditor.h>

#include "mtLED.h"
#include "mtFileManager.h"

constexpr uint8_t BACKSPACE_PAD_1 = 10;
constexpr uint8_t BACKSPACE_PAD_2 = 11;

constexpr uint8_t CAPS_LOCK_PAD_1 = 34;
constexpr uint8_t CAPS_LOCK_PAD_2 = 35;

constexpr uint8_t SPACE_PAD_1 = 43;
constexpr uint8_t SPACE_PAD_2 = 44;
constexpr uint8_t SPACE_PAD_3 = 45;
constexpr uint8_t SPACE_PAD_4 = 46;
constexpr uint8_t SPACE_PAD_5 = 47;

constexpr uint8_t F_PAD = 27;

constexpr uint8_t J_PAD = 30;

static uint32_t popUpLabelColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFF0000, // ramka
};

void cProjectEditor::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 650;
	prop2.y = 12;
	if(titleLabelProjectName == nullptr) titleLabelProjectName = display.createControl<cLabel>(&prop2);


	for(uint8_t i = 0; i<8; i++)
	{
		prop2.text = (char*)"";
		prop2.style = 	(controlStyleBackground | controlStyleCenterX | controlStyleCenterY);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 465;
		prop2.w = 800/8-6;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.y = 452;
		prop2.h = 58;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);
	}

	projectList.linesCount = 5;
	projectList.start = 0;
	projectList.length = 0;
	strControlProperties prop;
	prop.x = 0;
	prop.y = 35;
	prop.w = (800/4);
	prop.h = 25;
	prop.data = &projectList;
	if(fileListControl == nullptr)  fileListControl = display.createControl<cList>(&prop);

	strControlProperties prop3;
	prop3.x = 10;
	prop3.y = 120;
	prop3.w = 780;
	prop3.h = 280;
	if(keyboardControl == nullptr)  keyboardControl = display.createControl<cKeyboard>(&prop3);

	strControlProperties prop4;
	prop4.text = (char*)"";
	prop4.style = 	(controlStyleShow | controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleRoundedBorder);
	prop4.x = 393;
	prop4.y = 60;
	prop4.w = 765;
	prop4.h = 40;
	if(editName == nullptr)  editName = display.createControl<cEdit>(&prop4);

	strControlProperties prop5;

	prop5.x = 400;
	prop5.colors = popUpLabelColors;
	prop5.y = 350;

	prop5.h = 100;
	prop5.w = 800-(10);
	prop5.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleFont2 | controlStyleRoundedBorder);
	prop5.text = (char*)"";
	if(selectWindowLabel == nullptr)  selectWindowLabel = display.createControl<cLabel>(&prop5);

	strControlProperties prop6;

	prop6.x = 190;
	prop6.y = 170;
	prop6.style = controlStyleValue_0_100;
	prop6.h = 100;
	prop6.w = 420;

	if(loadHorizontalBarControl == nullptr)  loadHorizontalBarControl = display.createControl<cHorizontalBar>(&prop6);

}


void cProjectEditor::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(titleLabelProjectName);
	titleLabelProjectName = nullptr;


	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;
	}

	display.destroyControl(fileListControl);
	fileListControl = nullptr;

	display.destroyControl(keyboardControl);
	keyboardControl = nullptr;

	display.destroyControl(editName);
	editName = nullptr;

	display.destroyControl(selectWindowLabel);
	selectWindowLabel = nullptr;

	display.destroyControl(loadHorizontalBarControl);
	loadHorizontalBarControl = nullptr;

}

void cProjectEditor::showDefaultScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "File");
	display.refreshControl(titleLabel);

	if((fileManager.currentProjectName[0] == 0) || ( newProjectNotSavedFlag == 1 ) )
	{
		char currentPatch[PATCH_SIZE];
		uint16_t i = 0;
		strcpy(currentPatch,"Projects/Untitled");
		while((SD.exists(currentPatch)) && (i <= 9999))
		{
			i++;
			sprintf(currentPatch,"Projects/Untitled%d",i);
		}

		if(i == 0) display.setControlText(titleLabelProjectName, "Untitled");
		else
		{
			sprintf(currentPatch,"Untitled%d",i);
			display.setControlText(titleLabelProjectName, currentPatch);
		}
	}
	else
	{
		display.setControlText(titleLabelProjectName, fileManager.currentProjectName);
	}

	display.refreshControl(titleLabelProjectName);


	//lista
	display.setControlHide(fileListControl);

	// top label listy
	//display.setControlText(PE->topLabel[0], "");
	display.setControlHide(topLabel[0]);
	display.refreshControl(topLabel[0]);

	// bottom labels
	display.setControlText(topLabel[0], "New Project");
	display.setControlText(topLabel[1], "Open");

	display.setControlText(topLabel[4], "Save");
	display.setControlText(topLabel[5], "Save As");
	display.setControlText(topLabel[7], "Export");


	display.setControlText(topLabel[7], "Game");


	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	hideKeyboard();
//	display.setControlHide(keyboardControl);
//	display.refreshControl(keyboardControl);
//
	display.setControlHide(editName);
	display.refreshControl(editName);

	display.setControlHide(selectWindowLabel);
	display.refreshControl(selectWindowLabel);

	display.setControlHide(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);

	display.synchronizeRefresh();

}
//==============================================================================================================

void cProjectEditor::showProjectsList()
{
// lista
	projectList.start = 0;
	projectList.length = locationFilesCount;
	projectList.linesCount = 5;
	projectList.data = filesNames;

	display.setControlData(fileListControl,  &projectList);
	display.setControlShow(fileListControl);
	display.refreshControl(fileListControl);

	for(uint8_t i = 2; i < 8 ; i++)
	{
		display.setControlText(topLabel[i], "");
	}

// bottom labels
	display.setControlText(topLabel[0], "Open");
	display.setControlText(topLabel[1], "Cancel");


	for(uint8_t i = 0; i < 8 ; i++)
	{
		display.refreshControl(topLabel[i]);
	}
	display.synchronizeRefresh();
}


void cProjectEditor::showTemplatesList()
{
// lista
	projectList.start = 0;
	projectList.length = locationFilesCount;
	projectList.linesCount = 5;
	projectList.data = filesNames;

	display.setControlData(fileListControl,  &projectList);
	display.setControlShow(fileListControl);
	display.refreshControl(fileListControl);

// top label listy
	display.setControlText(topLabel[0], "Choose template");
	display.setControlShow(topLabel[0]);
	display.refreshControl(topLabel[0]);

// bottom labels
	display.setControlText(topLabel[0], "Create");
	display.setControlText(topLabel[1], "Cancel");
	display.setControlText(topLabel[4], "Test");

	display.refreshControl(topLabel[0]);
	display.refreshControl(topLabel[1]);
	display.refreshControl(topLabel[4]);

	display.synchronizeRefresh();

}


void cProjectEditor::showEnterNameKeyboard()
{

	display.setControlHide(fileListControl);
	display.refreshControl(fileListControl);

// top label listy
	//display.setControlHide(topLabel[0]);
	//display.refreshControl(topLabel[0]);

// bottom labels
	display.setControlText(topLabel[0], "Create");
	display.setControlText(topLabel[1], "Cancel");
	display.setControlText(topLabel[4], "Test");

	display.refreshControl(topLabel[0]);
	display.refreshControl(topLabel[1]);
	display.refreshControl(topLabel[4]);


	showKeyboard();
	showKeyboardEditName();


	display.synchronizeRefresh();


}

void cProjectEditor::showSaveAsKeyboard()
{
	display.setControlText(topLabel[0],"Cancel");

	for(uint8_t i = 1; i < 7 ; i++)
	{
		display.setControlText(topLabel[i], "");
	}

	display.setControlText(topLabel[7],"Save");

	for(uint8_t i = 0; i < 8 ; i++)
	{
		display.refreshControl(topLabel[i]);
	}

	showKeyboard();
	showKeyboardEditName();

	display.synchronizeRefresh();

}


void cProjectEditor::showKeyboard()
{

	leds.setLED(F_PAD, 1, 10);
	leds.setLED(J_PAD, 1, 10);

	if(keyboardShiftFlag) display.setControlValue(keyboardControl, keyboardPosition + 42);
	else display.setControlValue(keyboardControl, keyboardPosition);

	display.setControlShow(keyboardControl);
	display.refreshControl(keyboardControl);
}

void cProjectEditor::hideKeyboard()
{
	if(lastPressedPad == BACKSPACE_PAD_1 || lastPressedPad == BACKSPACE_PAD_2)
	{
		leds.setLED(BACKSPACE_PAD_1, 0, 0);
		leds.setLED(BACKSPACE_PAD_2, 0, 0);
	}
	else if(lastPressedPad == CAPS_LOCK_PAD_1 || lastPressedPad == CAPS_LOCK_PAD_2)
	{
		leds.setLED(CAPS_LOCK_PAD_1, 0, 0);
		leds.setLED(CAPS_LOCK_PAD_2, 0, 0);
	}
	else if(lastPressedPad >= SPACE_PAD_1 && lastPressedPad <=SPACE_PAD_5)
	{
		for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
		{
			leds.setLED(i, 0, 0);
		}
	}
	else
	{
		leds.setLED(lastPressedPad,0,0);
	}
	leds.setLED(F_PAD, 0, 0);
	leds.setLED(J_PAD, 0, 0);

	display.setControlHide(keyboardControl);
	display.refreshControl(keyboardControl);
}

void cProjectEditor::showKeyboardEditName()
{


	display.setControlValue(editName, editPosition);

	display.setControlText(editName, name);
	display.setControlShow(editName);
	display.refreshControl(editName);
}

void cProjectEditor::hideKeyboardEditName()
{
	display.setControlHide(editName);
	display.refreshControl(editName);
}

void cProjectEditor::showSaveLastWindow()
{
	display.setControlText(topLabel[0], "Cancel");
	display.setControlText(topLabel[1], "");

	display.setControlText(topLabel[4], "Don't Save");
	display.setControlText(topLabel[5], "");
	display.setControlText(topLabel[7], "Save");

	char currentInfo[100];

	if(( fileManager.currentProjectName[0] ) != 0 && ( newProjectNotSavedFlag == 0 ) )
	{
		sprintf(currentInfo,"Do you want to save the changes to \"%s\" ?", fileManager.currentProjectName);
	}
	else
	{
		uint16_t i = 0;
		strcpy(currentInfo,"Projects/Untitled");
		while((SD.exists(currentInfo)) && (i <= 9999))
		{
			i++;
			sprintf(currentInfo,"Projects/Untitled%d",i);
		}

		if(i == 0) strcpy(currentInfo,"Do you want to save the changes to \"Untitled\" ?");
		else sprintf(currentInfo,"Do you want to save the changes to \"Untitled%d\" ?",i);
	}

	display.setControlText(selectWindowLabel, currentInfo);
	display.setControlShow(selectWindowLabel);
	display.refreshControl(selectWindowLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	display.synchronizeRefresh();
}

void cProjectEditor::showOverwriteWindow()
{
	display.setControlText(topLabel[0], "Yes");
	for(uint8_t i=1 ;i < 7; i++)
	{
		display.setControlText(topLabel[i], "");
	}
	display.setControlText(topLabel[7], "No");

	char currentInfo[100];

	sprintf(currentInfo,"Do you want overwrite\"%s\" ?", name);

	display.setControlText(selectWindowLabel, currentInfo);
	display.setControlShow(selectWindowLabel);
	display.refreshControl(selectWindowLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	display.synchronizeRefresh();
}


void cProjectEditor::showOpeningHorizontalBar()
{
	display.setControlValue(loadHorizontalBarControl, openingProgress);
	display.setControlText(loadHorizontalBarControl, "Opening project...");
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

void cProjectEditor::showSaveingHorizontalBar()
{
	display.setControlValue(loadHorizontalBarControl, saveingProgress);
	display.setControlText(loadHorizontalBarControl, "Save project...");
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

