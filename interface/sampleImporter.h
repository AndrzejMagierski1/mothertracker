#ifndef INTERFACE_SAMPLEIMPORTER_H_
#define INTERFACE_SAMPLEIMPORTER_H_


#include <modulesBase.h>
#include "SD.h"

#include "mtStructs.h"

#define PREVIOUS_POSITION_LIFO	25
#define MAX_SELECT				48

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
	void showFilesTree();
	void showInstrumentsList();
	void showMemoryUsage();
	void showLoadHorizontalBar();
	void showCopyingHorizontalBar();
	void activateLabelsBorder();

	strList explorerList;
	strList instrumentList;

	strFrameData frameData;



	hControl topLabel[6] = {nullptr};
	hControl bottomLabel[4] = {nullptr};



	hControl folderListControl = nullptr;
	hControl fileListControl = nullptr;
	hControl instrumentListControl = nullptr;
	hControl memoryBarControl = nullptr;
	hControl frameControl = nullptr;

	hControl explorerListControl = nullptr;



	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;

	hControl loadHorizontalBarControl = nullptr;


	uint8_t selectedPlace = 0;

	void setDefaultScreenFunct();


	uint8_t changeFileSelection(int16_t value);
	uint8_t changeInstrumentSelection(int16_t value);

	void AddOrEnter();

//--------------------------------------------------------------
	FsFile sdLocation;
	static const uint8_t list_length_max = 100;
	char actualPath[255] = {'/',0};
	uint8_t dirLevel = 0;

	void BrowseOrAdd();
	void SelectFile();
	void goUpInActualPath();

// foldery
	void listOnlyFolderNames(char* path, uint8_t startPoint);

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

	uint32_t barColorsRed[3] = { 0xFF0000, 0xFF0000, 0xFF0000};
	uint32_t defaultColors[3] =
	{
		0xFFFFFF, // tekst
		0xFF0000, // tło
		0x00FF00, // ramka
	};
	uint32_t barColorsCyan[3] = { 0x00FFFF, 0x00FFFF, 0x00FFFF};

//----------------------------------
// aktualny instrument na belce tytułowej
	void showActualInstrument();
//char actualInstrName[SAMPLE_NAME_SIZE+4];

	void listAllFoldersFirst();
	void rewindListToBeggining();

	uint8_t explorerPositionTable[PREVIOUS_POSITION_LIFO];
	uint8_t explorerCurrentPosition;

	uint8_t selectionActive;
	uint8_t shiftPressed;
	uint8_t selectionTab[255];
	uint8_t selectionLength;
	uint8_t currSelectPlace=0;


	void updateSelection();

	uint8_t handleSelecting(uint8_t selectStart, uint8_t selectMax, int16_t value,uint8_t fileCheck);

	void cancelSelect();
	int16_t getSelectionStart();

	bool checkIfValidSelection(uint8_t positionToCheck);


	void handleMemoryBar();
	void calculateFileMemUsage();
	void calculateMemUsage();

	uint32_t currentFolderMemoryFileUsage[255];

	void setSelect(uint8_t place);

	void displayDelete(uint8_t onOff);

	uint8_t instrCopyStart;

	uint8_t copyType;//0- no copy ,1- sd to intr,2 -instr to instr

	char instrToCreate[15];
	char *parseNewName(uint8_t num);
	uint8_t willCopyInstrFit(uint8_t length);
	void handleSequenceCopyingLoading();


};





extern cSampleImporter sampleImporter;




#endif /* INTERFACE_SAMPLEIMPORTER_H_ */
