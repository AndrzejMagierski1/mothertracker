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

enum struct exportType
{
	song,
	songStems,
	pattern,
	patternStems
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
//	void showTemplatesList();

	void deactivateGui();
	void activateGui();

	void setDefaultScreenFunct();

//-------------------------------------------------------------

	uint8_t loadSamplesBank();

	uint8_t loadProjectValues();
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
	hControl selectWindowLabel = nullptr;
	hControl loadHorizontalBarControl = nullptr;
	hControl popupLabel = nullptr;
	hControl coverImg = nullptr;
	hControl processControl = nullptr;
	hControl bgLabel  = nullptr;


	mtKeyboardManager keyboardManager;

	void listOnlyFolderNames(const char* folder);

	SdFile sdLocation;
	uint8_t selectedLocation;
	uint16_t locationFilesCount;
	static const uint8_t files_list_length_max = 100;
	char locationFilesList[files_list_length_max][40];
	char *filesNames[files_list_length_max];

//-------------------------------------------------------------
	void showEnterNameKeyboard();

//-------------------------------------------------------------
	void functShowSaveLastWindow();
	void functShowSaveLastWindowBeforeOpen();
	void showSaveLastWindow();
	void functShowOverwriteWindow();

	void showDeleteLastWindow();


/*	void showOpeningHorizontalBar();
	void showSaveingHorizontalBar();*/
	void showExportingHorizontalBar();
	void showSaveAsKeyboard();
	void showOverwriteWindow();
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
	char currentInfo[100];
	char currentPatchProjectName[PATCH_SIZE];

	uint8_t isBusyFlag = 0;
	uint8_t exportInProgress = 0;
	uint8_t exportProgress = 0;
	uint8_t currentExportType; //0 - song, 1 - song stems, 2 - pattern, 3 - pattern stems
	uint8_t currentExportState = 0;
	uint8_t lastExportState = 0;


	// cover projektu
	uint8_t refreshCover = 0;
	elapsedMillis coverDelay;
	char projectCoverName[PROJECT_NAME_SIZE];
	void hideProjectCover();
	void refreshProjectCover(uint16_t delay_ms);
};





extern cProjectEditor projectEditor;




#endif /* INTERFACE_PROJECTEDITOR_H_ */
