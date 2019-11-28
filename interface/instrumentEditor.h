#ifndef INTERFACE_INSTRUMENTEDITOR_H_
#define INTERFACE_INSTRUMENTEDITOR_H_


#include <modulesBase.h>
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


const char envelopesLabels[3][11]=
{
		"Amp",
		"Filter",
		"WT Pos"

};

const char envStateLabels[2][11]=
{
		"On",
		"Off",
};

const char envLoopLabels[2][11]=
{
	"On",
	"Off"
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
	void showEnvLoop();

	void showParamsVelocity();

	void showParamsVolume();
	void showParamsPanning();
	void showParamsTune();
	void showParamsFineTune();
	void showFilterType();
	void showFilterCutOff();
	void showFilterResonance();
	void showParamsReverbSend();

	void showParamsGlide();

//----------------------------------

	void setDefaultScreenFunct();
	void clearDefaultScreenFunct();

	void setInstrumentEnvFunct();

	void setInstrumentParamsFunct();

//----------------------------------

	void setProjectSaveFlags();

//----------------------------------


	strBgLabelData bgLabelData;
	strFrameData frameData;

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;

	hControl label[8] = {nullptr};
	hControl barControl[8] = {nullptr};

	hControl filterModeListControl = nullptr;
	hControl envelopesListControl = nullptr;
	hControl envStateListControl = nullptr;
	hControl envLoopListControl = nullptr;

	hControl bgLabel;
	hControl frameControl = nullptr;


	uint8_t selectedPlace[3] = {0};

	// typ trybu/ekranu
	uint8_t mode = mtInstEditModeParams;
	uint8_t paramsMode = mtInstEditNormal;

	strInstrument * editorInstrument;



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
	strList envLoopList;

	const char* envelopeNames[3] =
	{
		&envelopesLabels[0][0],
		&envelopesLabels[1][0],
		&envelopesLabels[2][0],
	};
	const char* envStateNames[2] =
	{
		&envStateLabels[0][0],
		&envStateLabels[1][0],
	};
	const char* envLoopNames[2] =
	{
		&envLoopLabels[0][0],
		&envLoopLabels[1][0],
	};


	char volumeVal[4];
	char panningVal[7];
	char tuneVal[5];
	char fineTuneVal[4];
	char cutoffVal[8];
	char resonanceVal[8];
	char revSendVal[5];
	char envAttack[8];
	char envDecay[8];
	char envSustain[8];
	char envRelease[8];
	char envAmount[8];



	void cancelPopups();

	select_node_t selectNodes[MAX_SELECT_NODES];

	void addNode(editFunct_t funct , uint8_t nodeNum, uint8_t reverseInput);
	void removeNode(uint8_t nodeNum);
	void stepThroughNodes(int16_t value, uint8_t source);
	void clearAllNodes();
	void cancelMultiFrame();




};

extern cInstrumentEditor instrumentEditor;



#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
