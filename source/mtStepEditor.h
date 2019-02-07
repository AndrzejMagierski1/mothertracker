#ifndef MT_STEP_EDITOR_H
#define MT_STEP_EDITOR_H

#include "mtDisplay.h"
#include <stdint.h>

#include "SD.h"
#include "mtStructs.h"



enum enumMtStepEditorButtonFunction
{
	mtStepEditButtonFunctNone,
	mtStepEditButtonFunctPlay,
	mtStepEditButtonFunctStop,
	mtStepEditButtonFunctChangeStepParamsSel,
	mtStepEditButtonFunctShowNextStepParams,



	//-------------------------------
	mtStepEditorButtonFunctionCount
};

enum enumMtStepEditorPotFunction
{
	mtStepEditPotFunctNone,
	mtStepEditPotFunctChangeStepInstrument,
	mtStepEditPotFunctChangeStepNote,
	mtStepEditPotFunctChangeStepLength,
	mtStepEditPotFunctChangeStepVolume,
	mtStepEditPotFunctChangeSeqPosition,



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

	// aktulane wybrane parametry do edycji
	uint8_t actualTrackTableSelection[5];

	// parametry stepa
	strMtDispTrackTable trackTable;
	char fx1ActualNames[5][20];
	char fx2ActualNames[5][20];
	char fx3ActualNames[5][20];
	char fx4ActualNames[5][20];

	uint8_t getFxNameFromType(uint8_t fxType, char* ptrName);
//	uint8_t mtInstrumentEditorValuesTypes[mtInstrumentEditorPotValueCount] =
//	{
//		mtDispValueValueNone,
//		mtDispValueMultiValue4Row,
//		mtDispValueMultiValue3Row,
//		mtDispValueMultiValue3Row,
//		mtDispValueMultiValue3Row,
//		mtDispValueMultiValue3Row
//	};
//

	//funkcje przyciskow
	void play(uint8_t value);
	void stop(uint8_t value);
	void changeStepParamsSelection(uint8_t value);
	void showNextStepParams(uint8_t value);

	//funkcje potow
	void changeActualStepInstrument();
	void changeActualStepNote();
	void changeActualStepLength();
	void changeActualStepVeleocity();




	uint8_t	refreshStepEditor = 0;
	uint8_t	stepEditorModeStart = 0;
	uint8_t	editMode = 0;

//========================================================

	uint8_t actualTrack;
	uint8_t actualStep;


	uint8_t labelsChanged;
	uint8_t stepParametersChanged;




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
		"<  >",



	};

	//potencjometry w edytorze
	char *potsLabels[5];
	uint8_t potFunction[5];

	char mtStepEditorPotsLabels[mtStepEditorPotFunctionCount][20] =
	{
		{0},
		"Inst Note Len Vol",
		"Fx1",
		"Fx2",
		"Fx3",
		"Fx4"

	};

};


void stepEditorEvent(uint8_t event, void* param);

extern cMtStepEditor mtStepEditor;

#endif
