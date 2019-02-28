#ifndef MT_SAMPLE_BANK_EDITOR_H
#define MT_SAMPLE_BANK_EDITOR_H

#include "mtDisplay.h"
#include <stdint.h>

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
	void stop();


	void setEventFunct(void (*func)(uint8_t, void*, void*, void*)) { eventFunct = func; };

	uint8_t padsChange(uint8_t type, uint8_t n, uint8_t velo);
	void buttonChange(uint8_t button, uint8_t value);
	void potChange(uint8_t pot, int16_t value);
	void seqButtonChange(uint8_t type, uint8_t x, uint8_t y);

private:

	void processLabels();


	void setButtonLabel(uint8_t number, char * label);
	void updateButtonsFunctions();
	void setButtonFunction(uint8_t number, uint8_t function);

	void setPotsLabel(uint8_t number, char * label);
	void updatePotsFunctions();
	void setPotFunction(uint8_t number, uint8_t function);

	void (*eventFunct)(uint8_t, void*, void*, void*);



	//funkcje przyciskow
	void importSample();
	void removeSample();
	void renameSample();
	void browseSelectSample();
	void browseOpenFolder();
	void browseBack();
	void browseCancel();

	//funkcje potow
	void changeSampleListPos(int16_t value);



	uint8_t	refreshModule = 0;
	uint8_t	moduleStart = 0;


//========================================================
	// odswiezanie elementow modulu

	uint8_t labelsChanged;


	uint8_t samplesListChanged;
	static const uint8_t samples_list_length_max = SAMPLES_MAX;
	const uint8_t samples_list_pos = 3;
//	uint8_t samplesListEnabled = 0;
	char *samplesNames[SAMPLES_MAX];
	uint16_t samplesCount;






//=======================================================================

//przyciski  ------------------------------------------------------------
	enum
	{
		buttonFunctNone,
		buttonFunctImportSample,
		buttonFunctRemoveSample,
		buttonFunctRenameSample,

		buttonFunctBrowseSelectSample,
		buttonFunctBrowseOpenFolder,
		buttonFunctBrowseBack,
		buttonFunctBrowseCancel,




		//-------------------------------
		buttonFunctCount
	};

	char *buttonLabels[5];
	uint8_t buttonFunctions[5];

	char buttonFunctionLabels[buttonFunctCount][20] =
	{
		{0},
		"Import",
		"Remove",
		"Rename",

		"Select",
		"Open folder",
		"Back",
		"Cancel",



	};


//potencjometry  ---------------------------------------------------------

	enum
	{
		potFunctNone,
		potFunctChangeSamplesListPos,



		//-------------------------------
		potFunctCount
	};

	char *potLabels[5];
	uint8_t potFunctions[5];

	char potFunctionLabels[potFunctCount][20] =
	{
		{0},
		"Sample Bank:",


	};

	const uint16_t potFuncRes[potFunctCount] =
	{
			100, // potFunctionNone,
			30, //

	};

	const uint8_t potFuncAcc[potFunctCount] =
	{
			3, // potFunctionNone,
			1, //

	};

};


void sampleBankEditorEvent(uint8_t event, void* param);

extern cMtSampleBankEditor mtSampleBankEditor;

#endif
