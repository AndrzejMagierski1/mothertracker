#include "sampleEditor/sampleEditor.h"
#include "core/graphicProcessing.h"
#include "mtSequencer.h"
#include "mtPadBoard.h"
#include "mtPadsBacklight.h"
#include "mtAudioEngine.h"

cSampleEditor sampleEditor;

static cSampleEditor *SE = &sampleEditor;

static uint16_t mainScreenFramePlaces[4][4] =
{
		{(800/8)*0+1, 424, 800/8-3, 55},
		{(800/8)*1+1, 424, 800/8-3, 55},
		{(800/8)*2+1, 424, 800/8-3, 55},
		{(800/8)*6+1, 31, 800/4-3, 394},
};



//************************************************************************** ACTION FUNCTIONS DECLARATIONS
static  uint8_t functSwitchModule(uint8_t button);
static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

//Main screen functions
static  uint8_t functMainScreenLeft();
static  uint8_t functMainScreenRight();
static  uint8_t functMainScreenUp();
static  uint8_t functMainScreenDown();
static  uint8_t functMainScreenEncoder(int16_t value);

static  uint8_t functSelectStartPoint();
static  uint8_t functSelectEndPoint();
static  uint8_t functSelectZoom();
static  uint8_t functListUp();
static 	uint8_t functListDown();
static  uint8_t functUndo();
static  uint8_t functApply();


//**************************************************************************

void cSampleEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
		//todo: popup
		return;
	}

	if(mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX)
	{
		mtProject.values.lastUsedInstrument = 0; //todo: zamiast tego popup ze instrument = midi i ze nie mozna wejsc
	}

	screenType = mainScreen;

	editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];
	//dostosowac to tak zeby byl editorInstrument ktory zawsze ma dobre parametry

	if(editorInstrument->isActive)
	{
		spectrumParams.address = editorInstrument->sample.address;
		spectrumParams.length = editorInstrument->sample.length;
	}
	else
	{
		spectrumParams.address = nullptr;
		spectrumParams.length = 0;
	}

	setMainScreenFunctions();
	setCommonFunctions();

	reloadInstrumentName();
	reloadStartPointText();
	reloadEndPointText();
	reloadZoomText();
	reloadPointsData();
	reloadFrameData(screenType);

	GP.processSpectrum( &spectrumParams, &zoom, &spectrumData);


	showMainScreen();



}

void cSampleEditor::update()
{
	if(needRefreshSpectrum)
	{
		needRefreshSpectrum = 0;
		refreshSpectrum();
	}
	if(needRefreshPlayhead)
	{
		refreshPlayhead();
	}
}

void cSampleEditor::stop()
{

}

void cSampleEditor::setCommonFunctions()
{
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	FM->setPadsGlobal(functPads);

	//todo: popup instrument
	//todo: popup note
}
void cSampleEditor::setMainScreenFunctions()
{
	FM->clearButtonsRange(interfaceButton0, interfaceButton7);
	FM->clearButton(interfaceButtonLeft);
	FM->clearButton(interfaceButtonRight);
	FM->clearButton(interfaceButtonUp);
	FM->clearButton(interfaceButtonDown);
	FM->clearAllPots();

	if(!editorInstrument->isActive) return;

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functMainScreenLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functMainScreenRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functMainScreenUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functMainScreenDown);

	FM->setButtonObj(interfaceButton0, buttonPress, functSelectStartPoint);
	FM->setButtonObj(interfaceButton1, buttonPress, functSelectEndPoint);
	FM->setButtonObj(interfaceButton2, buttonPress, functSelectZoom);
	FM->setButtonObj(interfaceButton3, buttonPress, functUndo);
	FM->setButtonObj(interfaceButton5, buttonPress, functApply);
	FM->setButtonObj(interfaceButton6, buttonPress, functListUp);
	FM->setButtonObj(interfaceButton7, buttonPress, functListDown);

	FM->setPotObj(interfacePot0, functMainScreenEncoder, nullptr);

}
void cSampleEditor::setParamsScreenFunctions()
{
	FM->clearButtonsRange(interfaceButton0, interfaceButton7);
	FM->clearButton(interfaceButtonLeft);
	FM->clearButton(interfaceButtonRight);
	FM->clearButton(interfaceButtonUp);
	FM->clearButton(interfaceButtonDown);
	FM->clearAllPots();

	if(!editorInstrument->isActive) return;
}

void cSampleEditor::reloadInstrumentName()
{
	if(!editorInstrument->isActive)
	{
		sprintf(currentInstrumentName,"%d. Empty slot",mtProject.values.lastUsedInstrument);
	}
	else
	{
		sprintf(currentInstrumentName,"%d. %s",mtProject.values.lastUsedInstrument, editorInstrument->sample.file_name);
	}
}
void cSampleEditor::reloadStartPointText()
{
	float length = editorInstrument->sample.length/44100.0;
	float localStartPoint = (length * selection.startPoint) / MAX_16BIT;

	sprintf(startPointText, "%.3fs", localStartPoint);
}
void cSampleEditor::reloadEndPointText()
{
	float length = editorInstrument->sample.length/44100.0;
	float localEndPoint = (length * selection.endPoint) / MAX_16BIT;

	sprintf(endPointText, "%.3fs", localEndPoint);
}
void cSampleEditor::reloadZoomText()
{
	sprintf(zoomText, "%.2f", zoom.zoomValue);
}
void cSampleEditor::reloadPlayheadValue()
{
	constexpr uint16_t PLAYHEAD_REFRESH_CONSTRAIN_US = 3000;

	if( refreshPlayheadTimer > PLAYHEAD_REFRESH_CONSTRAIN_US)
	{
		refreshPlayheadTimer = 0;

		uint16_t playheadValue_16BIT = (uint16_t)((instrumentPlayer[0].getWavePosition() * ((selection.endPoint - selection.startPoint) / (float)MAX_16BIT)) + selection.startPoint);

		if(zoom.zoomValue == 1.0) playheadValue = (600 *  playheadValue_16BIT)/MAX_16BIT;
		else if(zoom.zoomValue > 1.0)
		{
			if((int32_t)playheadValue_16BIT < zoom.zoomStart || (int32_t)playheadValue_16BIT > zoom.zoomEnd) playheadValue = 0;
			else
			{
				playheadValue = map(playheadValue_16BIT, zoom.zoomStart, zoom.zoomEnd, 0 , 600);
			}
		}
		needShowPlayhead = 1;
	}
}


void cSampleEditor::reloadFrameData(enScreenType s)
{
	if(s == mainScreen)
	{
		frameData.placesCount = 4;

		for ( uint8_t i = 0; i < frameData.placesCount; i++ )
		{
			frameData.places[i] = &mainScreenFramePlaces[i][0];
		}
	}
}
void cSampleEditor::reloadPointsData()
{
	if(selectedPlace[cSampleEditor::mainScreen] == cSampleEditor::startPointPlace)
	{
		spectrumPointsData.selected = 1;
		zoom.zoomPosition = selection.startPoint;

		bool isZoomActive = (zoom.zoomValue > 1) ;
		bool isPointDisplayed = (selection.startPoint < zoom.zoomStart || selection.startPoint > zoom.zoomEnd);


		if(isZoomActive && isPointDisplayed)
		{
			needRefreshSpectrum = 1;
		}
	}
	else if(selectedPlace[cSampleEditor::mainScreen] == cSampleEditor::endPointPlace)
	{
		spectrumPointsData.selected = 8;
		zoom.zoomPosition = selection.endPoint;


		bool isZoomActive = (zoom.zoomValue > 1) ;
		bool isPointDisplayed = (selection.endPoint < zoom.zoomStart || selection.endPoint > zoom.zoomEnd);
		if(isZoomActive && isPointDisplayed)
		{
			needRefreshSpectrum = 1;
		}
	}
	else
	{
		spectrumPointsData.selected = 0;
	}

	if(needRefreshSpectrum)
	{
		zoom.zoomStart =  zoom.zoomPosition - zoom.zoomWidth/2;
		zoom.zoomEnd = zoom.zoomPosition + zoom.zoomWidth/2;
		if(zoom.zoomStart < 0)
		{
			zoom.zoomEnd = zoom.zoomWidth;
			zoom.zoomStart = 0;
		}
		else if(zoom.zoomEnd > MAX_16BIT)
		{
			zoom.zoomEnd = MAX_16BIT;
			zoom.zoomStart = MAX_16BIT-zoom.zoomWidth;
		}
	}


	if(selection.startPoint >= zoom.zoomStart && selection.startPoint <= zoom.zoomEnd)
	{
		spectrumPointsData.startPoint = ((selection.startPoint-zoom.zoomStart) * (spectrumData.width - 1)) / zoom.zoomWidth;
	}
	else spectrumPointsData.startPoint = -1;

	if(selection.endPoint >= zoom.zoomStart && selection.endPoint <= zoom.zoomEnd)
	{
		spectrumPointsData.endPoint = ((selection.endPoint-zoom.zoomStart) * (spectrumData.width - 1)) / zoom.zoomWidth;
	}
	else spectrumPointsData.endPoint = -1;

}
//******************* REFRESH - RELOAD + DISPLAY
void cSampleEditor::refreshSpectrumPoints()
{
	reloadPointsData();
	showSpectrumPoints();
}

void cSampleEditor::refreshStartPoint()
{
	refreshSpectrumPoints();

	reloadStartPointText();
	showStartPointText();
}
void cSampleEditor::refreshEndPoint()
{
	refreshSpectrumPoints();

	reloadEndPointText();
	showEndPointText();
}
void cSampleEditor::refreshZoom()
{

	reloadZoomText();
	showZoomText();
}
void cSampleEditor::refreshEffectList()
{

}

void cSampleEditor::refreshSpectrum()
{
	GP.processSpectrum( &spectrumParams, &zoom, &spectrumData);
	showSpectrum();
}

void cSampleEditor::refreshPlayhead()
{
	reloadPlayheadValue();
	if(needShowPlayhead)
	{
		needShowPlayhead = 0;
		display.setControlValue(playhead, playheadValue);
		showPlayhead();
	}
}
//*******************

//******************* PARAMETERS MODIFICATORS
//MainScreen
void cSampleEditor::modStartPoint(int16_t val)
{
	uint16_t move_step = zoom.zoomWidth / 480;
	val *= move_step;

	if(selection.startPoint + val > MAX_16BIT) selection.startPoint = MAX_16BIT;
	else if(selection.startPoint + val < 0) selection.startPoint = 0;
	else selection.startPoint += val;

	if(selection.startPoint > selection.endPoint)
	{
		selection.startPoint = selection.endPoint - 1;
	}

	bool isZoomActive = (zoom.zoomValue > 1) ;
	bool isNewPointChanged = zoom.lastChangedPoint != 1;
	bool isPointDisplayed = (selection.startPoint < zoom.zoomStart || selection.startPoint > zoom.zoomEnd);

	if(isZoomActive && (isNewPointChanged || isPointDisplayed))
	{
		needRefreshSpectrum = 1;
	}

	zoom.zoomPosition = selection.startPoint;

	refreshStartPoint();
}
void cSampleEditor::modEndPoint(int16_t val)
{
	uint16_t move_step = zoom.zoomWidth / 480;
	val *= move_step;

	if(selection.endPoint + val > MAX_16BIT) selection.endPoint = MAX_16BIT;
	else if(selection.endPoint + val < 0) selection.endPoint = 0;
	else selection.endPoint += val;

	if(selection.startPoint > selection.endPoint)
	{
		selection.endPoint = selection.startPoint + 1;
	}

	bool isZoomActive = (zoom.zoomValue > 1) ;
	bool isNewPointChanged = zoom.lastChangedPoint != 2;
	bool isPointDisplayed = (selection.endPoint < zoom.zoomStart || selection.endPoint > zoom.zoomEnd);

	if(isZoomActive && (isNewPointChanged || isPointDisplayed))
	{
		needRefreshSpectrum = 1;
	}

	zoom.zoomPosition = selection.endPoint;

	refreshEndPoint();
}

void cSampleEditor::modZoom(int16_t val)
{
	//todo: wziac pod uwage ze nie tylko bedzie to dlugosc z sampla
	GP.spectrumChangeZoom(val, editorInstrument->sample.length, &zoom);

	refreshZoom();
	refreshSpectrumPoints();
	needRefreshSpectrum = 1;
}
//*******************

//************************************************************************** ACTION FUNCTIONS
static uint8_t functSwitchModule(uint8_t button)
{
	SE->eventFunct(eventSwitchModule,SE,&button,0);
	return 1;
}

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(state == 1)
	{
		if(mtPadBoard.getEmptyVoice() == 0) SE->needRefreshPlayhead = 1;

		padsBacklight.setFrontLayer(1,20, pad);
		mtPadBoard.startInstrument(pad,SE->editorInstrument->sample.address,SE->editorInstrument->sample.length);
	}
	else if(state == 0)
	{
		if(mtPadBoard.getVoiceTakenByPad(pad) == 0)
		{
			SE->needRefreshPlayhead = 0;
			SE->hidePlayhead();
		}

		padsBacklight.setFrontLayer(0,0, pad);
		mtPadBoard.stopInstrument(pad);
	}
	return 1;
}

static  uint8_t functMainScreenLeft()
{
	if(SE->selectedPlace[cSampleEditor::mainScreen] > 0) SE->selectedPlace[cSampleEditor::mainScreen]--;

	SE->refreshSpectrumPoints();

	SE->showFrame();
	return 1;
}
static  uint8_t functMainScreenRight()
{
	if(SE->selectedPlace[cSampleEditor::mainScreen] < (SE->frameData.placesCount - 1 )) SE->selectedPlace[cSampleEditor::mainScreen]++;

	SE->refreshSpectrumPoints();

	SE->showFrame();
	return 1;
}
static  uint8_t functMainScreenUp()
{

	return 1;
}
static  uint8_t functMainScreenDown()
{

	return 1;
}
static  uint8_t functMainScreenEncoder(int16_t value)
{
	switch(SE->selectedPlace[cSampleEditor::mainScreen])
	{
		case cSampleEditor::startPointPlace : 	SE->modStartPoint(value);		break;
		case cSampleEditor::endPointPlace : 	SE->modEndPoint(value);			break;
		case cSampleEditor::zoomPlace : 		SE->modZoom(value);				break;
		case cSampleEditor::effectPlaces : break;
		default: break;
	}

	return 1;
}

static  uint8_t functSelectStartPoint()
{
	SE->selectedPlace[cSampleEditor::mainScreen] = cSampleEditor::startPointPlace;

	SE->refreshSpectrumPoints();

	SE->showFrame();

	return 1;
}
static  uint8_t functSelectEndPoint()
{
	SE->selectedPlace[cSampleEditor::mainScreen] = cSampleEditor::endPointPlace;

	SE->refreshSpectrumPoints();

	SE->showSpectrumPoints();

	SE->showFrame();
	return 1;
}
static  uint8_t functSelectZoom()
{
	SE->selectedPlace[cSampleEditor::mainScreen] = cSampleEditor::zoomPlace;

	SE->refreshSpectrumPoints();

	SE->showFrame();
	return 1;
}
static  uint8_t functListUp()
{
	if(SE->selectedPlace[cSampleEditor::mainScreen] != cSampleEditor::effectPlaces)
	{
		SE->selectedPlace[cSampleEditor::mainScreen] = cSampleEditor::effectPlaces;

		SE->refreshSpectrumPoints();

		SE->showFrame();
	}
	else
	{

	}

	return 1;
}
static 	uint8_t functListDown()
{
	if(SE->selectedPlace[cSampleEditor::mainScreen] != cSampleEditor::effectPlaces)
	{
		SE->selectedPlace[cSampleEditor::mainScreen] = cSampleEditor::effectPlaces;

		SE->refreshSpectrumPoints();

		SE->showFrame();
	}
	else
	{

	}

	return 1;
}
static  uint8_t functUndo()
{
	return 1;
}
static  uint8_t functApply()
{
	return 1;
}

