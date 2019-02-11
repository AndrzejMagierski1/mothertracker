#ifndef MT_INSTRUMENT_EDITOR_H
#define MT_INSTRUMENT_EDITOR_H

#include "mtDisplay.h"
#include <stdint.h>

#include "SD.h"
#include "mtStructs.h"

#define ZOOM_MIN 	1
#define ZOOM_MAX 	10
#define ZOOM_FACTOR 0.05


enum enumMtInstrumentEditorButtonFunction
{
	mtInstrumentEditorButtonFunctionNone,
	mtInstrumentEditorButtonFunctionPlay,
	mtInstrumentEditorButtonFunctionStop,
	mtInstrumentEditorButtonFunctionPlayMode,
	mtInstrumentEditorButtonFunctionEnvelopes,
	mtInstrumentEditorButtonFunctionInstrumentList,
	mtInstrumentEditorButtonFunctionSampleList,
	mtInstrumentEditorButtonFunctionParameters,
	mtInstrumentEditorButtonFunctionChangeGlideNote,
	mtInstrumentEditorButtonFunctionFilterType,
	mtInstrumentEditorButtonFunctionEnvelopeType,
	mtInstrumentEditorButtonFunctionEnvelopeAmp,
	mtInstrumentEditorButtonFunctionEnvelopeFilter,
	mtInstrumentEditorButtonFunctionEnvelopeEnable,

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
	mtInstrumentEditorPotFunctionViewZoom,
	mtInstrumentEditorPotFunctionPanning,
	mtInstrumentEditorPotFunctionGlide,
	mtInstrumentEditorPotFunctionFilter,
	mtInstrumentEditorPotFunctionAttack,
	mtInstrumentEditorPotFunctionDecay,
	mtInstrumentEditorPotFunctionSustaion,
	mtInstrumentEditorPotFunctionRelease,
	mtInstrumentEditorPotFunctionAmount,
	mtInstrumentEditorPotFunctionResonance,
	mtInstrumentEditorPotFunctionVolume,
	mtInstrumentEditorPotFunctionFinetune,
	mtInstrumentEditorPotFunctionTune,
	mtInstrumentEditorPotFunctionWavetablePos,

	//-------------------------------
	mtInstrumentEditorPotFunctionCount
};

enum enumMtInstrumentEditorValue
{
	mtInstrumentEditorValueNone,
	mtInstrumentEditorValuePanning,
	mtInstrumentEditorValueGlide,
	mtInstrumentEditorValueFilter,
	mtInstrumentEditorValueResonance,
	mtInstrumentEditorValueVolume,
	mtInstrumentEditorValueFinetune,
	mtInstrumentEditorValueTune,

	//-------------------------------
	mtInstrumentEditorPotValueCount
};

enum enumMtInstrumentEditorEvents
{
	mtInstrumentEditorSeqButtonsPress,
	mtInstrumentEditorEventPadPress,

};


const uint16_t mtInstrumentEditorPotsFuncRes[mtInstrumentEditorPotFunctionCount]=
{
		100,  //mtInstrumentEditorPotFunctionNone,
		100,  //mtInstrumentEditorPotFunctionStartPoint,
		100,  //mtInstrumentEditorPotFunctionEndPoint,
		100,  //mtInstrumentEditorPotFunctionLoopPoint1,
		100,  //mtInstrumentEditorPotFunctionLoopPoint2,
		20,  //mtInstrumentEditorPotFunctionInstrumentSelect,
		50,   //mtInstrumentEditorPotFunctionSampleSelect,
		100,  //mtInstrumentEditorPotFunctionViewPosition,
		100,  //mtInstrumentEditorPotFunctionVievZoom,
		100,  //mtInstrumentEditorPotFunctionPanning,
		100,  //mtInstrumentEditorPotFunctionGlide,
		100,	//mtInstrumentEditorPotFunctionFilter,
		100,	//mtInstrumentEditorPotFunctionAttack,
		100,	//mtInstrumentEditorPotFunctionDecay,
		100,	//mtInstrumentEditorPotFunctionSustaion,
		100,	//mtInstrumentEditorPotFunctionRelease,
		100,	//mtInstrumentEditorPotFunctionAmount,
		100,	//mtInstrumentEditorPotFunctionResonance,
		100,	//mtInstrumentEditorPotFunctionVolume,
		100,	//mtInstrumentEditorPotFunctionFineTune,
		100,	//mtInstrumentEditorPotFunctionTune,
		100,	//mtInstrumentEditorPotFunctionWavetablePos,

};


const char playModeFunctLabels[playModeMax][20]=
{
		"1-Shot",
		"Forward Loop",
		"Backward Loop",
		"Pingpong Loop",
};


const char glidePreviewDifLabels[4][20]=
{
		"Preview Off",
		"Note",
		"Octave",
		"2 Octaves",
};

const char filterTypeLabels[4][10]=
{
		"No Filter",
		"Low Pass",
		"High Pass",
		"Band Pass",
};


const char envelopeEnableLabels[2][10]=
{
		"Disabled",
		"Enabled",
};

const char envelopeTypeNames[INSTRUMEN_ENVELOPES_MAX][20]=
{
		"Amp Envelope",
		"Filter Envelope",
		"Pitch Envelope",
};


class cMtInstrumentEditor
{
public:
	void update();

	void startExisting(uint8_t instrumentIndex);
	void startEmpty();
	void stop();

	uint8_t padsChange(uint8_t type, uint8_t n, uint8_t velo);
	void buttonChange(uint8_t button, uint8_t value);
	void potChange(uint8_t pot, int16_t value);
	void seqButtonChange(uint8_t type, uint8_t x, uint8_t y);

	void setEventFunct(void (*func)(uint8_t, void*, void*, void*)) { eventFunct = func; };

private:
	void processSpectrum();
	void processPoints();
	void processLabels();
	void processParameters();
	void processEnvelopes();

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
	void showInstrumentList(uint8_t value);
	void play(uint8_t value);
	void stopPlaying(uint8_t value);
	void changePlayMode(uint8_t value);
	void showParameters(uint8_t value);
	void changeGlideNote(uint8_t value);
	void showEnvelopes(uint8_t value);
	void changeFilterType(uint8_t value);
	void changeEnvelopeType(uint8_t value);
	void setEnvelopeTypeAmp(uint8_t value);
	void setEnvelopeTypeFilter(uint8_t value);
	void setEnvelopeEnable(uint8_t value);

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
	void changeGlide(int16_t value);
	void changeFilter(int16_t value);
	void changeResonance(int16_t value);
	void changeAttack(int16_t value);
	void changeDecay(int16_t value);
	void changeSustain(int16_t value);
	void changeRelease(int16_t value);
	void changeAmount(int16_t value);
	void changeVolume(int16_t value);
	void changeFinetune(int16_t value);
	void changeTune(int16_t value);
	void changeWavetablePos(int16_t value);

	uint8_t	refreshInstrumentEditor = 0;
	uint8_t	instrumentEditorModeStart = 0;
	uint8_t	editMode = 0;

	void (*eventFunct)(uint8_t, void*, void*, void*);

//========================================================

	int8_t openedInstrumentIndex;
	strInstrument * editorInstrument;
	int8_t playNote = 24;
	uint8_t glidePreviewDif = 0;

	uint8_t spectrumChanged;
	uint8_t pointsChanged;
	uint8_t labelsChanged;
	uint8_t sampleListChanged;
	uint8_t instrumentListChanged;
	uint8_t parametersChanged;
	uint8_t envelopesChanged;


	uint16_t zoomWidth = MAX_16BIT;
	int32_t zoomStart =  0;
	int32_t zoomEnd = MAX_16BIT;
	uint8_t lastChangedPoint = 0;
	float zoomValue = 1;
	uint16_t zoomPosition = 0;

	uint8_t isPlayingSample;


	uint8_t envelopesEnabled = 0;
	uint8_t parametersEnabled = 0;

	uint8_t sampleListEnabled = 0;
	char *sampleNames[SAMPLES_MAX];

	uint8_t instrumentListEnabled = 0;
	char *instrumentNames[INSTRUMENTS_MAX];

	//parametry rysowania spektrum/parametrow/envelopow
	strMtDispSpectrum  spectrum;

	strMtDispValues  values;
	uint8_t valuesParameters[5];

	uint8_t mtInstrumentEditorValuesTypes[mtInstrumentEditorPotValueCount] =
	{
		mtDispValueValueNone,				//mtInstrumentEditorPotValueNone,
		mtDispValueValueLeftRight_0_100,	//mtInstrumentEditorPotValuePanning,
		mtDispValueValue_0_100,				//mtInstrumentEditorPotValueGlide,
		mtDispValueValue_0_100,				//mtInstrumentEditorPotValueFilter,
		mtDispValueValue_0_100,				//mtInstrumentEditorValueResonance,
		mtDispValueValue_0_100,				//mtInstrumentEditorValueVolume,
		mtDispValueValue_0_100,				//mtInstrumentEditorValueFinetune,
		mtDispValueValue_0_100,				//mtInstrumentEditorValueTune,
	};

	strMtDispEnvelope envelope;
	uint8_t envelopeType;

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
		"Envelopes",
		"Instrument",
		"Sample",
		"Parameters",
		"Preview Off",
		"No Filter",
		"Switch Envelope",
		"Amp Envelope",
		"Filter Envelope",
		"Disabled",
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
		"Attack",
		"Decay",
		"Sustain",
		"Release",
		"Amount",
		"Resonance",
		"Volume",
		"Finetune",
		"Tune",
		"Wavetable"
	};

};


void instrumentEditorEvent(uint8_t event, void* param);

extern cMtInstrumentEditor mtInstrumentEditor;

#endif
