
#include "projectEditor/projectEditor.h"

#include "core/interfacePopups.h"

#include "mtLED.h"
#include "fileManager.h"
//#include "mtFileManager.h"
#include "mtExporterWAV.h"
#include "display.h"

static uint32_t popUpLabelColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFF0000, // ramka
};



constexpr uint32_t coverRamAddres = 670000;

void cProjectEditor::initDisplayControls()
{


	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY | controlStyleFont4);
	prop2.x = 9;
	prop2.y = 13;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleCenterY | controlStyleRightX | controlStyleFont4);
	prop2.x = 769;
	prop2.y = 12;
	if(titleLabelProjectName == nullptr) titleLabelProjectName = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 2;
	prop2.y = 0;
	prop2.w = 795;
	prop2.h = 26;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);

//	strControlProperties prop5;
//
//	prop5.x = 400;
//	prop5.colors = popUpLabelColors;
//	prop5.y = 300;
//
//	prop5.h = 100;
//	prop5.w = 800-(10);
//	prop5.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleFont2 | controlStyleRoundedBorder);
//	prop5.text = (char*)"";
//	if(popupWindowLabel == nullptr)  popupWindowLabel = display.createControl<cLabel>(&prop5);

	labelArrow.bitmaps[0].bitmapIndex = displayArrowU;
	labelArrow.bitmaps[0].xValue =  (800/8)*0+(800/16);
	labelArrow.bitmaps[0].yValue = 460;
	labelArrow.bitmaps[1].bitmapIndex = displayArrowD;
	labelArrow.bitmaps[1].xValue =  (800/8)*1+(800/16);
	labelArrow.bitmaps[1].yValue = 460;

	for(uint8_t i = 0; i<8; i++)
	{
		prop2.value =  1;
		prop2.colors = interfaceGlobals.activeButtonLabelsColors;
		prop2.style = controlStyleCenterX;
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 424;
		prop2.h =  55;

		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);
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

	projectList.linesCount = 13;
	projectList.start = 0;
	projectList.length = 0;
	strControlProperties prop;
	prop.x = 1;
	prop.y = 29;
	prop.w = (800/8*3)-3;
	prop.h = 394;
	prop.style = controlStyleBackground;
	prop.data = &projectList;
	if(fileListControl == nullptr)  fileListControl = display.createControl<cList>(&prop);


	strControlProperties prop3;
	prop3.x = 13;
	prop3.y = 143;
	prop3.w = 780;
	prop3.h = 260;
	if(keyboardControl == nullptr)  keyboardControl = display.createControl<cKeyboard>(&prop3);

	strControlProperties prop4;
	prop4.text = (char*)"";
	prop4.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleFont2);
	prop4.x = 400;
	prop4.y = 29;
	prop4.w = 795;
	prop4.h = 90;
	if(editName == nullptr)  editName = display.createControl<cEdit>(&prop4);


	strControlProperties prop6;

	prop6.x = 190;
	prop6.y = 170;
	prop6.style = controlStyleValue_0_100;
	prop6.h = 100;
	prop6.w = 420;

	if(loadHorizontalBarControl == nullptr)  loadHorizontalBarControl = display.createControl<cHorizontalBar>(&prop6);

	if(processControl == nullptr) processControl = display.createControl<cProcessingPop>(&prop6);

	strControlProperties prop7;

	prop7.x = 400;
	prop7.y = 220;
	prop7.colors = popUpLabelColors;
	prop7.style = ( controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleFont1 | controlStyleRoundedBorder);
	prop7.h = 100;
	prop7.w = 420;
	prop7.text = (char*)"";

	if(popupLabel == nullptr)  popupLabel = display.createControl<cLabel>(&prop7);


	strControlProperties prop8;

	prop8.x = 500;
	prop8.y = 220;
	prop8.value = coverRamAddres;
	prop8.style = (controlStyleCenterX | controlStyleCenterY);

	if(coverImg == nullptr)  coverImg = display.createControl<cImg>(&prop8);
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
		display.destroyControl(label[i]);
		label[i] = nullptr;
	}

	display.destroyControl(bgLabel);
	bgLabel = nullptr;


	display.destroyControl(fileListControl);
	fileListControl = nullptr;

	display.destroyControl(keyboardControl);
	keyboardControl = nullptr;

	display.destroyControl(editName);
	editName = nullptr;

//	display.destroyControl(popupWindowLabel);
//	popupWindowLabel = nullptr;

	display.destroyControl(loadHorizontalBarControl);
	loadHorizontalBarControl = nullptr;

	display.destroyControl(popupLabel);
	popupLabel = nullptr;

	display.destroyControl(coverImg);
	coverImg = nullptr;

	display.destroyControl(processControl);
	processControl = nullptr;


	mtPopups.hideInfoPopup();
}

void cProjectEditor::showDefaultScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "File");
	display.refreshControl(titleLabel);

	strcpy(projectCoverName, newFileManager.getCurrentProjectName());
	display.setControlText(titleLabelProjectName, newFileManager.getCurrentProjectName());
	display.refreshControl(titleLabelProjectName);

	display.setControlData(label[0], &labelArrow);
	display.setRemoveControlStyle(label[0], controlStyleShowBitmap);


	display.setControlPosition(label[0], (800/8)*0+(800/16), -1);
	display.setControlSize(label[0], 800/8-6, -1);

	//lista
	display.setControlHide(fileListControl);

	display.setControlValue(label[0], 1);
	display.setControlValue(label[1], 1);
	display.setControlValue(label[2], 1);
	display.setControlValue(label[3], 1);
	display.setControlValue(label[4], 1);
	display.setControlValue(label[5], 1);
	display.setControlValue(label[6], 1);
	display.setControlValue(label[7], 1);


	display.setControlText(label[0], "New");
	display.setControlText(label[1], "Open");
	display.setControlText(label[2], "Import mod");
	display.setControlText(label[3], "");
	display.setControlText(label[4], "Save");
	display.setControlText(label[5], "Save As");
	display.setControlText(label[6], "Export");
	display.setControlText(label[7], "");


	display.setControlText2(label[0], "Project");
	display.setControlText2(label[1], "");
	display.setControlText2(label[2], "");
	display.setControlText2(label[3], "");
	display.setControlText2(label[4], "");
	display.setControlText2(label[5], "");
	display.setControlText2(label[6], "");
	display.setControlText2(label[7], "");

	for(uint8_t i = 0; i<8; i++)
	{

		display.setControlColors(label[i], interfaceGlobals.activeButtonLabelsColors);
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.setControlValue(bgLabel, 255);
	display.refreshControl(bgLabel);

	keyboardManager.deactivateKeyboard();
//	display.setControlHide(keyboardControl);
//	display.refreshControl(keyboardControl);

	display.setControlHide(editName);
	display.refreshControl(editName);


 	mtPopups.hideInfoPopup();
	//display.setControlHide(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	display.setControlHide(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);

//	display.setControlHide(popupLabel);
//	display.refreshControl(popupLabel);


	refreshProjectCover(200);

	display.synchronizeRefresh();
}


//==============================================================================================================
void cProjectEditor::showProjectsList()
{
// lista

	projectList.start = selectedProject;
	projectList.length = projectsListLength;
	projectList.linesCount = 13;
	projectList.data = projectsList;

	display.setControlData(fileListControl,  &projectList);
	display.setControlShow(fileListControl);
	display.refreshControl(fileListControl);

	display.setAddControlStyle(label[0], controlStyleShowBitmap);

	for(uint8_t i = 0; i < 8 ; i++)
	{
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");
	}

// bottom labels

	display.setControlPosition(label[0], (800/4)*0+(800/8), -1);
	display.setControlSize(label[0], 800/4-6, -1);
	display.setControlText(label[0], "Projects");
	display.setControlText(label[2], "Delete");
	display.setControlText(label[7], "Open");
	display.setControlText(label[6], "Cancel");
	display.setControlText(label[5], "");

	for(uint8_t i = 0; i < 8 ; i++)
	{
		display.refreshControl(label[i]);
	}

	display.setControlValue(bgLabel, 252);
	display.refreshControl(bgLabel);

	display.synchronizeRefresh();
}
void cProjectEditor::showModsList()
{
// lista

	projectList.start = selectedMod;
	projectList.length = modsListLength;
	projectList.linesCount = 13;
	projectList.data = modsList;

	display.setControlData(fileListControl,  &projectList);
	display.setControlShow(fileListControl);
	display.refreshControl(fileListControl);

	display.setAddControlStyle(label[0], controlStyleShowBitmap);

	for(uint8_t i = 0; i < 8 ; i++)
	{
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");
	}

// bottom labels

	display.setControlPosition(label[0], (800/4)*0+(800/8), -1);
	display.setControlSize(label[0], 800/4-6, -1);
	display.setControlText(label[0], "Projects");
	display.setControlText(label[2], "Delete");
	display.setControlText(label[7], "Open");
	display.setControlText(label[6], "Cancel");
	display.setControlText(label[5], "");

	for(uint8_t i = 0; i < 8 ; i++)
	{
		display.refreshControl(label[i]);
	}

	display.setControlValue(bgLabel, 252);
	display.refreshControl(bgLabel);

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

void cProjectEditor::showProcessingPopup(const char *text)
{
	if(isProcessingOn == 0)
	{
		display.setControlText(processControl, text);
		display.setControlShow(processControl);
		display.refreshControl(processControl);
		lastRefreshTime = millis();
		isProcessingOn = 1;
	}
}

void cProjectEditor::hideProcessingPopup()
{
	if(isProcessingOn == 1)
	{
		display.setControlHide(processControl);
		display.refreshControl(processControl);
		isProcessingOn = 0;
	}
}

void cProjectEditor::refreshProcessingPopup()
{
	if(isProcessingOn)
	{
		if((millis() - lastRefreshTime) > 250)
		{
			display.refreshControl(processControl);
			lastRefreshTime = millis();
		}
	}
}

/*void cProjectEditor::showPopupLabelNewProject()
{
	display.setControlText(popupLabel, "Creating new project...");
	display.setControlShow(popupLabel);
	display.refreshControl(popupLabel);
}
void cProjectEditor::hidePopupLabelNewProject()
{
	display.setControlHide(popupLabel);
	display.refreshControl(popupLabel);
}

void cProjectEditor::showPopupLabelSave()
{
	display.setControlValue(loadHorizontalBarControl, prepareSaveValue);
	display.setControlText(loadHorizontalBarControl, "Save project...");
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}
void cProjectEditor::hidePopupLabelSave()
{
	display.setControlHide(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

void cProjectEditor::showPopupLabelOpen()
{
	display.setControlValue(loadHorizontalBarControl, prepareOpenValue);
	display.setControlText(loadHorizontalBarControl, "Opening project...");
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}
void cProjectEditor::hidePopupLabelOpen()
{
	display.setControlHide(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}*/



void cProjectEditor::showEnterNameKeyboard()
{
	display.setControlHide(fileListControl);
	display.refreshControl(fileListControl);

// top label listy
	//display.setControlHide(topLabel[0]);
	//display.refreshControl(topLabel[0]);

// bottom labels
	display.setControlValue(label[0], 0);

	display.setControlText(label[0], "Create");
	display.setControlText(label[1], "Cancel");
	display.setControlText(label[4], "Test");

	display.refreshControl(label[0]);
	display.refreshControl(label[1]);
	display.refreshControl(label[4]);

	display.synchronizeRefresh();
}

void cProjectEditor::showSaveAsKeyboard()
{
	display.setControlValue(label[0], 0);

	for(uint8_t i = 0; i < 8 ; i++)
	{
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");
	}

	display.setControlText(label[0],"Enter");
	display.setControlText(label[5],"Auto Name");
	display.setControlText(label[6],"Cancel");
	display.setControlText(label[7],"Save");

	for(uint8_t i = 0; i < 8 ; i++)
	{
		display.refreshControl(label[i]);
	}

	hideProjectCover();

	display.synchronizeRefresh();
}


void cProjectEditor::showSaveLastWindow()
{
	display.setControlValue(label[0], 0);

	for(uint8_t i = 0; i < 8; i++)
	{
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");
	}

	display.setControlText(label[5], "Cancel");
	display.setControlText(label[6], "Don't save");
	display.setControlText(label[7], "Save");

	sprintf(currentInfo,"\"%s\"?", newFileManager.getCurrentProjectName());

 	mtPopups.showInfoPopup("Do you want to save the changes to ", currentInfo);
	//display.setControlText(popupWindowLabel, currentInfo);
	//display.setControlShow(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.synchronizeRefresh();
}

void cProjectEditor::showDeleteProjectLastWindow()
{
	display.setControlValue(label[0], 0);

	display.setControlText(label[2], "");
	display.setControlText(label[5], "");
	display.setControlText(label[6], "Cancel");
	display.setControlText(label[7], "Delete");

	sprintf(currentInfo,"\"%s\"?", projectsList[selectedProject]);

 	mtPopups.showInfoPopup("Do you want to delete project:", currentInfo);
	//display.setControlText(popupWindowLabel, currentInfo);
	//display.setControlShow(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.synchronizeRefresh();
}
void cProjectEditor::showDeleteModLastWindow()
{
	display.setControlValue(label[0], 0);

	display.setControlText(label[2], "");
	display.setControlText(label[5], "");
	display.setControlText(label[6], "Cancel");
	display.setControlText(label[7], "Delete");

	sprintf(currentInfo,"\"%s\"?", modsList[selectedMod]);


 	mtPopups.showInfoPopup("Do you want to delete file:", currentInfo);
	//display.setControlText(popupWindowLabel, currentInfo);
	//display.setControlShow(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.synchronizeRefresh();
}

void cProjectEditor::showOverwriteWindow()
{
	display.setControlValue(label[0], 0);

	for(uint8_t i=1 ;i < 7; i++)
	{
		display.setControlText(label[i], "");
	}

	display.setControlText(label[7], "Overwrite");
	display.setControlText(label[6], "Cancel");

	sprintf(currentInfo,"\"%s\"?", keyboardManager.getName());

 	mtPopups.showInfoPopup("Do you want overwrite ", currentInfo);
//
//	display.setControlText(popupWindowLabel, currentInfo);
//	display.setControlShow(popupWindowLabel);
//	display.refreshControl(popupWindowLabel);




	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.synchronizeRefresh();
}

void cProjectEditor::showStopPatternWindow()
{

	for(uint8_t i=0 ;i < 8; i++)
	{
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");
	}

	display.setControlText(label[7], "Yes");
	display.setControlText(label[6], "No");

	//sprintf(currentInfo,"This action will stop the pattern. Do you want to continue?");

 	mtPopups.showInfoPopup("This action will stop the pattern.", "Do you want to continue?");
	//display.setControlText(popupWindowLabel, currentInfo);
	//display.setControlShow(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.synchronizeRefresh();
}

void cProjectEditor::showExportingHorizontalBar()
{

	display.setControlValue(loadHorizontalBarControl, exportProgress);
	if(currentExportType == (int) exportPattern )		sprintf(currentInfo,"Exporting Pattern %d", mtProject.values.actualPattern);
	else if (currentExportType == (int) exportPatternStems )
	{
		uint8_t track_n = exporter.getStemsTrack();
		if(track_n < 8) sprintf(currentInfo,"Exporting Pattern Stems: Track%d ", track_n + 1);
		else if(track_n == 8) strcpy(currentInfo,"Exporting Pattern Stems: Reverb ");
		else if(track_n == 9) strcpy(currentInfo,"Exporting Pattern Stems: Mix ");

	}
	else if (currentExportType == (int) exportSong ) 	sprintf(currentInfo,"Exporting Song");
	else if (currentExportType == (int) exportSongStems )
	{
		uint8_t track_n = exporter.getStemsTrack();
		if(track_n < 8) sprintf(currentInfo,"Exporting Song Stems: Track%d ", track_n + 1);
		else if(track_n == 8) strcpy(currentInfo,"Exporting Song Stems: Reverb ");
		else if(track_n == 9) strcpy(currentInfo,"Exporting Song Stems: Mix ");
	}

	display.setControlText(loadHorizontalBarControl, currentInfo);
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);

 	mtPopups.hideInfoPopup();
	//display.setControlHide(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);
}


void cProjectEditor::showExportWindow()
{
	display.setControlText(label[0], "Export");
	display.setControlText2(label[0], "Song");
	display.setControlText(label[1], "Export");
	display.setControlText2(label[1], "Song Stems");
	display.setControlText(label[2], "Export");
	display.setControlText2(label[2], "Pattern");
	display.setControlText(label[3], "Export");
	display.setControlText2(label[3], "Patt Stems");
	display.setControlText(label[4], "Export");
	display.setControlText2(label[4], "as .it");
	display.setControlText(label[5], "");
	display.setControlText(label[6], "");
	display.setControlText(label[7], "Cancel");

	for(uint8_t i=0; i<5 ; i++)
	{
		display.setControlValue(label[i], 1);
		display.refreshControl(label[i]);
	}

	for(uint8_t i = 5; i <8; i++)
	{
		display.setControlValue(label[i], 0);
		display.refreshControl(label[i]);
	}
	//refreshe są w make

	display.setControlHide(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);

 	mtPopups.hideInfoPopup();
	//display.setControlHide(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	display.synchronizeRefresh();
}

void cProjectEditor::showLabelDuringExport()
{
	display.setControlText(label[0], "");
	display.setControlText2(label[0], "");
	display.setControlText(label[1], "");
	display.setControlText2(label[1], "");
	display.setControlText(label[2], "");
	display.setControlText2(label[2], "");
	display.setControlText(label[3], "");
	display.setControlText2(label[3], "");
	display.setControlText(label[4], "");
	display.setControlText2(label[4], "");
	display.setControlText(label[5], "");
	display.setControlText(label[6], "");
	display.setControlText(label[7], "Cancel");

	for(uint8_t i=0; i<5 ; i++)
	{
		display.setControlValue(label[i], 1);
		display.refreshControl(label[i]);
	}

	for(uint8_t i = 5; i <8; i++)
	{
		display.setControlValue(label[i], 0);
		display.refreshControl(label[i]);
	}

	display.synchronizeRefresh();
}



void cProjectEditor::hideProjectCover()
{
	 refreshCover = 0;
	 display.setControlHide(coverImg);
}
