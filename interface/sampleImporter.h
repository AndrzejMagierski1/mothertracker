#ifndef INTERFACE_SAMPLEIMPORTER_H_
#define INTERFACE_SAMPLEIMPORTER_H_


#include <modulesBase.h>
#include "SD.h"

#include "mtStructs.h"



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
	void calculateMemoryUsage();
	void calculateCurrentSelectMemorySize();
	void calculateLoadProgress();
	void calculateCopyingProgress();
	uint8_t memoryUsage;
	strCompareValue memoryUsageAdd;
	const strCompareValue memoryUsageAddOnMemoryFull = {100};
	uint32_t currentSelectMemorySize;
	uint8_t fullMemoryFlag = 0;
	uint8_t loadProgress = 0;
	uint8_t loadFlag = 0;
	uint8_t copyingProgress = 0;
	uint8_t lastCopyStatusFlag = 0;
	uint8_t currentCopyStatusFlag = 0;
	uint8_t firstUpdateFlag = 1;
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

};





extern cSampleImporter sampleImporter;




#endif /* INTERFACE_SAMPLEIMPORTER_H_ */
