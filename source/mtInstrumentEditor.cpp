
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

void cMtInstrumentEditor::update()
{
	if(!refreshInstrumentEditor) return;
	refreshInstrumentEditor = 0;


	//-----------------------------------------------------
	if(instrumentEditorModeStart)
	{
		instrumentEditorModeStart = 0;
		spectrumChanged = 2;
		pointsChanged = 2;
		labelsChanged = 2;
		lastChangedPoint = 0;

		updateButtonsFunctions();
		updatePotsFunctions();
	}
	//-----------------------------------------------------
	if(labelsChanged)
	{
		if(labelsChanged == 2)
		{
			updateButtonsFunctions();
			updatePotsFunctions();
			mtDisplay.setPotsLabels(1);
			mtDisplay.setButtonsLabels(1);
		}

		labelsChanged = 0;

		processLabels();

		//mtDisplay.setInstrumentEditorPotsLabels();
		//mtDisplay.setInstrumentEditorButtonsLabels();
	}
	//-----------------------------------------------------
	if(spectrumChanged)
	{
		if(spectrumChanged == 2)
		{
			mtDisplay.setSpectrum(1);
			mtDisplay.setSpectrumPoints(1);
			mtDisplay.setValue(0);
			mtDisplay.setEnvelopes(0);

			updateButtonsFunctions();
			updatePotsFunctions();
			labelsChanged = 1;
		}
		spectrumChanged = 0;
		//updateButtonsFunctions();
		//updatePotsFunctions();

		processSpectrum();
		mtDisplay.changeSpectrum(&spectrum);
	}
	//-----------------------------------------------------
	if(pointsChanged)
	{
		pointsChanged = 0;
		//updateButtonsFunctions();
		//updatePotsFunctions();

		//instrumentPlayer[0].change(editorInstrument, &editorMod);

		processPoints();
		mtDisplay.changeSpectrumPoints(&spectrum);
	}
	//-----------------------------------------------------
	if(sampleListChanged)
	{
		updateButtonsFunctions();
		updatePotsFunctions();

		if(!sampleListEnabled)
		{
			sampleListChanged = 0;
			mtDisplay.setList(3, 0, 0, 0, 0, 0);
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

			if(editorInstrument->sampleIndex < 0) editorInstrument->sampleIndex = 0;
			mtDisplay.setList(3, 3, 1, editorInstrument->sampleIndex, sampleNames, SAMPLES_MAX);
		}
	}
	//-----------------------------------------------------
	if(instrumentListChanged)
	{
		updateButtonsFunctions();
		updatePotsFunctions();

		if(!instrumentListEnabled)
		{
			instrumentListChanged = 0;
			mtDisplay.setList(0, 0, 0, 0, 0, 0);
			return;
		}

		if(instrumentListChanged == 2) // pokaz liste
		{
			instrumentListChanged = 0;
			//przetworz tablice adresow nazw sampli na podstawie nazw z banku sampli

			for(uint8_t i = 0; i < INSTRUMENTS_MAX; i++)
			{
				instrumentNames[i] = mtProject.instrument[i].name;
			}

			if(openedInstrumentIndex < 0) openedInstrumentIndex = 0;
			mtDisplay.setList(0, 0, 1, openedInstrumentIndex, instrumentNames, INSTRUMENTS_MAX);
		}
	}
	if(parametersChanged)
	{
		if(parametersChanged == 2)
		{
			mtDisplay.setSpectrum(0);
			mtDisplay.setSpectrumPoints(0);
			mtDisplay.setValue(1);

			updateButtonsFunctions();
			updatePotsFunctions();
			updateParameters();
			labelsChanged = 1;
		}
		parametersChanged = 0;

		processParameters();

		mtDisplay.changeValues(&values);
		//instrumentPlayer[0].change(editorInstrument, &editorMod);
	}
	if(envelopesChanged)
	{
		if(envelopesChanged == 2)
		{
			mtDisplay.setSpectrum(0);
			mtDisplay.setSpectrumPoints(0);
			mtDisplay.setValue(0);
			mtDisplay.setEnvelopes(1);

			updateButtonsFunctions();
			updatePotsFunctions();
			labelsChanged = 1;
		}
		envelopesChanged = 0;

		processEnvelopes();

		mtDisplay.changeEnvelopes(&envelope);
		//instrumentPlayer[0].change(editorInstrument, &editorMod);

	}


}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtInstrumentEditor::startExisting(uint8_t instrumentIndex)
{
	openedInstrumentIndex = instrumentIndex;
	editorInstrument = &mtProject.instrument[instrumentIndex];

	instrumentEditorModeStart = 1;
	refreshInstrumentEditor = 1;
}

void cMtInstrumentEditor::startEmpty()
{
	openedInstrumentIndex = -1;

	editorInstrument->sampleIndex = 0;
	editorInstrument->playMode = 0;

	editorInstrument->startPoint = 0;
	editorInstrument->loopPoint1 = 0;
	editorInstrument->loopPoint2 = SAMPLE_POINT_POS_MAX;
	editorInstrument->endPoint = SAMPLE_POINT_POS_MAX;

	editorInstrument->envelope[envAmp].delay = 0;
	editorInstrument->envelope[envAmp].attack = 0;
	editorInstrument->envelope[envAmp].hold = 0;
	editorInstrument->envelope[envAmp].decay = 0;
	editorInstrument->envelope[envAmp].sustain = 1;
	editorInstrument->envelope[envAmp].release = 0;
	editorInstrument->panning = 0;

	sampleListEnabled = 1;
	sampleListChanged = 2;
	instrumentEditorModeStart = 1;
	refreshInstrumentEditor = 1;
}

void cMtInstrumentEditor::stop()
{
	mtDisplay.setSpectrum(0);
	mtDisplay.setSpectrumPoints(0);
	mtDisplay.setValue(0);
	mtDisplay.setEnvelopes(0);
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
uint8_t cMtInstrumentEditor::padsChange(uint8_t type, uint8_t n, uint8_t velo)
{

	return 0;
}

void cMtInstrumentEditor::buttonChange(uint8_t button, uint8_t value)
{
	switch(buttonFunction[button])
	{
	case mtInstrumentEditorButtonFunctionNone  				: 					break;
	case mtInstrumentEditorButtonFunctionPlay  				:	play(value);			break;
	case mtInstrumentEditorButtonFunctionStop  				:	stopPlaying(value);			break;
	case mtInstrumentEditorButtonFunctionPlayMode  			: 	changePlayMode(value);	break;
	case mtInstrumentEditorButtonFunctionEnvelopes  		: 	showEnvelopes(value);	break;
	case mtInstrumentEditorButtonFunctionInstrumentList 	: 	showInstrumentList(value);	break;
	case mtInstrumentEditorButtonFunctionSampleList  		: 	showSampleList(value);	break;
	case mtInstrumentEditorButtonFunctionParameters  		: 	showParameters(value);	break;
	case mtInstrumentEditorButtonFunctionChangeGlideNote	: 	changeGlideNote(value);	break;
	case mtInstrumentEditorButtonFunctionFilterType			: 	changeFilterType(value);break;
	case mtInstrumentEditorButtonFunctionEnvelopeType		: 	changeEnvelopeType(value);break;
	case mtInstrumentEditorButtonFunctionEnvelopeAmp		: 	setEnvelopeTypeAmp(value);	break;
	case mtInstrumentEditorButtonFunctionEnvelopeFilter		: 	setEnvelopeTypeFilter(value);break;
	case mtInstrumentEditorButtonFunctionEnvelopeEnable		: 	setEnvelopeEnable(value);break;
	default: break;
	}

	refreshInstrumentEditor = 1;
}


void cMtInstrumentEditor::potChange(uint8_t pot, int16_t value)
{
	switch(potFunction[pot])
	{
		case mtInstrumentEditorPotFunctionNone				: 	break;
		case mtInstrumentEditorPotFunctionStartPoint		:	modStartPoint(value);		break;
		case mtInstrumentEditorPotFunctionEndPoint			:	modEndPoint(value);			break;
		case mtInstrumentEditorPotFunctionLoopPoint1		:	modLoopPoint1(value);		break;
		case mtInstrumentEditorPotFunctionLoopPoint2		:	modLoopPoint2(value);		break;
		case mtInstrumentEditorPotFunctionInstrumentSelect	:	selectInstrument(value);	break;
		case mtInstrumentEditorPotFunctionSampleSelect		:	selectSample(value);		break;
		case mtInstrumentEditorPotFunctionViewPosition		:	changeView(value);			break;
		case mtInstrumentEditorPotFunctionViewZoom			:	changeZoom(value);			break;
		case mtInstrumentEditorPotFunctionPanning  			: 	changePanning(pot, value);	break;
		case mtInstrumentEditorPotFunctionGlide  			: 	changeGlide(value);			break;
		case mtInstrumentEditorPotFunctionFilter  			: 	changeFilter(value);		break;
		case mtInstrumentEditorPotFunctionAttack			:	changeAttack(value);		break;
		case mtInstrumentEditorPotFunctionDecay  			: 	changeDecay(value);			break;
		case mtInstrumentEditorPotFunctionSustaion  		: 	changeSustain(value);		break;
		case mtInstrumentEditorPotFunctionRelease  			: 	changeRelease(value);		break;
		case mtInstrumentEditorPotFunctionAmount  			: 	changeAmount(value);		break;
		case mtInstrumentEditorPotFunctionResonance  		: 	changeResonance(value);		break;
		default: break;
	}

	refreshInstrumentEditor = 1;
}


void cMtInstrumentEditor::seqButtonChange(uint8_t type, uint8_t x, uint8_t y)
{

	stop();
	eventFunct(mtInstrumentEditorSeqButtonsPress, &x, &y, 0);


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


	uint16_t offset_pixel;


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



	int16_t * sampleData;
	uint32_t resolution;



	if(zoomValue > 1.0)
	{
		zoomWidth = (MAX_16BIT/zoomValue);
		zoomStart =  zoomPosition - zoomWidth/2;
		zoomEnd = zoomPosition + zoomWidth/2;

		if(zoomStart < 0)
		{
			zoomEnd = zoomWidth;
			zoomStart = 0;
			offset_pixel = ((zoomPosition-zoomStart) * 479) / zoomWidth;
		}
		else if(zoomEnd > MAX_16BIT)
		{
			zoomEnd = MAX_16BIT;
			zoomStart = MAX_16BIT-zoomWidth;
			offset_pixel = ((zoomPosition-zoomStart) * 479) / zoomWidth;
		}
		else
		{
			offset_pixel = ((zoomPosition-zoomStart) * 479) / zoomWidth;
		}


		uint32_t offset = ((float)zoomPosition/MAX_16BIT) * mtProject.sampleBank.sample[editorInstrument->sampleIndex].length;

		sampleData = mtProject.sampleBank.sample[editorInstrument->sampleIndex].address + offset;

		resolution = (((float)zoomWidth/MAX_16BIT) * mtProject.sampleBank.sample[editorInstrument->sampleIndex].length ) / 480;


		Serial.print(zoomValue);
		Serial.print("   ");
		Serial.print(zoomStart);
		Serial.print("   ");
		Serial.print(zoomEnd);
		Serial.print("   ");

		Serial.println();

	}
	else
	{

		offset_pixel = 0;
		zoomWidth = MAX_16BIT;
		zoomStart = 0;
		zoomEnd = MAX_16BIT;
		sampleData = mtProject.sampleBank.sample[editorInstrument->sampleIndex].address;
		resolution = mtProject.sampleBank.sample[editorInstrument->sampleIndex].length / 480;
	}


// TODO:
	// do zrobienia zooomowanie


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

			up = up/1000;
			low = low/1000;

			spectrum.upperData[i] =  up;
			spectrum.lowerData[i] = low;
		}
	}

	up = 0;
	low = 0;
	step = 0;


	for(uint16_t i = offset_pixel; i < 480; i++)
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

	if(resolution <= 1) spectrum.spectrumType = 1;
	else spectrum.spectrumType = 0;

}


void cMtInstrumentEditor::processPoints()
{

	spectrum.pointsType = editorInstrument->playMode;

	if(editorInstrument->startPoint >= zoomStart && editorInstrument->startPoint <= zoomEnd)
	{
		spectrum.startPoint = ((editorInstrument->startPoint-zoomStart) * 479) / zoomWidth;
	}
	else spectrum.startPoint = -1;

	if(editorInstrument->endPoint >= zoomStart && editorInstrument->endPoint <= zoomEnd)
	{
		spectrum.endPoint = ((editorInstrument->endPoint-zoomStart) * 479) / zoomWidth;
	}
	else spectrum.endPoint = -1;

	if(editorInstrument->loopPoint1 >= zoomStart && editorInstrument->loopPoint1 <= zoomEnd)
	{
		spectrum.loopPoint1 = ((editorInstrument->loopPoint1-zoomStart) * 479) / zoomWidth;
	}
	else spectrum.loopPoint1 = -1;

	if(editorInstrument->loopPoint2 >= zoomStart && editorInstrument->loopPoint2 <= zoomEnd)
	{
		spectrum.loopPoint2 = ((editorInstrument->loopPoint2-zoomStart) * 479) / zoomWidth;
	}
	else spectrum.loopPoint2 = -1;

}

void cMtInstrumentEditor::processLabels()
{

	for(uint8_t i = 0; i < 5; i++)
	{
		switch(buttonFunction[i])
		{
		case mtInstrumentEditorButtonFunctionPlayMode:
			setButtonLabel(mtInstrumentEditorButtonFunctionPlayMode, (char*)&playModeFunctLabels[editorInstrument->playMode][0]);
		break;

		case mtInstrumentEditorButtonFunctionChangeGlideNote:
			setButtonLabel(mtInstrumentEditorButtonFunctionChangeGlideNote, (char*)&glidePreviewDifLabels[glidePreviewDif][0]);
		break;

		case mtInstrumentEditorButtonFunctionFilterType:
			if(!editorInstrument->filterEnable) setButtonLabel(mtInstrumentEditorButtonFunctionFilterType, (char*)&filterTypeLabels[0][0]);
			else setButtonLabel(mtInstrumentEditorButtonFunctionFilterType, (char*)&filterTypeLabels[editorInstrument->filterType+1][0]);
		break;

		case mtInstrumentEditorButtonFunctionEnvelopeEnable:
			setButtonLabel(mtInstrumentEditorButtonFunctionEnvelopeEnable, (char*)&envelopeEnableLabels[editorInstrument->envelope[envelopeType].enable][0]);
		break;

		default: break;
		}
	}

}

void cMtInstrumentEditor::processParameters()
{
	// odswieza wartosci tylko te ktore sa aktulanie wyswietlana (5 wartosci w valuesParameters[5])
	for(uint8_t i = 0; i < 5; i++)
	{
		switch(valuesParameters[i])
		{
			case mtInstrumentEditorValueNone:
			{
				values.type[i] = mtInstrumentEditorValuesTypes[mtInstrumentEditorValueNone];
				values.value[i] = 0;
				break;
			}
			case mtInstrumentEditorValuePanning:
			{
				values.type[i] = mtInstrumentEditorValuesTypes[mtInstrumentEditorValuePanning];
				values.value[i] = editorInstrument->panning;
				break;
			}
			case mtInstrumentEditorValueGlide:
			{
				values.type[i] = mtInstrumentEditorValuesTypes[mtInstrumentEditorValueGlide];
				values.value[i] =  (editorInstrument->glide*100)/GLIDE_MAX;
				break;
			}
			case mtInstrumentEditorValueFilter:
			{
				values.type[i] = mtInstrumentEditorValuesTypes[mtInstrumentEditorValueFilter];
				values.value[i] =  (editorInstrument->cutOff*100);
				break;
			}
			case mtInstrumentEditorValueResonance:
			{
				if(editorInstrument->resonance < RESONANCE_MIN) editorInstrument->resonance = RESONANCE_MIN;
				values.type[i] = mtInstrumentEditorValuesTypes[mtInstrumentEditorValueResonance];
				values.value[i] =  ((editorInstrument->resonance - RESONANCE_MIN)/(RESONANCE_MAX-RESONANCE_MIN))*100;
				break;
			}
			case mtInstrumentEditorValue2:
			{

				break;
			}
			default:
			{
				values.type[i] = mtInstrumentEditorValuesTypes[mtInstrumentEditorValueNone];
				values.value[i] = 0;
				break;
			}
		}
	}
}

void cMtInstrumentEditor::processEnvelopes()
{

	envelope.type = envelopeType;
	envelope.attack  = (editorInstrument->envelope[envelopeType].attack*100)/ATTACK_MAX;
	envelope.decay   = (editorInstrument->envelope[envelopeType].decay*100)/DECAY_MAX;
	envelope.sustain = (editorInstrument->envelope[envelopeType].sustain*100);
	envelope.release = (editorInstrument->envelope[envelopeType].release*100)/RELEASE_MAX;

	envelope.amount  = (editorInstrument->envelope[envelopeType].amount*100);


	for(uint8_t i = 0; i < INSTRUMEN_ENVELOPES_MAX; i++)
	{
		envelope.names[i] = (char*)&envelopeTypeNames[i][0];
	}
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtInstrumentEditor::updateParameters()
{
	setParameter(0, mtInstrumentEditorValueNone);
	setParameter(1, mtInstrumentEditorValueNone);
	setParameter(2, mtInstrumentEditorValueNone);
	setParameter(3, mtInstrumentEditorValueNone);
	setParameter(4, mtInstrumentEditorValueNone);

	if(parametersEnabled)
	{
		setParameter(0, mtInstrumentEditorValuePanning);
		setParameter(1, mtInstrumentEditorValueGlide);


		setParameter(3, mtInstrumentEditorValueResonance);
		setParameter(4, mtInstrumentEditorValueFilter);
	}


}

void cMtInstrumentEditor::setParameter(uint8_t number, uint8_t param)
{
	valuesParameters[number] = param;
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

	mtDisplay.changeButtonsLabels(buttonLabels);
}


void cMtInstrumentEditor::updateButtonsFunctions()
{
	setButtonFunction(0, mtInstrumentEditorButtonFunctionNone);
	setButtonFunction(1, mtInstrumentEditorButtonFunctionNone);
	setButtonFunction(2, mtInstrumentEditorButtonFunctionNone);
	setButtonFunction(3, mtInstrumentEditorButtonFunctionNone);
	setButtonFunction(4, mtInstrumentEditorButtonFunctionNone);


	if(parametersEnabled)
	{
		setButtonFunction(0, mtInstrumentEditorButtonFunctionPlay);
		setButtonFunction(1, mtInstrumentEditorButtonFunctionChangeGlideNote);
		setButtonFunction(2, mtInstrumentEditorButtonFunctionParameters);

		setButtonFunction(4, mtInstrumentEditorButtonFunctionFilterType);
	}
	else if(envelopesEnabled)
	{
		setButtonFunction(0, mtInstrumentEditorButtonFunctionPlay);
		setButtonFunction(1, mtInstrumentEditorButtonFunctionEnvelopeAmp);
		setButtonFunction(2, mtInstrumentEditorButtonFunctionEnvelopeFilter);
		if(envelopeType>envAmp) setButtonFunction(3, mtInstrumentEditorButtonFunctionEnvelopeEnable);
		setButtonFunction(4, mtInstrumentEditorButtonFunctionEnvelopes);
	}
	else
	{
		setButtonFunction(0, mtInstrumentEditorButtonFunctionInstrumentList);
		setButtonFunction(1, mtInstrumentEditorButtonFunctionPlayMode);
		setButtonFunction(2, mtInstrumentEditorButtonFunctionParameters);
		setButtonFunction(3, mtInstrumentEditorButtonFunctionSampleList);
		setButtonFunction(4, mtInstrumentEditorButtonFunctionEnvelopes);
	}


	buttonLabels[0] = (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[0]][0];
	buttonLabels[1] = (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[1]][0];
	buttonLabels[2] = (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[2]][0];
	buttonLabels[3] = (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[3]][0];
	buttonLabels[4] = (char *)&mtInstrumentEditorButtonsLabels[buttonFunction[4]][0];

	mtDisplay.changeButtonsLabels(buttonLabels);
}


void cMtInstrumentEditor::setButtonFunction(uint8_t number, uint8_t function)
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

	mtDisplay.changePotsLabels(potsLabels);
}



void cMtInstrumentEditor::updatePotsFunctions()
{
	setPotFunction(0, mtInstrumentEditorPotFunctionNone);
	setPotFunction(1, mtInstrumentEditorPotFunctionNone);
	setPotFunction(2, mtInstrumentEditorPotFunctionNone);
	setPotFunction(3, mtInstrumentEditorPotFunctionNone);
	setPotFunction(4, mtInstrumentEditorPotFunctionNone);


	if(parametersEnabled)
	{
		setPotFunction(0, mtInstrumentEditorPotFunctionPanning);
		setPotFunction(1, mtInstrumentEditorPotFunctionGlide);

		setPotFunction(3, mtInstrumentEditorPotFunctionResonance);
		setPotFunction(4, mtInstrumentEditorPotFunctionFilter);
	}
	else if(envelopesEnabled)
	{
		setPotFunction(0, mtInstrumentEditorPotFunctionAmount);
		setPotFunction(1, mtInstrumentEditorPotFunctionAttack);
		setPotFunction(2, mtInstrumentEditorPotFunctionDecay);
		setPotFunction(3, mtInstrumentEditorPotFunctionSustaion);
		setPotFunction(4, mtInstrumentEditorPotFunctionRelease);
	}
	else
	{
		setPotFunction(0, mtInstrumentEditorPotFunctionStartPoint);
		setPotFunction(2, mtInstrumentEditorPotFunctionViewZoom);
		setPotFunction(4, mtInstrumentEditorPotFunctionEndPoint);

		if(editorInstrument->playMode >= loopForward)
		{
			setPotFunction(1, mtInstrumentEditorPotFunctionLoopPoint1);
			setPotFunction(3, mtInstrumentEditorPotFunctionLoopPoint2);
		}
		if(sampleListEnabled) setPotFunction(3, mtInstrumentEditorPotFunctionSampleSelect);
		if(instrumentListEnabled) setPotFunction(0, mtInstrumentEditorPotFunctionInstrumentSelect);
		//setPotFunction(4, mtInstrumentEditorPotFunctionViewZoom);
	}



	potsLabels[0] = (char *)&mtInstrumentEditorPotsLabels[potFunction[0]][0];
	potsLabels[1] = (char *)&mtInstrumentEditorPotsLabels[potFunction[1]][0];
	potsLabels[2] = (char *)&mtInstrumentEditorPotsLabels[potFunction[2]][0];
	potsLabels[3] = (char *)&mtInstrumentEditorPotsLabels[potFunction[3]][0];
	potsLabels[4] = (char *)&mtInstrumentEditorPotsLabels[potFunction[4]][0];

	mtDisplay.changePotsLabels(potsLabels);
}


void cMtInstrumentEditor::setPotFunction(uint8_t number, uint8_t function)
{
	potFunction[number] = function;
	AnalogInputs.setPotResolution(number, mtInstrumentEditorPotsFuncRes[function]);
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// instrument modification

void cMtInstrumentEditor::modStartPoint(int16_t value)
{
	// obliczenie kroku przesuniecia w zaleznosci od ilosci widzianych probek na wyswietlaczu
	uint16_t move_step = zoomWidth / 480;
	uint16_t dif;
	value = value * move_step;

	if(editorInstrument->startPoint + value < SAMPLE_POINT_POS_MIN) editorInstrument->startPoint  = 0;
	else if(editorInstrument->startPoint + value > SAMPLE_POINT_POS_MAX ) editorInstrument->startPoint  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->startPoint += value;

	if(editorInstrument->startPoint > editorInstrument->endPoint) editorInstrument->startPoint = editorInstrument->endPoint-1;
	if(editorInstrument->startPoint > editorInstrument->loopPoint1)
	{
		dif = editorInstrument->loopPoint2 - editorInstrument->loopPoint1;
		editorInstrument->loopPoint1 = editorInstrument->startPoint;

		if(editorInstrument->loopPoint1 + dif > editorInstrument->endPoint)
		{
			editorInstrument->loopPoint2 = editorInstrument->endPoint;
			editorInstrument->loopPoint1 = editorInstrument->loopPoint2 - dif;
			editorInstrument->startPoint = editorInstrument->loopPoint1;
		}
		else
		{
			editorInstrument->loopPoint2 = editorInstrument->loopPoint1 + dif;
		}
	}

	// odswiez spektrum tylko jesli: zoom wiekszy niz 1, ostatnio modyfikowany inny punkt, punkt jest poza widocznym obszarem
	if(zoomValue > 1 && lastChangedPoint != 1
		&& (editorInstrument->startPoint < zoomStart || editorInstrument->startPoint > zoomEnd)) spectrumChanged = 1;

	lastChangedPoint = 1;
	pointsChanged = 1;
}

void cMtInstrumentEditor::modEndPoint(int16_t value)
{
	uint16_t move_step = zoomWidth / 480;
	uint16_t dif;
	value = value * move_step;

	if(editorInstrument->endPoint + value < SAMPLE_POINT_POS_MIN) editorInstrument->endPoint  = 0;
	else if(editorInstrument->endPoint + value > SAMPLE_POINT_POS_MAX ) editorInstrument->endPoint  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->endPoint += value;

	if(editorInstrument->endPoint < editorInstrument->startPoint) editorInstrument->endPoint = editorInstrument->startPoint+1;
	if(editorInstrument->endPoint < editorInstrument->loopPoint2)
	{
		dif = editorInstrument->loopPoint2 - editorInstrument->loopPoint1;

		editorInstrument->loopPoint2 = editorInstrument->endPoint;

		if(editorInstrument->loopPoint2 - dif < editorInstrument->startPoint)
		{
			editorInstrument->loopPoint1 = editorInstrument->startPoint;
			editorInstrument->loopPoint2 = editorInstrument->loopPoint1 + dif;
			editorInstrument->endPoint = editorInstrument->loopPoint2;
		}
		else
		{
			editorInstrument->loopPoint1 = editorInstrument->loopPoint2 - dif;
		}
	}

	if(zoomValue > 1 && lastChangedPoint != 2
			&& (editorInstrument->endPoint < zoomStart || editorInstrument->endPoint > zoomEnd)) spectrumChanged = 1;

	lastChangedPoint = 2;
	pointsChanged = 1;
}

void cMtInstrumentEditor::modLoopPoint1(int16_t value)
{
	uint16_t move_step = zoomWidth / 480;
	value = value * move_step;

	if(editorInstrument->loopPoint1 + value < SAMPLE_POINT_POS_MIN) editorInstrument->loopPoint1  = 0;
	else if(editorInstrument->loopPoint1 + value > SAMPLE_POINT_POS_MAX ) editorInstrument->loopPoint1  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->loopPoint1 += value;

	if(editorInstrument->loopPoint1 < editorInstrument->startPoint) editorInstrument->loopPoint1 = editorInstrument->startPoint;
	if(editorInstrument->loopPoint1 > editorInstrument->loopPoint2) editorInstrument->loopPoint1 = editorInstrument->loopPoint2-1;

	if(zoomValue > 1 && lastChangedPoint != 3
			&& (editorInstrument->loopPoint1 < zoomStart || editorInstrument->loopPoint1 > zoomEnd)) spectrumChanged = 1;

	lastChangedPoint = 3;
	pointsChanged = 1;
}

void cMtInstrumentEditor::modLoopPoint2(int16_t value)
{
	uint16_t move_step = zoomWidth / 480;
	value = value * move_step;

	if(editorInstrument->loopPoint2 + value < SAMPLE_POINT_POS_MIN) editorInstrument->loopPoint2  = 0;
	else if(editorInstrument->loopPoint2 + value > SAMPLE_POINT_POS_MAX ) editorInstrument->loopPoint2  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->loopPoint2 += value;

	if(editorInstrument->loopPoint2 > editorInstrument->endPoint) editorInstrument->loopPoint2 = editorInstrument->endPoint;
	if(editorInstrument->loopPoint2 < editorInstrument->loopPoint1) editorInstrument->loopPoint2 = editorInstrument->loopPoint1+1;

	if(zoomValue > 1 && lastChangedPoint != 4
			&& (editorInstrument->loopPoint2 < zoomStart || editorInstrument->loopPoint2 > zoomEnd)) spectrumChanged = 1;

	lastChangedPoint = 4;
	pointsChanged = 1;
}

void cMtInstrumentEditor::changePlayMode(uint8_t value)
{
	if(value == 1)
	{

		if(editorInstrument->playMode < playModeMax-1 ) editorInstrument->playMode++;
		else editorInstrument->playMode = 0;

		pointsChanged = 1;
		labelsChanged = 1;
	}
}


void cMtInstrumentEditor::changePanning(uint8_t pot, int16_t value)
{
	if(editorInstrument->panning + value < PANNING_MIN) editorInstrument->panning = PANNING_MIN;
	else if(editorInstrument->panning + value > PANNING_MAX ) editorInstrument->panning = PANNING_MAX;
	else editorInstrument->panning += value;

	parametersChanged = 1;
}

void cMtInstrumentEditor::changeGlide(int16_t value)
{
	value = value * 100;

	if(editorInstrument->glide + value < GLIDE_MIN) editorInstrument->glide = GLIDE_MIN;
	else if(editorInstrument->glide + value > GLIDE_MAX ) editorInstrument->glide = GLIDE_MAX;
	else editorInstrument->glide += value;


	parametersChanged = 1;
}

void cMtInstrumentEditor::changeGlideNote(uint8_t value)
{
	if(value == 1)
	{
		glidePreviewDif++;
		if(glidePreviewDif > 3) glidePreviewDif = 0;
		labelsChanged = 1;
	}
}

void cMtInstrumentEditor::changeFilter(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->cutOff + fVal < MIN_CUTOFF) editorInstrument->cutOff = MIN_CUTOFF;
	else if(editorInstrument->cutOff + fVal > MAX_CUTOFF ) editorInstrument->cutOff = MAX_CUTOFF;
	else editorInstrument->cutOff += fVal;

	parametersChanged = 1;
}

void cMtInstrumentEditor::changeFilterType(uint8_t value)
{
	if(value == 1)
	{
		if(!editorInstrument->filterEnable)
		{
			editorInstrument->filterEnable = 1;
			editorInstrument->filterType = lowPass;
		}
		else if(editorInstrument->filterType == lowPass)
		{
			editorInstrument->filterEnable = 1;
			editorInstrument->filterType = highPass;
		}
		else if(editorInstrument->filterType == highPass)
		{
			editorInstrument->filterEnable = 1;
			editorInstrument->filterType = bandPass;
		}
		else if(editorInstrument->filterType == bandPass)
		{
			editorInstrument->filterEnable = 0;
			editorInstrument->filterType = lowPass;
		}


		labelsChanged = 1;
	}
}

void cMtInstrumentEditor::changeResonance(int16_t value)
{
	float fVal = value * 0.05;

	if(editorInstrument->resonance + fVal < RESONANCE_MIN) editorInstrument->resonance = RESONANCE_MIN;
	else if(editorInstrument->resonance + fVal > RESONANCE_MAX ) editorInstrument->resonance = RESONANCE_MAX;
	else editorInstrument->resonance += fVal;

	parametersChanged = 1;
}

void cMtInstrumentEditor::changeEnvelopeType(uint8_t value)
{
	if(value == 1)
	{
		envelopeType++;
		if(envelopeType > envelopeTypeMax - 1) envelopeType = 0;

		envelopesChanged = 1;
	}
}

void cMtInstrumentEditor::setEnvelopeTypeAmp(uint8_t value)
{
	if(value == 1)
	{
		envelopeType = envelopeTypeAmp;

		envelopesChanged = 1;
		labelsChanged = 2;
	}
}

void cMtInstrumentEditor::setEnvelopeTypeFilter(uint8_t value)
{
	if(value == 1)
	{
		envelopeType = envelopeTypeFilter;

		envelopesChanged = 1;
		labelsChanged = 2;
	}
}

void cMtInstrumentEditor::setEnvelopeEnable(uint8_t value)
{
	if(value == 1)
	{
		editorInstrument->envelope[envelopeType].enable = !editorInstrument->envelope[envelopeType].enable;
		labelsChanged = 1;
	}
}

void cMtInstrumentEditor::showParameters(uint8_t value)
{
	if(value == 1)
	{
		if(parametersEnabled)
		{
			parametersEnabled = 0;
			spectrumChanged   = 2;
		}
		else
		{
			parametersEnabled = 1;
			parametersChanged = 2;
		}

		if(sampleListEnabled)
		{
			sampleListEnabled = 0;
			sampleListChanged = 1;
		}
	}
}

void cMtInstrumentEditor::showEnvelopes(uint8_t value)
{
	if(value == 1)
	{
		if(envelopesEnabled)
		{
			envelopesEnabled = 0;
			spectrumChanged   = 2;
		}
		else
		{
			envelopesEnabled = 1;
			envelopesChanged = 2;
		}

		if(sampleListEnabled)
		{
			sampleListEnabled = 0;
			sampleListChanged = 1;
		}
	}
}

void cMtInstrumentEditor::changeAttack(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[envelopeType].attack + value < 0) editorInstrument->envelope[envelopeType].attack = 0;
	else if(editorInstrument->envelope[envelopeType].attack + value > ATTACK_MAX ) editorInstrument->envelope[envelopeType].attack = ATTACK_MAX;
	else editorInstrument->envelope[envelopeType].attack += value;

	envelopesChanged = 1;
}

void cMtInstrumentEditor::changeDecay(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[envelopeType].decay + value < 0) editorInstrument->envelope[envelopeType].decay = 0;
	else if(editorInstrument->envelope[envelopeType].decay + value > DECAY_MAX ) editorInstrument->envelope[envelopeType].decay = DECAY_MAX;
	else editorInstrument->envelope[envelopeType].decay += value;

	envelopesChanged = 1;
}

void cMtInstrumentEditor::changeSustain(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->envelope[envelopeType].sustain + fVal < 0) editorInstrument->envelope[envelopeType].sustain = 0;
	else if(editorInstrument->envelope[envelopeType].sustain + fVal > SUSTAIN_MAX ) editorInstrument->envelope[envelopeType].sustain = SUSTAIN_MAX;
	else editorInstrument->envelope[envelopeType].sustain += fVal;

	envelopesChanged = 1;
}

void cMtInstrumentEditor::changeRelease(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[envelopeType].release + value < 0) editorInstrument->envelope[envelopeType].release = 0;
	else if(editorInstrument->envelope[envelopeType].release + value > RELEASE_MAX ) editorInstrument->envelope[envelopeType].release = RELEASE_MAX;
	else editorInstrument->envelope[envelopeType].release += value;

	envelopesChanged = 1;
}

void cMtInstrumentEditor::changeAmount(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->envelope[envelopeType].amount + fVal < 0) editorInstrument->envelope[envelopeType].amount = 0;
	else if(editorInstrument->envelope[envelopeType].amount + fVal > AMOUNT_MAX ) editorInstrument->envelope[envelopeType].amount = AMOUNT_MAX;
	else editorInstrument->envelope[envelopeType].amount += fVal;

	envelopesChanged = 1;
}


void cMtInstrumentEditor::selectInstrument(int16_t value)
{
	if(openedInstrumentIndex + value < 0) openedInstrumentIndex  = 0;
	else if(openedInstrumentIndex + value > INSTRUMENTS_MAX-1) openedInstrumentIndex  = INSTRUMENTS_MAX-1;
	else openedInstrumentIndex += value;

	startExisting(openedInstrumentIndex);

	mtDisplay.changeList(0, openedInstrumentIndex);
}


void cMtInstrumentEditor::selectSample(int16_t value)
{
	if(editorInstrument->sampleIndex + value < 0) editorInstrument->sampleIndex  = 0;
	else if(editorInstrument->sampleIndex + value > SAMPLES_MAX-1) editorInstrument->sampleIndex  = SAMPLES_MAX-1;
	else editorInstrument->sampleIndex += value;

	mtDisplay.changeList(3, editorInstrument->sampleIndex);

	lastChangedPoint = 0;
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


void cMtInstrumentEditor::showInstrumentList(uint8_t value)
{
	if(value == 1)
	{
		if(instrumentListEnabled)
		{
			instrumentListEnabled = 0;
			instrumentListChanged = 1;
		}
		else
		{
			instrumentListEnabled = 1;
			instrumentListChanged = 2;
		}
	}
}


void cMtInstrumentEditor::changeView(int16_t value)
{

}

void cMtInstrumentEditor::changeZoom(int16_t value)
{
	//if(zoomValue > 1) lastChangedPoint = 0;

	float fVal = value * ZOOM_FACTOR;

	if(zoomValue + fVal < ZOOM_MIN) zoomValue  = ZOOM_MIN;
	else if(zoomValue + fVal > ZOOM_MAX ) zoomValue  = ZOOM_MAX;
	else zoomValue += fVal;


	spectrumChanged = 1;
	pointsChanged = 1;
}

void cMtInstrumentEditor::play(uint8_t value)
{
	if(value == 1)
	{
		isPlayingSample = 1;
		if(editorInstrument->glide > 0)
		{
			switch(	glidePreviewDif)
			{
				case 0: playNote = 24;	break;
				case 1: playNote = (playNote == 24)? 25 : 24; 	break;
				case 2: playNote = (playNote == 24)? 36 : 24; 	break;
				case 3: playNote = (playNote == 24)? 47 : 24; 	break;
			}
		}

		instrumentPlayer[0].noteOn(openedInstrumentIndex, playNote, 100);
	}
	else if(value == 0)
	{
		if(isPlayingSample) instrumentPlayer[0].noteOff();
		isPlayingSample = 0;
	}
}

void cMtInstrumentEditor::stopPlaying(uint8_t value)
{

	if(isPlayingSample) instrumentPlayer[0].noteOff();


	isPlayingSample = 0;

}
