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

	// elementy modulu





	//funkcje przyciskow



	//funkcje potow




	uint8_t	refreshModule = 0;
	uint8_t	moduleStart = 0;


//========================================================
	// odswiezanie elementow modulu

	uint8_t labelsChanged;






//=======================================================================

//przyciski  ------------------------------------------------------------
	enum
	{
		buttonFunctNone,



		//-------------------------------
		buttonFunctCount
	};

	char *buttonLabels[5];
	uint8_t buttonFunctions[5];

	char buttonFunctionLabels[buttonFunctCount][20] =
	{
		{0},



	};



//potencjometry  ---------------------------------------------------------

	enum
	{
		potFunctNone,




		//-------------------------------
		potFunctCount
	};

	char *potLabels[5];
	uint8_t potFunctions[5];

	char potFunctionLabels[potFunctCount][20] =
	{
		{0},


	};

	const uint16_t potFuncRes[potFunctCount] =
	{
			100, // potFunctionNone,

	};

	const uint8_t potFuncAcc[potFunctCount] =
	{
			3, // potFunctionNone,

	};

};


void sampleBankEditorEvent(uint8_t event, void* param);

extern cMtSampleBankEditor mtSampleBankEditor;

#endif
