

#include <samplePlayback.h>
#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtPadsBacklight.h"
#include "interfacePopups.h"

#include "graphicProcessing.h"
#include "mtFileManager.h"


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

static 	uint8_t functPreview(uint8_t state);

static  uint8_t functSelectStart(uint8_t state);
static  uint8_t functSelectLoop1(uint8_t state);
static  uint8_t functSelectLoop2(uint8_t state);
static  uint8_t functSelectEnd(uint8_t state);
static  uint8_t functSelectZoom();
static  uint8_t functPlayMode(uint8_t button);

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

static uint8_t functShift(uint8_t value);



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);
static uint8_t functStepNote(uint8_t value);

static void modWavetablePostion(int16_t value);
static void modStartPoint(int16_t value);
static void modEndPoint(int16_t value);
static void modLoopPoint1(int16_t value);
static void modLoopPoint2(int16_t value);
static void changeZoom(int16_t value);
static void changePlayModeSelection(int16_t value);



constexpr uint32_t PLAY_REFRESH_US = 5000;


void cSamplePlayback::update()
{
	if(refreshSpectrum)
	{
		GP.processSpectrum(editorInstrument, &zoom, &spectrum);

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
		if(instrumentPlayer[0].getInterfaceEndReleaseFlag())
		{
			instrumentPlayer[0].clearInterfaceEndReleaseFlag();
		}
	}

}

void cSamplePlayback::start(uint32_t options)
{
	moduleRefresh = 1;

	//--------------------------------------------------------------------

	loadedInstrumentType =  mtProject.instrument[mtProject.values.lastUsedInstrument].sample.type;

	if(loadedInstrumentType == mtSampleTypeWavetable)
	{
		if(selectedPlace != 1)
		{
			selectedPlace = 1;
		}
	}

	if(mtProject.values.lastUsedInstrument < INSTRUMENTS_COUNT)
	{
		editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];

		if(loadedInstrumentType == mtSampleTypeWaveFile)
		{
			//warunki
			if(editorInstrument->loopPoint1 >= editorInstrument->loopPoint2) editorInstrument->loopPoint1 = editorInstrument->loopPoint2 - 1;

			if(((editorInstrument->loopPoint1 >= editorInstrument->endPoint) && (editorInstrument->loopPoint2 >= editorInstrument->endPoint))
					|| ((editorInstrument->loopPoint1 <= editorInstrument->startPoint) && (editorInstrument->loopPoint2 <= editorInstrument->startPoint))
					|| ((editorInstrument->loopPoint1 <= editorInstrument->startPoint) && (editorInstrument->loopPoint2 >= editorInstrument->endPoint)))
			{
				editorInstrument->loopPoint1 = editorInstrument->startPoint+1;
				editorInstrument->loopPoint2 = editorInstrument->endPoint-1;
			}
			else if((editorInstrument->loopPoint1 >= editorInstrument->startPoint) && (editorInstrument->loopPoint1 <= editorInstrument->endPoint) &&
					(editorInstrument->loopPoint2 >= editorInstrument->endPoint))
			{
				editorInstrument->loopPoint2 = editorInstrument->endPoint - 1;
			}
			else if((editorInstrument->loopPoint2 >= editorInstrument->startPoint) && (editorInstrument->loopPoint2 <= editorInstrument->endPoint) &&
					(editorInstrument->loopPoint1 <= editorInstrument->startPoint))
			{
				editorInstrument->loopPoint1 = editorInstrument->startPoint +1;
			}
			/////////////////////////////////////////////////////////////////////

			// wykrywanie czy wczytywany inny niz poprzednio/nowy sampel
			//--------------------------------------------------------------------

			points.selected = (selectedPlace >= 1 && selectedPlace <= 4) ? selectedPlace : 0;
		}


		// wykrywanie czy wczytywany inny niz poprzednio/nowy sampel
		// patrzy na dlugosc sampla
		if(editorInstrument->sample.length != lastSampleLength)
		{
			lastSampleLength = editorInstrument->sample.length;

			uint16_t start_end_width = editorInstrument->endPoint - editorInstrument->startPoint;

			if(start_end_width < MAX_16BIT/4) // wysrodkowanie startPoint z zoomem jesli waski loop
			{
				start_end_width += start_end_width/20; // dodatkowe po 10% po bokach

				GP.spectrumAutoZoomIn(editorInstrument->startPoint, start_end_width, editorInstrument->sample.length, &zoom);
			}
			else
			{
				GP.spectrumResetZoom(editorInstrument->startPoint, editorInstrument->sample.length ,&zoom);
			}
		}


		//--------------------------------------------------------------------

		GP.processSpectrum(editorInstrument, &zoom, &spectrum);
		processPoints();

	}

	if(loadedInstrumentType == mtSampleTypeWaveFile)
	{
		selectCorrectPoints();
		clearAllNodes();
		cancelMultiFrame();
	}

	// ustawienie funkcji
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

	FM->setButtonObj(interfaceButtonInstr, functInstrument);
	FM->setButtonObj(interfaceButtonNote, functStepNote);

	if(mtProject.values.lastUsedInstrument >= INSTRUMENTS_COUNT)
	{
		display.hideAllControls();
		showTitleBar();
		display.synchronizeRefresh();
		return;
	}

	if(loadedInstrumentType == mtSampleTypeWaveFile)
	{
		listPlayMode();
		showPlayModeList();
		showZoomValue();
	}

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

	FM->setButtonObj(interfaceButton0, functPreview);
	FM->setButtonObj(interfaceButton1, functSelectStart);
	FM->setButtonObj(interfaceButton2, functSelectLoop1);
	FM->setButtonObj(interfaceButton3, functSelectLoop2);
	FM->setButtonObj(interfaceButton4, functSelectEnd);

	FM->setButtonObj(interfaceButton5, buttonPress, functSelectZoom);

	FM->setButtonObj(interfaceButton6, buttonPress, functPlayMode);
	FM->setButtonObj(interfaceButton7, buttonPress, functPlayMode);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setPadsGlobal(functPads);
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

void cSamplePlayback::selectCorrectPoints()
{
	if(selectedPlace == 1)
	{
		points.selected = selectStart;
	}
	else if(selectedPlace == 2)
	{
		points.selected = selectLoop1;
	}
	else if(selectedPlace == 3)
	{
		points.selected = selectLoop2;
	}
	else if(selectedPlace == 4)
	{
		points.selected = selectEnd;
	}
}

void cSamplePlayback::listPlayMode()
{
	for(uint8_t i = 0; i < playModeCount; i++)
	{
		playModeNames[i] = (char*)&playModeFunctLabels[i][0];
	}
}

void cSamplePlayback::cancelPopups()
{
	if(mtPopups.getStepPopupState() != stepPopupNone)
	{
		mtPopups.hideStepPopups();

		start(0);

		//setDefaultScreenFunct();
		//showDefaultScreen();
	}
}


//==============================================================================================================
static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
		sequencer.stop();
	}

	if(state == 1)
	{
		//uint8_t note = mtPadBoard.convertPadToNote(pad);
		//if(note > 48) note = 48;
		//editorInstrument->tune = note;

		if(mtPadBoard.getEmptyVoice() < 0) return 1;

		if(SP->loadedInstrumentType == mtSampleTypeWaveFile)
		{
			if(mtPadBoard.getEmptyVoice() == 0)
			{
				SP->playPitch = notes[mtPadBoard.convertPadToNote(pad)];
				SP->playProgresValueTim = ((( (SP->editorInstrument->sample.length/44100.0 ) * SP->editorInstrument->startPoint) / MAX_16BIT) * 1000000) /SP->playPitch;
				SP->refreshPlayProgressValue = 0;
				SP->loopDirection = 0;
				SP->isPlayingSample = 1;
			}
		}

		padsBacklight.setFrontLayer(1,20, pad);
		mtPadBoard.startInstrument(pad, mtProject.values.lastUsedInstrument,-1);
	}
	else if(state == 0)
	{
		padsBacklight.setFrontLayer(0,0, pad);
		mtPadBoard.stopInstrument(pad);
		if(SP->loadedInstrumentType == mtSampleTypeWaveFile)
		{
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
	}

	return 1;
}

static  uint8_t functSelectStart(uint8_t state)
{
	if((state > buttonPress) && (state != UINT8_MAX)) return 1;

	if(SP->loadedInstrumentType == mtSampleTypeWaveFile)
	{
		if(state == UINT8_MAX || state == buttonPress) // called from inside of this module
		{
			if(SP->zoom.zoomValue > 1.0 && SP->zoom.lastChangedPoint != 1)
			{
				SP->zoom.lastChangedPoint = 1;
				SP->zoom.zoomPosition = SP->editorInstrument->startPoint;
				SP->refreshSpectrum = 1;
			}

			if(state == buttonPress)
			{
				SP->addNode(modStartPoint, 0);

				SP->frameData.multisel[1].frameNum = 1;
				SP->frameData.multisel[1].isActive = 1;
				SP->frameData.multiSelActiveNum  += 1;
			}

			if(SP->frameData.multiSelActiveNum < 2)
			{
				SP->points.selected = 0;
			}

			SP->points.selected |= selectStart;
			SP->selectedPlace = 1;
		}
		else if(state == buttonRelease)
		{
			if(SP->frameData.multiSelActiveNum)
			{
				SP->points.selected &= ~selectStart;

				if(SP->frameData.multisel[1].isActive)
				{
					SP->removeNode(0);
					SP->frameData.multiSelActiveNum  -= 1;

					SP->frameData.multisel[1].isActive = 0;

					if(SP->frameData.multiSelActiveNum == 0)
					{
						SP->selectedPlace = 1;
						SP->points.selected |= selectStart;
					}
				}
			}
		}

		SP->refreshPoints = 1;
	}
	else if(SP->loadedInstrumentType == mtSampleTypeWavetable)
	{
		SP->selectedPlace = 1;
	}

	SP->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLoop1(uint8_t state)
{
	if((state > buttonPress) && (state != UINT8_MAX)) return 1;
	if(SP->loadedInstrumentType != mtSampleTypeWaveFile) return 1;

	if(state == UINT8_MAX || state == buttonPress)
	{
		if(SP->editorInstrument->playMode == singleShot) return 1;

		if(SP->zoom.zoomValue > 1.0 && SP->zoom.lastChangedPoint != 3)
		{
			SP->zoom.lastChangedPoint = 3;
			SP->zoom.zoomPosition = SP->editorInstrument->loopPoint1;
			SP->refreshSpectrum = 1;
		}

		if(state == buttonPress)
		{
			SP->addNode(modLoopPoint1, 1);

			SP->frameData.multisel[2].frameNum = 2;
			SP->frameData.multisel[2].isActive = 1;
			SP->frameData.multiSelActiveNum  += 1;
		}

		if(SP->frameData.multiSelActiveNum < 2)
		{
			SP->points.selected = 0;
		}

		SP->points.selected |= selectLoop1;
		SP->selectedPlace = 2;
	}
	else if(state == buttonRelease)
	{
		if(SP->frameData.multiSelActiveNum)
		{
			SP->points.selected &= ~selectLoop1;

			if(SP->frameData.multisel[2].isActive)
			{
				SP->removeNode(1);
				SP->frameData.multiSelActiveNum  -= 1;

				SP->frameData.multisel[2].isActive = 0;

				if(SP->frameData.multiSelActiveNum == 0)
				{
					SP->selectedPlace = 2;
					SP->points.selected |= selectLoop1;
				}
			}
		}
	}

	SP->refreshPoints = 1;
	SP->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLoop2(uint8_t state)
{
	if((state > buttonPress) && (state != UINT8_MAX)) return 1;
	if(SP->loadedInstrumentType != mtSampleTypeWaveFile) return 1;

	if(state == UINT8_MAX || state == buttonPress)
	{
		if(SP->editorInstrument->playMode == singleShot) return 1;

		if(SP->zoom.zoomValue > 1.0 && SP->zoom.lastChangedPoint != 4)
		{
			SP->zoom.lastChangedPoint = 4;
			SP->zoom.zoomPosition = SP->editorInstrument->loopPoint2;
			SP->refreshSpectrum = 1;
		}

		if(state == buttonPress)
		{
			SP->addNode(modLoopPoint2, 2);

			SP->frameData.multisel[3].frameNum = 3;
			SP->frameData.multisel[3].isActive = 1;
			SP->frameData.multiSelActiveNum  += 1;
		}

		if(SP->frameData.multiSelActiveNum < 2)
		{
			SP->points.selected = 0;
		}

		SP->points.selected |= selectLoop2;
		SP->selectedPlace = 3;

	}
	else if(state == buttonRelease)
	{
		if(SP->frameData.multiSelActiveNum)
		{
			SP->points.selected &= ~selectLoop2;

			if(SP->frameData.multisel[3].isActive)
			{
				SP->removeNode(2);
				SP->frameData.multiSelActiveNum  -= 1;

				SP->frameData.multisel[3].isActive = 0;

				if(SP->frameData.multiSelActiveNum == 0)
				{
					SP->selectedPlace = 3;
					SP->points.selected |= selectLoop2;
				}
			}
		}
	}

	SP->refreshPoints = 1;

	SP->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectEnd(uint8_t state)
{
	if((state > buttonPress) && (state != UINT8_MAX)) return 1;
	if(SP->loadedInstrumentType != mtSampleTypeWaveFile) return 1;

	if(state == UINT8_MAX || state == buttonPress)
	{
		if(SP->zoom.zoomValue > 1.0 && SP->zoom.lastChangedPoint != 2)
		{
			SP->zoom.lastChangedPoint = 2;
			SP->zoom.zoomPosition = SP->editorInstrument->endPoint;
			SP->refreshSpectrum = 1;
		}

		if(state == buttonPress)
		{
			SP->addNode(modEndPoint, 3);

			SP->frameData.multisel[4].frameNum = 4;
			SP->frameData.multisel[4].isActive = 1;
			SP->frameData.multiSelActiveNum  += 1;
		}

		if(SP->frameData.multiSelActiveNum < 2)
		{
			SP->points.selected = 0;
		}

		SP->points.selected |= selectEnd;
		SP->selectedPlace = 4;
	}
	else if(state == buttonRelease)
	{
		if(SP->frameData.multiSelActiveNum)
		{
			SP->points.selected &= ~selectEnd;

			if(SP->frameData.multisel[4].isActive)
			{
				SP->removeNode(3);
				SP->frameData.multiSelActiveNum  -= 1;

				SP->frameData.multisel[4].isActive = 0;

				if(SP->frameData.multiSelActiveNum == 0)
				{
					SP->selectedPlace = 4;
					SP->points.selected |= selectEnd;
				}
			}
		}
	}

	SP->refreshPoints = 1;

	SP->activateLabelsBorder();

	return 1;
}


static  uint8_t functSelectZoom()
{
	if(SP->loadedInstrumentType != mtSampleTypeWaveFile) return 1;

	SP->cancelMultiFrame();
	SP->clearAllNodes();

	SP->refreshPoints = 1;
	SP->points.selected = 0;

	SP->selectedPlace = 5;
	SP->activateLabelsBorder();

	return 1;
}


static  uint8_t functPlayMode(uint8_t button)
{
	if(SP->loadedInstrumentType != mtSampleTypeWaveFile) return 1;
//	if(button == interfaceButton6)
//	{
//		SP->changePlayModeSelection(-1);
//	}
//	else //if(button == interfaceButton7)
//	{
//		SP->changePlayModeSelection(1);
//	}

	SP->cancelMultiFrame();
	SP->clearAllNodes();

	SP->selectedPlace = 6;
	SP->activateLabelsBorder();

	SP->points.selected = 0;
	SP->refreshPoints = 1;

	return 1;
}

static  uint8_t functEncoder(int16_t value)
{
	if(SP->frameData.multiSelActiveNum != 0)
	{
		SP->stepThroughNodes(value);
	}
	else
	{
		switch(SP->selectedPlace)
		{
		case 0: break;
		case 1:
			if(SP->loadedInstrumentType == mtSampleTypeWavetable)
			{
				modWavetablePostion(value);
			}
			else if(SP->loadedInstrumentType == mtSampleTypeWaveFile)
			{
				modStartPoint(value);
			}
			break;
		case 2: modLoopPoint1(value); 			break;
		case 3: modLoopPoint2(value); 			break;
		case 4: modEndPoint(value); 			break;
		case 5: changeZoom(value);				break;
		case 6: changePlayModeSelection(value);	break;
		}
	}

	return 1;
}

static  uint8_t functLeft()
{
	if(SP->frameData.multiSelActiveNum != 0) return 1;
	if(SP->loadedInstrumentType != mtSampleTypeWaveFile) return 1;

	if(SP->selectedPlace > 1) SP->selectedPlace--;

	switch(SP->selectedPlace)
	{
		case 0: break;
		case 1: functSelectStart(UINT8_MAX);		break;
		case 2:
			if(SP->editorInstrument->playMode == singleShot)
			{
				SP->selectedPlace = 1;
				functSelectStart(UINT8_MAX);
			}
			else
			{
				functSelectLoop1(UINT8_MAX);
			}
			break;
		case 3:
			if(SP->editorInstrument->playMode == singleShot)
			{
				SP->selectedPlace = 1;
				functSelectStart(UINT8_MAX);
			}
			else
			{
				functSelectLoop2(UINT8_MAX);
			}
			break;
		case 4: functSelectEnd(UINT8_MAX);		break;
		case 5: functSelectZoom();		break;
		case 6:
		{
		SP->selectedPlace = 6;
		SP->points.selected = 0;
		break;
		}
	}

	SP->activateLabelsBorder();

	return 1;
}


static  uint8_t functRight()
{
	if(SP->frameData.multiSelActiveNum != 0) return 1;
	if(SP->loadedInstrumentType != mtSampleTypeWaveFile) return 1;

	if(SP->selectedPlace < SP->frameData.placesCount-1) SP->selectedPlace++;

	switch(SP->selectedPlace)
	{
		case 1: functSelectStart(UINT8_MAX);		break;
		case 2:
			if(SP->editorInstrument->playMode == singleShot)
			{
				SP->selectedPlace = 4;
				functSelectEnd(UINT8_MAX);
			}
			else
			{
				functSelectLoop1(UINT8_MAX);
			}
			break;
		case 3:
			if(SP->editorInstrument->playMode == singleShot)
			{
				SP->selectedPlace = 4;
				functSelectEnd(UINT8_MAX);
			}
			else
			{
				functSelectLoop2(UINT8_MAX);
			}
			break;
		case 4: functSelectEnd(UINT8_MAX);		break;
		case 5: functSelectZoom();		break;
		case 6:
		{
		SP->selectedPlace = 6;
		SP->points.selected = 0;
		break;
		}
	}

	SP->activateLabelsBorder();

	return 1;
}


static  uint8_t functUp()
{
	if(SP->frameData.multiSelActiveNum != 0)
	{
		SP->stepThroughNodes(1);
	}
	else
	{
		switch(SP->selectedPlace)
		{
		case 1:
			if(SP->loadedInstrumentType == mtSampleTypeWavetable)
			{
				modWavetablePostion(1);
			}
			else if(SP->loadedInstrumentType == mtSampleTypeWaveFile)
			{
				modStartPoint(1);
			}
			break;
		case 2: modLoopPoint1(1); 			break;
		case 3: modLoopPoint2(1); 			break;
		case 4: modEndPoint(1); 			break;
		case 5: changeZoom(1);				break;
		case 6: changePlayModeSelection(-1);	break;
		}
	}

	return 1;
}


static  uint8_t functDown()
{
	if(SP->frameData.multiSelActiveNum != 0)
	{
		SP->stepThroughNodes(-1);
	}
	else
	{
		switch(SP->selectedPlace)
		{
		case 1:
			if(SP->loadedInstrumentType == mtSampleTypeWavetable)
			{
				modWavetablePostion(-1);
			}
			else if(SP->loadedInstrumentType == mtSampleTypeWaveFile)
			{
				modStartPoint(1);
			}
			break;
		case 2: modLoopPoint1(-1); 			break;
		case 3: modLoopPoint2(-1); 			break;
		case 4: modEndPoint(-1); 			break;
		case 5: changeZoom(-1);				break;
		case 6: changePlayModeSelection(1);	break;
		}
	}
	return 1;
}

static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		sequencer.play();
	}
	else
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

static 	uint8_t functPreview(uint8_t state)
{
	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
		sequencer.stop();
	}

	if(state == 1)
	{
		if(mtPadBoard.getEmptyVoice() < 0) return 1;

		if(SP->loadedInstrumentType == mtSampleTypeWaveFile)
		{
			if(mtPadBoard.getEmptyVoice() == 0)
			{
				SP->playPitch = notes[mtPadBoard.convertPadToNote(12)];
				SP->playProgresValueTim = ((( (SP->editorInstrument->sample.length/44100.0 ) * SP->editorInstrument->startPoint) / MAX_16BIT) * 1000000) /SP->playPitch;
				SP->refreshPlayProgressValue = 0;
				SP->loopDirection = 0;
				SP->isPlayingSample = 1;
			}
		}

		mtPadBoard.startInstrument(12, mtProject.values.lastUsedInstrument,-1);
	}
	else if(state == 0)
	{
		mtPadBoard.stopInstrument(12);

		if(SP->loadedInstrumentType == mtSampleTypeWaveFile)
		{
			if((!SP->editorInstrument->envelope[envAmp].enable) || (SP->editorInstrument->envelope[envAmp].release == 0))
			{
				if(mtPadBoard.getVoiceTakenByPad(12) == 0)
				{
					SP->playProgressValue=0;
					SP->playProgressInSpectrum = 0;
					SP->isPlayingSample = 0;
					SP->refreshSpectrumProgress = 1;
					SP->hidePreviewValue();
				}
			}
		}
	}

	return 1;
}


//======================================================================================================================
static void changeZoom(int16_t value)
{
	GP.spectrumChangeZoom(value, SP->editorInstrument->sample.length, &SP->zoom);

	SP->refreshSpectrum = 1;
	SP->refreshPoints = 1;

	SP->showZoomValue();
}

static void changePlayModeSelection(int16_t value)
{
	if(SP->editorInstrument->playMode + value < 0) SP->editorInstrument->playMode = 0;
	else if(SP->editorInstrument->playMode + value > playModeCount-1) SP->editorInstrument->playMode = playModeCount-1;
	else  SP->editorInstrument->playMode += value;

	if((SP->editorInstrument->playMode == singleShot) && (value < 0 )) SP->hideLoopPoints();
	else if(value > 0 )
	{
		if(SP->editorInstrument->loopPoint1 >= SP->editorInstrument->loopPoint2) SP->editorInstrument->loopPoint1 = SP->editorInstrument->loopPoint2 - 1;

		if(((SP->editorInstrument->loopPoint1 >= SP->editorInstrument->endPoint) && (SP->editorInstrument->loopPoint2 >= SP->editorInstrument->endPoint))
		|| ((SP->editorInstrument->loopPoint1 <= SP->editorInstrument->startPoint) && (SP->editorInstrument->loopPoint2 <= SP->editorInstrument->startPoint))
		|| ((SP->editorInstrument->loopPoint1 <= SP->editorInstrument->startPoint) && (SP->editorInstrument->loopPoint2 >= SP->editorInstrument->endPoint)))
		{
			SP->editorInstrument->loopPoint1 = SP->editorInstrument->startPoint+1;
			SP->editorInstrument->loopPoint2 = SP->editorInstrument->endPoint-1;
		}
		else if((SP->editorInstrument->loopPoint1 >= SP->editorInstrument->startPoint) && (SP->editorInstrument->loopPoint1 <= SP->editorInstrument->endPoint) &&
		(SP->editorInstrument->loopPoint2 >= SP->editorInstrument->endPoint))
		{
			SP->editorInstrument->loopPoint2 = SP->editorInstrument->endPoint - 1;
		}
		else if((SP->editorInstrument->loopPoint2 >= SP->editorInstrument->startPoint) && (SP->editorInstrument->loopPoint2 <= SP->editorInstrument->endPoint) &&
		(SP->editorInstrument->loopPoint1 <= SP->editorInstrument->startPoint))
		{
			SP->editorInstrument->loopPoint1 = SP->editorInstrument->startPoint +1;
		}

		SP->showLoopPoints();
	}


	SP->refreshPoints = 1;

	display.setControlValue(SP->playModeListControl, SP->editorInstrument->playMode);
	display.refreshControl(SP->playModeListControl);

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;

}

static void modStartPoint(int16_t value)
{
	// obliczenie kroku przesuniecia w zaleznosci od ilosci widzianych probek na wyswietlaczu
	uint16_t move_step = SP->zoom.zoomWidth / 600;
	uint16_t dif;
	value = value * move_step;

	if(SP->editorInstrument->startPoint + value < SAMPLE_POINT_POS_MIN)SP-> editorInstrument->startPoint  = 0;
	else if(SP->editorInstrument->startPoint + value > SAMPLE_POINT_POS_MAX ) SP->editorInstrument->startPoint  = SAMPLE_POINT_POS_MAX;
	else SP->editorInstrument->startPoint += value;

	if(SP->editorInstrument->startPoint >= SP->editorInstrument->endPoint)
	{
		SP->editorInstrument->startPoint = SP->editorInstrument->endPoint - 2;
	}

	if(SP->editorInstrument->playMode != singleShot)
	{
		if(SP->editorInstrument->startPoint >= SP->editorInstrument->loopPoint1)
		{
			dif = SP->editorInstrument->loopPoint2 - SP->editorInstrument->loopPoint1;
			SP->editorInstrument->loopPoint1 = SP->editorInstrument->startPoint + 1;

			if(SP->editorInstrument->loopPoint1 + dif > SP->editorInstrument->endPoint)
			{
				SP->editorInstrument->loopPoint2 = SP->editorInstrument->endPoint - 1;
				SP->editorInstrument->loopPoint1 = SP->editorInstrument->loopPoint2 - dif;
				SP->editorInstrument->startPoint = SP->editorInstrument->loopPoint1 - 1;
				instrumentPlayer[0].setStatusBytes(LP1_MASK);
				instrumentPlayer[0].setStatusBytes(LP2_MASK);
			}
			else
			{
				SP->editorInstrument->loopPoint2 = SP->editorInstrument->loopPoint1 + dif;
				instrumentPlayer[0].setStatusBytes(LP2_MASK);
			}

			SP->showLoopPoint1Value();
			SP->showLoopPoint2Value();
		}
	}

	// odswiez spektrum tylko jesli: zoom wiekszy niz 1, ostatnio modyfikowany inny punkt, punkt jest poza widocznym obszarem
	if(SP->zoom.zoomValue > 1 && (SP->zoom.lastChangedPoint != 1
	   || (SP->editorInstrument->startPoint < SP->zoom.zoomStart || SP->editorInstrument->startPoint > SP->zoom.zoomEnd)))
	{
		SP->refreshSpectrum = 1;
	}

	SP->zoom.zoomPosition = SP->editorInstrument->startPoint;
	SP->zoom.lastChangedPoint = 1;
	SP->refreshPoints = 1;

	SP->showStartPointValue();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

static void modEndPoint(int16_t value)
{
	uint16_t move_step = SP->zoom.zoomWidth / 600;
	uint16_t dif;
	value = value * move_step;

	if(SP->editorInstrument->endPoint + value < SAMPLE_POINT_POS_MIN) SP->editorInstrument->endPoint  = 0;
	else if(SP->editorInstrument->endPoint + value > SAMPLE_POINT_POS_MAX ) SP->editorInstrument->endPoint  = SAMPLE_POINT_POS_MAX;
	else SP->editorInstrument->endPoint += value;


	if(SP->editorInstrument->endPoint <= SP->editorInstrument->startPoint)
	{
		SP->editorInstrument->endPoint = SP->editorInstrument->startPoint + 2;
	}

	if(SP->editorInstrument->playMode != singleShot)
	{
		if(SP->editorInstrument->endPoint < SP->editorInstrument->loopPoint2)
		{
			dif = SP->editorInstrument->loopPoint2 - SP->editorInstrument->loopPoint1;

			SP->editorInstrument->loopPoint2 = SP->editorInstrument->endPoint - 1;

			if(SP->editorInstrument->loopPoint2 - dif < SP->editorInstrument->startPoint)
			{
				SP->editorInstrument->loopPoint1 = SP->editorInstrument->startPoint + 1;

				SP->editorInstrument->loopPoint2 = SP->editorInstrument->loopPoint1 + dif;
				SP->editorInstrument->endPoint = SP->editorInstrument->loopPoint2 + 1;

				instrumentPlayer[0].setStatusBytes(LP1_MASK);
				instrumentPlayer[0].setStatusBytes(LP2_MASK);
			}
			else
			{
				SP->editorInstrument->loopPoint1 = SP->editorInstrument->loopPoint2 - dif;
				instrumentPlayer[0].setStatusBytes(LP1_MASK);

			}

			SP->showLoopPoint1Value();
			SP->showLoopPoint2Value();
		}
	}

	if(SP->zoom.zoomValue > 1 && (SP->zoom.lastChangedPoint != 2
			|| (SP->editorInstrument->endPoint < SP->zoom.zoomStart || SP->editorInstrument->endPoint > SP->zoom.zoomEnd))) SP->refreshSpectrum = 1;

	SP->zoom.zoomPosition = SP->editorInstrument->endPoint;
	SP->zoom.lastChangedPoint = 2;
	SP->refreshPoints = 1;

	SP->showEndPointValue();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

static void modLoopPoint1(int16_t value)
{
	uint16_t move_step = SP->zoom.zoomWidth / 600;
	value = value * move_step;

	if(SP->editorInstrument->loopPoint1 + value < SAMPLE_POINT_POS_MIN) SP->editorInstrument->loopPoint1  = 0;
	else if(SP->editorInstrument->loopPoint1 + value > SAMPLE_POINT_POS_MAX ) SP->editorInstrument->loopPoint1  = SAMPLE_POINT_POS_MAX;
	else SP->editorInstrument->loopPoint1 += value;

	if(SP->editorInstrument->loopPoint1 <= SP->editorInstrument->startPoint) SP->editorInstrument->loopPoint1 = SP->editorInstrument->startPoint + 1;
	if(SP->editorInstrument->loopPoint1 >= SP->editorInstrument->loopPoint2) SP->editorInstrument->loopPoint1 = SP->editorInstrument->loopPoint2 - 1;

	if(SP->zoom.zoomValue > 1 && (SP-> zoom.lastChangedPoint != 3
			|| (SP->editorInstrument->loopPoint1 < SP->zoom.zoomStart || SP->editorInstrument->loopPoint1 > SP->zoom.zoomEnd))) SP->refreshSpectrum = 1;

	instrumentPlayer[0].setStatusBytes(LP1_MASK);

	SP->zoom.zoomPosition = SP->editorInstrument->loopPoint1;
	SP->zoom.lastChangedPoint = 3;
	SP->refreshPoints = 1;

	SP->showLoopPoint1Value();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

static void modLoopPoint2(int16_t value)
{
	uint16_t move_step = SP->zoom.zoomWidth / 600;
	value = value * move_step;

	if(SP->editorInstrument->loopPoint2 + value < SAMPLE_POINT_POS_MIN) SP->editorInstrument->loopPoint2  = 0;
	else if(SP->editorInstrument->loopPoint2 + value > SAMPLE_POINT_POS_MAX ) SP->editorInstrument->loopPoint2  = SAMPLE_POINT_POS_MAX;
	else SP->editorInstrument->loopPoint2 += value;

	if(SP->editorInstrument->loopPoint2 >= SP->editorInstrument->endPoint) SP->editorInstrument->loopPoint2 = SP->editorInstrument->endPoint - 1;
	if(SP->editorInstrument->loopPoint2 <= SP->editorInstrument->loopPoint1) SP->editorInstrument->loopPoint2 = SP->editorInstrument->loopPoint1 + 1;

	if(SP->zoom.zoomValue > 1 && ( SP->zoom.lastChangedPoint != 4
			|| (SP->editorInstrument->loopPoint2 < SP->zoom.zoomStart || SP->editorInstrument->loopPoint2 > SP->zoom.zoomEnd))) SP->refreshSpectrum = 1;

	instrumentPlayer[0].setStatusBytes(LP2_MASK);

	SP->zoom.zoomPosition = SP->editorInstrument->loopPoint2;
	SP->zoom.lastChangedPoint = 4;
	SP->refreshPoints = 1;
	SP->showLoopPoint2Value();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

static void modWavetablePostion(int16_t value)
{
	if(SP->editorInstrument->wavetableCurrentWindow + value < 0) SP->editorInstrument->wavetableCurrentWindow  = 0;
	else if(SP->editorInstrument->wavetableCurrentWindow + value > MAX_WAVETABLE_WINDOW) SP->editorInstrument->wavetableCurrentWindow = MAX_WAVETABLE_WINDOW;
	else SP->editorInstrument->wavetableCurrentWindow += value;

	instrumentPlayer[0].setStatusBytes(WT_POS_SEND_MASK);

	SP->showWavetablePosition();
	SP->refreshSpectrum = 1;

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
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


void cSamplePlayback::calcPlayProgressValue()
{
//	float  recTimeValue = editorInstrument->sample.length/44100.0;
//	uint32_t localRecTimeValue = recTimeValue * 1000000;
//	uint32_t localEndTimeValue = ((( recTimeValue  * editorInstrument->endPoint) / MAX_16BIT) * 1000000)/playPitch;
//	uint32_t localLoopPoint1Value = 0;
//	uint32_t localLoopPoint2Value = 0;
//	if(editorInstrument->playMode != singleShot)
//	{
//		localLoopPoint1Value = ((( recTimeValue  * editorInstrument->loopPoint1) / MAX_16BIT) * 1000000)/playPitch;
//		localLoopPoint2Value = ((( recTimeValue  * editorInstrument->loopPoint2) / MAX_16BIT) * 1000000)/playPitch;
//	}
//
//	if(editorInstrument->playMode == loopForward)
//	{
//		if(playProgresValueTim >= localLoopPoint2Value ) playProgresValueTim = localLoopPoint1Value;
//	}
//	else if(editorInstrument->playMode == loopBackward)
//	{
//		if((playProgresValueTim >= localLoopPoint2Value ) && (loopDirection == 0 ))
//		{
//			loopDirection = 1;
//			playProgresValueBackwardTim = 0;
//		}
//		else if((int32_t)(localLoopPoint2Value - playProgresValueBackwardTim) <= (int32_t)localLoopPoint1Value )
//		{
//			playProgresValueBackwardTim = 0;
//		}
//	}
//	else if(editorInstrument->playMode == loopPingPong)
//	{
//		if((playProgresValueTim >= localLoopPoint2Value ) && (!loopDirection))
//		{
//			loopDirection = 1;
//			playProgresValueBackwardTim = 0;
//		}
//		else if(( (int32_t)(localLoopPoint2Value - playProgresValueBackwardTim) <= (int32_t)localLoopPoint1Value ) && (loopDirection))
//		{
//			loopDirection = 0;
//			playProgresValueTim = localLoopPoint1Value;
//		}
//	}
//
//	if((playProgresValueTim >= localEndTimeValue) && (!loopDirection))
//	{
//		playProgressValue=0;
//		playProgressInSpectrum = 0;
//		isPlayingSample = 0;
//		refreshSpectrumProgress = 1;
//		return;
//	}
	if( refreshPlayProgressValue > PLAY_REFRESH_US)
	{
		refreshPlayProgressValue = 0;

//		if(!loopDirection)
//		{
//			playProgressValue = playPitch * MAX_16BIT*(playProgresValueTim/(float)localRecTimeValue);
//		}
//		else
//		{
//			playProgressValue = playPitch * MAX_16BIT*((int32_t)(localLoopPoint2Value - playProgresValueBackwardTim)/(float)localRecTimeValue);
//		}
		playProgressValue = instrumentPlayer[0].getWavePosition();

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


static  uint8_t functInstrument(uint8_t state)
{
	if(state == buttonRelease)
	{
		SP->cancelPopups();
	}
	else if(state == buttonPress)
	{
		mtPopups.showStepPopup(stepPopupInstr, mtProject.values.lastUsedInstrument);
		//SE->lightUpPadBoard();
	}

	return 1;
}

static uint8_t functStepNote(uint8_t value)
{
	if(value == buttonRelease)
	{
		SP->cancelPopups();

	}
	else if(value == buttonPress)
	{
		mtPopups.showStepPopup(stepPopupNote, -1);
		//SE->lightUpPadBoard();
	}

	return 1;
}

/*/////////// MultiSelect Functions ////////////////*/
void cSamplePlayback::addNode(editFunct_t funct , uint8_t nodeNum)
{
	if(selectNodes[nodeNum].isActive == 0)
	{
		selectNodes[nodeNum].isActive = 1;
		selectNodes[nodeNum].editFunct = funct;
	}
}

void cSamplePlayback::removeNode(uint8_t nodeNum)
{
	selectNodes[nodeNum].isActive = 0;
	selectNodes[nodeNum].editFunct = NULL;
}

void cSamplePlayback::stepThroughNodes(int16_t value)
{
	//kolejnosc wykonywania funkcji w loop pointach ma znaczenie
	//
	if(value < 0)
	{
		for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
		{
			if(selectNodes[node].isActive)
			{
				if(selectNodes[node].editFunct != NULL)
				{
					selectNodes[node].editFunct(value);
				}
			}
		}
	}
	else if(value > 0)
	{
		for(uint8_t node = MAX_SELECT_NODES; node > 0; node--)
		{
			if(selectNodes[node-1].isActive)
			{
				if(selectNodes[node-1].editFunct != NULL)
				{
					selectNodes[node-1].editFunct(value);
				}
			}
		}
	}
}

void cSamplePlayback::clearAllNodes()
{
	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		selectNodes[node].isActive = 0;
		selectNodes[node].editFunct = NULL;
	}
}

void cSamplePlayback::cancelMultiFrame()
{
	for(uint8_t i = 0; i < MAX_SELECT_NODES; i++)
	{
		SP->frameData.multisel[i].isActive = 0;
	}

	SP->frameData.multiSelActiveNum = 0;
}
///////////////////////////////////////////////////////////////////////////
