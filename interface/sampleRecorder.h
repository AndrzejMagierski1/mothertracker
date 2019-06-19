#ifndef INTERFACE_SAMPLERECORDER_H_
#define INTERFACE_SAMPLERECORDER_H_


#include <modulesBase.h>
#include "SD.h"


#include "mtStructs.h"





const uint8_t recordsCount = 5;
const char recordsNamesLabels[recordsCount][15] =
{
		"rec",
		"rec",
		"rec",
		"rec",
		"rec",

};




class cSampleRecorder: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cSampleRecorder()
	{
//		inActiveInstrumentsCount = 0;
//		inActiveInstrumentIndex = 0;
		editorInstrument = nullptr;
//		openedInstrFromActive = 0;
//		openedInstrumentIndex = 0;

		spectrumControl = nullptr;
		pointsControl = nullptr;
		topLabel[8] = {nullptr};
		bottomLabel[8] = {nullptr};
	}
	virtual ~cSampleRecorder() {}

	void showDefaultScreen();
	void showZoomValue();
	void showPlayModeList();

	void setDefaultScreenFunct();


	void processSpectrum();
	void processPoints();
	void modStartPoint(int16_t value);
	void modEndPoint(int16_t value);
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

	char *playModeNames[recordsCount];


//----------------------------------
// odtwarzanie
	uint8_t isPlayingSample = 0;
	int8_t playNote = 24;
	uint8_t glidePreviewDif = 0;

//----------------------------------



};

extern cSampleRecorder sampleRecorder;



#endif /* INTERFACE_SAMPLERECORDER_H_ */
