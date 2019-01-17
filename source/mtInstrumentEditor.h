#ifndef MT_INSTRUMENT_EDITOR_H
#define MT_INSTRUMENT_EDITOR_H


#include <stdint.h>

#include "SD.h"
#include "mtStructs.h"

enum enumMtInstrumentEditorMode
{
	mtInstrumentEditorModeDisabled,
	mtInstrumentEditorModeWaveEdit,
	mtInstrumentEditorModeInstrumentSelect,
	mtInstrumentEditorModeSampleSelect,
};

enum enumMtInstrumentEditorButtonFunction
{
	mtInstrumentEditorButtonFunctionNone,
	mtInstrumentEditorButtonFunctionPlay,
	mtInstrumentEditorButtonFunctionStop,
	mtInstrumentEditorButtonFunctionPlayMode,
	mtInstrumentEditorButtonFunctionAmpEnvelope,
	mtInstrumentEditorButtonFunctionInstrumentSelect,
	mtInstrumentEditorButtonFunctionSampleList,


	//-------------------------------
	mtInstrumentEditorButtonFunctionCount
};

enum enumMtInstrumentEditorPotFunction
{
	mtInstrumentEditorPotFunctionNone,
	mtInstrumentEditorPotFunctionStartPoint,
	mtInstrumentEditorPotFunctionEndPoint,
	mtInstrumentEditorPotFunctionLoopPoint1,
	mtInstrumentEditorPotFunctionLoopPoint2,
	mtInstrumentEditorPotFunctionInstrumentSelect,
	mtInstrumentEditorPotFunctionSampleSelect,
	mtInstrumentEditorPotFunctionViewPosition,
	mtInstrumentEditorPotFunctionVievZoom,

	//-------------------------------
	mtInstrumentEditorPotFunctionCount
};

const uint8_t potsFuncResolutions[mtInstrumentEditorPotFunctionCount]=
{
		100,
		100,
		100,
		100,
		100,
		100,
		50,
		100,
		100,

};







class cMtInstrumentEditor
{
public:
	void begin();
	void update();

	void startExisting(uint8_t instrumentIndex);
	void startEmpty();

	void buttonChange(uint8_t button, uint8_t value);
	void potChange(uint8_t pot, int16_t value);

private:
	void processSpectrum();
	void processPoints();
	void processLabels();

	void setButtonLabel(uint8_t number, char * label);
	void updateButtons();
	void setButton(uint8_t number, uint8_t function);

	void setPotsLabel(uint8_t number, char * label);
	void processPotsLabels();
	void updatePots();
	void setPotFunction(uint8_t number, uint8_t function);

	//funkcje przyciskow
	void showSampleList(uint8_t value);
	void play(uint8_t value);
	void stop(uint8_t value);

	//funkcje potow
	void modStartPoint(int16_t value);
	void modEndPoint(int16_t value);
	void modLoopPoint1(int16_t value);
	void modLoopPoint2(int16_t value);
	void selectInstrument(int16_t value);
	void selectSample(int16_t value);
	void changeView(int16_t value);
	void changeZoom(int16_t value);






	uint8_t	instrumentEditorMode = mtInstrumentEditorModeDisabled;
	uint8_t	refreshInstrumentEditor = 0;
	uint8_t	instrumentEditorModeStart = 0;
	uint8_t	editMode = 0;

//========================================================

	int8_t openedInstrumentIndex;
	strInstrument editorInstrument;


	uint8_t spectrumChanged;
	uint8_t pointsChanged;
	uint8_t labelsChanged;
	uint8_t sampleListChanged;


	uint16_t viewStart = 0;
	uint16_t viewLength = MAX_16BIT;

	uint8_t isPlayingSample;


	uint8_t sampleListEnabled = 0;
	char *sampleNames[SAMPLES_MAX];


	//parametry rysowania spektrum dla wyswietalcza
	strSpectrum  spectrum;

//========================================================

	//przyciski w edytorze
	char *buttonLabels[5];
	uint8_t buttonFunction[5];

	char mtInstrumentEditorButtonsLabels[mtInstrumentEditorButtonFunctionCount][20] =
	{
			{0},
			"Play",
			"Stop",
			"1-Shot",
			"Amp Envelope",
			"Instrument",
			"Sample",
	};

	//potencjometry w edytorze
	char *potsLabels[5];
	uint8_t potFunction[5];

	char mtInstrumentEditorPotsLabels[mtInstrumentEditorPotFunctionCount][20] =
	{
			{0},
			"Start Point",
			"End Point",
			"Loop Point 1",
			"Loop Point 2",
			"Instrument",
			"Sample",
			"View",
			"Zoom",
	};

};



extern cMtInstrumentEditor mtInstrumentEditor;

#endif
