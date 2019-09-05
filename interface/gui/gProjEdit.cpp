#include <projectEditor.h>

#include "mtLED.h"


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

void cProjectEditor::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);

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


}


void cProjectEditor::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

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

}

void cProjectEditor::showDefaultScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "File");
	display.refreshControl(titleLabel);

	//lista
	display.setControlHide(fileListControl);

	// top label listy
	//display.setControlText(PE->topLabel[0], "");
	display.setControlHide(topLabel[0]);
	display.refreshControl(topLabel[0]);

	// bottom labels
	display.setControlText(topLabel[0], "New");
	display.setControlText(topLabel[1], "Open");
	display.setControlText(topLabel[4], "Save");

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

// top label listy
	display.setControlText(topLabel[0], "Open project");
	display.setControlShow(topLabel[0]);
	display.refreshControl(topLabel[0]);

// bottom labels
	display.setControlText(topLabel[0], "Open");
	display.setControlText(topLabel[1], "Cancel");

	display.refreshControl(topLabel[0]);
	display.refreshControl(topLabel[1]);

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
