#ifndef INTERFACE_INSTRUMENTEDITOR_H_
#define INTERFACE_INSTRUMENTEDITOR_H_


#include "core/modulesBase.h"
#include "SD.h"

#include "mtStructs.h"

#include "mtSequencer.h"

enum mtInstrumentEditorMode
{
	mtInstEditModeParams,
	mtInstEditModeEnv,

};
enum mtInstrumentEditorParamsMode
{
	mtInstEditNormal,
	mtInstEditMidi,

};

const uint8_t filterModeCount = 4;

const char filterModeFunctLabels[filterModeCount][11]=
{
		"Disabled",
		"Low-pass",
		"High-pass",
		"Band-pass",
};


const char envelopesLabels[6][20]=
{
		"Volume",
		"Panning",
		"Cutoff",
		"Wavetable Position",
		"Granular Position",
		"Finetune"

};

const char envStateLabels[3][11]=
{
		"Off",
		"Envelope",
		"LFO"
};

//const char envLoopLabels[2][11]=
//{
//	"On",
//	"Off"
//};

const char lfoShapeLabels[lfoShapeCount][11]=
{
		"Rev Saw",
		"Saw",
		"Triangle",
		"Square",
		"Random"
};

const char lfoSpeedLabels[ENVELOPE_SPEED_OTHER_COUNT][15]=
{
		"128 steps",
		"96 steps",
		"64 steps",
		"48 steps",
		"32 steps",
		"24 steps",
		"16 steps",
		"12 steps",
		"8 steps",
		"6 steps",
		"4 steps",
		"3 steps",
		"2 steps",
		"3/2 step",
		"1 step",
		"3/4 step",
		"1/2 step",
		"3/8 step",
		"1/3 step",
		"1/4 step",
		"3/16 step",
		"1/6 step",
		"1/8 step",
		"1/12 step",
		"1/16 step",
		"1/24 step",
		"1/32 step",
		"1/48 step",
		"1/64 step"
};

#undef MAX_SELECT_NODES
#define MAX_SELECT_NODES	8


class cInstrumentEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cInstrumentEditor()
	{

	}
	~cInstrumentEditor() {}

	void showDefaultScreen();

	void showInstrumentEnv();
	void showInstrumentParams();
	void showInstrumentMidiParams();
	void showTitleBar();

	void activateLabelsBorder();
	void lightUpPadBoard();
	void clearPadBoard();

	void showEnvList();
	void showEnvState();
	void showEnvAttack();
	void showEnvDecay();
	void showEnvSustain();
	void showEnvRelease();
	void showEnvAmount();
//	void showEnvLoop();


	void showLfoShape();
	void showLfoSpeed();
	void showLfoAmount();


	void showParamsVelocity();

	void showParamsVolume();
	void showParamsPanning();
	void showParamsTune();
	void showParamsFineTune();
	void showFilterType();
	void showFilterCutOff();
	void showFilterResonance();
	void showParamsDelaySend();
	void showParamsReverbSend();

	void showParamsGlide();

	void refreshFramePosition();
//----------------------------------

	void setDefaultScreenFunct();
	void clearDefaultScreenFunct();

	void setInstrumentEnvFunct();

	void setInstrumentParamsFunct();
//----------------------------------
	void setProjectSaveFlags();
	void setDefaultValue(uint8_t place);
//first screen
	void setDefaultVolume();
	void setDefaultPanning();
	void setDefaultTune();
	void setDefaultFinetune();
	void setDefaultFilterType();
	void setDefaultCutoff();
	void setDefaultResonance();
	void setDefaultDelaySend();
	void setDefaultReverbSend();
//env screen
	void setDefaultEnvAttack();
	void setDefaultEnvDecay();
	void setDefaultEnvSustain();
	void setDefaultEnvRelease();
	void setDefaultEnvAmount();
//lfo
	void setDefaultLfoShape();
	void setDefaultLfoSpeed();
	void setDefaultLfoAmount();
//MIDI
	void setDefaultMidiVelocity();
//----------------------------------

	strLabelData labelArrow;
	strFrameData frameData;

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;

	hControl label[8] = {nullptr};
	hControl doubleLabelAutomation = nullptr;
	hControl barControl[8] = {nullptr};

	hControl filterModeListControl = nullptr;
	hControl envelopesListControl = nullptr;
	hControl envStateListControl = nullptr;
	hControl shapeListControl = nullptr;
	hControl speedListControl = nullptr;
//	hControl envLoopListControl = nullptr;

	hControl bgLabel;
	hControl frameControl = nullptr;


	uint8_t selectedPlace[3] = {0};

	// typ trybu/ekranu
	uint8_t mode = mtInstEditModeParams;
	uint8_t paramsMode = mtInstEditNormal;

	strInstrument * editorInstrument;

	bool rollListOver; // "przekrecanie" listy przy klikaniu przycisku pod ekranem

	uint8_t selectedEnvelope = 0;

//----------------------------------
// aktualny instrument na belce tytułowej
	void showActualInstrument();
	//char actualInstrName[SAMPLE_NAME_SIZE+4];

//----------------------------------
// lista play mode

	strList filterModeList;

	//void listData();

	const char* filterModeNames[filterModeCount] =
	{
		&filterModeFunctLabels[0][0],
		&filterModeFunctLabels[1][0],
		&filterModeFunctLabels[2][0],
		&filterModeFunctLabels[3][0],
	};

	uint8_t filterModeListPos;

//----------------------------------
// odtwarzanie
	uint8_t isPlayingSample = 0;

//----------------------------------
// envelopes lists

	strList envelopesList;
	strList envStateList;

	strList lfoShapeList;
	strList lfoSpeedList;
//	strList envLoopList;

	const char* envelopeNames[6] =
	{
		&envelopesLabels[0][0],
		&envelopesLabels[1][0],
		&envelopesLabels[2][0],
		&envelopesLabels[3][0],
		&envelopesLabels[4][0],
		&envelopesLabels[5][0]
	};
	const char* envStateNames[3] =
	{
		&envStateLabels[0][0],
		&envStateLabels[1][0],
		&envStateLabels[2][0]
	};
//	const char* envLoopNames[2] =
//	{
//		&envLoopLabels[0][0],
//		&envLoopLabels[1][0],
//	};

	const char * lfoShapeNames[lfoShapeCount] =
	{
		&lfoShapeLabels[0][0],
		&lfoShapeLabels[1][0],
		&lfoShapeLabels[2][0],
		&lfoShapeLabels[3][0],
		&lfoShapeLabels[4][0]
	};

	const char * lfoSpeedNamesAmp[ENVELOPE_SPEED_AMP_COUNT] =
	{
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 1][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 2][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 3][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 4][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 5][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 6][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 7][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 8][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 9][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 10][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 11][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 12][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 13][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 14][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 15][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 16][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 17][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 18][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 19][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 20][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 21][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 22][0],
		&lfoSpeedLabels[ENVELOPE_SPEED_CONSTRAIN_OFFSET + 23][0]
	};

	const char * lfoSpeedNamesOthers[ENVELOPE_SPEED_OTHER_COUNT] =
	{
		&lfoSpeedLabels[0][0],
		&lfoSpeedLabels[1][0],
		&lfoSpeedLabels[2][0],
		&lfoSpeedLabels[3][0],
		&lfoSpeedLabels[4][0],
		&lfoSpeedLabels[5][0],
		&lfoSpeedLabels[6][0],
		&lfoSpeedLabels[7][0],
		&lfoSpeedLabels[8][0],
		&lfoSpeedLabels[9][0],
		&lfoSpeedLabels[10][0],
		&lfoSpeedLabels[11][0],
		&lfoSpeedLabels[12][0],
		&lfoSpeedLabels[13][0],
		&lfoSpeedLabels[14][0],
		&lfoSpeedLabels[15][0],
		&lfoSpeedLabels[16][0],
		&lfoSpeedLabels[17][0],
		&lfoSpeedLabels[18][0],
		&lfoSpeedLabels[19][0],
		&lfoSpeedLabels[20][0],
		&lfoSpeedLabels[21][0],
		&lfoSpeedLabels[22][0],
		&lfoSpeedLabels[23][0],
		&lfoSpeedLabels[24][0],
		&lfoSpeedLabels[25][0],
		&lfoSpeedLabels[26][0],
		&lfoSpeedLabels[27][0],
		&lfoSpeedLabels[28][0],
	};

	char volumeVal[4];
	char panningVal[7];
	char tuneVal[5];
	char fineTuneVal[5];
	char cutoffVal[8];
	char resonanceVal[8];
	char delaySendVal[5];
	char reverbSendVal[5];
	char envAttack[8];
	char envDecay[8];
	char envSustain[8];
	char envRelease[8];
	char envAmount[8];

	char lfoShape[8];
	char lfoSpeed[8];
	char lfoAmount[8];



	void cancelPopups();

	select_node_t selectNodes[MAX_SELECT_NODES];

	void addNode(editFunct_t funct , uint8_t nodeNum, uint8_t reverseInput = 0);
	void removeNode(uint8_t nodeNum);
	void stepThroughNodes(int16_t value, uint8_t source);
	void clearAllNodes();
	void cancelMultiFrame();

//sends screen
	void switchToSendScreen();
	void setSendScreenFunct();
	void showSendScreen();
	void showSendScreenFrame();
	bool isSendsWindow;
	uint8_t selectedPlaceSends;


};

extern cInstrumentEditor instrumentEditor;



#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
