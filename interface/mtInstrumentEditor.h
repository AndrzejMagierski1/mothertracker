#ifndef MT_INSTRUMENT_EDITOR_H
#define MT_INSTRUMENT_EDITOR_H

#include "display.h"
#include <stdint.h>

#include "SD.h"
#include "mtStructs.h"

#define ZOOM_MIN 	1
#define ZOOM_MAX 	10
#define ZOOM_FACTOR 0.05



enum enumMtInstrumentEditorEvents
{
	mtInstrumentEditorSeqButtonsPress,
	mtInstrumentEditorEventPadPress,

};



const char playModeFunctLabels[playModeCount][20]=
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
	void waitToLoadSpectrum();

	void setButtonLabel(uint8_t number, char * label);
	void updateButtonsFunctions();
	void setButtonFunction(uint8_t number, uint8_t function);

	void setPotsLabel(uint8_t number, char * label);
	void processPotsLabels();
	void updatePotsFunctions();
	void setPotFunction(uint8_t number, uint8_t function);

	void updateParameters();
	void setParameter(uint8_t number, uint8_t param);

	//player
	void playInstrumentByPad(uint8_t type, int8_t pad, int8_t velocity);


	//funkcje przyciskow
	void showSampleList(uint8_t value);
	void showInstrumentList(uint8_t value);
	void play(uint8_t value);
	void stopPlaying(uint8_t value);
	void changePlayMode(uint8_t value);
	void showParameters();
	void changeGlideNote(uint8_t value);
	void showEnvelopes();
	void showSpectrum();
	void changeFilterType(uint8_t value);
	void changeEnvelopeType(uint8_t value);
	void setEnvelopeTypeAmp(uint8_t value);
	void setEnvelopeTypeFilter(uint8_t value);
	void setEnvelopeEnable(uint8_t value);
	void changeParamsPage(uint8_t value);
	void addInstrument(uint8_t value);
	void removeInstrument();
	void addSample(uint8_t value);
	void createInstrument(uint8_t value);
	void cancelCreateInstrument(uint8_t value);
	void importInstrument(uint8_t value);

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
	void changeReverbSend(int16_t value);
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
	void selectAddInstrument(int16_t value);

	uint8_t	refreshInstrumentEditor = 0;
	uint8_t	instrumentEditorModeStart = 0;
	uint8_t	editMode = 0;

	void (*eventFunct)(uint8_t, void*, void*, void*);

//========================================================

	int8_t openedInstrumentIndex; // index w globalnej liscie instrumentow
	int8_t openedInstrFromActive; // index z listy tylko aktywnych ( activeInstruments[] )
	int8_t inActiveInstrumentIndex;
	int8_t activeInstruments[INSTRUMENTS_COUNT];
	uint8_t inActiveInstrumentsCount;
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

	const uint8_t sample_list_pos = 1;
	uint8_t sampleListEnabled = 0;
	char *sampleNames[SAMPLES_COUNT];

	uint8_t instrumentListMode = 0;
	const uint8_t instrument_list_pos = 0;
	uint8_t instrumentListEnabled = 0;
	char *instrumentNames[INSTRUMENTS_COUNT];

	//parametry rysowania spektrum/parametrow/envelopow
	const uint16_t SPECTRUM_DRAW_DELAY_VALUE = 500;
	strMtDispSpectrum  spectrum;
	elapsedMillis spectrumDrawDelay;

	strMtDispValues  values;
	uint8_t valuesParameters[5];

	uint8_t parametersPage = 0;

	enum
	{
		valueNone,
		valuePanning,
		valueGlide,
		valueFilter,
		valueResonance,
		valueVolume,
		valueFinetune,
		valueTune,
		valueReverbSend,

		//-------------------------------
		valueCount
	};

	uint32_t valuesTypes[valueCount] =
	{
		0,				//mtInstrumentEditorPotValueNone,
		controlStyleValueLeftRight_100_100,	//mtInstrumentEditorPotValuePanning,
		controlStyleValue_0_100,				//mtInstrumentEditorPotValueGlide,
		controlStyleValue_0_100,				//mtInstrumentEditorPotValueFilter,
		controlStyleValue_0_100,				//mtInstrumentEditorValueResonance,
		controlStyleValue_0_100,				//mtInstrumentEditorValueVolume,
		controlStyleValueLeftRight_100_100,	//mtInstrumentEditorValueFinetune,
		controlStyleValueLeftRight_24_24,	//mtInstrumentEditorValueTune,
		controlStyleValue_0_100,				//mtInstrumentEditorValueFinetune,
	};

	strMtDispEnvelope envelope;
	uint8_t envelopeType;

//========================================================

	//przyciski
	enum
	{
		buttonFunctNone,
		buttonFunctPlay,
		buttonFunctStop,
		buttonFunctPlayMode,
		buttonFunctEnvelopes,
		buttonFunctInstrumentList,
		buttonFunctSampleList,
		buttonFunctParameters,
		buttonFunctChangeGlideNote,
		buttonFunctFilterType,
		buttonFunctEnvelopeType,
		buttonFunctEnvelopeAmp,
		buttonFunctEnvelopeFilter,
		buttonFunctEnvelopeEnable,
		buttonFunctParamsNextPage,
		buttonFunctInstrumentAdd,
		buttonFunctInstrumentRemove,
		buttonFunctSampleAdd,
		buttonFunctInstrumentCreate,
		buttonFunctInstrumentCreateCancel,
		buttonFunctInstrumentImport,


		//-------------------------------
		buttonFunctCount
	};

	char *buttonLabels[5];
	uint8_t buttonFunctions[5];

	char buttonFunctionLabels[buttonFunctCount][20] =
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
		"Next page",
		"Add inst.",
		"Remove inst.",
		"Add sample",
		"Create",
		"Cancel",
		"Import inst.",

	};

	//potencjometry
	enum
	{
		potFunctNone,
		potFunctStartPoint,
		potFunctEndPoint,
		potFunctLoopPoint1,
		potFunctLoopPoint2,
		potFunctInstrumentSelect,
		potFunctSampleSelect,
		potFunctViewPosition,
		potFunctViewZoom,
		potFunctPanning,
		potFunctGlide,
		potFunctFilter,
		potFunctAttack,
		potFunctDecay,
		potFunctSustaion,
		potFunctRelease,
		potFunctAmount,
		potFunctResonance,
		potFunctVolume,
		potFunctFinetune,
		potFunctTune,
		potFunctWavetablePos,
		potFunctAddInstrumentSelect,
		potFunctReverbSend,

		//-------------------------------
		potFunctCount
	};

	char *potLabels[5];
	uint8_t potFunctions[5];

	char potFunctionLabels[potFunctCount][20] =
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
		"Wavetable",
		"Select slot",
		"Reverb Send",

	};

	const uint16_t potFuncRes[potFunctCount] =
	{
			100, 	 // potFunctionNone,
			100,     //potFunctStartPoint,
			100,     //potFunctEndPoint,
			100,     //potFunctLoopPoint1,
			100,     //potFunctLoopPoint2,
			20,      //potFunctInstrumentSelect,
			20,      //potFunctSampleSelect,
			100,     //potFunctViewPosition,
			100,     //potFunctViewZoom,
			100,     //potFunctPanning,
			100,     //potFunctGlide,
			100,     //potFunctFilter,
			100,     //potFunctAttack,
			100,     //potFunctDecay,
			100,     //potFunctSustaion,
			100,     //potFunctRelease,
			100,     //potFunctAmount,
			100,     //potFunctResonance,
			100,     //potFunctVolume,
			100,     //potFunctFinetune,
			100,     //potFunctTune,
            100,     //potFunctWavetablePos,
			20,		 //potFunctAddInstrumentSelect,
			100,	 //potFunctReverbSend,
	};

	const uint8_t potFuncAcc[potFunctCount] =
	{
			3, 	   //potFunctionNone,
			3,	   //potFunctStartPoint,
			3,	   //potFunctEndPoint,
			3,	   //potFunctLoopPoint1,
			3,	   //potFunctLoopPoint2,
			0,	   //potFunctInstrumentSelect,
			0,	   //potFunctSampleSelect,
			3,	   //potFunctViewPosition,
			3,	   //potFunctViewZoom,
			3,	   //potFunctPanning,
			3,	   //potFunctGlide,
			3,	   //potFunctFilter,
			3,	   //potFunctAttack,
			3,	   //potFunctDecay,
			3,	   //potFunctSustaion,
			3,	   //potFunctRelease,
			3,	   //potFunctAmount,
			3,	   //potFunctResonance,
			3,	   //potFunctVolume,
			3,	   //potFunctFinetune,
			3,	   //potFunctTune,
			3,	   //potFunctWavetablePos,
			0,	   //potFunctAddInstrumentSelect,
			3,	   //potFunctReverbSend,
	};

};


void instrumentEditorEvent(uint8_t event, void* param);

extern cMtInstrumentEditor mtInstrumentEditor;

#endif
