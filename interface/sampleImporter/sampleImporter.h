#ifndef INTERFACE_SAMPLEIMPORTER_H_
#define INTERFACE_SAMPLEIMPORTER_H_


#include "core/modulesBase.h"
#include "SD.h"
#include "mtKeyboardManager.h"
#include "mtStructs.h"

//#define PREVIOUS_POSITION_LIFO	25
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
	void showActualInstrument();
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

	strLabelData labelArrow[2];

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

	hControl editName = nullptr;
	hControl keyboardControl = nullptr;


	mtKeyboardManager keyboardManager;

	uint8_t selectedPlace = 0;

	void setDefaultScreenFunct();


	uint8_t changeFileSelection(int16_t value);
	uint8_t changeInstrumentSelection(int16_t value);


	void AddEnterOrRename();

	void AddNextControl();
	void AddNextDisplay(uint8_t active);


// coopy paste
	void copyInstruments();
	void pasteInstruments();

	uint8_t instrCopyStart;


//--------------------------------------------------------------
	void BrowseOrAdd();
	void importSamples();


// pliki wybranym folderze
	uint16_t locationExplorerCount = 0;
	char **explorerNames;
	uint8_t selectedFile = 0;
	char copyingInfo[20];
	uint32_t* currentFolderMemoryFileUsage;


//--------------------------------------------------------------

// instrumenty
	void listInstrumentSlots();
	uint8_t selectedSlot = 0;



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
	uint8_t copyElement=0;
	uint8_t sampleType = 0;
	uint8_t copyElementsCount;
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
	void showFileList();
	void rewindListToBeggining();
	void moveInstrListToEnd();


	static const uint8_t maxListLength = 100;
	uint8_t selectionActive[2]={0,0};
	uint8_t shiftPressed;
	uint8_t selectionTab[2][maxListLength];
	uint8_t selectionLength[2];
	uint8_t currSelectPlace=0;


	void updateSelection();

	uint8_t handleSelecting(uint8_t selectStart, uint8_t selectMax, int16_t value,uint8_t fileCheck);

	void cancelSelect(uint8_t placeToCancel);
	int16_t getSelectionStart(uint8_t whichSelect);
	int16_t getSelectionEnd(uint8_t whichSelect);

	bool checkIfValidSelection(uint8_t positionToCheck);


	void handleMemoryBar();
	void calculateFileMemUsage();
	void calculateMemUsage();
	char memoryUsageValue[8];



	void setSelect();
	void frameSelectMode(uint8_t place,uint8_t mode);

	void displayDelete(uint8_t onOff);

	char instrToCreate[15];
	char *parseNewName(uint8_t num);
	uint8_t willCopyInstrFit(uint8_t length);
	uint8_t getActiveInstrInSelection();
	void resetInstrSel();

	void handleSequenceCopyingLoading();

	void showRenameKeyboard();


	uint8_t folderIsChanged = 1;

	void previewColorControl();
	void renameColorControl();
	void deleteColorControl();

	bool checkIfAnyInstrActive();

	uint8_t addNextFlag = 0;
	uint8_t addNextOffset = 0;

};





extern cSampleImporter sampleImporter;




#endif /* INTERFACE_SAMPLEIMPORTER_H_ */
