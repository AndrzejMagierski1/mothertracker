
#include "mtDisplay.h"
#include "AnalogInputs.h"
#include "sdram.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"

#include "mtInstrumentEditor.h"


cMtInstrumentEditor mtInstrumentEditor;


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtInstrumentEditor::begin()
{



}


void cMtInstrumentEditor::update()
{
	if(instrumentEditorMode == mtInstrumentEditorModeDisabled) return;
	if(!refreshInstrumentEditor) return;
	refreshInstrumentEditor = 0;

	//---------------------------------------------------------------------------------------
	if(instrumentEditorMode == mtInstrumentEditorModeWaveEdit)
	{
		//-----------------------------------------------------
		if(instrumentEditorModeStart)
		{
			instrumentEditorModeStart = 0;
			spectrumChanged = 1;
			pointsChanged = 1;
			labelsChanged = 1;

			updateButtons();
			updatePots();

			mtDisplay.setMode(mtDisplayModeInstrumentEditor);
		}
		//-----------------------------------------------------
		if(labelsChanged)
		{
			labelsChanged = 0;

			processLabels();

			//mtDisplay.setInstrumentEditorPotsLabels();
			//mtDisplay.setInstrumentEditorButtonsLabels();
		}
		//-----------------------------------------------------
		if(spectrumChanged)
		{
			spectrumChanged = 0;
			updateButtons();
			updatePots();

			processSpectrum();
			mtDisplay.setInstrumentEditorSpectrum(&spectrum);
		}
		//-----------------------------------------------------
		if(pointsChanged)
		{
			pointsChanged = 0;
			updateButtons();
			updatePots();

			instrumentPlayer[0].change(&editorInstrument, &editorMod);

			processPoints();
			mtDisplay.setInstrumentEditorPoints(&spectrum);
		}
		//-----------------------------------------------------
		if(sampleListChanged)
		{
			updateButtons();
			updatePots();

			if(!sampleListEnabled)
			{
				sampleListChanged = 0;
				mtDisplay.setInstrumentEditorSampleList(0, 0, 0);
				return;
			}

			if(sampleListChanged == 2) // pokaz liste
			{
				sampleListChanged = 0;
				//przetworz tablice adresow nazw sampli na podstawie nazw z banku sampli

				for(uint8_t i = 0; i < SAMPLES_MAX; i++)
				{
					sampleNames[i] = mtProject.sampleBank.sample[i].file_name;
				}

				if(editorInstrument.sampleIndex < 0) editorInstrument.sampleIndex = 0;
				mtDisplay.setInstrumentEditorSampleList(editorInstrument.sampleIndex,
														sampleNames, SAMPLES_MAX);
			}
		}
		if(parametersChanged)
		{
			parametersChanged = 0;

			updateButtons();
			updatePots();

			mtDisplay.setInstrumentEditorParameters(&params);

		}

	}
	//---------------------------------------------------------------------------------------
	else if(instrumentEditorMode == mtInstrumentEditorParameters)
	{


	}
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtInstrumentEditor::startExisting(uint8_t instrumentIndex)
{
	openedInstrumentIndex = instrumentIndex;
	editorInstrument = mtProject.instrument[instrumentIndex];

	instrumentEditorMode = mtInstrumentEditorModeWaveEdit;
	instrumentEditorModeStart = 1;
	refreshInstrumentEditor = 1;
}

void cMtInstrumentEditor::startEmpty()
{
	openedInstrumentIndex = -1;

	editorInstrument.sampleIndex = 0;
	editorInstrument.playMode = 0;

	editorInstrument.startPoint = 0;
	editorInstrument.loopPoint1 = 0;
	editorInstrument.loopPoint2 = SAMPLE_POINT_POS_MAX;
	editorInstrument.endPoint = SAMPLE_POINT_POS_MAX;

	editorInstrument.ampDelay = 0;
	editorInstrument.ampAttack = 0;
	editorInstrument.ampHold = 0;
	editorInstrument.ampDecay = 0;
	editorInstrument.ampSustain = 100;
	editorInstrument.ampRelease = 0;
	editorInstrument.panning = 0;

	sampleListEnabled = 1;
	sampleListChanged = 2;
	instrumentEditorMode = mtInstrumentEditorModeWaveEdit;
	instrumentEditorModeStart = 1;
	refreshInstrumentEditor = 1;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
void cMtInstrumentEditor::buttonChange(uint8_t button, uint8_t value)
{
	switch(buttonFunction[button])
	{
	case mtInstrumentEditorButtonFunctionNone  				: 					break;
	case mtInstrumentEditorButtonFunctionPlay  				:	play(value);	break;
	case mtInstrumentEditorButtonFunctionStop  				:	stop(value);			break;
	case mtInstrumentEditorButtonFunctionPlayMode  			: 	changePlayMode(value);	break;
	case mtInstrumentEditorButtonFunctionEnvelopes  		: 	break;
	case mtInstrumentEditorButtonFunctionInstrumentSelect  	: 	break;
	case mtInstrumentEditorButtonFunctionSampleList  		: 	showSampleList(value);	break;
	case mtInstrumentEditorButtonFunctionParameters  		: 	showParameters(value);	break;
	default: break;
	}

	refreshInstrumentEditor = 1;
}


void cMtInstrumentEditor::potChange(uint8_t pot, int16_t value)
{
	switch(potFunction[pot])
	{
		case mtInstrumentEditorPotFunctionNone: 			break;
		case mtInstrumentEditorPotFunctionStartPoint:		modStartPoint(value);		break;
		case mtInstrumentEditorPotFunctionEndPoint:			modEndPoint(value);			break;
		case mtInstrumentEditorPotFunctionLoopPoint1:		modLoopPoint1(value);		break;
		case mtInstrumentEditorPotFunctionLoopPoint2:		modLoopPoint2(value);		break;
		case mtInstrumentEditorPotFunctionInstrumentSelect:	selectInstrument(value);	break;
		case mtInstrumentEditorPotFunctionSampleSelect:		selectSample(value);		break;
		case mtInstrumentEditorPotFunctionViewPosition:		changeView(value);			break;
		case mtInstrumentEditorPotFunctionVievZoom:			changeZoom(value);			break;
		case mtInstrumentEditorPotFunctionPanning  		: 	changePanning(pot, value);		break;
		case mtInstrumentEditorPotFunctionGlide  		: 	changeGlide(pot, value);			break;
		case mtInstrumentEditorPotFunctionFilter  		: 	changeFilter(pot, value);		break;
		default: break;
	}

	refreshInstrumentEditor = 1;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// spectrum processing


void cMtInstrumentEditor::processSpectrum()
{
	//TODO: uwaga tu wazna kolejnosc + do sprawdzenia
	if(openedInstrumentIndex < 0 || mtProject.instrument[openedInstrumentIndex].sampleIndex < 0)
	{
		for(uint16_t i = 0; i < 480; i++)
		{
			spectrum.upperData[i] = 0;
			spectrum.lowerData[i] = 0;
		}

		return;
	}


	int16_t * sampleData = mtProject.sampleBank.sample[editorInstrument.sampleIndex].address;
	//uint32_t * samplesCount = (uint32_t*)sampleData;


	uint32_t resolution = mtProject.sampleBank.sample[editorInstrument.sampleIndex].length / 480;


	int16_t up = 0;
	int16_t low = 0;

	uint32_t step = 0;


	for(uint16_t i = 0; i < 480; i++)
	{
		low = up = 0; //*(sampleData+step);

		for(uint16_t j = 0; j < resolution; j++)
		{
			int16_t sample = *(sampleData+step+j);


			if(sample > up)  up = sample;
			else if(sample < low) low = sample;

		}
		step+= resolution;

		up = up/1000;
		low = low/1000;

		spectrum.upperData[i] =  up;
		spectrum.lowerData[i] = low;
	}

	if(mtProject.sampleBank.sample[editorInstrument.sampleIndex].length > 480) spectrum.spectrumType = 0;
	else spectrum.spectrumType = 1;

}


void cMtInstrumentEditor::processPoints()
{

	spectrum.pointsType = editorInstrument.playMode;

	spectrum.startPoint = (editorInstrument.startPoint * 479) / SAMPLE_POINT_POS_MAX;
	spectrum.endPoint   = (editorInstrument.endPoint   * 479) / SAMPLE_POINT_POS_MAX;
	spectrum.loopPoint1 = (editorInstrument.loopPoint1 * 479) / SAMPLE_POINT_POS_MAX;
	spectrum.loopPoint2 = (editorInstrument.loopPoint2 * 479) / SAMPLE_POINT_POS_MAX;


}

void cMtInstrumentEditor::processLabels()
{

	if(instrumentEditorMode == mtInstrumentEditorModeWaveEdit)
	{
		for(uint8_t i = 0; i < 5; i++)
		{
			switch(buttonFunction[i])
			{
			case mtInstrumentEditorButtonFunctionPlayMode:

				setButtonLabel(mtInstrumentEditorButtonFunctionPlayMode, (char*)&playModeFunctLabels[editorInstrument.playMode][0]);

			break;

			default: break;
			}

		}

	}


}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// BUTTONS

void cMtInstrumentEditor::setButtonLabel(uint8_t function, char* label)
{
	uint8_t i = 0;
	mtInstrumentEditorButtonsLabels[function][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		mtInstrumentEditorButtonsLabels[function][i] = label[i];
		i++;
	}
	mtInstrumentEditorButtonsLabels[function][i] = 0;

}


void cMtInstrumentEditor::updateButtons()
{
	setButton(0, mtInstrumentEditorButtonFunctionNone);
	setButton(1, mtInstrumentEditorButtonFunctionNone);
	setButton(2, mtInstrumentEditorButtonFunctionNone);
	setButton(3, mtInstrumentEditorButtonFunctionNone);
	setButton(4, mtInstrumentEditorButtonFunctionNone);


	switch(instrumentEditorMode)
	{
		case mtInstrumentEditorModeWaveEdit:
		{
			if(parametersEnabled)
			{
				setButton(0, mtInstrumentEditorButtonFunctionPlay);
				setButton(2, mtInstrumentEditorButtonFunctionParameters);

			}
			else
			{
				setButton(0, mtInstrumentEditorButtonFunctionPlay);
				setButton(1, mtInstrumentEditorButtonFunctionPlayMode);
				setButton(2, mtInstrumentEditorButtonFunctionParameters);
				setButton(3, mtInstrumentEditorButtonFunctionSampleList);
				setButton(4, mtInstrumentEditorButtonFunctionInstrumentSelect);
			}
			break;
		}
	}

	buttonLabels[0] = (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[0]][0];
	buttonLabels[1] = (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[1]][0];
	buttonLabels[2] = (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[2]][0];
	buttonLabels[3] = (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[3]][0];
	buttonLabels[4] = (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[4]][0];

	mtDisplay.setInstrumentEditorButtonsLabels(buttonLabels);
}


void cMtInstrumentEditor::setButton(uint8_t number, uint8_t function)
{
	buttonFunction[number] = function;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// POTS

void cMtInstrumentEditor::setPotsLabel(uint8_t function, char* label)
{
	uint8_t i = 0;
	mtInstrumentEditorPotsLabels[function][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		mtInstrumentEditorPotsLabels[function][i] = label[i];
		i++;
	}
	mtInstrumentEditorPotsLabels[function][i] = 0;

	mtDisplay.setInstrumentEditorPotsLabels(&potsLabels[0]);
}



void cMtInstrumentEditor::updatePots()
{
	setPotFunction(0, mtInstrumentEditorPotFunctionNone);
	setPotFunction(1, mtInstrumentEditorPotFunctionNone);
	setPotFunction(2, mtInstrumentEditorPotFunctionNone);
	setPotFunction(3, mtInstrumentEditorPotFunctionNone);
	setPotFunction(4, mtInstrumentEditorPotFunctionNone);


	switch(instrumentEditorMode)
	{
		case mtInstrumentEditorModeWaveEdit:
		{

			if(parametersEnabled)
			{
				setPotFunction(0, mtInstrumentEditorPotFunctionPanning);
				setPotFunction(1, mtInstrumentEditorPotFunctionGlide);
				setPotFunction(4, mtInstrumentEditorPotFunctionFilter);
			}
			else
			{
				setPotFunction(0, mtInstrumentEditorPotFunctionStartPoint);
				setPotFunction(1, mtInstrumentEditorPotFunctionEndPoint);
				if(sampleListEnabled) setPotFunction(3, mtInstrumentEditorPotFunctionSampleSelect);
				if(editorInstrument.playMode >= loopForward)
				{
					setPotFunction(1, mtInstrumentEditorPotFunctionLoopPoint1);
					setPotFunction(2, mtInstrumentEditorPotFunctionLoopPoint2);
					if(!sampleListEnabled) setPotFunction(3, mtInstrumentEditorPotFunctionEndPoint);
				}
				setPotFunction(4, mtInstrumentEditorPotFunctionViewPosition);
			}
			break;
		}
	}

	potsLabels[0] = (char *)&mtInstrumentEditorPotsLabels[potFunction[0]][0];
	potsLabels[1] = (char *)&mtInstrumentEditorPotsLabels[potFunction[1]][0];
	potsLabels[2] = (char *)&mtInstrumentEditorPotsLabels[potFunction[2]][0];
	potsLabels[3] = (char *)&mtInstrumentEditorPotsLabels[potFunction[3]][0];
	potsLabels[4] = (char *)&mtInstrumentEditorPotsLabels[potFunction[4]][0];

	mtDisplay.setInstrumentEditorPotsLabels(potsLabels);
}


void cMtInstrumentEditor::setPotFunction(uint8_t number, uint8_t function)
{
	potFunction[number] = function;
	AnalogInputs.setPotResolution(number, potsFuncResolutions[function]);
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// instrument modification

void cMtInstrumentEditor::modStartPoint(int16_t value)
{
	// obliczenie kroku przesuniecia w zaleznosci od ilosci widzianych probek na wyswietlaczu
	uint16_t move_step = viewLength / 480;
	uint16_t dif;
	value = value * move_step;

	if(editorInstrument.startPoint + value < SAMPLE_POINT_POS_MIN) editorInstrument.startPoint  = 0;
	else if(editorInstrument.startPoint + value > SAMPLE_POINT_POS_MAX ) editorInstrument.startPoint  = SAMPLE_POINT_POS_MAX;
	else editorInstrument.startPoint += value;

	if(editorInstrument.startPoint > editorInstrument.endPoint) editorInstrument.startPoint = editorInstrument.endPoint-1;


	if(editorInstrument.startPoint > editorInstrument.loopPoint1)
	{
		dif = editorInstrument.loopPoint2 - editorInstrument.loopPoint1;
		editorInstrument.loopPoint1 = editorInstrument.startPoint;
		editorInstrument.loopPoint2 = editorInstrument.loopPoint1 + dif;

		if(editorInstrument.loopPoint2 > editorInstrument.endPoint)
		{
			editorInstrument.loopPoint2 = editorInstrument.endPoint;
			editorInstrument.loopPoint1 = editorInstrument.loopPoint2 - dif;
			editorInstrument.startPoint = editorInstrument.loopPoint1;
		}
	}



	pointsChanged = 1;
}

void cMtInstrumentEditor::modEndPoint(int16_t value)
{
	uint16_t move_step = viewLength / 480;
	uint16_t dif;
	value = value * move_step;

	if(editorInstrument.endPoint + value < SAMPLE_POINT_POS_MIN) editorInstrument.endPoint  = 0;
	else if(editorInstrument.endPoint + value > SAMPLE_POINT_POS_MAX ) editorInstrument.endPoint  = SAMPLE_POINT_POS_MAX;
	else editorInstrument.endPoint += value;

	if(editorInstrument.endPoint < editorInstrument.startPoint) editorInstrument.endPoint = editorInstrument.startPoint+1;
	if(editorInstrument.endPoint < editorInstrument.loopPoint2)
	{
		dif = editorInstrument.loopPoint2 - editorInstrument.loopPoint1;

		editorInstrument.loopPoint2 = editorInstrument.endPoint;
		editorInstrument.loopPoint1 = editorInstrument.loopPoint2 - dif;

		if(editorInstrument.loopPoint1 < editorInstrument.startPoint)
		{
			editorInstrument.loopPoint1 = editorInstrument.startPoint;
			editorInstrument.loopPoint2 = editorInstrument.loopPoint1 + dif;
			editorInstrument.endPoint = editorInstrument.loopPoint2;
		}
	}



	pointsChanged = 1;
}

void cMtInstrumentEditor::modLoopPoint1(int16_t value)
{
	uint16_t move_step = viewLength / 480;
	value = value * move_step;

	if(editorInstrument.loopPoint1 + value < SAMPLE_POINT_POS_MIN) editorInstrument.loopPoint1  = 0;
	else if(editorInstrument.loopPoint1 + value > SAMPLE_POINT_POS_MAX ) editorInstrument.loopPoint1  = SAMPLE_POINT_POS_MAX;
	else editorInstrument.loopPoint1 += value;

	if(editorInstrument.loopPoint1 < editorInstrument.startPoint) editorInstrument.loopPoint1 = editorInstrument.startPoint;
	if(editorInstrument.loopPoint1 > editorInstrument.loopPoint2) editorInstrument.loopPoint1 = editorInstrument.loopPoint2-1;

	pointsChanged = 1;
}

void cMtInstrumentEditor::modLoopPoint2(int16_t value)
{
	uint16_t move_step = viewLength / 480;
	value = value * move_step;

	if(editorInstrument.loopPoint2 + value < SAMPLE_POINT_POS_MIN) editorInstrument.loopPoint2  = 0;
	else if(editorInstrument.loopPoint2 + value > SAMPLE_POINT_POS_MAX ) editorInstrument.loopPoint2  = SAMPLE_POINT_POS_MAX;
	else editorInstrument.loopPoint2 += value;

	if(editorInstrument.loopPoint2 > editorInstrument.endPoint) editorInstrument.loopPoint2 = editorInstrument.endPoint;
	if(editorInstrument.loopPoint2 < editorInstrument.loopPoint1) editorInstrument.loopPoint2 = editorInstrument.loopPoint1+1;

	pointsChanged = 1;
}

void cMtInstrumentEditor::changePlayMode(uint8_t value)
{
	if(value == 1)
	{

		if(editorInstrument.playMode < playModeMax-1 ) editorInstrument.playMode++;
		else editorInstrument.playMode = 0;

		pointsChanged = 1;
		labelsChanged = 1;
	}
}


void cMtInstrumentEditor::changePanning(uint8_t pot, int16_t value)
{
	if(editorInstrument.panning + value < PANNING_MIN) editorInstrument.panning  = 0;
	else if(editorInstrument.panning + value > PANNING_MAX ) editorInstrument.panning  = PANNING_MAX;
	else editorInstrument.panning += value;

	params.type[pot] = 1;
	params.value[pot] = editorInstrument.panning;


	mtDisplay.setInstrumentEditorParameters(&params);


}

void cMtInstrumentEditor::changeGlide(uint8_t pot, int16_t value)
{



}

void cMtInstrumentEditor::changeFilter(uint8_t pot, int16_t value)
{


}

void cMtInstrumentEditor::showParameters(uint8_t value)
{
	if(value == 1)
	{
		if(parametersEnabled)
		{
			parametersEnabled = 0;
			spectrumChanged = 1;
		}
		else
		{
			parametersEnabled = 1;
			parametersChanged = 1;
		}
	}
}

void cMtInstrumentEditor::selectInstrument(int16_t value)
{

}

void cMtInstrumentEditor::selectSample(int16_t value)
{
	if(editorInstrument.sampleIndex + value < 0) editorInstrument.sampleIndex  = 0;
	else if(editorInstrument.sampleIndex + value >= SAMPLES_MAX) editorInstrument.sampleIndex  = SAMPLES_MAX-1;
	else editorInstrument.sampleIndex += value;

	mtDisplay.setInstrumentEditorSampleListPos(editorInstrument.sampleIndex);
	spectrumChanged = 1;
}

void cMtInstrumentEditor::showSampleList(uint8_t value)
{
	if(value == 1)
	{
		if(sampleListEnabled)
		{
			sampleListEnabled = 0;
			sampleListChanged = 1;
		}
		else
		{
			sampleListEnabled = 1;
			sampleListChanged = 2;
		}
	}
}

void cMtInstrumentEditor::changeView(int16_t value)
{

}

void cMtInstrumentEditor::changeZoom(int16_t value)
{

}


void cMtInstrumentEditor::play(uint8_t value)
{
	if(value == 1)
	{
		isPlayingSample = 1;
		instrumentPlayer[0].play(&editorInstrument, &editorMod);
	}
	else if(value == 0)
	{
		if(isPlayingSample) instrumentPlayer[0].stop();
		isPlayingSample = 0;
	}
}

void cMtInstrumentEditor::stop(uint8_t value)
{

	if(isPlayingSample) instrumentPlayer[0].stop();


	isPlayingSample = 0;

}
