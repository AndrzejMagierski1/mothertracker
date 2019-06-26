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

	void activateLabelsBorder();


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

//----------------------------------

	void setDefaultScreenFunct();

	void setInstrumentEnvFunct();
	void setInstrumentParamsFunct();

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
//----------------------------------


	strFrameData frameData;


	hControl titleLabel;
	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl barControl[8];

	hControl filterModeListControl;
	hControl envelopesListControl;
	hControl envStateListControl;
	hControl envLoopListControl;


	hControl listControl;

	hControl frameControl;


	uint8_t selectedPlace[2] = {0};


	uint8_t mode;

	strInstrument * editorInstrument;



	uint8_t selectedEnvelope = 0;

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
// envelopes list

strList envelopesList;
strList envStateList;
strList envLoopList;



char *envelopeNames[2];
char *envStateNames[2];
char *envLoopNames[2];


};

extern cInstrumentEditor instrumentEditor;



#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
