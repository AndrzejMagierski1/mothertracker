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
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 650;
	prop2.y = 12;
	if(titleLabelProjectName == nullptr) titleLabelProjectName = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);



	for(uint8_t i = 0; i<8; i++)
	{
		prop2.text = (char*)"";
		prop2.style = 	(controlStyleBackground | controlStyleCenterX | controlStyleCenterY);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 465;
		prop2.w = 800/8-6;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.y = 437;
		prop2.h = 28;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);
	}

	projectList.linesCount = 13;
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

		uint16_t i = 0;
		strcpy(currentPatchProjectName,"Projects/Untitled");
		while((SD.exists(currentPatchProjectName)) && (i <= 9999))
		{
			i++;
			sprintf(currentPatchProjectName,"Projects/Untitled%d",i);
		}

		if(i == 0) display.setControlText(titleLabelProjectName, "Untitled");
		else
		{
			sprintf(currentPatchProjectName,"Untitled%d",i);
			display.setControlText(titleLabelProjectName, currentPatchProjectName);
		}
	}
	else
	{
		display.setControlText(titleLabelProjectName, fileManager.currentProjectName);
	}

	display.refreshControl(titleLabelProjectName);


	//lista
	display.setControlHide(fileListControl);


	display.setControlText(topLabel[0], "New");
	display.setControlText(bottomLabel[0], "Project");
	display.setControlText(bottomLabel[1], "Open");
	display.setControlText(bottomLabel[2], "");
	display.setControlText(bottomLabel[3], "");
	display.setControlText(bottomLabel[4], "Save");
	display.setControlText(bottomLabel[5], "Save As");
	display.setControlText(bottomLabel[6], "Export");
	display.setControlText(bottomLabel[7], "Game");


	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
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

	for(uint8_t i = 1; i<8; i++)
	{
		makeBigBottomLabel(i);
	}
	makeSmallBottomLabel(0);
	display.synchronizeRefresh();

}
//==============================================================================================================

void cProjectEditor::showProjectsList()
{
// lista
	projectList.start = 0;
	projectList.length = locationFilesCount;
	projectList.linesCount = 13;
	projectList.data = filesNames;

	display.setControlData(fileListControl,  &projectList);
	display.setControlShow(fileListControl);
	display.refreshControl(fileListControl);

	for(uint8_t i = 2; i < 8 ; i++)
	{
		display.setControlText(bottomLabel[i], "");
	}

// bottom labels
	display.setControlText(bottomLabel[0], "Open");
	display.setControlText(bottomLabel[1], "Cancel");


	for(uint8_t i = 0; i < 8 ; i++)
	{
		display.refreshControl(bottomLabel[i]);
	}
	makeBigBottomLabel(0);
	display.synchronizeRefresh();
}


//void cProjectEditor::showTemplatesList()
//{
//// lista
//	projectList.start = 0;
//	projectList.length = locationFilesCount;
//	projectList.linesCount = 5;
//	projectList.data = filesNames;
//
//	display.setControlData(fileListControl,  &projectList);
//	display.setControlShow(fileListControl);
//	display.refreshControl(fileListControl);
//
//// top label listy
//	display.setControlText(bottomLabel[0], "Choose template");
//	display.setControlShow(bottomLabel[0]);
//	display.refreshControl(bottomLabel[0]);
//
//// bottom labels
//	display.setControlText(bottomLabel[0], "Create");
//	display.setControlText(bottomLabel[1], "Cancel");
//	display.setControlText(bottomLabel[4], "Test");
//
//	display.refreshControl(bottomLabel[0]);
//	display.refreshControl(bottomLabel[1]);
//	display.refreshControl(bottomLabel[4]);
//
//	display.synchronizeRefresh();
//
//}


void cProjectEditor::showEnterNameKeyboard()
{

	display.setControlHide(fileListControl);
	display.refreshControl(fileListControl);

// top label listy
	//display.setControlHide(topLabel[0]);
	//display.refreshControl(topLabel[0]);

// bottom labels
	display.setControlText(bottomLabel[0], "Create");
	display.setControlText(bottomLabel[1], "Cancel");
	display.setControlText(bottomLabel[4], "Test");

	display.refreshControl(bottomLabel[0]);
	display.refreshControl(bottomLabel[1]);
	display.refreshControl(bottomLabel[4]);


	showKeyboard();
	showKeyboardEditName();

	makeBigBottomLabel(0);
	display.synchronizeRefresh();


}

void cProjectEditor::showSaveAsKeyboard()
{
	display.setControlText(bottomLabel[0],"Cancel");

	for(uint8_t i = 1; i < 7 ; i++)
	{
		display.setControlText(bottomLabel[i], "");
	}

	display.setControlText(bottomLabel[7],"Save");

	for(uint8_t i = 0; i < 8 ; i++)
	{
		display.refreshControl(bottomLabel[i]);
	}

	showKeyboard();
	showKeyboardEditName();
	makeBigBottomLabel(0);
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
	display.setControlText(bottomLabel[0], "Cancel");
	display.setControlText(bottomLabel[1], "");

	display.setControlText(bottomLabel[4], "Don't Save");
	display.setControlText(bottomLabel[5], "");
	display.setControlText(bottomLabel[6], "");
	display.setControlText(bottomLabel[7], "Save");


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
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}
	makeBigBottomLabel(0);
	display.synchronizeRefresh();
}

void cProjectEditor::showOverwriteWindow()
{
	display.setControlText(bottomLabel[0], "Yes");
	for(uint8_t i=1 ;i < 7; i++)
	{
		display.setControlText(bottomLabel[i], "");
	}
	display.setControlText(bottomLabel[7], "No");

	sprintf(currentInfo,"Do you want overwrite\"%s\" ?", name);

	display.setControlText(selectWindowLabel, currentInfo);
	display.setControlShow(selectWindowLabel);
	display.refreshControl(selectWindowLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}
	makeBigBottomLabel(0);
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

void cProjectEditor::makeSmallBottomLabel(uint8_t i)
{
	display.setControlPosition(bottomLabel[i], -1, 465);
	display.setControlSize(bottomLabel[i], -1, 30);

	display.setControlShow(topLabel[i]);
	display.setControlShow(bottomLabel[i]);
	display.refreshControl(bottomLabel[i]);
	display.refreshControl(topLabel[i]);
}
void cProjectEditor::makeBigBottomLabel(uint8_t i)
{
	display.setControlPosition(bottomLabel[i], -1, 452);
	display.setControlSize(bottomLabel[i], -1, 59);

	display.setControlHide(topLabel[i]);
	display.setControlShow(bottomLabel[i]);
	display.refreshControl(bottomLabel[i]);
	display.refreshControl(topLabel[i]);
}

void cProjectEditor::showExportWindow()
{
	display.setControlText(topLabel[0], "Export");
	display.setControlText(bottomLabel[0], "Song");
	display.setControlText(topLabel[1], "Export");
	display.setControlText(bottomLabel[1], "Song Stems");
	display.setControlText(topLabel[2], "Export");
	display.setControlText(bottomLabel[2], "Pattern");
	display.setControlText(topLabel[3], "Export");
	display.setControlText(bottomLabel[3], "Patt Stems");
	display.setControlText(topLabel[4], "Export");
	display.setControlText(bottomLabel[4], "To MOD");
	display.setControlText(bottomLabel[5], "");
	display.setControlText(bottomLabel[6], "");
	display.setControlText(bottomLabel[7], "Go Back");

	for(uint8_t i=0; i<5 ; i++)
	{
		makeSmallBottomLabel(i);
	}

	for(uint8_t i = 5; i <8; i++)
	{
		makeBigBottomLabel(i);
	}
	//refreshe są w make
	display.synchronizeRefresh();
}
