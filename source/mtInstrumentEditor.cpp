
#include "mtDisplay.h"
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
		if(instrumentEditorStart)
		{
			instrumentEditorStart = 0;
			spectrumChanged = 1;
			pointsChanged = 1;
			labelsChanged = 1;


			updateButtons();
			updatePots();

			mtDisplay.setMode(mtDisplayModeInstrumentEditor);
		}

		if(spectrumChanged)
		{
			spectrumChanged = 0;

			processSpectrum();
			mtDisplay.setInstrumentEditorSpectrum(&spectrum);
		}
		if(pointsChanged)
		{
			pointsChanged = 0;

			playMem1.setTimePoints(&editorInstrument);

			processPoints();
			mtDisplay.setInstrumentEditorPoints(&spectrum);
		}
		if(labelsChanged)
		{
			labelsChanged = 0;

			processLabels();

			//mtDisplay.setInstrumentEditorPotsLabels();
			//mtDisplay.setInstrumentEditorButtonsLabels();
		}
	}
	//---------------------------------------------------------------------------------------
	else if(instrumentEditorMode == mtInstrumentEditorModeInstrumentSelect)
	{





	}
	//---------------------------------------------------------------------------------------
	else if(instrumentEditorMode == mtInstrumentEditorModeSampleSelect)
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
	instrumentEditorStart = 1;
	refreshInstrumentEditor = 1;
}

void cMtInstrumentEditor::startEmpty()
{
	openedInstrumentIndex = -1;

	editorInstrument.sampleIndex = -1;
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

	instrumentEditorMode = mtInstrumentEditorModeInstrumentSelect;
	instrumentEditorStart = 1;
	refreshInstrumentEditor = 1;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
void cMtInstrumentEditor::buttonChange(uint8_t button, uint8_t value)
{
	switch(buttonFunction[button])
	{
	case mtInstrumentEditorButtonFunctionNone  		: 	break;
	case mtInstrumentEditorButtonFunctionPlay  		:	play(value);	break;
	case mtInstrumentEditorButtonFunctionStop  		:	stop(); break;
	case mtInstrumentEditorButtonFunctionPlayMode  	: 	break;

	default: break;
	}
	refreshInstrumentEditor = 1;
}

uint8_t pot1;

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
		default: break;
	}

/*
	switch(pot)
	{
	case 0:
	{
		pot1 += value;
		char label[20] = {"SP: "};
		label[4] = pot1/100+48;
		label[5] = (pot1%100)/10   +48;
		label[6] = pot1%10  +48;
		label[7] = 0;

		setPotsLabel(mtInstrumentEditorPotFunctionStartPoint,label);

		break;
	}
	case 2:
	{

		break;
	}

	default: break;
	}

*/

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


	spectrum.spectrumType = 0;



}


void cMtInstrumentEditor::processPoints()
{

	spectrum.startPoint = (editorInstrument.startPoint * 480) / SAMPLE_POINT_POS_MAX;
	spectrum.endPoint   = (editorInstrument.endPoint * 480) / SAMPLE_POINT_POS_MAX;
	spectrum.loopPoint1 = (editorInstrument.loopPoint1 * 480) / SAMPLE_POINT_POS_MAX;
	spectrum.loopPoint2 = (editorInstrument.loopPoint2 * 480) / SAMPLE_POINT_POS_MAX;


}

void cMtInstrumentEditor::processLabels()
{




}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// BUTTONS

void cMtInstrumentEditor::setButtonLabel(uint8_t number, char * label)
{
	uint8_t i = 0;
	buttonLabels[number][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		buttonLabels[number][i] = label[i];
		i++;
	}
	buttonLabels[number][i] = 0;
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
			if(isPlayingSample == 0)
			{
				setButton(0, mtInstrumentEditorButtonFunctionPlay);
			}
			else
			{
				setButton(0, mtInstrumentEditorButtonFunctionStop);
			}

			setButton(1, mtInstrumentEditorButtonFunctionPlayMode);
			setButton(2, mtInstrumentEditorButtonFunctionAmpEnvelope);
			setButton(3, mtInstrumentEditorButtonFunctionSampelSelect);
			setButton(4, mtInstrumentEditorButtonFunctionInstrumentSelect);

			break;
		}
	}


	setButtonLabel(0, (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[0]][0]);
	setButtonLabel(1, (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[1]][0]);
	setButtonLabel(2, (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[2]][0]);
	setButtonLabel(3, (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[3]][0]);
	setButtonLabel(4, (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[4]][0]);

	mtDisplay.setInstrumentEditorButtonsLabels(&buttonLabels[0][0]);
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
	potsLabels[function][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		mtInstrumentEditorPotsLabels[function][i] = label[i];
		i++;
	}
	potsLabels[function][i] = 0;

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
			setPotFunction(0, mtInstrumentEditorPotFunctionStartPoint);
			setPotFunction(1, mtInstrumentEditorPotFunctionEndPoint);

			if(editorInstrument.playMode == samplePlayModeLoop)
			{
				setPotFunction(1, mtInstrumentEditorPotFunctionLoopPoint1);
				setPotFunction(2, mtInstrumentEditorPotFunctionLoopPoint2);
				setPotFunction(3, mtInstrumentEditorPotFunctionEndPoint);
			}


			setPotFunction(4, mtInstrumentEditorPotFunctionViewPosition);

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
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// instrument modification

void cMtInstrumentEditor::modStartPoint(int16_t value)
{
	// obliczenie kroku przesuniecia w zaleznosci od ilosci widzianych probek na wyswietlaczu
	uint16_t move_step = viewLength / 480;
	value = value * move_step;

	if(editorInstrument.startPoint + value < SAMPLE_POINT_POS_MIN) editorInstrument.startPoint  = 0;
	else if(editorInstrument.startPoint + value > SAMPLE_POINT_POS_MAX ) editorInstrument.startPoint  = SAMPLE_POINT_POS_MAX;
	else editorInstrument.startPoint += value;

	if(editorInstrument.startPoint > editorInstrument.endPoint) editorInstrument.startPoint = editorInstrument.endPoint-1;
	if(editorInstrument.startPoint > editorInstrument.loopPoint1) editorInstrument.loopPoint1 = editorInstrument.startPoint;
	if(editorInstrument.loopPoint1 > editorInstrument.loopPoint2) editorInstrument.loopPoint2 = editorInstrument.loopPoint1+1;


	pointsChanged = 1;
}

void cMtInstrumentEditor::modEndPoint(int16_t value)
{
	uint16_t move_step = viewLength / 480;
	value = value * move_step;

	if(editorInstrument.endPoint + value < SAMPLE_POINT_POS_MIN) editorInstrument.endPoint  = 0;
	else if(editorInstrument.endPoint + value > SAMPLE_POINT_POS_MAX ) editorInstrument.endPoint  = SAMPLE_POINT_POS_MAX;
	else editorInstrument.endPoint += value;

	if(editorInstrument.endPoint < editorInstrument.startPoint) editorInstrument.endPoint = editorInstrument.startPoint+1;
	if(editorInstrument.endPoint < editorInstrument.loopPoint2) editorInstrument.loopPoint2 = editorInstrument.endPoint;
	if(editorInstrument.loopPoint2 < editorInstrument.loopPoint1) editorInstrument.loopPoint1 = editorInstrument.loopPoint2-1;


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

void cMtInstrumentEditor::selectInstrument(int16_t value)
{

}

void cMtInstrumentEditor::selectSample(int16_t value)
{

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
		playMem1.play(&editorInstrument,100);
	}
	else if(value == 0)
	{
		if(isPlayingSample) playMem1.stopLoopMode();
		isPlayingSample = 0;
	}
}

void cMtInstrumentEditor::stop()
{

	if(isPlayingSample) playMem1.stopLoopMode();


	isPlayingSample = 0;

}
