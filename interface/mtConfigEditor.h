#ifndef MT_CONFIG_EDITOR_H
#define MT_CONFIG_EDITOR_H

#include "display.h"
#include <stdint.h>

#include "mtStructs.h"



enum mtConfigEditorEvents
{
	mtConfigEditorEventNone,
	mtConfigEditorEventSeqButtonsPress,
	mtConfigEditorEventPadPress,

	mtConfigEditorEventCount
};

enum mtConfigEditorStartMode
{
	mtConfigEditorStartModeDefault,
	mtConfigEditorStartModeGlobals,
	mtConfigEditorStartModeConfig,
	mtConfigEditorStartModeMixer,

};

class cMtConfigEditor
{
public:
	void update();


	void start(uint8_t mode);
	void stop();


	void setEventFunct(void (*func)(uint8_t, void*, void*, void*)) { eventFunct = func; };

	uint8_t padsChange(uint8_t type, uint8_t n, uint8_t velo);
	void buttonChange(uint8_t button, uint8_t value);
	void potChange(uint8_t pot, int16_t value);
	void seqButtonChange(uint8_t type, uint8_t x, uint8_t y);

private:

	void processLabels();
	void processParameters();


	void setButtonLabel(uint8_t number, char * label);
	void updateButtonsFunctions();
	void setButtonFunction(uint8_t number, uint8_t function);

	void setPotsLabel(uint8_t number, char * label);
	void updatePotsFunctions();
	void setPotFunction(uint8_t number, uint8_t function);

	void (*eventFunct)(uint8_t, void*, void*, void*);

	// elementy modulu

	void updateParameters();
	void setParameter(uint8_t number, uint8_t param);
	void switchParametersType(uint8_t type);

	uint8_t parametersChanged;
	strMtDispValues  values;
	uint8_t valuesParameters[5];
	uint8_t parametersType = 0;

	enum
	{
		valueNone,
		// globals
		valueMasterVolume,
		valueTempo,
		valueReverbRoomSize,
		valueReverbDamping,
		valueReverbPanning,


		// config
		valueCodecInput,
		valueCodecOutput,
		valueCodecMicGain,
		valueCodecLineInputLevel,
		valueCodecLineOutputLevel,


		//-------------------------------
		valueCount
	};

	uint32_t valuesTypes[valueCount] =
	{
		0,				//
		//globals
		controlStyleValue_0_100,				//
		controlStyleValueNumberOnly,			//
		controlStyleValue_0_100,				//
		controlStyleValue_0_100,				//
		controlStyleValueLeftRight_100_100,	//


		//config
		controlStyleValueNumberOnly,			//
		controlStyleValueNumberOnly,			//
		controlStyleValueNumberOnly,			//
		controlStyleValueNumberOnly,			//
		controlStyleValueNumberOnly,			//
	};


	//funkcje przyciskow



	//funkcje potow
	void changeMasterVolume(int16_t value);
	void changeReverbPanning(int16_t value);
	void changeReverbRoomSize(int16_t value);
	void changeReverbDamping(int16_t value);

	void changeInputSignal(int16_t value);
	void changeOutputSignal(int16_t value);
	void changeMicGain(int16_t value);
	void changeLineInLevel(int16_t value);
	void changeLineOutLevel(int16_t value);

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
		potFunctMasterVolume,
		potFunctReverbRoomSize,
		potFunctReverbDamping,
		potFunctReverbPanning,


		potFunctCodecInput,
		potFunctCodecOutput,
		potFunctCodecMicGain,
		potFunctCodecLineInputLevel,
		potFunctCodecLineOutputLevel,

		//-------------------------------
		potFunctCount
	};

	char *potLabels[5];
	uint8_t potFunctions[5];

	char potFunctionLabels[potFunctCount][20] =
	{
		{0},
		"Master Volume",
		"Reverb Size",
		"Reverb Damping",
		"Reverb Panning",


		"Signal Input",
		"Signal Output",
		"Mic Gain",
		"Line In Sens",
		"Line Out Level",



	};

	const uint16_t potFuncRes[potFunctCount] =
	{
			100, // potFunctionNone,
			100, // potFunctionNone,
			100, // potFunctionNone,
			100, // potFunctionNone,
			100, // potFunctionNone,


			100, // potFunctionNone,
			100, // potFunctionNone,
			100, // potFunctionNone,
			100, // potFunctionNone,
			100, // potFunctionNone,

	};

	const uint8_t potFuncAcc[potFunctCount] =
	{
			3, // potFunctionNone,
			3, // potFunctionNone,
			3, // potFunctionNone,
			3, // potFunctionNone,
			3, // potFunctionNone,

			3, // potFunctionNone,
			3, // potFunctionNone,
			3, // potFunctionNone,
			3, // potFunctionNone,
			3, // potFunctionNone,

	};

};


void configEditorEvent(uint8_t event, void* param);

extern cMtConfigEditor mtConfigEditor;

#endif
