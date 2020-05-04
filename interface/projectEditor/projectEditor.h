#ifndef INTERFACE_PROJECTEDITOR_H_
#define INTERFACE_PROJECTEDITOR_H_


#include "core/modulesBase.h"
#include "SD.h"
#include "mtKeyboardManager.h"


enum mtProjectStartMode
{
	mtProjectStartModeDoNothing,
	mtProjectStartModeOpenLast,
	mtProjectStartModeOpenProject,
	mtProjectStartModeNewProject,

};



class cProjectEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cProjectEditor() {}
	virtual ~cProjectEditor() {}

//-------------------------------------------------------------
	void showDefaultScreen();
	void showProjectsList();
	void showModsList();
//	void showTemplatesList();


	void setDefaultScreenFunct();
	void setExportWindowFuncts();
//-------------------------------------------------------------
	void processProjectList();
	void processModsList();

	uint8_t loadSamplesBank();

	uint8_t loadProjectValues();
//-------------------------------------------------------------

	void createNewProject();
	void openProject();
	void saveProject();
	void saveAsProject();





//-------------------------------------------------------------
	strLabelData labelArrow;
	strList projectList;
	strFrameData frameData;

	hControl label[8] = {};
	hControl editName = nullptr;
	hControl fileListControl = nullptr;
	hControl keyboardControl = nullptr;
	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl titleLabelProjectName = nullptr;
	hControl instrumentLabel = nullptr;
	//hControl popupWindowLabel = nullptr;
	hControl loadHorizontalBarControl = nullptr;
	hControl popupLabel = nullptr;
	hControl coverImg = nullptr;
	hControl processControl = nullptr;
	hControl bgLabel  = nullptr;


	mtKeyboardManager keyboardManager;



	//SdDir sdLocation;
	//uint16_t projectsfoundCount;
	//static const uint8_t files_list_length_max = 100;
	//char locationFilesList[files_list_length_max][40];
	//char* filesNames[files_list_length_max];
	uint8_t selectedProject;
	char** projectsList;
	uint8_t projectsListLength;

	uint8_t selectedMod;
	char** modsList;
	uint8_t modsListLength;


//-------------------------------------------------------------
	void showEnterNameKeyboard();

//-------------------------------------------------------------
	void functShowSaveLastWindow();
	void functShowSaveLastWindowBeforeOpen();
	void functShowSaveLastWindowBeforeImportMod();
	void showSaveLastWindow();
	void functShowOverwriteWindow();

	void showDeleteProjectLastWindow();
	void showDeleteModLastWindow();


/*	void showOpeningHorizontalBar();
	void showSaveingHorizontalBar();*/
	void showExportingHorizontalBar();
	void showSaveAsKeyboard();
	void showOverwriteWindow();
	void showStopPatternWindow();
	void setStopPatternFunct();
	void showExportWindow();
	void showLabelDuringExport();

	//*****************************************General popouts
	uint32_t lastRefreshTime = 0;
	bool isProcessingOn;
	void showProcessingPopup(const char *text);
	void hideProcessingPopup();
	void refreshProcessingPopup();
//*****************************************newProjectPopup
/*	void showPopupLabelNewProject();
	void hidePopupLabelNewProject();*/
	elapsedMillis newProjectPopupDelay;
	uint8_t newProjectPopupFlag;
//********************************************************
//*****************************************savePopup
/*	void showPopupLabelSave();
	void hidePopupLabelSave();*/
	elapsedMillis savePopupDelay;
	uint8_t savePopupFlag;
	//uint8_t prepareSaveValue;
//********************************************************
//*****************************************openPopup
/*	void showPopupLabelOpen();
	void hidePopupLabelOpen();*/
	elapsedMillis openPopupDelay;
	uint8_t openPopupFlag;
	//uint8_t prepareOpenValue;
//********************************************************

	elapsedMillis deletePopupDelay;
	uint8_t deletePopupFlag;



	uint8_t newProjectNotSavedFlag = 0;
	uint8_t createNewProjectFlag = 0;
	//uint8_t currentOpeningStatus = 0;
	uint8_t openInProgressFlag = 0;
	//uint8_t lastOpeningStatus = 0;
	//uint8_t openingProgress = 0;
	uint8_t saveInProgressFlag = 0;
	uint8_t deleteInProgressFlag = 0;
	uint8_t newProjectOnSaveEndFlag = 0;
	uint8_t openOnSaveEndFlag = 0;
	//uint8_t saveingProgress = 0;
	uint8_t projectListActiveFlag = 0;
	uint8_t modsListActiveFlag = 0;
	char currentInfo[100];
	char currentPatchProjectName[PATCH_SIZE];


	uint8_t exportProgress = 0;
	uint8_t currentExportType; //0 - song, 1 - song stems, 2 - pattern, 3 - pattern stems
	uint8_t currentExportState = 0;
	uint8_t lastExportState = 0;

	enum stopAction
	{
		stopActionOpen = 1,
		stopActionNewProject,
		stopActionExport,
		stopActionImportMod,
		stopActionExportIt
	};
	uint8_t stopAction = 0;

	// cover projektu
	uint8_t refreshCover = 0;
	elapsedMillis coverDelay;
	char projectCoverName[PROJECT_NAME_SIZE];
	void hideProjectCover();
	void refreshProjectCover(uint16_t delay_ms);
};





extern cProjectEditor projectEditor;




#endif /* INTERFACE_PROJECTEDITOR_H_ */
