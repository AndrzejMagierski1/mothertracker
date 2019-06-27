#include <projectEditor.h>



void cProjectEditor::initDisplayControls()
{

	// inicjalizacja kontrolek
	for(uint8_t i = 0; i<4; i++)
	{
		strControlProperties prop1;
		prop1.text = (char*)"";
		prop1.style = 	(/*controlStyleShow |*/ controlStyleCenterX);
		prop1.x = (800/4)*i+(800/8);
		prop1.y = 5;
		prop1.w = 800/4;
		prop1.h = 25;
		prop1.colors = &topLabelColors[0];

		if(topLabel[0] == nullptr) topLabel[0] = display.createControl<cLabel>(&prop1);
	}

	for(uint8_t i = 0; i<8; i++)
	{
		strControlProperties prop2;
		prop2.text = (char*)"";
		prop2.style = 	(controlStyleShow | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 450;
		prop2.w = 800/8-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);
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
	prop3.x = 50;
	prop3.y = 240;
	prop3.w = 750;
	prop3.h = 210;
	if(keyboardControl == nullptr)  keyboardControl = display.createControl<cKeyboard>(&prop3);

	strControlProperties prop4;
	prop4.text = (char*)"";
	prop4.style = 	(controlStyleShow | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
	prop4.x = 415;
	prop4.y = 190;
	prop4.w = 730;
	prop4.h = 30;
	if(editName == nullptr)  editName = display.createControl<cEdit>(&prop4);


}


void cProjectEditor::destroyDisplayControls()
{
	for(uint8_t i = 0; i<4; i++)
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
	//lista
	display.setControlHide(fileListControl);

	// top label listy
	//display.setControlText(PE->topLabel[0], "");
	display.setControlHide(topLabel[0]);
	display.refreshControl(topLabel[0]);

	// bottom labels
	display.setControlText(bottomLabel[0], "New");
	display.setControlText(bottomLabel[1], "Open");
	display.setControlText(bottomLabel[4], "Save");

	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(bottomLabel[i]);
	}


	display.setControlHide(keyboardControl);
	display.refreshControl(keyboardControl);

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
	display.setControlText(bottomLabel[0], "Open");
	display.setControlText(bottomLabel[1], "Cancel");

	display.refreshControl(bottomLabel[0]);
	display.refreshControl(bottomLabel[1]);

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
	display.setControlText(bottomLabel[0], "Create");
	display.setControlText(bottomLabel[1], "Cancel");
	display.setControlText(bottomLabel[4], "New");

	display.refreshControl(bottomLabel[0]);
	display.refreshControl(bottomLabel[1]);
	display.refreshControl(bottomLabel[4]);

	display.synchronizeRefresh();

}


void cProjectEditor::showEnterNameKeyboard()
{

	display.setControlHide(fileListControl);
	display.refreshControl(fileListControl);

// top label listy
	display.setControlHide(topLabel[0]);
	display.refreshControl(topLabel[0]);

// bottom labels
	display.setControlText(bottomLabel[0], "Create");
	display.setControlText(bottomLabel[1], "Cancel");
	display.setControlText(bottomLabel[4], "New");

	display.refreshControl(bottomLabel[0]);
	display.refreshControl(bottomLabel[1]);
	display.refreshControl(bottomLabel[4]);


	showKeyboard();
	showKeyboardEditName();


	display.synchronizeRefresh();


}


void cProjectEditor::showKeyboard()
{

	if(keyboardShiftFlag) display.setControlValue(keyboardControl, keyboardPosition + 42);
	else display.setControlValue(keyboardControl, keyboardPosition);

	display.setControlShow(keyboardControl);
	display.refreshControl(keyboardControl);
}

void cProjectEditor::hideKeyboard()
{
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
