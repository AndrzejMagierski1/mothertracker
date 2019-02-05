#ifndef MT_STEP_EDITOR_H
#define MT_STEP_EDITOR_H

#include "mtDisplay.h"
#include <stdint.h>

#include "SD.h"
#include "mtStructs.h"



enum enumMtStepEditorButtonFunction
{
	mtStepEditorButtonFunctionNone,
	mtStepEditorButtonFunctionPlay,
	mtStepEditorButtonFunctionStop,


	//-------------------------------
	mtStepEditorButtonFunctionCount
};

enum enumMtStepEditorPotFunction
{
	mtStepEditorPotFunctionNone,
	mtStepEditorPotFunctionStepParams,
	mtStepEditorPotFunctionStepFx1,
	mtStepEditorPotFunctionStepFx2,
	mtStepEditorPotFunctionStepFx3,
	mtStepEditorPotFunctionStepFx4,


	//-------------------------------
	mtStepEditorPotFunctionCount
};

enum enumMtStepEditorValue
{
	mtStepEditorValueNone,


	//-------------------------------
	mtStepEditorPotValueCount
};


enum mtStepEditorEvents
{
	mtStepEditorEventNone,
	mtStepEditorEventSeqButtonsPress,
	mtStepEditorEventPadPress,

	mtStepEditorEventCount
};

const uint16_t stepEditorPotsFuncRes[mtStepEditorPotFunctionCount] =
{
		100, // mtStepEditorPotFunctionNone,
		100, //	mtStepEditorPotFunctionStepParams,
		100, //	mtStepEditorPotFunctionStepFx1,
		100, //	mtStepEditorPotFunctionStepFx2,
		100, //	mtStepEditorPotFunctionStepFx3,
		100  //	mtStepEditorPotFunctionStepFx4,
};


const char cddcvyjkhLabels[4][20]=
{
		"1-Shot",
		"Forward Loop",
		"Backward Loop",
		"Pingpong Loop",
};



class cMtStepEditor
{
public:
	void update();


	void showStep(uint8_t track, uint8_t step);
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

	void play(uint8_t value);
	void stop(uint8_t value);


	//funkcje potow




	uint8_t	refreshStepEditor = 0;
	uint8_t	stepEditorModeStart = 0;
	uint8_t	editMode = 0;

//========================================================

	uint8_t actualTrack;
	uint8_t actualStep;


	uint8_t labelsChanged;





	uint8_t instrumentListEnabled = 0;
	char *instrumentNames[INSTRUMENTS_MAX];




//========================================================

	//przyciski w edytorze
	char *buttonLabels[5];
	uint8_t buttonFunction[5];

	char mtStepEditorButtonsLabels[mtStepEditorButtonFunctionCount][20] =
	{
		{0},
		"Play",
		"Stop",

	};

	//potencjometry w edytorze
	char *potsLabels[5];
	uint8_t potFunction[5];

	char mtStepEditorPotsLabels[mtStepEditorPotFunctionCount][20] =
	{
		{0},
		"Params",
		"Fx1",
		"Fx2",
		"Fx3",
		"Fx4"

	};

};


void stepEditorEvent(uint8_t event, void* param);

extern cMtStepEditor mtStepEditor;

#endif
