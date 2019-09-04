

#include "sampleEditor.h"
#include "mtPadBoard.h"
#include "mtAudioEngine.h"






cSampleEditor sampleEditor;
static cSampleEditor* SE = &sampleEditor;

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



static uint8_t functShift(uint8_t value);



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);
static uint8_t functStepNote(uint8_t value);






void cSampleEditor::update()
{
	if(refreshSpectrum)
	{
		processSpectrum();

		display.refreshControl(SE->spectrumControl);

		refreshSpectrum = 0;
	}

	if(refreshPoints)
	{
		processPoints();


		display.refreshControl(SE->pointsControl);

		refreshPoints = 0;
	}



}

void cSampleEditor::start(uint32_t options)
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

//--------------------------------------------------------------------

	processSpectrum();
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


void cSampleEditor::stop()
{

	moduleRefresh = 0;
}


void cSampleEditor::setDefaultScreenFunct()
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

	FM->setButtonObj(interfaceButtonInstr, functInstrument);

	FM->setButtonObj(interfaceButton0, buttonPress, functSelectStart);
	FM->setButtonObj(interfaceButton1, buttonPress, functSelectLoop1);
	FM->setButtonObj(interfaceButton2, buttonPress, functSelectLoop2);
	FM->setButtonObj(interfaceButton3, buttonPress, functSelectEnd);

	FM->setButtonObj(interfaceButton5, buttonPress, functSelectZoom);

	FM->setButtonObj(interfaceButton6, buttonPress, functPlayMode);
	FM->setButtonObj(interfaceButton7, buttonPress, functPlayMode);
	FM->setButtonObj(interfaceButtonNote, functStepNote);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);


}

//==============================================================================================================


void cSampleEditor::processPoints()
{

	points.pointsType = editorInstrument->playMode;

	if(editorInstrument->startPoint >= zoomStart && editorInstrument->startPoint <= zoomEnd)
	{
		points.startPoint = ((editorInstrument->startPoint-zoomStart) * 599) / zoomWidth;
	}
	else points.startPoint = -1;

	if(editorInstrument->endPoint >= zoomStart && editorInstrument->endPoint <= zoomEnd)
	{
		points.endPoint = ((editorInstrument->endPoint-zoomStart) * 599) / zoomWidth;
	}
	else points.endPoint = -1;

	if(editorInstrument->loopPoint1 >= zoomStart && editorInstrument->loopPoint1 <= zoomEnd)
	{
		points.loopPoint1 = ((editorInstrument->loopPoint1-zoomStart) * 599) / zoomWidth;
	}
	else points.loopPoint1 = -1;

	if(editorInstrument->loopPoint2 >= zoomStart && editorInstrument->loopPoint2 <= zoomEnd)
	{
		points.loopPoint2 = ((editorInstrument->loopPoint2-zoomStart) * 599) / zoomWidth;
	}
	else points.loopPoint2 = -1;

}


void cSampleEditor::processSpectrum()
{

	//refreshSpectrum = 1;


	// uwaga tu wazna kolejnosc + do sprawdzenia
	if(mtProject.values.lastUsedInstrument < 0 || mtProject.instrument[mtProject.values.lastUsedInstrument].isActive == 0 )
	{
		for(uint16_t i = 0; i < 600; i++)
		{
			spectrum.upperData[i] = 0;
			spectrum.lowerData[i] = 0;
		}

		return;
	}

	uint16_t offset_pixel;
	int16_t * sampleData;

	if(editorInstrument->sample.type == mtSampleTypeWavetable)
	{
		zoomWidth = MAX_16BIT;
		zoomStart = 0;
		zoomValue = 1;
		zoomEnd = MAX_16BIT;
		uint16_t windowSize = editorInstrument->sample.wavetable_window_size;

		sampleData = editorInstrument->sample.address
				+ (mtProject.instrument[mtProject.values.lastUsedInstrument].wavetableCurrentWindow * windowSize);

		float resolution = windowSize / 600.0;

		int16_t up = 0;
		int16_t low = 0;
		float step = 0;

		for(uint16_t i = 0; i < 600; i++)
		{
			low = up = 0; //*(sampleData+step);

			for(uint16_t j = 0; j < resolution; j++)
			{
				int16_t sample = *(sampleData+(uint32_t)step+j);

				if(sample > up)  up = sample;
				else if(sample < low) low = sample;

			}
			step+= resolution;

			up = up/300;
			low = low/300;

			spectrum.upperData[i] =  up;
			spectrum.lowerData[i] = low;
		}

		//if(resolution <= 1)
		spectrum.spectrumType = 1;
		//else spectrum.spectrumType = 0;


		return;
	}

	uint32_t resolution;


	switch(lastChangedPoint)
	{
		case 0: zoomPosition = editorInstrument->startPoint; break; //MAX_16BIT/2; break;

		case 1:
			zoomPosition = editorInstrument->startPoint;
		break;
		case 2:
			zoomPosition = editorInstrument->endPoint;
		break;
		case 3:
			zoomPosition = editorInstrument->loopPoint1;
		break;
		case 4:
			zoomPosition = editorInstrument->loopPoint2;
		break;

		default: zoomPosition = editorInstrument->startPoint; break; //MAX_16BIT/2; break;
	}







	if(zoomValue > 1.0)
	{
		zoomWidth = (MAX_16BIT/zoomValue);
		zoomStart =  zoomPosition - zoomWidth/2;
		zoomEnd = zoomPosition + zoomWidth/2;

		if(zoomStart < 0)
		{
			zoomEnd = zoomWidth;
			zoomStart = 0;
			offset_pixel = ((zoomPosition-zoomStart) * 599) / zoomWidth;
		}
		else if(zoomEnd > MAX_16BIT)
		{
			zoomEnd = MAX_16BIT;
			zoomStart = MAX_16BIT-zoomWidth;
			offset_pixel = ((zoomPosition-zoomStart) * 599) / zoomWidth;
		}
		else
		{
			offset_pixel = ((zoomPosition-zoomStart) * 599) / zoomWidth;
		}


		uint32_t offset = ((float)zoomPosition/MAX_16BIT) * editorInstrument->sample.length;

		sampleData = editorInstrument->sample.address + offset;

		resolution = (((float)zoomWidth/MAX_16BIT) * editorInstrument->sample.length ) / 600;


//		Serial.print(zoomValue);
//		Serial.print("   ");
//		Serial.print(zoomStart);
//		Serial.print("   ");
//		Serial.print(zoomEnd);
//		Serial.print("   ");
//
//		Serial.println();

	}
	else
	{

		offset_pixel = 0;
		zoomWidth = MAX_16BIT;
		zoomStart = 0;
		zoomEnd = MAX_16BIT;
		sampleData = editorInstrument->sample.address;
		resolution = editorInstrument->sample.length / 600;
	}


	if(resolution < 1) resolution = 1;


	int16_t up = 0;
	int16_t low = 0;

	uint32_t step = 0;



	if(offset_pixel > 0)
	{
		for(int16_t i = offset_pixel-1; i >= 0; i--)
		{
			low = up = 0; //*(sampleData+step);

			for(uint16_t j = 0; j < resolution; j++)
			{
				int16_t sample = *(sampleData-step+j);

				if(sample > up)  up = sample;
				else if(sample < low) low = sample;

			}
			step+= resolution;

			up = up/300;
			low = low/300;

			spectrum.upperData[i] =  up;
			spectrum.lowerData[i] = low;
		}
	}

	up = 0;
	low = 0;
	step = 0;


	for(uint16_t i = offset_pixel; i < 600; i++)
	{
		low = up = 0; //*(sampleData+step);

		for(uint16_t j = 0; j < resolution; j++)
		{
			int16_t sample = *(sampleData+step+j);


			if(sample > up)  up = sample;
			else if(sample < low) low = sample;

		}
		step+= resolution;

		up = up/300;
		low = low/300;

		spectrum.upperData[i] =  up;
		spectrum.lowerData[i] = low;
	}

	if(resolution <= 1) spectrum.spectrumType = 1;
	else spectrum.spectrumType = 0;

}

void cSampleEditor::listPlayMode()
{

	for(uint8_t i = 0; i < effectsCount; i++)
	{
		playModeNames[i] = (char*)&effectNamesLabels[i][0];
	}


}



//==============================================================================================================


static  uint8_t functSelectStart()
{
	SE->points.selected = 1;
	SE->selectedPlace = 0;
	SE->activateLabelsBorder();

	if(SE->zoomValue > 1.0)
	{
		SE->refreshSpectrum = 1;
	}

	SE->refreshPoints = 1;

	return 1;
}

static  uint8_t functSelectLoop1()
{
	SE->points.selected = 3;
	SE->selectedPlace = 1;
	SE->activateLabelsBorder();

	if(SE->zoomValue > 1.0)
	{
		SE->refreshSpectrum = 1;
	}
	SE->refreshPoints = 1;

	return 1;
}

static  uint8_t functSelectLoop2()
{
	SE->points.selected = 4;
	SE->selectedPlace = 2;
	SE->activateLabelsBorder();

	if(SE->zoomValue > 1.0)
	{
		SE->refreshSpectrum = 1;
	}
	SE->refreshPoints = 1;


	return 1;
}

static  uint8_t functSelectEnd()
{
	SE->points.selected = 2;
	SE->selectedPlace = 3;
	SE->activateLabelsBorder();

	if(SE->zoomValue > 1.0)
	{
		SE->refreshSpectrum = 1;
	}
	SE->refreshPoints = 1;

	return 1;
}


static  uint8_t functSelectZoom()
{

	SE->selectedPlace = 5;
	SE->activateLabelsBorder();

	return 1;
}


static  uint8_t functPlayMode(uint8_t button)
{
	if(button == interfaceButton6)
	{
		SE->changePlayModeSelection(-1);
	}
	else //if(button == interfaceButton7)
	{
		SE->changePlayModeSelection(1);
	}

	SE->selectedPlace = 6;
	SE->activateLabelsBorder();
	SE->points.selected = 0;

	SE->refreshPoints = 1;

	return 1;
}

static  uint8_t functEncoder(int16_t value)
{


	switch(SE->selectedPlace)
	{
	case 0: SE->modStartPoint(value); 			break;
	case 1: SE->modLoopPoint1(value); 			break;
	case 2: SE->modLoopPoint2(value); 			break;
	case 3: SE->modEndPoint(value); 			break;
	case 5: SE->changeZoom(value);				break;
	case 6: SE->changePlayModeSelection(value);	break;
	}




	return 1;
}

static  uint8_t functSelectStart();
static  uint8_t functSelectLoop1();
static  uint8_t functSelectLoop2();



static  uint8_t functLeft()
{
	if(SE->selectedPlace > 0) SE->selectedPlace--;

	switch(SE->selectedPlace)
	{
		case 0: functSelectStart();		break;
		case 1: functSelectLoop1(); 	break;
		case 2: functSelectLoop2();		break;
		case 3: functSelectEnd();		break;
		case 4: SE->selectedPlace--;	functSelectEnd();break;
		case 5: functSelectZoom();		break;
		case 6:
		{
		SE->selectedPlace = 6;
		SE->activateLabelsBorder();
		SE->points.selected = 0;
		break;
		}
	}


	return 1;
}


static  uint8_t functRight()
{
	if(SE->selectedPlace < SE->frameData.placesCount-1) SE->selectedPlace++;

	switch(SE->selectedPlace)
	{
		case 0: functSelectStart();		break;
		case 1: functSelectLoop1(); 	break;
		case 2: functSelectLoop2();		break;
		case 3: functSelectEnd();		break;
		case 4: SE->selectedPlace++;	functSelectZoom();		break;
		case 5: functSelectZoom();		break;
		case 6:
		{
		SE->selectedPlace = 6;
		SE->activateLabelsBorder();
		SE->points.selected = 0;
		break;
		}
	}



	return 1;
}


static  uint8_t functUp()
{
	switch(SE->selectedPlace)
	{
	case 0: SE->modStartPoint(1); 			break;
	case 1: SE->modLoopPoint1(1); 			break;
	case 2: SE->modLoopPoint2(1); 			break;
	case 3: SE->modEndPoint(1); 			break;
	case 5: SE->changeZoom(1);				break;
	case 6: SE->changePlayModeSelection(-1);	break;
	}

	return 1;
}


static  uint8_t functDown()
{
	switch(SE->selectedPlace)
	{
	case 0: SE->modStartPoint(-1); 			break;
	case 1: SE->modLoopPoint1(-1); 			break;
	case 2: SE->modLoopPoint2(-1); 			break;
	case 3: SE->modEndPoint(-1); 			break;
	case 5: SE->changeZoom(-1);				break;
	case 6: SE->changePlayModeSelection(1);	break;
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

	SE->eventFunct(eventSwitchModule,SE,&button,0);

	return 1;
}


//======================================================================================================================
void cSampleEditor::changeZoom(int16_t value)
{

	float fVal = value * ZOOM_FACTOR;

	if(zoomValue + fVal < ZOOM_MIN) zoomValue  = ZOOM_MIN;
	else if(zoomValue + fVal > ZOOM_MAX ) zoomValue  = ZOOM_MAX;
	else zoomValue += fVal;

	refreshSpectrum = 1;
	refreshPoints = 1;

	showZoomValue();

}

void cSampleEditor::changePlayModeSelection(int16_t value)
{
	if(editorInstrument->playMode + value < 0) editorInstrument->playMode = 0;
	else if(editorInstrument->playMode + value > playModeCount-1) editorInstrument->playMode = playModeCount-1;
	else  editorInstrument->playMode += value;



	refreshPoints = 1;

	display.setControlValue(playModeListControl, editorInstrument->playMode);
	display.refreshControl(playModeListControl);

}

void cSampleEditor::modStartPoint(int16_t value)
{
	// obliczenie kroku przesuniecia w zaleznosci od ilosci widzianych probek na wyswietlaczu
	uint16_t move_step = zoomWidth / 480;
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
	if(zoomValue > 1 && lastChangedPoint != 1
		&& (editorInstrument->startPoint < zoomStart || editorInstrument->startPoint > zoomEnd)) refreshSpectrum = 1;

	lastChangedPoint = 1;
	refreshPoints = 1;
}

void cSampleEditor::modEndPoint(int16_t value)
{
	uint16_t move_step = zoomWidth / 480;
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

			editorInstrument->loopPoint2 = editorInstrument->endPoint;

			if(editorInstrument->loopPoint2 - dif < editorInstrument->startPoint)
			{
				editorInstrument->loopPoint1 = editorInstrument->startPoint;
				editorInstrument->loopPoint2 = editorInstrument->loopPoint1 + dif;
				editorInstrument->endPoint = editorInstrument->loopPoint2;
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

	if(zoomValue > 1 && lastChangedPoint != 2
			&& (editorInstrument->endPoint < zoomStart || editorInstrument->endPoint > zoomEnd)) refreshSpectrum = 1;

	lastChangedPoint = 2;
	refreshPoints = 1;
}

void cSampleEditor::modLoopPoint1(int16_t value)
{
	uint16_t move_step = zoomWidth / 480;
	value = value * move_step;

	if(editorInstrument->loopPoint1 + value < SAMPLE_POINT_POS_MIN) editorInstrument->loopPoint1  = 0;
	else if(editorInstrument->loopPoint1 + value > SAMPLE_POINT_POS_MAX ) editorInstrument->loopPoint1  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->loopPoint1 += value;

	if(editorInstrument->loopPoint1 < editorInstrument->startPoint) editorInstrument->loopPoint1 = editorInstrument->startPoint;
	if(editorInstrument->loopPoint1 >= editorInstrument->loopPoint2) editorInstrument->loopPoint1 = editorInstrument->loopPoint2-1;

	if(zoomValue > 1 && lastChangedPoint != 3
			&& (editorInstrument->loopPoint1 < zoomStart || editorInstrument->loopPoint1 > zoomEnd)) refreshSpectrum = 1;

	instrumentPlayer[0].setStatusBytes(LP1_MASK);


	lastChangedPoint = 3;
	refreshPoints = 1;
}

void cSampleEditor::modLoopPoint2(int16_t value)
{
	uint16_t move_step = zoomWidth / 480;
	value = value * move_step;

	if(editorInstrument->loopPoint2 + value < SAMPLE_POINT_POS_MIN) editorInstrument->loopPoint2  = 0;
	else if(editorInstrument->loopPoint2 + value > SAMPLE_POINT_POS_MAX ) editorInstrument->loopPoint2  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->loopPoint2 += value;

	if(editorInstrument->loopPoint2 > editorInstrument->endPoint) editorInstrument->loopPoint2 = editorInstrument->endPoint;
	if(editorInstrument->loopPoint2 <= editorInstrument->loopPoint1) editorInstrument->loopPoint2 = editorInstrument->loopPoint1+1;

	if(zoomValue > 1 && lastChangedPoint != 4
			&& (editorInstrument->loopPoint2 < zoomStart || editorInstrument->loopPoint2 > zoomEnd)) refreshSpectrum = 1;

	instrumentPlayer[0].setStatusBytes(LP2_MASK);

	lastChangedPoint = 4;
	refreshPoints = 1;
}


static uint8_t functShift(uint8_t value)
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
	{
		sequencer.stop();
	}

	if(value == 1)
	{
		//eventFunct(mtInstrumentEditorEventPadPress, &interfacePadStop, 0, 0);
		sequencer.stop();

		SE->isPlayingSample = 1;
		if(SE->editorInstrument->glide > 0)
		{
			switch(	SE->glidePreviewDif)
			{
				case 0: SE->playNote = 24;	break;
				case 1: SE->playNote = (SE->playNote == 24)? 25 : 24; 	break;
				case 2: SE->playNote = (SE->playNote == 24)? 36 : 24; 	break;
				case 3: SE->playNote = (SE->playNote == 24)? 47 : 24; 	break;
			}
		}

		instrumentPlayer[0].noteOn(mtProject.values.lastUsedInstrument, SE->playNote, -1);
	}
	else if(value == 0)
	{
		if(SE->isPlayingSample) instrumentPlayer[0].noteOff();
		SE->isPlayingSample = 0;
	}

	return 1;
}


static  uint8_t functInstrument(uint8_t state)
{
	if(state == buttonPress)
	{
		uint8_t buttonId  = interfaceButtonInstr;
		SE->eventFunct(eventSwitchModule, SE, &buttonId, 0);
	}
	else if(state == buttonHold)
	{

	}

	return 1;
}

static uint8_t functStepNote(uint8_t value)
{
	if(value == buttonRelease)
	{
		SE->hideNotePopout();
		SE->setDefaultScreenFunct();
	}
	else if(value == buttonHold)
	{
		for(uint8_t i = 0; i < 48; i++)
		{
			interfaceGlobals.padNamesPointer[i] = (char*)mtNotes[mtPadBoard.getNoteFromPad(i)];
		}

		SE->FM->clearButtonsRange(interfaceButton0, interfaceButton7);
		SE->FM->clearButtonsRange(interfaceButtonUp, interfaceButtonRight);
		SE->FM->clearAllPots();

		SE->showNotePopout();
	}

	return 1;
}
