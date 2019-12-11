#ifndef INTERFACE_SAMPLEIMPORTER_H_
#define INTERFACE_SAMPLEIMPORTER_H_


#include <modulesBase.h>
#include "SD.h"

#include "mtStructs.h"

#define PREVIOUS_POSITION_LIFO	25
#define MAX_SELECT				48

enum
{
	listFiles,
	listInstruments,
};

class cSampleImporter: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cSampleImporter() {}
	virtual ~cSampleImporter() {}

	void showDefaultScreen();
	void deactivateGui();
	void showFilesTree();
	void showInstrumentsList();
	void refreshInstrumentsList();
	void showMemoryUsage();
	void showLoadHorizontalBar();
	void showCopyingHorizontalBar();
	void showDeletingHorizontalBar(uint8_t progress);
	void showOpeningHorizontalBar(uint8_t progress);
	void hideHorizontalBar();
	void activateLabelsBorder();

	strList explorerList;
	strList instrumentList;

	strFrameData frameData;



	hControl label[8] = {nullptr};



	hControl folderListControl = nullptr;
	hControl fileListControl = nullptr;
	hControl instrumentListControl = nullptr;
	hControl memoryBarControl = nullptr;
	hControl frameControl = nullptr;

	hControl explorerListControl = nullptr;

	hControl bgLabel;


	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;

	hControl loadHorizontalBarControl = nullptr;

	hControl editName;
	hControl keyboardControl;


	uint8_t selectedPlace = 0;

	void setDefaultScreenFunct();


	uint8_t changeFileSelection(int16_t value);
	uint8_t changeInstrumentSelection(int16_t value);


//	void checkWavetableLabel();
	void AddEnterOrRename();

	void AddNextControl();
	void AddNextDisplay(uint8_t active);
//	void showAddWT();
//	void hideAddWT();

//--------------------------------------------------------------
	FsFile sdLocation;
	static const uint8_t list_length_max = 100;
	char actualPath[255] = {'/',0};
	uint8_t dirLevel = 0;

	void BrowseOrAdd();
	void SelectFile();
	void goUpInActualPath();

// foldery
	uint8_t checkIfNameValid(char * name);
	void listOnlyFolderNames(char* path);

/*	uint16_t locationFolderCount;
	char locationFolderList[list_length_max][40];
	char *folderNames[list_length_max];
	uint8_t selectedFolder = 0;*/
	//char *folderPath;

// pliki wave w wybranym folderze


	void listOnlyWaveNames(char* folder,uint8_t startPoint);
	void listOnlyWavFromActualPath(uint8_t startPoint);
	uint8_t isWavFile(char* fileName);

	uint16_t locationExplorerCount;
	char locationExplorerList[list_length_max][40];
	char *explorerNames[list_length_max];
	uint8_t selectedFile = 0;
	char copyingInfo[20];
//--------------------------------------------------------------

// instrumenty
	void listInstrumentSlots();
	// przenisone do interfaceGlobals
	//char slotNames[INSTRUMENTS_COUNT][SAMPLE_NAME_SIZE+4];
	//char *ptrSlotNames[INSTRUMENTS_COUNT];
	uint8_t selectedSlot = 0;

//	uint16_t instrumentsCount;
//	char instrumentsList[list_length_max][20];
//	char *instrumentsNames[list_length_max];
//	uint8_t selectedInstrument;


//--------------------------------------------------------------
//memory

	void calculateLoadProgress();
	void calculateCopyingProgress();
	uint8_t memoryUsage;
	strCompareValue memoryUsageAdd;
	const strCompareValue memoryUsageAddOnMemoryFull = {100};
	int32_t currentSelectMemorySize;
	uint8_t fullMemoryFlag = 0;
	uint8_t loadProgress = 0;
	uint8_t copyingProgress = 0;
	uint8_t lastCopyStatusFlag = 0;
	uint8_t currentCopyStatusFlag = 0;
	//uint8_t copyQueue = 0;
	uint8_t copyElement=0;
	uint8_t sampleType = 0;
	uint8_t copyElementMax;
	uint8_t firstMemBarLoadFlag=0;

	uint8_t lastLoadStatusFlag = 0;
	uint8_t currentLoadStatusFlag = 0;
//--------------------------------------------------------------
//odsluch
	void playSdFile();
	void playSampleFromBank();
	void stopPlaying();


	uint8_t playMode;

	enum
	{
		playModeStop,
		playModeSdFile,
		playModeSampleBank,

	};



//----------------------------------
// aktualny instrument na belce tytułowej
	void showActualInstrument();
//char actualInstrName[SAMPLE_NAME_SIZE+4];
	void showFileList();
	void listAllFoldersFirst();
	void rewindListToBeggining();
	void moveInstrListToEnd();

	uint8_t explorerPositionTable[PREVIOUS_POSITION_LIFO];
	uint8_t explorerCurrentPosition;

	uint8_t selectionActive[2]={0,0};
	uint8_t shiftPressed;
	uint8_t selectionTab[2][255];
	uint8_t selectionLength[2];
	uint8_t currSelectPlace=0;

	uint8_t instrCopied=0;
	uint8_t instrActiveInSel =0;

	void updateSelection();

	uint8_t handleSelecting(uint8_t selectStart, uint8_t selectMax, int16_t value,uint8_t fileCheck);

	void cancelSelect(uint8_t placeToCancel);
	int16_t getSelectionStart(uint8_t whichSelect);

	bool checkIfValidSelection(uint8_t positionToCheck);


	void handleMemoryBar();
	void calculateFileMemUsage();
	void calculateMemUsage();

	uint32_t currentFolderMemoryFileUsage[255];
//	uint8_t currentFolderIsWavetableFlag[255];

	void setSelect();
	void frameSelectMode(uint8_t place,uint8_t mode);

	void displayDelete(uint8_t onOff);
	uint8_t instrCopyStart;

	uint8_t copyType;//0- no copy ,1- sd to intr,2 -instr to instr

	char instrToCreate[15];
	char *parseNewName(uint8_t num);
	uint8_t willCopyInstrFit(uint8_t length);
	uint8_t getActiveInstrInSelection();
	void resetInstrSel();

	void handleSequenceCopyingLoading();

	char name[33];
	uint8_t keyboardPosition;
	int8_t editPosition;
	void showKeyboard();
	void hideKeyboard();
	void showKeyboardEditName();
	void hideKeyboardEditName();
	void showEnterNameKeyboard();
	void showRenameKeyboard();

	uint8_t keyboardActiveFlag = 0;
	uint8_t keyboardShiftFlag = 0;
	uint8_t lastPressedPad;

	uint8_t isBusy;

	uint8_t deleteInProgress = 0;
	uint8_t deleteStart = 0;
	uint8_t deleteCurrentPos = 0;
	uint8_t instrToDelete = 0;
	uint8_t instrDeleted = 0;
	void processDeleting();

	uint8_t openingInProgress = 0;
	uint8_t openCalcStart = 0;
	uint8_t openCalcEnd = 0;
	uint8_t openCurrentPos =0;
	void processDirFileSizes();


	void previewColorControl();
	void renameColorControl();
	void deleteColorControl();

	bool checkIfAnyInstrActive();

	uint8_t addOrReplaceFlag = 0;

};





extern cSampleImporter sampleImporter;




#endif /* INTERFACE_SAMPLEIMPORTER_H_ */
