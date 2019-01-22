#ifndef MT_INSTRUMENT_EDITOR_H
#define MT_INSTRUMENT_EDITOR_H

#include "mtDisplay.h"
#include <stdint.h>

#include "SD.h"
#include "mtStructs.h"

enum enumMtInstrumentEditorMode
{
	mtInstrumentEditorModeDisabled,
	mtInstrumentEditorModeWaveEdit,
	mtInstrumentEditorParameters,
};

enum enumMtInstrumentEditorButtonFunction
{
	mtInstrumentEditorButtonFunctionNone,
	mtInstrumentEditorButtonFunctionPlay,
	mtInstrumentEditorButtonFunctionStop,
	mtInstrumentEditorButtonFunctionPlayMode,
	mtInstrumentEditorButtonFunctionEnvelopes,
	mtInstrumentEditorButtonFunctionInstrumentSelect,
	mtInstrumentEditorButtonFunctionSampleList,
	mtInstrumentEditorButtonFunctionParameters,


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
	mtInstrumentEditorPotFunctionPanning,
	mtInstrumentEditorPotFunctionGlide,
	mtInstrumentEditorPotFunctionFilter,
	mtInstrumentEditorPotFunction1,
	mtInstrumentEditorPotFunction2,


	//-------------------------------
	mtInstrumentEditorPotFunctionCount
};

enum enumMtInstrumentEditorValue
{
	mtInstrumentEditorValueNone,
	mtInstrumentEditorValuePanning,
	mtInstrumentEditorValueGlide,
	mtInstrumentEditorValueFilter,
	mtInstrumentEditorValue1,
	mtInstrumentEditorValue2,


	//-------------------------------
	mtInstrumentEditorPotValueCount
};



const uint16_t potsFuncResolutions[mtInstrumentEditorPotFunctionCount]=
{
		100,  //mtInstrumentEditorPotFunctionNone,
		100,  //mtInstrumentEditorPotFunctionStartPoint,
		100,  //mtInstrumentEditorPotFunctionEndPoint,
		100,  //mtInstrumentEditorPotFunctionLoopPoint1,
		100,  //mtInstrumentEditorPotFunctionLoopPoint2,
		100,  //mtInstrumentEditorPotFunctionInstrumentSelect,
		50,   //mtInstrumentEditorPotFunctionSampleSelect,
		100,  //mtInstrumentEditorPotFunctionViewPosition,
		100,  //mtInstrumentEditorPotFunctionVievZoom,
		100,  //mtInstrumentEditorPotFunctionPanning,
		100,  //mtInstrumentEditorPotFunctionGlide,
		100,  //mtInstrumentEditorPotFunctionFilter,
		100,  //mtInstrumentEditorPotFunction1,
		100,  //mtInstrumentEditorPotFunction2,


};


const char playModeFunctLabels[playModeMax][20]=
{
		"1-Shot",
		"Forward Loop",
		"Backward Loop",
		"Pingpong Loop",
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
	void processParameters();

	void setButtonLabel(uint8_t number, char * label);
	void updateButtonsFunctions();
	void setButtonFunction(uint8_t number, uint8_t function);

	void setPotsLabel(uint8_t number, char * label);
	void processPotsLabels();
	void updatePotsFunctions();
	void setPotFunction(uint8_t number, uint8_t function);

	void updateParameters();
	void setParameter(uint8_t number, uint8_t param);

	//funkcje przyciskow
	void showSampleList(uint8_t value);
	void play(uint8_t value);
	void stop(uint8_t value);
	void changePlayMode(uint8_t value);
	void showParameters(uint8_t value);


	//funkcje potow
	void modStartPoint(int16_t value);
	void modEndPoint(int16_t value);
	void modLoopPoint1(int16_t value);
	void modLoopPoint2(int16_t value);
	void selectInstrument(int16_t value);
	void selectSample(int16_t value);
	void changeView(int16_t value);
	void changeZoom(int16_t value);
	void changePanning(uint8_t pot, int16_t value);
	void changeGlide(uint8_t pot, int16_t value);
	void changeFilter(uint8_t pot, int16_t value);



	uint8_t	instrumentEditorMode = mtInstrumentEditorModeDisabled;
	uint8_t	refreshInstrumentEditor = 0;
	uint8_t	instrumentEditorModeStart = 0;
	uint8_t	editMode = 0;

//========================================================

	int8_t openedInstrumentIndex;
	strInstrument editorInstrument;
	strMtModAudioEngine  editorMod;

	uint8_t spectrumChanged;
	uint8_t pointsChanged;
	uint8_t labelsChanged;
	uint8_t sampleListChanged;
	uint8_t parametersChanged;



	uint16_t viewStart = 0;
	uint16_t viewLength = MAX_16BIT;

	uint8_t isPlayingSample;



	uint8_t parametersEnabled = 0;

	uint8_t sampleListEnabled = 0;
	char *sampleNames[SAMPLES_MAX];


	//parametry rysowania spektrum/parametrow dla wyswietalcza
	strMtDispSpectrum  spectrum;

	strMtDispValues  values;
	uint8_t valuesParameters[5];

	uint8_t mtInstrumentEditorValuesTypes[mtInstrumentEditorPotValueCount] =
	{
			mtDispValueValueNone,				//mtInstrumentEditorPotValueNone,
			mtDispValueValueLeftRight_0_100,	//mtInstrumentEditorPotValuePanning,
			mtDispValueValue_0_100,				//mtInstrumentEditorPotValueGlide,
			mtDispValueValue_0_100,				//mtInstrumentEditorPotValueFilter,
			mtDispValueValueNone,				//mtInstrumentEditorPotValue1,
			mtDispValueValueNone,				//mtInstrumentEditorPotValue2,
	};


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
		"Parameters",
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
		"Panning",
		"Glide",
		"Filter",
		"",
		"",
	};

};



extern cMtInstrumentEditor mtInstrumentEditor;

#endif
