#ifndef INTERFACE_SAMPLEPLAYBACK_H_
#define INTERFACE_SAMPLEPLAYBACK_H_


#include <modulesBase.h>
#include "SD.h"

#include "mtSequencer.h"

#include "mtStructs.h"


const float ZOOM_MIN 	=	1;
const float ZOOM_MAX 	=	10;
const float ZOOM_FACTOR =	0.05;


const char playModeFunctLabels[playModeCount][15]=
{
		"1-Shot",
		"Forward Loop",
		"Backward Loop",
		"Pingpong Loop",
};




class cSamplePlayback: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cSamplePlayback()
	{
		inActiveInstrumentsCount = 0;
		inActiveInstrumentIndex = 0;
		editorInstrument = nullptr;
		openedInstrFromActive = 0;
		openedInstrumentIndex = 0;

		spectrumControl = nullptr;
		pointsControl = nullptr;
		topLabel[8] = {nullptr};
		bottomLabel[8] = {nullptr};
	}
	virtual ~cSamplePlayback() {}

	void showDefaultScreen();
	void showZoomValue();
	void showPlayModeList();

	void setDefaultScreenFunct();


	void processSpectrum();
	void processPoints();
	void modStartPoint(int16_t value);
	void modEndPoint(int16_t value);
	void modLoopPoint1(int16_t value);
	void modLoopPoint2(int16_t value);
	void changeZoom(int16_t value);
	void changePlayModeSelection(int16_t value);

	void activateLabelsBorder();

	strFrameData frameData;

	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl playModeListControl;
	hControl spectrumControl;
	hControl pointsControl;
	hControl frameControl;


	uint8_t selectedPlace = 0;


//----------------------------------
// spectrum + punkty
	uint8_t refreshSpectrum = 0;
	uint8_t refreshPoints = 0;

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
	char zoomTextValue[6];
	uint16_t zoomPosition = 0;

	strTrackerSpectrum spectrum;
	strTrackerPoints points;

//----------------------------------
// lista play mode
	strList playModeList;

	void listPlayMode();

	char *playModeNames[playModeCount];


//----------------------------------
// odtwarzanie
	uint8_t isPlayingSample = 0;
	int8_t playNote = 24;
	uint8_t glidePreviewDif = 0;

//----------------------------------



};

extern cSamplePlayback samplePlayback;



#endif /* INTERFACE_SAMPLEPLAYBACK_H_ */