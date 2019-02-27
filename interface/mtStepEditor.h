#ifndef MT_STEP_EDITOR_H
#define MT_STEP_EDITOR_H

#include "mtDisplay.h"
#include <stdint.h>

#include "SD.h"
#include "mtStructs.h"





enum mtStepEditorEvents
{
	mtStepEditorEventNone,
	mtStepEditorEventSeqButtonsPress,
	mtStepEditorEventPadPress,

	mtStepEditorEventCount
};


const char stepParamsLabels[4][20]=
{
		"Inst:",
		"Note:",
		"Length:",
		"Volume:",
};


const char stepFxLabels[3][20]=
{
		"Fx:",
		"Value1:",
		"Value2:",
};

const char stepEditStepFxNames[17][20]=
{
	"---",			//FX_TYPE_NONE,
	"Offset",			//FX_TYPE_OFFSET,
	"Glide",			//FX_TYPE_GLIDE,	// 	czas p
	"Slide",			//FX_TYPE_SLIDE,
	"Arp up",			//FX_TYPE_ARP_UP,
	"Arp dwn",			//FX_TYPE_ARP_DOWN,
	"SP",			//FX_TYPE_SP,
	"LP1",			//FX_TYPE_LP1,
	"LP2",			//FX_TYPE_LP2,
	"uTune",			//FX_TYPE_MICROTUNE,
	"Play mode",			//FX_TYPE_SAMPLE_PLAYMODE,
	"Jump step",			//FX_TYPE_JUMP_TO_STEP,
	"Jump patt",			//FX_TYPE_JUMP_TO_PATTERN,
	"Panning",			//FX_TYPE_PANNING,
	"Roll Pan",			//FX_TYPE_PANNING_ROLL,
	"Slice",			//FX_TYPE_SLICE_NUMBER,
	"Probability"			//FX_TYPE_PROBABILITY,
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
	void processStepParameters();



	void setButtonLabel(uint8_t number, char * label);
	void updateButtonsFunctions();
	void setButtonFunction(uint8_t number, uint8_t function);

	void setPotsLabel(uint8_t number, char * label);
	void updatePotsFunctions();
	void setPotFunction(uint8_t number, uint8_t function);

	void (*eventFunct)(uint8_t, void*, void*, void*);

	void showActualParamOnPads();
	void clearPads();
	void moveActualStep(uint8_t direction);
	void setActualEditedStepParam(uint8_t param);


	//funkcje przyciskow
	void play(uint8_t value);
	void stop(uint8_t value);
	void changeStepParamsSelection(uint8_t value);
	void changeStepFxSelection(uint8_t fx, uint8_t value);

	//funkcje potow
	void changeActualStepNote(int16_t value);
	void changeActualStepInstrument(int16_t value);
	void changeActualStepVolume(int16_t value);
	void changeActualStepFx1(int16_t value);
	void changeActualStepFx1Params(int16_t value);



	uint8_t	refreshStepEditor = 0;
	uint8_t	stepEditorModeStart = 0;
	uint8_t	editMode = 0;

//========================================================

	uint8_t getFxNameFromType(uint8_t fxType, char* ptrName);


	enum
	{
		stepParamNote,
		stepParamInstr,
		//stepParamLength,
		stepParamVolume,

		stepParamFx,
		stepParamFxParams,

		//-------------------------------
		stepParamsCount
	};

	enum
	{
		stepParamType,
		stepParamVal1,
		stepParamVal2,

		//-------------------------------
		stepFxCount
	};


	uint8_t actualTrack;
	uint8_t actualStep;


	uint8_t labelsChanged;
	uint8_t stepParametersChanged;




	uint8_t instrumentListEnabled = 0;
	char *instrumentNames[INSTRUMENTS_MAX];

	//ostatnio wybrany globalnie instrument
	uint8_t last_selected_instrument = 0;
	uint8_t last_selected_length = 1;

	// aktulane wybrane parametry do edycji
	uint8_t actualTrackTableSelection[5];

	// parametry stepa
	strMtDispTrackTable trackTable;
	char fx1ActualNames[5][20];
	char fx2ActualNames[5][20];
	char fx3ActualNames[5][20];
	char fx4ActualNames[5][20];


//=======================================================================

//przyciski  ------------------------------------------------------------
	enum
	{
		buttonFunctNone,
		buttonFunctPlay,
		buttonFunctStop,
		buttonFunctChangeStepParamsSel,
		buttonFunctShowNextStepFx1,
		buttonFunctShowNextStepFx2,
		buttonFunctShowNextStepFx3,
		buttonFunctShowNextStepFx4,

		//-------------------------------
		buttonFunctCount
	};



	//przyciski w edytorze
	char *buttonLabels[5];
	uint8_t buttonFunction[5];

	char buttonFunctionLabels[buttonFunctCount][20] =
	{
		{0},
		"Play",
		"Stop",
		"<  >",
		"<  >",
		"<  >",
		"<  >",
		"<  >",



	};

//potencjometry  ---------------------------------------------------------

	enum
	{
		potFunctNone,
		potFunctChangeStepNote,
		potFunctChangeStepInstr,
		potFunctChangeStepVolume,
		potFunctChangeStepFx,
		potFunctChangeStepFxParams,



		//-------------------------------
		potFunctionCount
	};


	char *potLabels[5];
	uint8_t potFunctions[5];

	char potFunctionLabels[potFunctionCount][20] =
	{
		{0},
		"Note",
		"Instrument",
		"Volume",
		"Fx1",
		"Fx params"

	};


	const uint16_t potFuncRes[potFunctionCount] =
	{
		100,	//potFunctNone,
		30,	//potFunctChangeStepParam,
		30,	//potFunctChangeStepFx1,
		30,	//potFunctChangeStepFx2,
		30,	//potFunctChangeStepFx3,
		30,	//potFunctChangeStepFx4,


	};

	const uint8_t potFuncAcc[potFunctionCount] =
	{
		0,	//potFunctNone,
		3,	//potFunctChangeStepParam,
		3,	//potFunctChangeStepFx1,
		3,	//potFunctChangeStepFx2,
		3,	//potFunctChangeStepFx3,
		3,	//potFunctChangeStepFx4,


	};


};


void stepEditorEvent(uint8_t event, void* param);

extern cMtStepEditor mtStepEditor;

#endif
