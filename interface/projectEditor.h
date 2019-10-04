#ifndef INTERFACE_PROJECTEDITOR_H_
#define INTERFACE_PROJECTEDITOR_H_


#include <modulesBase.h>
#include "SD.h"

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
//	void showTemplatesList();


	void setDefaultScreenFunct();

//-------------------------------------------------------------

	uint8_t loadSamplesBank();

	uint8_t loadProjectValues();
//-------------------------------------------------------------

	strList projectList;

	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl editName;
	hControl fileListControl;
	hControl keyboardControl;
	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl titleLabelProjectName = nullptr;
	hControl instrumentLabel = nullptr;
	hControl selectWindowLabel = nullptr;
	hControl loadHorizontalBarControl = nullptr;
	uint32_t topLabelColors[3] =
	{
			0x3F3F3F, // tekst
			0x000000, // tło
			0x3F3F3F, // ramka
	};


	uint32_t projectOptions = 0;

	void makeSmallBottomLabel(uint8_t i);
	void makeBigBottomLabel(uint8_t i);

	void listOnlyFolderNames(const char* folder);

	FsFile sdLocation;
	uint8_t selectedLocation;
	uint16_t locationFilesCount;
	static const uint8_t files_list_length_max = 100;
	char locationFilesList[files_list_length_max][40];
	char *filesNames[files_list_length_max];

//-------------------------------------------------------------


	char name[33];
	uint8_t keyboardPosition;
	int8_t editPosition;
	void showKeyboard();
	void hideKeyboard();
	void showKeyboardEditName();
	void hideKeyboardEditName();
	void showEnterNameKeyboard();

	uint8_t keyboardActiveFlag = 0;
	uint8_t keyboardShiftFlag = 0;
	uint8_t lastPressedPad;
//-------------------------------------------------------------
	void functShowSaveLastWindow();
	void functShowSaveLastWindowBeforeOpen();
	void showSaveLastWindow();
	void functShowOverwriteWindow();
	void showOpeningHorizontalBar();
	void showSaveingHorizontalBar();
	void showSaveAsKeyboard();
	void showOverwriteWindow();
	void showExportWindow();
	uint8_t newProjectNotSavedFlag = 0;
	uint8_t createNewProjectFlag = 0;
	uint8_t currentOpeningStatus = 0;
	uint8_t openInProgressFlag = 0;
	uint8_t lastOpeningStatus = 0;
	uint8_t openingProgress = 0;
	uint8_t saveInProgressFlag = 0;
	uint8_t newProjectOnSaveEndFlag = 0;
	uint8_t openOnSaveEndFlag = 0;
	uint8_t saveingProgress = 0;
	uint8_t currentSaveStatus = 0;
	uint8_t lastSaveStatus = 0;
	uint8_t projectListActiveFlag = 1;
	char currentInfo[100];
};





extern cProjectEditor projectEditor;




#endif /* INTERFACE_PROJECTEDITOR_H_ */
