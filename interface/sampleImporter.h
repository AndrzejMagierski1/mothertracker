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
	void showFolderTree();
	void showFilesTree();
	void showInstrumentsList();
	void showMemoryUsage();
	void showLoadHorizontalBar();
	void showCopyingHorizontalBar();
	void activateLabelsBorder();

	strList folderList;
	strList fileList;
	strList instrumentList;

	strFrameData frameData;


	hControl topLabel[4];
	hControl bottomLabel[4];

	hControl folderListControl;
	hControl fileListControl;
	hControl instrumentListControl;
	hControl memoryBarControl;
	hControl addMemoryBarControl;
	hControl frameControl;

	hControl loadHorizontalBarControl;


	uint8_t selectedPlace = 0;

	void setDefaultScreenFunct();


	uint8_t changeFolderSelection(int16_t value);
	uint8_t changeFileSelection(int16_t value);
	uint8_t changeInstrumentSelection(int16_t value);

//--------------------------------------------------------------
	FsFile sdLocation;
	static const uint8_t list_length_max = 100;
	char actualPath[255] = {0};
	uint8_t dirLevel;

	void BrowseFolder();
	void SelectFile();
	void goUpInActualPath();

// foldery
	void listOnlyFolderNames(char* path);

	uint16_t locationFolderCount;
	char locationFolderList[list_length_max][20];
	char *folderNames[list_length_max];
	uint8_t selectedFolder = 0;
	//char *folderPath;

// pliki wave w wybranym folderze


	void listOnlyWaveNames(char* folder);
	void listOnlyWavFromActualPath();
	uint8_t isWavFile(char* fileName);

	uint16_t locationFileCount;
	char locationFileList[list_length_max][20];
	char *fileNames[list_length_max];
	uint8_t selectedFile = 0;

//--------------------------------------------------------------

// instrumenty
	void listInstrumentSlots();
	char slotNames[INSTRUMENTS_COUNT][36];
	char *ptrSlotNames[INSTRUMENTS_COUNT];
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
	uint8_t memoryUsageAdd;
	uint32_t currentSelectMemorySize;
	uint8_t fullMemoryFlag = 0;
	uint8_t loadProgress = 0;
	uint8_t loadFlag = 0;
	uint8_t copyingProgress = 0;
	uint8_t lastCopyStatusFlag = 0;
	uint8_t currentCopyStatusFlag = 0;
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

	uint32_t barColorsRed[3] = { 0xFF0000, 0xFF0000, 0x000000};
	uint32_t defaultColors[3] =
	{
		0xFFFFFF, // tekst
		0x222222, // tło
		0xFFFFFF, // ramka
	};
	uint32_t barColorsCyan[3] = { 0x00FFFF, 0x00FFFF, 0x00FFFF};
};





extern cSampleImporter sampleImporter;




#endif /* INTERFACE_SAMPLEIMPORTER_H_ */
