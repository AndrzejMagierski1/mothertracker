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
	mtInstEditModeInstrList,


};


const uint8_t filterModeCount = 4;

const char filterModeFunctLabels[filterModeCount][11]=
{
		"Disabled",
		"Low-pass",
		"High-pass",
		"Band-pass",
};


const char envelopesLabels[filterModeCount][11]=
{
		"Amp",
		"Filter",

};

const char envStateLabels[filterModeCount][11]=
{
		"On",
		"Off",
};

const char envLoopLabels[filterModeCount][11]=
{
	"On",
	"Off"
};



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
	void showInstrumentList();


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


	void showParamsVolume();
	void showParamsPanning();
	void showParamsTune();
	void showParamsFineTune();
	void showFilterFilterType();
	void showFilterCutOff();
	void showFilterResonance();
	void showParamsReverbSend();

	void showParamsGlide();

	void showInstrList(uint8_t n);
//----------------------------------

	void setDefaultScreenFunct();

	void setInstrumentEnvFunct();
	void setInstrumentParamsFunct();
	void setInstrumentListFunct();

//----------------------------------


	void changeEnvList(int16_t value);
	void changeEnvState(int16_t value);
	void changeEnvAttack(int16_t value);
	void changeEnvDecay(int16_t value);
	void changeEnvSustain(int16_t value);
	void changeEnvRelease(int16_t value);
	void changeEnvAmount(int16_t value);
	void changeEnvLoop(int16_t value);


	void changeParamsVolume(int16_t value);
	void changeParamsPanning(int16_t value);
	void changeParamsTune(int16_t value);
	void changeParamsFineTune(int16_t value);
	void changeFilterFilterType(int16_t value);
	void changeFilterCutOff(int16_t value);
	void changeFilterResonance(int16_t value);
	void changeParamsReverbSend(int16_t value);


	void changeParamsGlide(int16_t value);

	void changeSelectedInstrument(int16_t value, uint8_t type = 0);
//----------------------------------


	strFrameData frameData;

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;

	hControl topLabel[8] = {nullptr};
	hControl bottomLabel[8] = {nullptr};
	hControl barControl[8] = {nullptr};

	hControl filterModeListControl = nullptr;
	hControl envelopesListControl = nullptr;
	hControl envStateListControl = nullptr;
	hControl envLoopListControl = nullptr;

	hControl intrumentsListControl[4] = {nullptr};

	hControl frameControl = nullptr;


	uint8_t selectedPlace[3] = {0};

	// typ trybu/ekranu
	uint8_t mode;

	strInstrument * editorInstrument;



	uint8_t selectedEnvelope = 0;

//----------------------------------
// aktualny instrument na belce tytułowej
	void showActualInstrument();
	//char actualInstrName[SAMPLE_NAME_SIZE+4];

//----------------------------------
// lista play mode

	strList filterModeList;

	void listData();

	char *filterModeNames[filterModeCount];

	uint8_t filterModeListPos;

//----------------------------------
// odtwarzanie
	uint8_t isPlayingSample = 0;

//----------------------------------
// envelopes lists

	strList envelopesList;
	strList envStateList;
	strList envLoopList;

	char *envelopeNames[2];
	char *envStateNames[2];
	char *envLoopNames[2];

	//----------------------------------
	// instruments lists

	strList intrumentsList[4];

	uint8_t selectedInstrument = 0;

	char intrumentsNames[INSTRUMENTS_COUNT][SAMPLE_NAME_SIZE+4];
	char *ptrintrumentsNames[INSTRUMENTS_COUNT];

	char volumeVal[4];
	char panningVal[5];
	char tuneVal[4];
	char fineTuneVal[4];
	char cutoffVal[8];
	char resonanceVal[8];
	char revSendVal[4];
	char envAttack[8];
	char envDecay[8];
	char envSustain[8];
	char envRelease[8];
	char envAmount[8];

	void listInstruments();


};

extern cInstrumentEditor instrumentEditor;



#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
