#ifndef INTERFACE_INSTRUMENTEDITOR_H_
#define INTERFACE_INSTRUMENTEDITOR_H_


#include <modulesBase.h>
#include "SD.h"

#include "mtStructs.h"

#include "mtSequencer.h"

enum mtInstrumentEditorMode
{
	mtInstEditModeVolume,
	mtInstEditModeFilter,
	mtInstEditModeParams,

};


const uint8_t filterModeCount = 4;

const char filterModeFunctLabels[filterModeCount][11]=
{
		"Disabled",
		"Low-pass",
		"High-pass",
		"Band-pass",
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

	void showInstrumentVolume();
	void showInstrumentFilter();
	void showInstrumentParams();

	void activateLabelsBorder();

	void showVolumeAttack();
	void showVolumeDecay();
	void showVolumeSustain();
	void showVolumeRelease();
	void showVolumeAmount();
	void showFilterAttack();
	void showFilterDecay();
	void showFilterSustain();
	void showFilterRelease();
	void showFilterAmount();
	void showFilterFilterType();
	void showFilterCutOff();
	void showFilterResonance();
	void showParamsVolume();
	void showParamsTune();
	void showParamsFineTune();
	void showParamsGlide();
	void showParamsPanning();
	void showParamsVibrato();
	void showParamsTremolo();
	void showParamsReverbSend();
//----------------------------------

	void setDefaultScreenFunct();

	void setInstrumentVolumeFunct();
	void setInstrumentFilterFunct();
	void setInstrumentParamsFunct();

//----------------------------------

	void changeVolumeAttack(int16_t value);
	void changeVolumeDecay(int16_t value);
	void changeVolumeSustain(int16_t value);
	void changeVolumeRelease(int16_t value);
	void changeVolumeAmount(int16_t value);

	void changeFilterAttack(int16_t value);
	void changeFilterDecay(int16_t value);
	void changeFilterSustain(int16_t value);
	void changeFilterRelease(int16_t value);
	void changeFilterAmount(int16_t value);
	void changeFilterFilterType(int16_t value);
	void changeFilterCutOff(int16_t value);
	void changeFilterResonance(int16_t value);

	void changeParamsVolume(int16_t value);
	void changeParamsTune(int16_t value);
	void changeParamsFineTune(int16_t value);
	void changeParamsGlide(int16_t value);
	void changeParamsPanning(int16_t value);
	void changeParamsVibrato(int16_t value);
	void changeParamsTremolo(int16_t value);
	void changeParamsReverbSend(int16_t value);

//----------------------------------


	strFrameData frameData;


	hControl titleLabel;
	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl barControl[8];

	hControl filterModeListControl;


	hControl listControl;

	hControl frameControl;


	uint8_t selectedPlace[3] = {0};


	uint8_t mode;

	strInstrument * editorInstrument;

//----------------------------------
// lista play mode

		strList filterModeList;

		void listFilterMode();

		char *filterModeNames[filterModeCount];

		uint8_t filterModeListPos;

//----------------------------------
// odtwarzanie
	uint8_t isPlayingSample = 0;





};

extern cInstrumentEditor instrumentEditor;



#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
