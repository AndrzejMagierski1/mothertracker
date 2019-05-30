#ifndef NEWINTERFACE_SAMPLEPLAYBACK_H_
#define NEWINTERFACE_SAMPLEPLAYBACK_H_


#include "modulesBase.h"

#include "SD.h"

#include "mtSequencer.h"

#include "mtStructs.h"



class cSamplePlayback: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	cSamplePlayback() {}
	~cSamplePlayback() {}

	void showDefaultScreen();

	void processSpectrum();

//----------------------------------
	uint8_t refreshSpectrum = 0;


	int8_t openedInstrumentIndex; // index w globalnej liscie instrumentow
	int8_t openedInstrFromActive; // index z listy tylko aktywnych ( activeInstruments[] )
	int8_t inActiveInstrumentIndex;
	int8_t activeInstruments[INSTRUMENTS_COUNT];
	uint8_t inActiveInstrumentsCount;
	strInstrument * editorInstrument;



	uint16_t zoomWidth = MAX_16BIT;
	int32_t zoomStart =  0;
	int32_t zoomEnd = MAX_16BIT;
	uint8_t lastChangedPoint = 0;
	float zoomValue = 1;
	uint16_t zoomPosition = 0;


	strTrackerSpectrum spectrum;
//----------------------------------



	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl spectrumControl;


	uint8_t instrumentListMode = 0;
	const uint8_t instrument_list_pos = 0;
	uint8_t instrumentListEnabled = 0;
	char *instrumentNames[INSTRUMENTS_COUNT];



};

extern cSamplePlayback samplePlayback;



#endif /* NEWINTERFACE_SAMPLEPLAYBACK_H_ */
