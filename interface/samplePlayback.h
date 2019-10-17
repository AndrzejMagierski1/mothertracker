#ifndef INTERFACE_SAMPLEPLAYBACK_H_
#define INTERFACE_SAMPLEPLAYBACK_H_


#include <modulesBase.h>
#include "SD.h"

#include "mtSequencer.h"

#include "mtStructs.h"


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
//		inActiveInstrumentsCount = 0;
//		inActiveInstrumentIndex = 0;
		editorInstrument = nullptr;
//		openedInstrFromActive = 0;
//		openedInstrumentIndex = 0;
		progressCursor = nullptr;
		spectrumControl = nullptr;
		pointsControl = nullptr;
		topLabel[7] = {nullptr};
		bottomLabel[6] = {nullptr};

		lastSampleLength = 0;
	}
	virtual ~cSamplePlayback() {}

	void showDefaultScreen();
	void showZoomValue();
	void showPlayModeList();
	void showTitleBar();

	void setDefaultScreenFunct();
	void cancelPopups();

	void processPoints();
	void modStartPoint(int16_t value);
	void modEndPoint(int16_t value);
	void modLoopPoint1(int16_t value);
	void modLoopPoint2(int16_t value);
	void changeZoom(int16_t value);
	void changePlayModeSelection(int16_t value);
	void showPreviewValue();
	void hidePreviewValue();

	char startPointValueText[8];
	char endPointValueText[8];
	char loopPoint1ValueText[8];
	char loopPoint2ValueText[8];
	char playTimeValueText[8];
	void showStartPointValue();
	void showEndPointValue();
	void showLoopPoint1Value();
	void showLoopPoint2Value();

	void hideLoopPoints();
	void showLoopPoints();

	void calcPlayProgressValue();
	void activateLabelsBorder();

	strFrameData frameData;

	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl playModeListControl;
	hControl spectrumControl;
	hControl pointsControl;
	hControl frameControl;
	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;
	hControl progressCursor;



	uint8_t selectedPlace = 0;


//----------------------------------
// spectrum + punkty

	uint8_t refreshSpectrum = 0;
	uint8_t refreshPoints = 0;
	uint8_t refreshSpectrumProgress = 0;
	uint32_t playProgressValue = 0; // 0 - MAX_LEN_RECORD
	uint16_t playProgressInSpectrum = 0; // 0 - 600
	uint8_t loopDirection = 0;
	elapsedMicros playProgresValueBackwardTim = 0;
	elapsedMicros playProgresValueTim = 0;
	elapsedMicros refreshPlayProgressValue = 0;
	float playPitch = 1.0 ;


	strInstrument * editorInstrument;
	uint32_t lastSampleLength = 0xFFFFFFFF;

	strZoomParams zoom;

	char zoomTextValue[7];

	strTrackerSpectrum spectrum;
	strTrackerPoints points;

//----------------------------------
// aktualny instrument na belce tytułowej
	void showActualInstrument();
	//char actualInstrName[SAMPLE_NAME_SIZE+4];

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
