#ifndef MT_SAMPLE_BANK_EDITOR_H
#define MT_SAMPLE_BANK_EDITOR_H

#include "display.h"
#include <stdint.h>
#include "SD.h"

#include "mtStructs.h"



enum mtSampleBankEditorEvents
{
	mtSampleBankEditorEventNone,
	mtSampleBankEditorEventSeqButtonsPress,
	mtSampleBankEditorEventPadPress,

	mtSampleBankEditorEventCount
};



class cMtSampleBankEditor
{
public:
	void update();


	void start();
	void startRecorder();
	void stop();


	void setEventFunct(void (*func)(uint8_t, void*, void*, void*)) { eventFunct = func; };

	uint8_t padsChange(uint8_t type, uint8_t n, uint8_t velo);
	void buttonChange(uint8_t button, uint8_t value);
	void potChange(uint8_t pot, int16_t value);
	void seqButtonChange(uint8_t type, uint8_t x, uint8_t y);

	uint8_t loadSamplesBank();

private:

	void processLabels();
	void processSpectrum();
	void processPoints();


	void setButtonLabel(uint8_t number, char * label);
	void updateButtonsFunctions();
	void setButtonFunction(uint8_t number, uint8_t function);

	void setPotsLabel(uint8_t number, char * label);
	void updatePotsFunctions();
	void setPotFunction(uint8_t number, uint8_t function);

	void (*eventFunct)(uint8_t, void*, void*, void*);

	strList filesList;
	strList samplesList;
	strList slotsList;
	hControl potControls[5];
	hControl buttonControls[5];
	hControl filesListControl;
	hControl samplesListControl;
	hControl slotsListControl;

	//funkcje przyciskow
	void importSample(uint8_t type);
	void removeSample();
	void renameSample();
	void browseSelectSample();
	void browseOpenFolder();
	void browseBack();
	void browseCancel();
	void browseSelectSlot();
	void saveRecording();
	void clearRecording();
	void trimRecording();


	//funkcje potow
	void changeSampleListPos(int16_t value);
	void changeFilesListPos(int16_t value);
	void changeSlotsListPos(int16_t value);

	void changeStartPoint(int16_t value);
	void changeStopPoint(int16_t value);
	void changeZoom(int16_t value);

	uint8_t	refreshModule = 0;
	uint8_t	moduleStart = 0;


//========================================================
	// odswiezanie elementow modulu

	uint8_t labelsChanged;
	uint8_t filesListChanged;
	uint8_t samplesListChanged;
	uint8_t slotListChanged;
	uint8_t spectrumChanged;
	uint8_t pointsChanged;

	static const uint8_t samples_list_length_max = SAMPLES_MAX;
	const uint8_t samples_list_pos = 3;
//	uint8_t samplesListEnabled = 0;
	char samplesNames[SAMPLES_COUNT][32];
	uint8_t samplesIndex[SAMPLES_COUNT];
	char *ptrSamplesNames[SAMPLES_COUNT];
	uint16_t samplesCount;
	uint8_t sampleListPos;
	uint8_t samplesListEnabled;

	//lista plikow/folderow---------------------------------------------
	static const uint8_t files_list_length_max = 100;
	const uint8_t files_list_pos = 0;
	uint8_t filesListEnabled = 0;

	char *filesNames[128];
	char filePath[255];
	char fileName[32];

	uint8_t dirLevel;
	uint8_t selectedFileType;
	uint8_t selectedLocation = 0;
	char locationFilesList[files_list_length_max][20];
	uint16_t locationFilesCount;
	uint8_t sampleType;

	FsFile sdLocation;

	//lista slotow---------------------------------------------
	const uint8_t slot_list_index = 1;
	uint8_t slotListEnabled = 0;

	char slotNames[SAMPLES_COUNT][36];
	char *ptrSlotNames[SAMPLES_COUNT];
	uint8_t selectedSlot;

	//-------------------------------------------------------------------

	void listOnlyDirAndWavFromActualPath();
	uint8_t isWavFile(char* fileName);
	void getSelectedFileType();

	void listSampleSlots();

	//-------------------------------------------------------------------
	// spectrum
	uint8_t spectrumEnabled;

	enum
	{
		recordingStatusClear,
		recordingStatusRecording,
		recordingStatusRecorded,
	};

	uint16_t zoomWidth = MAX_16BIT;
	int32_t zoomStart =  0;
	int32_t zoomEnd = MAX_16BIT;
	uint8_t lastChangedPoint = 0;
	float zoomValue = 1;
	uint16_t zoomPosition = 0;

	const uint16_t SPECTRUM_DRAW_DELAY_VALUE = 500;
	strMtDispSpectrum  spectrum;
	elapsedMillis spectrumDrawDelay;

	uint16_t startPoint;
	uint16_t stopPoint = MAX_16BIT;

	uint8_t recordingStatus = recordingStatusClear;

	//-------------------------------------------------------------------
	//ogolne
	uint8_t playMode = 0;
	enum
	{
		playModeStop,
		playModeSdFile,
		playModeSampleBank,

	};

	void play(uint8_t type);
	void playSdFile();
	void playSampleFromBank();
	void stopPlaying();


	//-------------------------------------------------------------------
	// recorder


//=======================================================================

//przyciski  ------------------------------------------------------------
	enum
	{
		buttonFunctNone,
		buttonFunctImportSample,
		buttonFunctImportWavetable,
		buttonFunctRemoveSample,
		buttonFunctRenameSample,

		buttonFunctBrowseSelectSample,
		buttonFunctBrowseOpenFolder,
		buttonFunctBrowseBack,
		buttonFunctBrowseCancel,

		buttonFunctSelectSampleSlot,

		buttonFunctSaveRecording,
		buttonFucntTrimRecording,
		buttonClearRecording,


		//-------------------------------
		buttonFunctCount
	};

	char *buttonLabels[5];
	uint8_t buttonFunctions[5];

	char buttonFunctionLabels[buttonFunctCount][20] =
	{
		{0},
		"Import sample",
		"Import wtable",
		"Remove",
		"Rename",

		"Select",
		"Open",
		"Back",
		"Cancel",

		"Select",

		"Save",
		"Trim",
		"Clear",


	};


//potencjometry  ---------------------------------------------------------

	enum
	{
		potFunctNone,
		potFunctChangeSamplesListPos,
		potFunctChangeFileListPos,
		potFunctChangeSlotListPos,

		potFunctChangeStartPoint,
		potFunctChangeStopPoint,
		potFunctChangeZoom,

		//-------------------------------
		potFunctCount
	};

	char *potLabels[5];
	uint8_t potFunctions[5];

	char potFunctionLabels[potFunctCount][20] =
	{
		{0},
		"Sample Bank",
		"Select file",
		"Select slot",

		"Start",
		"Stop",
		"Zoom",

	};

	const uint16_t potFuncRes[potFunctCount] =
	{
			100, // potFunctionNone,
			30, //
			30,
			30,

			100,
			100,
			100,

	};

	const uint8_t potFuncAcc[potFunctCount] =
	{
			0, // potFunctionNone,
			0, //
			0,
			0,

			3,
			3,
			3,
	};

};


void sampleBankEditorEvent(uint8_t event, void* param);

extern cMtSampleBankEditor mtSampleBankEditor;

#endif
