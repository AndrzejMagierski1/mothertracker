

#include <samplePlayback.h>
#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtPadsBacklight.h"

#include "graphicProcessing.h"



cSamplePlayback samplePlayback;
static cSamplePlayback* SP = &samplePlayback;

extern strMtProject mtProject;


static  uint8_t functPlayAction();
static  uint8_t functRecAction();


static  uint8_t functInstrument(uint8_t state);


static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functSelectStart();
static  uint8_t functSelectLoop1();
static  uint8_t functSelectLoop2();
static  uint8_t functSelectEnd();
static  uint8_t functSelectZoom();
static  uint8_t functPlayMode(uint8_t button);

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

static uint8_t functShift(uint8_t value);



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);
static uint8_t functStepNote(uint8_t value);



constexpr uint32_t PLAY_REFRESH_US = 5000;


void cSamplePlayback::update()
{
	if(refreshSpectrum)
	{
		processSpectrum(editorInstrument, &zoom, &spectrum);

		display.refreshControl(SP->spectrumControl);

		refreshSpectrum = 0;
	}

	if(refreshPoints)
	{
		processPoints();


		display.refreshControl(SP->pointsControl);

		refreshPoints = 0;
	}
	if(refreshSpectrumProgress)
	{
		display.setControlValue(progressCursor,playProgressInSpectrum);
		display.refreshControl(progressCursor);
		refreshSpectrumProgress = 0;
	}
	if(isPlayingSample)
	{
		calcPlayProgressValue();
		if(isPlayingSample)	showPreviewValue(); // w calcPlayProgress jest mozliwosc wyzerowania tej flagi wtedy nie chcemy wyswietlac wartosci;
		else
		{
			hidePreviewValue();
			mtPadBoard.clearVoice(0);
		}
		if(instrumentPlayer[0].getInterfaceEndReleaseFlag())
		{
			instrumentPlayer[0].clearInterfaceEndReleaseFlag();

			playProgressValue=0;
			playProgressInSpectrum = 0;
			isPlayingSample = 0;
			refreshSpectrumProgress = 1;
			hidePreviewValue();
			mtPadBoard.clearVoice(0);
		}
		if(instrumentPlayer[0].getInterfacePlayingEndFlag())
		{
			instrumentPlayer[0].clearInterfacePlayingEndFlag();

			playProgressValue=0;
			playProgressInSpectrum = 0;
			isPlayingSample = 0;
			refreshSpectrumProgress = 1;
			hidePreviewValue();
			mtPadBoard.clearVoice(0);
		}
	}
	else
	{
		if(instrumentPlayer[0].getInterfacePlayingEndFlag())
		{
			instrumentPlayer[0].clearInterfacePlayingEndFlag();
		}
	}

}

void cSamplePlayback::start(uint32_t options)
{
	moduleRefresh = 1;

	points.selected = (selectedPlace >= 0 && selectedPlace <= 3) ? selectedPlace+1 : 0;


//--------------------------------------------------------------------


	mtPadBoard.setPadNotes(mtProject.values.padBoardScale,
			mtProject.values.padBoardNoteOffset,
			mtProject.values.padBoardRootNote = 36);

//	mtPadBoard.configureInstrumentPlayer(mtProject.values.padBoardMaxVoices);
	mtPadBoard.configureInstrumentPlayer(8);

	editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];


	// wykrywanie czy wczytywany inny niz poprzednio/nowy sampel
	// patrzy na dlugosc sampla
	if(editorInstrument->sample.length != lastSampleLength)
	{
		lastSampleLength = editorInstrument->sample.length;


		uint16_t start_end_width = editorInstrument->endPoint - editorInstrument->startPoint;

		if(start_end_width < MAX_16BIT/4) // wysrodkowanie startPoint z zoomem jesli
		{
			start_end_width += start_end_width/20; // dodatkowe po 10% po bokach
			zoom.zoomResolution = ((( start_end_width)/MAX_16BIT) * editorInstrument->sample.length) / 600;

			uint16_t min_resolution = editorInstrument->sample.length/MAX_16BIT + 1;
			if(zoom.zoomResolution < min_resolution) zoom.zoomResolution  = min_resolution;

			zoom.zoomValue = editorInstrument->sample.length/((zoom.zoomResolution)*600.0);
		}
		else
		{
			zoom.zoomResolution = editorInstrument->sample.length / 600;
			zoom.zoomValue = 1.0;
		}
	}


	FM->setPadsGlobal(functPads);

//--------------------------------------------------------------------

	processSpectrum(editorInstrument, &zoom, &spectrum);
	processPoints();
	listPlayMode();

	showPlayModeList();
	showZoomValue();

	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonEnvelopes, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();

	activateLabelsBorder();
}


void cSamplePlayback::stop()
{

	moduleRefresh = 0;
}


void cSamplePlayback::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

//	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	FM->setButtonObj(interfaceButtonShift, functShift);
//	FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);


	FM->setButtonObj(interfaceButton0, buttonPress, functSelectStart);
	FM->setButtonObj(interfaceButton1, buttonPress, functSelectLoop1);
	FM->setButtonObj(interfaceButton2, buttonPress, functSelectLoop2);
	FM->setButtonObj(interfaceButton3, buttonPress, functSelectEnd);

	FM->setButtonObj(interfaceButton5, buttonPress, functSelectZoom);

	FM->setButtonObj(interfaceButton6, buttonPress, functPlayMode);
	FM->setButtonObj(interfaceButton7, buttonPress, functPlayMode);

	FM->setButtonObj(interfaceButtonInstr, functInstrument);
	FM->setButtonObj(interfaceButtonNote, functStepNote);

	FM->setPotObj(interfacePot0, functEncoder, nullptr);


}

//==============================================================================================================


void cSamplePlayback::processPoints()
{

	points.pointsType = editorInstrument->playMode;

	if(editorInstrument->startPoint >= zoom.zoomStart && editorInstrument->startPoint <= zoom.zoomEnd)
	{
		points.startPoint = ((editorInstrument->startPoint-zoom.zoomStart) * 599) / zoom.zoomWidth;
	}
	else points.startPoint = -1;

	if(editorInstrument->endPoint >= zoom.zoomStart && editorInstrument->endPoint <= zoom.zoomEnd)
	{
		points.endPoint = ((editorInstrument->endPoint-zoom.zoomStart) * 599) / zoom.zoomWidth;
	}
	else points.endPoint = -1;

	if(editorInstrument->loopPoint1 >= zoom.zoomStart && editorInstrument->loopPoint1 <= zoom.zoomEnd)
	{
		points.loopPoint1 = ((editorInstrument->loopPoint1-zoom.zoomStart) * 599) / zoom.zoomWidth;
	}
	else points.loopPoint1 = -1;

	if(editorInstrument->loopPoint2 >= zoom.zoomStart && editorInstrument->loopPoint2 <= zoom.zoomEnd)
	{
		points.loopPoint2 = ((editorInstrument->loopPoint2-zoom.zoomStart) * 599) / zoom.zoomWidth;
	}
	else points.loopPoint2 = -1;

}



void cSamplePlayback::listPlayMode()
{

	for(uint8_t i = 0; i < playModeCount; i++)
	{
		playModeNames[i] = (char*)&playModeFunctLabels[i][0];
	}


}



//==============================================================================================================
static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
	{
		sequencer.stop();
	}

	if(state == 1)
	{
		//uint8_t note = mtPadBoard.convertPadToNote(pad);
		//if(note > 48) note = 48;
		//editorInstrument->tune = note;

		if(mtPadBoard.getEmptyVoice() < 0) return 1;

		if(mtPadBoard.getEmptyVoice() == 0)
		{
			SP->playPitch = notes[mtPadBoard.convertPadToNote(pad)];
			SP->playProgresValueTim = ((( (SP->editorInstrument->sample.length/44100.0 ) * SP->editorInstrument->startPoint) / MAX_16BIT) * 1000000) /SP->playPitch;
			SP->refreshPlayProgressValue = 0;
			SP->loopDirection = 0;
			SP->isPlayingSample = 1;
		}



		padsBacklight.setFrontLayer(1,20, pad);
		mtPadBoard.startInstrument(pad, mtProject.values.lastUsedInstrument,-1);


	}
	else if(state == 0)
	{
		padsBacklight.setFrontLayer(0,0, pad);
		mtPadBoard.stopInstrument(pad);
		if((!SP->editorInstrument->envelope[envAmp].enable) || (SP->editorInstrument->envelope[envAmp].release == 0))
		{
			if(mtPadBoard.getVoiceTakenByPad(pad) == 0)
			{
				SP->playProgressValue=0;
				SP->playProgressInSpectrum = 0;
				SP->isPlayingSample = 0;
				SP->refreshSpectrumProgress = 1;
				SP->hidePreviewValue();
			}
		}

	}

	return 1;
}

static  uint8_t functSelectStart()
{
	SP->points.selected = 1;
	SP->selectedPlace = 0;
	SP->activateLabelsBorder();



	if(SP->zoom.zoomValue > 1.0 )
	{
		SP->refreshSpectrum = 1;
		SP->zoom.lastChangedPoint = 1;
	}



	SP->refreshPoints = 1;

	return 1;
}

static  uint8_t functSelectLoop1()
{
	if(SP->editorInstrument->playMode == singleShot) return 1;
	SP->points.selected = 3;
	SP->selectedPlace = 1;
	SP->activateLabelsBorder();

	if(SP->zoom.zoomValue > 1.0 )
	{
		SP->refreshSpectrum = 1;
		SP->zoom.lastChangedPoint = 3;
	}
	SP->refreshPoints = 1;

	return 1;
}

static  uint8_t functSelectLoop2()
{
	if(SP->editorInstrument->playMode == singleShot) return 1;
	SP->points.selected = 4;
	SP->selectedPlace = 2;
	SP->activateLabelsBorder();

	if(SP->zoom.zoomValue > 1.0 )
	{
		SP->refreshSpectrum = 1;
		SP->zoom.lastChangedPoint = 4;
	}
	SP->refreshPoints = 1;


	return 1;
}

static  uint8_t functSelectEnd()
{
	SP->points.selected = 2;
	SP->selectedPlace = 3;
	SP->activateLabelsBorder();

	if(SP->zoom.zoomValue > 1.0 )
	{
		SP->zoom.lastChangedPoint = 2;
		SP->refreshSpectrum = 1;
	}
	SP->refreshPoints = 1;

	return 1;
}


static  uint8_t functSelectZoom()
{

	SP->selectedPlace = 5;
	SP->activateLabelsBorder();

	return 1;
}


static  uint8_t functPlayMode(uint8_t button)
{
//	if(button == interfaceButton6)
//	{
//		SP->changePlayModeSelection(-1);
//	}
//	else //if(button == interfaceButton7)
//	{
//		SP->changePlayModeSelection(1);
//	}

	SP->selectedPlace = 6;
	SP->activateLabelsBorder();
	SP->points.selected = 0;

	SP->refreshPoints = 1;

	return 1;
}

static  uint8_t functEncoder(int16_t value)
{


	switch(SP->selectedPlace)
	{
	case 0: SP->modStartPoint(value); 			break;
	case 1: SP->modLoopPoint1(value); 			break;
	case 2: SP->modLoopPoint2(value); 			break;
	case 3: SP->modEndPoint(value); 			break;
	case 5: SP->changeZoom(value);				break;
	case 6: SP->changePlayModeSelection(value);	break;
	}




	return 1;
}

static  uint8_t functSelectStart();
static  uint8_t functSelectLoop1();
static  uint8_t functSelectLoop2();



static  uint8_t functLeft()
{
	if(SP->selectedPlace > 0) SP->selectedPlace--;

	switch(SP->selectedPlace)
	{
		case 0: functSelectStart();		break;
		case 1:
			if(SP->editorInstrument->playMode == singleShot)
			{
				SP->selectedPlace = 0;
				functSelectStart();
			}
			else
			{
				functSelectLoop1();
			}
			break;
		case 2:
			if(SP->editorInstrument->playMode == singleShot)
			{
				SP->selectedPlace = 0;
				functSelectStart();
			}
			else
			{
				functSelectLoop2();
			}
			break;
		case 3: functSelectEnd();		break;
		case 4: SP->selectedPlace--;	functSelectEnd();break;
		case 5: functSelectZoom();		break;
		case 6:
		{
		SP->selectedPlace = 6;
		SP->activateLabelsBorder();
		SP->points.selected = 0;
		break;
		}
	}


	return 1;
}


static  uint8_t functRight()
{
	if(SP->selectedPlace < SP->frameData.placesCount-1) SP->selectedPlace++;

	switch(SP->selectedPlace)
	{
		case 0: functSelectStart();		break;
		case 1:
			if(SP->editorInstrument->playMode == singleShot)
			{
				SP->selectedPlace = 3;
				functSelectEnd();
			}
			else
			{
				functSelectLoop1();
			}
			break;
		case 2:
			if(SP->editorInstrument->playMode == singleShot)
			{
				SP->selectedPlace = 3;
				functSelectEnd();
			}
			else
			{
				functSelectLoop2();
			}
			break;
		case 3: functSelectEnd();		break;
		case 4: SP->selectedPlace++;	functSelectZoom();		break;
		case 5: functSelectZoom();		break;
		case 6:
		{
		SP->selectedPlace = 6;
		SP->activateLabelsBorder();
		SP->points.selected = 0;
		break;
		}
	}



	return 1;
}


static  uint8_t functUp()
{
	switch(SP->selectedPlace)
	{
	case 0: SP->modStartPoint(1); 			break;
	case 1: SP->modLoopPoint1(1); 			break;
	case 2: SP->modLoopPoint2(1); 			break;
	case 3: SP->modEndPoint(1); 			break;
	case 5: SP->changeZoom(1);				break;
	case 6: SP->changePlayModeSelection(-1);	break;
	}

	return 1;
}


static  uint8_t functDown()
{
	switch(SP->selectedPlace)
	{
	case 0: SP->modStartPoint(-1); 			break;
	case 1: SP->modLoopPoint1(-1); 			break;
	case 2: SP->modLoopPoint2(-1); 			break;
	case 3: SP->modEndPoint(-1); 			break;
	case 5: SP->changeZoom(-1);				break;
	case 6: SP->changePlayModeSelection(1);	break;
	}

	return 1;
}




static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == 0)
	{
		sequencer.play();
	}
	else if(sequencer.getSeqState() == 1)
	{
		sequencer.stop();
	}

	return 1;
}



static  uint8_t functRecAction()
{


	return 1;
}




static uint8_t functSwitchModule(uint8_t button)
{

	SP->eventFunct(eventSwitchModule,SP,&button,0);

	return 1;
}


//======================================================================================================================
void cSamplePlayback::changeZoom(int16_t value)
{


	uint16_t max_resolution = editorInstrument->sample.length / 600;
	//uint16_t max_resolution = ((600/MAX_16BIT)*editorInstrument->sample.length) / 600;
	uint16_t min_resolution = editorInstrument->sample.length/MAX_16BIT + 1;

	if(zoom.zoomResolution - value < min_resolution) zoom.zoomResolution  = min_resolution;
	else if(zoom.zoomResolution - value > max_resolution ) zoom.zoomResolution = max_resolution;
	else zoom.zoomResolution -= value;

	if(zoom.zoomResolution == max_resolution) zoom.zoomValue = 1.0;
	else zoom.zoomValue = editorInstrument->sample.length/((zoom.zoomResolution)*600.0);



	refreshSpectrum = 1;
	refreshPoints = 1;

	showZoomValue();

}

void cSamplePlayback::changePlayModeSelection(int16_t value)
{


	if(editorInstrument->playMode + value < 0) editorInstrument->playMode = 0;
	else if(editorInstrument->playMode + value > playModeCount-1) editorInstrument->playMode = playModeCount-1;
	else  editorInstrument->playMode += value;

	if((editorInstrument->playMode == singleShot) && (value < 0 )) hideLoopPoints();
	else if((editorInstrument->playMode == loopForward) && (value > 0) )
	{

		if(editorInstrument->loopPoint1 >= editorInstrument->loopPoint2) editorInstrument->loopPoint1 = editorInstrument->loopPoint2 - 1;

		if(editorInstrument->loopPoint1 <= editorInstrument->startPoint) editorInstrument->loopPoint1 = editorInstrument->startPoint+1;
		if(editorInstrument->loopPoint2 >= editorInstrument->endPoint) editorInstrument->loopPoint2 = editorInstrument->endPoint-1;
		showLoopPoints();
	}


	refreshPoints = 1;

	display.setControlValue(playModeListControl, editorInstrument->playMode);
	display.refreshControl(playModeListControl);

}

void cSamplePlayback::modStartPoint(int16_t value)
{
	// obliczenie kroku przesuniecia w zaleznosci od ilosci widzianych probek na wyswietlaczu
	uint16_t move_step = zoom.zoomWidth / 600;
	uint16_t dif;
	value = value * move_step;

	if(editorInstrument->startPoint + value < SAMPLE_POINT_POS_MIN) editorInstrument->startPoint  = 0;
	else if(editorInstrument->startPoint + value > SAMPLE_POINT_POS_MAX ) editorInstrument->startPoint  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->startPoint += value;

	if(editorInstrument->startPoint > editorInstrument->endPoint) editorInstrument->startPoint = editorInstrument->endPoint-1;

	if(editorInstrument->playMode != singleShot)
	{
		if(editorInstrument->startPoint > editorInstrument->loopPoint1)
		{
			dif = editorInstrument->loopPoint2 - editorInstrument->loopPoint1;
			editorInstrument->loopPoint1 = editorInstrument->startPoint;

			if(editorInstrument->loopPoint1 + dif > editorInstrument->endPoint)
			{
				editorInstrument->loopPoint2 = editorInstrument->endPoint;
				editorInstrument->loopPoint1 = editorInstrument->loopPoint2 - dif;
				editorInstrument->startPoint = editorInstrument->loopPoint1;
				instrumentPlayer[0].setStatusBytes(LP1_MASK);
				instrumentPlayer[0].setStatusBytes(LP2_MASK);
			}
			else
			{
				editorInstrument->loopPoint2 = editorInstrument->loopPoint1 + dif;
				instrumentPlayer[0].setStatusBytes(LP2_MASK);
			}
		}
	}

	// odswiez spektrum tylko jesli: zoom wiekszy niz 1, ostatnio modyfikowany inny punkt, punkt jest poza widocznym obszarem
	if(zoom.zoomValue > 1 && zoom.lastChangedPoint != 1
		&& (editorInstrument->startPoint < zoom.zoomStart || editorInstrument->startPoint > zoom.zoomEnd)) refreshSpectrum = 1;

	zoom.lastChangedPoint = 1;
	refreshPoints = 1;

	showStartPointValue();
}

void cSamplePlayback::modEndPoint(int16_t value)
{
	uint16_t move_step = zoom.zoomWidth / 600;
	uint16_t dif;
	value = value * move_step;

	if(editorInstrument->endPoint + value < SAMPLE_POINT_POS_MIN) editorInstrument->endPoint  = 0;
	else if(editorInstrument->endPoint + value > SAMPLE_POINT_POS_MAX ) editorInstrument->endPoint  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->endPoint += value;

	if(editorInstrument->endPoint < editorInstrument->startPoint) editorInstrument->endPoint = editorInstrument->startPoint+1;

	if(editorInstrument->playMode != singleShot)
	{
		if(editorInstrument->endPoint < editorInstrument->loopPoint2)
		{
			dif = editorInstrument->loopPoint2 - editorInstrument->loopPoint1;

			editorInstrument->loopPoint2 = editorInstrument->endPoint - 1;

			if(editorInstrument->loopPoint2 - dif <= editorInstrument->startPoint)
			{
				editorInstrument->loopPoint1 = editorInstrument->startPoint + 1;
				editorInstrument->loopPoint2 = editorInstrument->loopPoint1 + dif;
				editorInstrument->endPoint = editorInstrument->loopPoint2 + 1;
				instrumentPlayer[0].setStatusBytes(LP1_MASK);
				instrumentPlayer[0].setStatusBytes(LP2_MASK);
			}
			else
			{
				editorInstrument->loopPoint1 = editorInstrument->loopPoint2 - dif;
				instrumentPlayer[0].setStatusBytes(LP1_MASK);
			}
		}
	}

	if(zoom.zoomValue > 1 && zoom.lastChangedPoint != 2
			&& (editorInstrument->endPoint < zoom.zoomStart || editorInstrument->endPoint > zoom.zoomEnd)) refreshSpectrum = 1;

	zoom.lastChangedPoint = 2;
	refreshPoints = 1;

	showEndPointValue();
}

void cSamplePlayback::modLoopPoint1(int16_t value)
{
	uint16_t move_step = zoom.zoomWidth / 600;
	value = value * move_step;

	if(editorInstrument->loopPoint1 + value < SAMPLE_POINT_POS_MIN) editorInstrument->loopPoint1  = 0;
	else if(editorInstrument->loopPoint1 + value > SAMPLE_POINT_POS_MAX ) editorInstrument->loopPoint1  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->loopPoint1 += value;

	if(editorInstrument->loopPoint1 <= editorInstrument->startPoint) editorInstrument->loopPoint1 = editorInstrument->startPoint+1;
	if(editorInstrument->loopPoint1 >= editorInstrument->loopPoint2) editorInstrument->loopPoint1 = editorInstrument->loopPoint2-1;

	if(zoom.zoomValue > 1 && zoom.lastChangedPoint != 3
			&& (editorInstrument->loopPoint1 < zoom.zoomStart || editorInstrument->loopPoint1 > zoom.zoomEnd)) refreshSpectrum = 1;

	instrumentPlayer[0].setStatusBytes(LP1_MASK);


	zoom.lastChangedPoint = 3;
	refreshPoints = 1;

	showLoopPoint1Value();
}

void cSamplePlayback::modLoopPoint2(int16_t value)
{
	uint16_t move_step = zoom.zoomWidth / 600;
	value = value * move_step;

	if(editorInstrument->loopPoint2 + value < SAMPLE_POINT_POS_MIN) editorInstrument->loopPoint2  = 0;
	else if(editorInstrument->loopPoint2 + value > SAMPLE_POINT_POS_MAX ) editorInstrument->loopPoint2  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->loopPoint2 += value;

	if(editorInstrument->loopPoint2 >= editorInstrument->endPoint) editorInstrument->loopPoint2 = editorInstrument->endPoint - 1;
	if(editorInstrument->loopPoint2 <= editorInstrument->loopPoint1) editorInstrument->loopPoint2 = editorInstrument->loopPoint1+1;

	if(zoom.zoomValue > 1 && zoom.lastChangedPoint != 4
			&& (editorInstrument->loopPoint2 < zoom.zoomStart || editorInstrument->loopPoint2 > zoom.zoomEnd)) refreshSpectrum = 1;

	instrumentPlayer[0].setStatusBytes(LP2_MASK);

	zoom.lastChangedPoint = 4;
	refreshPoints = 1;
	showLoopPoint2Value();
}


static uint8_t functShift(uint8_t value)
{
//	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
//	{
//		sequencer.stop();
//	}
//
//	if(value == 1)
//	{
//		//eventFunct(mtInstrumentEditorEventPadPress, &interfacePadStop, 0, 0);
//		sequencer.stop();
//
//		SP->isPlayingSample = 1;
//		SP->playProgresValueTim = (( (SP->editorInstrument->sample.length/44100.0 ) * SP->editorInstrument->startPoint) / MAX_16BIT) * 1000000;
//		SP->refreshPlayProgressValue = 0;
//		SP->loopDirection = 0;
//		SP->playPitch = 1.0;
//		if(SP->editorInstrument->glide > 0)
//		{
//			switch(	SP->glidePreviewDif)
//			{
//				case 0: SP->playNote = 24;	break;
//				case 1: SP->playNote = (SP->playNote == 24)? 25 : 24; 	break;
//				case 2: SP->playNote = (SP->playNote == 24)? 36 : 24; 	break;
//				case 3: SP->playNote = (SP->playNote == 24)? 47 : 24; 	break;
//			}
//		}
//
//		instrumentPlayer[0].noteOn(mtProject.values.lastUsedInstrument, SP->playNote, -1);
//	}
//	else if(value == 0)
//	{
//		if(SP->isPlayingSample) instrumentPlayer[0].noteOff();
//		SP->isPlayingSample = 0;
//		SP->playProgressValue = 0;
//		SP->playProgressInSpectrum = 0;
//
//		SP->refreshSpectrumProgress = 1;
//
//		//todo: jezeli shift ma byc uzywany rownolegle z klawiaturą trzeba bedzie dodac flage poniewaz shift zajmuje voice 0 ktory moze zajac takze padboard
//		// releasy moglyby sie nie zgadzac - dlatego na razie nie ma tu obsługi
//	}

	return 1;
}

static  uint8_t functInstrument(uint8_t state)
{
	if(state == buttonPress)
	{
		uint8_t buttonId  = interfaceButtonInstr;
		SP->eventFunct(eventSwitchModule, SP, &buttonId, 0);
	}
	else if(state == buttonHold)
	{

	}

	return 1;
}

void cSamplePlayback::calcPlayProgressValue()
{
	float  recTimeValue = editorInstrument->sample.length/44100.0;
	uint32_t localRecTimeValue = recTimeValue * 1000000;
	uint32_t localEndTimeValue = ((( recTimeValue  * editorInstrument->endPoint) / MAX_16BIT) * 1000000)/playPitch;
	uint32_t localLoopPoint1Value = 0;
	uint32_t localLoopPoint2Value = 0;
	if(editorInstrument->playMode != singleShot)
	{
		localLoopPoint1Value = ((( recTimeValue  * editorInstrument->loopPoint1) / MAX_16BIT) * 1000000)/playPitch;
		localLoopPoint2Value = ((( recTimeValue  * editorInstrument->loopPoint2) / MAX_16BIT) * 1000000)/playPitch;
	}

	if(editorInstrument->playMode == loopForward)
	{
		if(playProgresValueTim >= localLoopPoint2Value ) playProgresValueTim = localLoopPoint1Value;
	}
	else if(editorInstrument->playMode == loopBackward)
	{
		if((playProgresValueTim >= localLoopPoint2Value ) && (loopDirection == 0 ))
		{
			loopDirection = 1;
			playProgresValueBackwardTim = 0;
		}
		else if((localLoopPoint2Value - playProgresValueBackwardTim) <= localLoopPoint1Value )
		{
			playProgresValueBackwardTim = 0;
		}
	}
	else if(editorInstrument->playMode == loopPingPong)
	{
		if((playProgresValueTim >= localLoopPoint2Value ) && (!loopDirection))
		{
			loopDirection = 1;
			playProgresValueBackwardTim = 0;
		}
		else if(((localLoopPoint2Value - playProgresValueBackwardTim) <= localLoopPoint1Value ) && (loopDirection))
		{
			loopDirection = 0;
			playProgresValueTim = localLoopPoint1Value;
		}
	}

	if((playProgresValueTim >= localEndTimeValue) && (!loopDirection))
	{
		playProgressValue=0;
		playProgressInSpectrum = 0;
		isPlayingSample = 0;
		refreshSpectrumProgress = 1;
		return;
	}
	if( refreshPlayProgressValue > PLAY_REFRESH_US)
	{
		refreshPlayProgressValue = 0;

		if(!loopDirection)
		{
			playProgressValue = playPitch * MAX_16BIT*(playProgresValueTim/(float)localRecTimeValue);
		}
		else
		{
			playProgressValue = playPitch * MAX_16BIT*((localLoopPoint2Value - playProgresValueBackwardTim)/(float)localRecTimeValue);
		}


		if(zoom.zoomValue == 1.0) playProgressInSpectrum = (600 *  playProgressValue)/MAX_16BIT;
		else if(zoom.zoomValue > 1.0)
		{
			if(playProgressValue < zoom.zoomStart || playProgressValue > zoom.zoomEnd) playProgressInSpectrum = 0;
			else
			{
				playProgressInSpectrum = map(playProgressValue, zoom.zoomStart, zoom.zoomEnd, 0 , 600);
			}
		}


		refreshSpectrumProgress = 1;
	}


}

static uint8_t functStepNote(uint8_t value)
{
	if(value == buttonRelease)
	{
		SP->hideNotePopout();
		SP->setDefaultScreenFunct();
	}
	else if(value == buttonHold)
	{
		for(uint8_t i = 0; i < 48; i++)
		{
			interfaceGlobals.padNamesPointer[i] = (char*)mtNotes[mtPadBoard.getNoteFromPad(i)];
		}

		SP->FM->clearButtonsRange(interfaceButton0, interfaceButton7);
		SP->FM->clearButtonsRange(interfaceButtonUp, interfaceButtonRight);
		SP->FM->clearAllPots();

		SP->showNotePopout();
	}

	return 1;
}
