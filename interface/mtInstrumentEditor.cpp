#include "mtInstrumentEditor.h"

#include "mtDisplay.h"
#include "mtPadBoard.h"
#include "AnalogInputs.h"
#include "sdram.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"
#include "mtFileManager.h"



#include "mtInterfaceDefs.h"


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
		//pointsChanged = 2;
		labelsChanged = 2;
		lastChangedPoint = 0;
		envelopesEnabled = 0;
		parametersEnabled = 0;
		sampleListEnabled = 0;
		instrumentListEnabled = 0;
		//instrumentPlayer[0].clean();
	}
	//-----------------------------------------------------
	if(labelsChanged)
	{
		if(labelsChanged == 2)
		{
			mtDisplay.setPotsLabels(1);
			mtDisplay.setButtonsLabels(1);
		}

		labelsChanged = 0;

		updateButtonsFunctions();
		updatePotsFunctions();

		processLabels();
	}
	//-----------------------------------------------------
	if(spectrumChanged && mtProject.instruments_count > 0 && mtProject.sampleBank.samples_count > 0)
	{
		if(spectrumChanged == 2)
		{
			mtDisplay.setSpectrum(1);
			mtDisplay.setSpectrumPoints(1);
			mtDisplay.setValue(0);
			mtDisplay.setEnvelopes(0);

			refreshInstrumentEditor = 1;
			labelsChanged = 1;
		}

		if(spectrumDrawDelay > SPECTRUM_DRAW_DELAY_VALUE)
		{
			spectrumChanged = 0;
			if(mtProject.sampleBank.sample[editorInstrument->sampleIndex].type == mtSampleTypeWavetable)
			{
				mtDisplay.setSpectrumPoints(0);
				instrumentPlayer[0].modWavetableWindow(editorInstrument->wavetableCurrentWindow);
			}
			else
			{
				mtDisplay.setSpectrumPoints(1);
				pointsChanged = 1;
			}

			processSpectrum();
			mtDisplay.changeSpectrum(&spectrum);
		}
		else
		{
			refreshInstrumentEditor = 1;
		}
	}
	//-----------------------------------------------------
	if(pointsChanged)
	{
		pointsChanged = 0;

		processPoints();
		mtDisplay.changeSpectrumPoints(&spectrum);
	}
	//-----------------------------------------------------
	if(sampleListChanged)
	{
		labelsChanged = 1;
		refreshInstrumentEditor = 1;

		if(!sampleListEnabled)
		{
			sampleListChanged = 0;
			mtDisplay.setList(sample_list_pos, 0, 0, 0, 0, 0);
			return;
		}

		if(sampleListChanged == 2) // pokaz liste
		{
			if(mtProject.sampleBank.samples_count > 0)
			{
				//przetworz tablice adresow nazw sampli na podstawie nazw z banku sampli
				for(uint8_t i = 0; i < SAMPLES_MAX; i++)
				{
					sampleNames[i] = mtProject.sampleBank.sample[i].file_name;
				}
				mtDisplay.setList(sample_list_pos, sample_list_pos, 1, editorInstrument->sampleIndex, sampleNames, mtProject.sampleBank.samples_count );
			}
			else
			{
				sampleNames[0] = buttonFunctionLabels[0]; // przypisanie pustego
				mtDisplay.setList(sample_list_pos, sample_list_pos, 1, 0, sampleNames, 1 );
			}
		}

		sampleListChanged = 0;
	}
	//---------------------------------------------------------
	if(instrumentListChanged)
	{
		labelsChanged = 1;
		refreshInstrumentEditor = 1;

		if(!instrumentListEnabled)
		{
			instrumentListChanged = 0;
			mtDisplay.setList(instrument_list_pos, 0, 0, 0, 0, 0);
			return;
		}

		if(instrumentListChanged == 2) // pokaz liste
		{
			if(instrumentListMode == 0 && mtProject.instruments_count > 0)
			{
				mtDisplay.setList(instrument_list_pos, instrument_list_pos, 1, openedInstrFromActive, instrumentNames, mtProject.instruments_count);
			}
			else if(instrumentListMode == 1 && inActiveInstrumentsCount > 0)
			{
				mtDisplay.setList(instrument_list_pos, instrument_list_pos, 1, inActiveInstrumentIndex, instrumentNames, inActiveInstrumentsCount);
			}
			else
			{
				instrumentNames[0] = buttonFunctionLabels[0]; // przypisanie pustego
				mtDisplay.setList(instrument_list_pos, instrument_list_pos, 1, 0, instrumentNames, 1);
			}
		}

		instrumentListChanged = 0;
	}
	//---------------------------------------------------------
	if(parametersChanged)
	{
		if(parametersChanged == 2)
		{
			mtDisplay.setSpectrum(0);
			mtDisplay.setSpectrumPoints(0);
			mtDisplay.setEnvelopes(0);
			mtDisplay.setValue(1);
			mtDisplay.setList(instrument_list_pos, 0, 0, 0, 0, 0);
			mtDisplay.setList(sample_list_pos, 0, 0, 0, 0, 0);

			updateParameters();
			labelsChanged = 1;
			refreshInstrumentEditor = 1;
		}

		parametersChanged = 0;
		processParameters();

		mtDisplay.changeValues(&values);
	}
	//---------------------------------------------------------
	if(envelopesChanged)
	{
		if(envelopesChanged == 2)
		{
			mtDisplay.setSpectrum(0);
			mtDisplay.setSpectrumPoints(0);
			mtDisplay.setValue(0);
			mtDisplay.setEnvelopes(1);
			mtDisplay.setList(instrument_list_pos, 0, 0, 0, 0, 0);
			mtDisplay.setList(sample_list_pos, 0, 0, 0, 0, 0);

			labelsChanged = 1;
			refreshInstrumentEditor = 1;
		}
		envelopesChanged = 0;

		processEnvelopes();

		mtDisplay.changeEnvelopes(&envelope);
	}


}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtInstrumentEditor::startExisting(uint8_t instrumentIndex)
{

	mtPadBoard.setPadNotes(mtProject.values.padBoardScale,
			mtProject.values.padBoardNoteOffset,
			mtProject.values.padBoardRootNote);

	mtPadBoard.configureInstrumentPlayer(mtProject.values.padBoardMaxVoices);

	if(mtProject.sampleBank.samples_count == 0)
	{
		strcpy(buttonFunctionLabels[buttonFunctSampleList], "No samples");
	}
	if(mtProject.instruments_count == 0)
	{
		strcpy(buttonFunctionLabels[buttonFunctInstrumentList], "No instruments");

		//askToCreateNewInstr();

		instrumentEditorModeStart = 1;
		refreshInstrumentEditor = 1;
		return;
	}


	// utworzenie listy tylko aktywnych instryumentow w otwartym projeckie
	openedInstrumentIndex = -1;
	openedInstrFromActive = 0;
	uint8_t activeInstr = 0;
	for(uint8_t i = 0; i < INSTRUMENTS_MAX; i++)
	{
		if(mtProject.instrument[i].isActive)
		{
			if(openedInstrumentIndex == -1) openedInstrumentIndex = i; // tymczasowo przypisz pierwszy aktywny jako otwarty
			instrumentNames[activeInstr] = mtProject.instrument[i].name;
			activeInstruments[activeInstr] = i;
			if(i == instrumentIndex) // jesli znaleziono wybrany jako aktywny
			{
				openedInstrFromActive = activeInstr; // zapisz jego index z listy tylko aktywnych
				openedInstrumentIndex = i;	// zapisz otwarty jako wybrany
			}
			activeInstr++;
		}
	}



	if(openedInstrumentIndex == -1) // jesli wybrano nieaktywny i jest brak aktywnych - niemozliwe jesli (count > 0)
	{

	}
	else if(openedInstrumentIndex != instrumentIndex) // jesli wybrany nie jest aktywny
	{
		// przypisano juz wczesniej pierwszy aktywny jako otwierany (openedInstrumentIndex = i;	// zapisz otwarty jako wybrany)

	}

	//openedInstrumentIndex = openedInstrFromActive;
	editorInstrument = &mtProject.instrument[openedInstrumentIndex];






	strcpy(buttonFunctionLabels[buttonFunctInstrumentList], "Instrument ");
	strcat(buttonFunctionLabels[buttonFunctInstrumentList], mtProject.instrument[openedInstrumentIndex].name);

	if(mtProject.sampleBank.samples_count > 0)
	{
		int8_t sample_index = mtProject.instrument[openedInstrumentIndex].sampleIndex;
		if(sample_index >= 0)
		{
			strcpy(buttonFunctionLabels[buttonFunctSampleList], mtProject.sampleBank.sample[sample_index].file_name);
		}
	}



	instrumentEditorModeStart = 1;
	refreshInstrumentEditor = 1;
}


void cMtInstrumentEditor::stop()
{
	mtDisplay.setSpectrum(0);
	mtDisplay.setSpectrumPoints(0);
	mtDisplay.setValue(0);
	mtDisplay.setEnvelopes(0);
	mtDisplay.setList(0, 0, 0, 0, 0, 0);
	mtDisplay.setList(1, 0, 0, 0, 0, 0);
	mtDisplay.setList(2, 0, 0, 0, 0, 0);
	mtDisplay.setList(3, 0, 0, 0, 0, 0);
	mtDisplay.setList(4, 0, 0, 0, 0, 0);

	envelopesEnabled = 0;
	parametersEnabled = 0;
	sampleListEnabled = 0;
	instrumentListEnabled = 0;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
uint8_t cMtInstrumentEditor::padsChange(uint8_t type, uint8_t n, uint8_t velo)
{

	if(n == interfacePadPlay)
	{
		play(type);
	}
	else if(n > interfacePadKeybord0)
	{
	/*
  		if(parametersEnabled) {}
		else if(envelopesEnabled) {}
		else
	*/
			playInstrumentByPad(type, n-interfacePadKeybord0, -1);
	}

	if(type == 1)
	{

		if(n == interfacePadProjectEditor)
		{
			stop();
			eventFunct(mtInstrumentEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadSampleBank)
		{
			stop();
			eventFunct(mtInstrumentEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadConfig)
		{
			stop();
			eventFunct(mtInstrumentEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadSettings)
		{
			stop();
			eventFunct(mtInstrumentEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadStop)
		{
			eventFunct(mtInstrumentEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadParams)
		{
			showParameters();
		}
		else if(n == interfacePadEnvelopes)
		{
			showEnvelopes();
		}
		else if(n == interfacePadInstrumentEditor)
		{
			showSpectrum();
		}


	}


	return 0;
}

void cMtInstrumentEditor::buttonChange(uint8_t button, uint8_t value)
{
	if(value == 1)
	{
		switch(buttonFunctions[button])
		{
		case buttonFunctNone  				: 							break;
		case buttonFunctPlay  				:	play(1);				break;
		case buttonFunctStop  				:	stopPlaying(1);			break;
		case buttonFunctPlayMode  			: 	changePlayMode(1);		break;
		case buttonFunctEnvelopes  			: 	showEnvelopes();		break;
		case buttonFunctInstrumentList 		: 	showInstrumentList(1);	break;
		case buttonFunctSampleList  		: 	showSampleList(1);		break;
		case buttonFunctParameters  		: 	showParameters();		break;
		case buttonFunctChangeGlideNote		: 	changeGlideNote(1);		break;
		case buttonFunctFilterType			: 	changeFilterType(1);	break;
		case buttonFunctEnvelopeType		: 	changeEnvelopeType(1);	break;
		case buttonFunctEnvelopeAmp			: 	setEnvelopeTypeAmp(1);	break;
		case buttonFunctEnvelopeFilter		: 	setEnvelopeTypeFilter(1);break;
		case buttonFunctEnvelopeEnable		: 	setEnvelopeEnable(1);	break;
		case buttonFunctParamsNextPage		:	changeParamsPage(1); 	break;
		case buttonFunctInstrumentAdd		:	addInstrument(1); 	 	break;
		case buttonFunctInstrumentRemove	:	removeInstrument(); 	break;
		case buttonFunctSampleAdd			:	addSample(1); 		 	break;
		case buttonFunctInstrumentCreate	:   createInstrument(1);	break;
		case buttonFunctInstrumentCreateCancel: cancelCreateInstrument(1); break;
		case buttonFunctInstrumentImport	:	importInstrument(1);	break;


		default: break;
		}
	}
	else if(value == 0)
	{
		switch(buttonFunctions[button])
		{
		case buttonFunctPlay  	:	play(0);			break;
		default: break;
		}

	}

	refreshInstrumentEditor = 1;
}


void cMtInstrumentEditor::potChange(uint8_t pot, int16_t value)
{
	switch(potFunctions[pot])
	{
		case potFunctNone				: 	break;
		case potFunctStartPoint			:	modStartPoint(value);		break;
		case potFunctEndPoint			:	modEndPoint(value);			break;
		case potFunctLoopPoint1			:	modLoopPoint1(value);		break;
		case potFunctLoopPoint2			:	modLoopPoint2(value);		break;
		case potFunctInstrumentSelect	:	selectInstrument(value);	break;
		case potFunctSampleSelect		:	selectSample(value);		break;
		case potFunctViewPosition		:	changeView(value);			break;
		case potFunctViewZoom			:	changeZoom(value);			break;
		case potFunctPanning  			: 	changePanning(pot, value);	break;
		case potFunctGlide  			: 	changeGlide(value);			break;
		case potFunctFilter  			: 	changeFilter(value);		break;
		case potFunctAttack				:	changeAttack(value);		break;
		case potFunctDecay  			: 	changeDecay(value);			break;
		case potFunctSustaion  			: 	changeSustain(value);		break;
		case potFunctRelease  			: 	changeRelease(value);		break;
		case potFunctAmount  			: 	changeAmount(value);		break;
		case potFunctResonance  		: 	changeResonance(value);		break;
		case potFunctVolume             :   changeVolume(value);        break;
		case potFunctFinetune           :   changeFinetune(value);      break;
		case potFunctTune               :   changeTune(value);          break;
		case potFunctWavetablePos       :   changeWavetablePos(value);  break;
		case potFunctAddInstrumentSelect:   selectAddInstrument(value); break;
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

void cMtInstrumentEditor::waitToLoadSpectrum()
{
	labelsChanged = 1;
	spectrumChanged = 1;
	spectrumDrawDelay = 0;
	spectrum.spectrumType = 3;
	mtDisplay.changeSpectrum(&spectrum);
}

void cMtInstrumentEditor::processSpectrum()
{
/*	if(SpectrumDrawDelay < SPECTRUM_DRAW_DELAY_VALUE)
	{
		spectrum.spectrumType = 3;
		return;
	}
*/
	// uwaga tu wazna kolejnosc + do sprawdzenia
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
	int16_t * sampleData;

	if(mtProject.sampleBank.sample[editorInstrument->sampleIndex].type == mtSampleTypeWavetable)
	{
		zoomWidth = MAX_16BIT;
		zoomStart = 0;
		zoomValue = 1;
		zoomEnd = MAX_16BIT;
		uint16_t windowSize = mtProject.sampleBank.sample[editorInstrument->sampleIndex].wavetable_window_size;

		sampleData = mtProject.sampleBank.sample[editorInstrument->sampleIndex].address
				+ (mtProject.instrument[openedInstrumentIndex].wavetableCurrentWindow * windowSize);

		float resolution = windowSize / 480.0;

		int16_t up = 0;
		int16_t low = 0;
		float step = 0;

		for(uint16_t i = 0; i < 480; i++)
		{
			low = up = 0; //*(sampleData+step);

			for(uint16_t j = 0; j < resolution; j++)
			{
				int16_t sample = *(sampleData+(uint32_t)step+j);

				if(sample > up)  up = sample;
				else if(sample < low) low = sample;

			}
			step+= resolution;

			up = up/1000;
			low = low/1000;

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
		sampleData = mtProject.sampleBank.sample[editorInstrument->sampleIndex].address;
		resolution = mtProject.sampleBank.sample[editorInstrument->sampleIndex].length / 480;
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
		switch(buttonFunctions[i])
		{
		case buttonFunctPlayMode:
			setButtonLabel(buttonFunctPlayMode, (char*)&playModeFunctLabels[editorInstrument->playMode][0]);
		break;

		case buttonFunctChangeGlideNote:
			setButtonLabel(buttonFunctChangeGlideNote, (char*)&glidePreviewDifLabels[glidePreviewDif][0]);
		break;

		case buttonFunctFilterType:
			if(!editorInstrument->filterEnable) setButtonLabel(buttonFunctFilterType, (char*)&filterTypeLabels[0][0]);
			else setButtonLabel(buttonFunctFilterType, (char*)&filterTypeLabels[editorInstrument->filterType+1][0]);
		break;

		case buttonFunctEnvelopeEnable:
			setButtonLabel(buttonFunctEnvelopeEnable, (char*)&envelopeEnableLabels[editorInstrument->envelope[envelopeType].enable][0]);
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
			case valueNone:
			{
				values.type[i] = valuesTypes[valueNone];
				values.value1[i] = 0;
				break;
			}
			case valuePanning:
			{
				values.type[i] = valuesTypes[valuePanning];
				values.value1[i] = editorInstrument->panning;
				break;
			}
			case valueGlide:
			{
				values.type[i] = valuesTypes[valueGlide];
				values.value1[i] =  (editorInstrument->glide*100)/GLIDE_MAX;
				break;
			}
			case valueFilter:
			{
				values.type[i] = valuesTypes[valueFilter];
				values.value1[i] =  (editorInstrument->cutOff*100);
				break;
			}
			case valueResonance:
			{
				//if(editorInstrument->resonance < RESONANCE_MIN) editorInstrument->resonance = RESONANCE_MIN;
				values.type[i] = valuesTypes[valueResonance];
				values.value1[i] =  ((editorInstrument->resonance - RESONANCE_MIN)/(RESONANCE_MAX-RESONANCE_MIN))*100;
				break;
			}
			case valueVolume:
			{
				values.type[i] = valuesTypes[valueVolume];
				values.value1[i] = editorInstrument->volume;
				break;
			}
			case valueFinetune:
			{
				values.type[i] = valuesTypes[valueFinetune];
				values.value1[i] = editorInstrument->fineTune;
				break;
			}
			case valueTune:
			{
				values.type[i] = valuesTypes[valueTune];
				values.value1[i] = editorInstrument->tune;
				break;
			}
			default:
			{
				values.type[i] = valuesTypes[valueNone];
				values.value1[i] = 0;
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
	setParameter(0, valueNone);
	setParameter(1, valueNone);
	setParameter(2, valueNone);
	setParameter(3, valueNone);
	setParameter(4, valueNone);

	if(parametersEnabled)
	{
		if(parametersPage == 0)
		{
			setParameter(0, valueVolume);
			setParameter(1, valuePanning);

			setParameter(2, valueResonance);
			setParameter(3, valueFilter);


		}
		else if(parametersPage == 1)
		{
			setParameter(0, valueGlide);

			setParameter(2, valueFinetune);
			setParameter(3, valueTune);
		}



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
	buttonFunctionLabels[function][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		buttonFunctionLabels[function][i] = label[i];
		i++;
	}
	buttonFunctionLabels[function][i] = 0;

	mtDisplay.changeButtonsLabels(buttonLabels);
}


void cMtInstrumentEditor::updateButtonsFunctions()
{
	setButtonFunction(0, buttonFunctNone);
	setButtonFunction(1, buttonFunctNone);
	setButtonFunction(2, buttonFunctNone);
	setButtonFunction(3, buttonFunctNone);
	setButtonFunction(4, buttonFunctNone);

	if(parametersEnabled)
	{
		if(parametersPage == 0)
		{
			setButtonFunction(2, buttonFunctParameters);
			setButtonFunction(3, buttonFunctFilterType);
			setButtonFunction(4, buttonFunctParamsNextPage);
		}
		else if(parametersPage == 1)
		{
			//setButtonFunction(0, buttonFunctPlay);
			//setButtonFunction(1, buttonFunctChangeGlideNote);
			//setButtonFunction(2, buttonFunctParameters);
			//setButtonFunction(4, buttonFunctFilterType);
			setButtonFunction(2, buttonFunctParameters);
			setButtonFunction(4, buttonFunctParamsNextPage);
		}
	}
	else if(envelopesEnabled)
	{
		setButtonFunction(0, buttonFunctEnvelopeAmp);
		setButtonFunction(1, buttonFunctEnvelopeFilter);
		setButtonFunction(4, buttonFunctEnvelopeEnable);
	}
	else
	{
		if(instrumentListEnabled)
		{
			if(instrumentListMode == 0)
			{
				setButtonFunction(0, buttonFunctInstrumentList);
				if(mtProject.instruments_count > 0) setButtonFunction(1, buttonFunctSampleList);
				setButtonFunction(2, buttonFunctInstrumentAdd);
				setButtonFunction(3, buttonFunctInstrumentRemove);
				setButtonFunction(4, buttonFunctInstrumentImport);
			}
			else if(instrumentListMode == 1)
			{
				setButtonFunction(0, buttonFunctInstrumentCreate);
				setButtonFunction(1, buttonFunctInstrumentCreateCancel);
			}
		}
		else if(sampleListEnabled)
		{
			setButtonFunction(0, buttonFunctInstrumentList);
			setButtonFunction(1, buttonFunctSampleList);
			setButtonFunction(2, buttonFunctSampleAdd);
		}
		else
		{
			if(mtProject.instruments_count > 0)
			{
				if(mtProject.sampleBank.sample[editorInstrument->sampleIndex].type == mtSampleTypeWaveFile)
				{
					setButtonFunction(4, buttonFunctPlayMode);
				}
				setButtonFunction(1, buttonFunctSampleList);
			}
			setButtonFunction(0, buttonFunctInstrumentList);
		}


		//setButtonFunction(2, buttonFunctParameters);
		//setButtonFunction(3, buttonFunctEnvelopes);
	}


	buttonLabels[0] = (char *)&buttonFunctionLabels[buttonFunctions[0]][0];
	buttonLabels[1] = (char *)&buttonFunctionLabels[buttonFunctions[1]][0];
	buttonLabels[2] = (char *)&buttonFunctionLabels[buttonFunctions[2]][0];
	buttonLabels[3] = (char *)&buttonFunctionLabels[buttonFunctions[3]][0];
	buttonLabels[4] = (char *)&buttonFunctionLabels[buttonFunctions[4]][0];

	mtDisplay.changeButtonsLabels(buttonLabels);
}

void cMtInstrumentEditor::setButtonFunction(uint8_t number, uint8_t function)
{
	buttonFunctions[number] = function;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// POTS

void cMtInstrumentEditor::setPotsLabel(uint8_t function, char* label)
{
	uint8_t i = 0;
	potFunctionLabels[function][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		potFunctionLabels[function][i] = label[i];
		i++;
	}
	potFunctionLabels[function][i] = 0;

	mtDisplay.changePotsLabels(potLabels);
}



void cMtInstrumentEditor::updatePotsFunctions()
{
	setPotFunction(0, potFunctNone);
	setPotFunction(1, potFunctNone);
	setPotFunction(2, potFunctNone);
	setPotFunction(3, potFunctNone);
	setPotFunction(4, potFunctNone);


	if(parametersEnabled) // parametry
	{
		if(parametersPage == 0)
		{
			setPotFunction(0, potFunctVolume);
			setPotFunction(1, potFunctPanning);
			setPotFunction(2, potFunctResonance);
			setPotFunction(3, potFunctFilter);
		}
		else if(parametersPage == 1)
		{
			setPotFunction(0, potFunctGlide);
			setPotFunction(2, potFunctFinetune);
			setPotFunction(3, potFunctTune);
		}
	}
	else if(envelopesEnabled) // envelopy
	{
		setPotFunction(0, potFunctAmount);
		setPotFunction(1, potFunctAttack);
		setPotFunction(2, potFunctDecay);
		setPotFunction(3, potFunctSustaion);
		setPotFunction(4, potFunctRelease);
	}
	else // spectrum
	{
		if(instrumentListEnabled)
		{
			if(instrumentListMode == 0) 		setPotFunction(0, potFunctInstrumentSelect);
			else if(instrumentListMode == 1) 	setPotFunction(0, potFunctAddInstrumentSelect);

			if(sampleListEnabled) setPotFunction(1, potFunctSampleSelect);
		}
		else if(sampleListEnabled)
		{
			setPotFunction(1, potFunctSampleSelect);
		}
		else if(mtProject.instruments_count > 0 && mtProject.sampleBank.samples_count > 0)
		{
			if(mtProject.sampleBank.sample[editorInstrument->sampleIndex].type == mtSampleTypeWavetable)
			{
				setPotFunction(0, potFunctWavetablePos);
			}
			else
			{
				setPotFunction(0, potFunctStartPoint);
				setPotFunction(2, potFunctViewZoom);
				setPotFunction(4, potFunctEndPoint);

				if(editorInstrument->playMode >= loopForward)
				{
					setPotFunction(1, potFunctLoopPoint1);
					setPotFunction(3, potFunctLoopPoint2);
				}
			}
		}
	}



	potLabels[0] = (char *)&potFunctionLabels[potFunctions[0]][0];
	potLabels[1] = (char *)&potFunctionLabels[potFunctions[1]][0];
	potLabels[2] = (char *)&potFunctionLabels[potFunctions[2]][0];
	potLabels[3] = (char *)&potFunctionLabels[potFunctions[3]][0];
	potLabels[4] = (char *)&potFunctionLabels[potFunctions[4]][0];

	mtDisplay.changePotsLabels(potLabels);
}


void cMtInstrumentEditor::setPotFunction(uint8_t number, uint8_t function)
{
	potFunctions[number] = function;
	AnalogInputs.setPotResolution(number, potFuncRes[function]);
	AnalogInputs.setPotAcceleration(number, potFuncAcc[function]);
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
				instrumentPlayer[0].setStatusByte(LP1_MASK);
				instrumentPlayer[0].setStatusByte(LP2_MASK);
			}
			else
			{
				editorInstrument->loopPoint2 = editorInstrument->loopPoint1 + dif;
				instrumentPlayer[0].setStatusByte(LP2_MASK);
			}
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
				instrumentPlayer[0].setStatusByte(LP1_MASK);
				instrumentPlayer[0].setStatusByte(LP2_MASK);
			}
			else
			{
				editorInstrument->loopPoint1 = editorInstrument->loopPoint2 - dif;
				instrumentPlayer[0].setStatusByte(LP1_MASK);
			}
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
	if(editorInstrument->loopPoint1 >= editorInstrument->loopPoint2) editorInstrument->loopPoint1 = editorInstrument->loopPoint2-1;

	if(zoomValue > 1 && lastChangedPoint != 3
			&& (editorInstrument->loopPoint1 < zoomStart || editorInstrument->loopPoint1 > zoomEnd)) spectrumChanged = 1;

	instrumentPlayer[0].setStatusByte(LP1_MASK);


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
	if(editorInstrument->loopPoint2 <= editorInstrument->loopPoint1) editorInstrument->loopPoint2 = editorInstrument->loopPoint1+1;

	if(zoomValue > 1 && lastChangedPoint != 4
			&& (editorInstrument->loopPoint2 < zoomStart || editorInstrument->loopPoint2 > zoomEnd)) spectrumChanged = 1;

	instrumentPlayer[0].setStatusByte(LP2_MASK);

	lastChangedPoint = 4;
	pointsChanged = 1;
}

void cMtInstrumentEditor::changePlayMode(uint8_t value)
{
	if(value == 1)
	{
		if(editorInstrument->playMode < playModeCount-1 ) editorInstrument->playMode++;
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

	instrumentPlayer[0].setStatusByte(PANNING_MASK);

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

	instrumentPlayer[0].setStatusByte(CUTOFF_MASK);

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
	else if(editorInstrument->resonance + fVal > RESONANCE_MAX) editorInstrument->resonance = RESONANCE_MAX;
	else editorInstrument->resonance += fVal;

	instrumentPlayer[0].setStatusByte(RESONANCE_MASK);

	parametersChanged = 1;
}

void cMtInstrumentEditor::changeVolume(int16_t value)
{
	if(editorInstrument->volume + value < 0) editorInstrument->volume = 0;
	else if(editorInstrument->volume + value > MAX_INSTRUMENT_VOLUME) editorInstrument->volume = MAX_INSTRUMENT_VOLUME;
	else editorInstrument->volume += value;

	instrumentPlayer[0].setStatusByte(VOLUME_MASK);

	parametersChanged = 1;
}

void cMtInstrumentEditor::changeFinetune(int16_t value)
{
	if(editorInstrument->fineTune + value < MIN_INSTRUMENT_FINETUNE) editorInstrument->fineTune = MIN_INSTRUMENT_FINETUNE;
	else if(editorInstrument->fineTune + value > MAX_INSTRUMENT_FINETUNE) editorInstrument->fineTune = MAX_INSTRUMENT_FINETUNE;
	else editorInstrument->fineTune += value;

	instrumentPlayer[0].setStatusByte(FINETUNE_MASK);

	parametersChanged = 1;
}

void cMtInstrumentEditor::changeTune(int16_t value)
{
	if(editorInstrument->tune + value < MIN_INSTRUMENT_TUNE) editorInstrument->tune = MIN_INSTRUMENT_TUNE;
	else if(editorInstrument->tune + value > MAX_INSTRUMENT_TUNE) editorInstrument->tune = MAX_INSTRUMENT_TUNE;
	else editorInstrument->tune += value;

	instrumentPlayer[0].setStatusByte(TUNE_MASK);

	parametersChanged = 1;
}

void cMtInstrumentEditor::changeWavetablePos(int16_t value)
{
	if(editorInstrument->wavetableCurrentWindow + value < 0) editorInstrument->wavetableCurrentWindow  = 0;
	else if(editorInstrument->wavetableCurrentWindow + value > MAX_WAVETABLE_WINDOW) editorInstrument->wavetableCurrentWindow = MAX_WAVETABLE_WINDOW;
	else editorInstrument->wavetableCurrentWindow += value;

	spectrumChanged = 1;
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

void cMtInstrumentEditor::changeParamsPage(uint8_t value)
{
	if(value == 1)
	{
		if(parametersPage == 0) parametersPage = 1;
		else if(parametersPage == 1) parametersPage = 0;

		parametersChanged = 2;
	}
}

void cMtInstrumentEditor::showParameters()
{
	if(!parametersEnabled)
	{
		parametersEnabled = 1;
		parametersChanged = 2;

		envelopesEnabled = 0;

		if(sampleListEnabled)
		{
			sampleListEnabled = 0;
			sampleListChanged = 1;
		}
		if(instrumentListEnabled)
		{
			instrumentListEnabled = 0;
			instrumentListChanged = 1;
		}

		refreshInstrumentEditor = 1;
	}
	else
	{
		if(parametersPage == 0) parametersPage = 1;
		else if(parametersPage == 1) parametersPage = 0;

		parametersChanged = 2;

		refreshInstrumentEditor = 1;
	}

}

void cMtInstrumentEditor::showEnvelopes()
{
	if(!envelopesEnabled)
	{
		envelopesEnabled = 1;
		envelopesChanged = 2;


		parametersEnabled = 0;

		if(sampleListEnabled)
		{
			sampleListEnabled = 0;
			sampleListChanged = 1;
		}
		if(instrumentListEnabled)
		{
			instrumentListEnabled = 0;
			instrumentListChanged = 1;
		}

		refreshInstrumentEditor = 1;
	}
}


void cMtInstrumentEditor::showSpectrum()
{
	parametersEnabled = 0;
	envelopesEnabled = 0;
	spectrumChanged   = 2;

	refreshInstrumentEditor = 1;
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

// ----------------------------------------------------------------------------------------------
// instrument
void cMtInstrumentEditor::selectInstrument(int16_t value)
{
	if(openedInstrFromActive + value < 0) openedInstrFromActive = 0;
	else if(openedInstrFromActive + value > mtProject.instruments_count-1) openedInstrFromActive  = mtProject.instruments_count-1;
	else openedInstrFromActive += value;

	openedInstrumentIndex = activeInstruments[openedInstrFromActive];
	mtProject.values.lastUsedInstrument = openedInstrumentIndex;
	editorInstrument = &mtProject.instrument[openedInstrumentIndex];

	mtDisplay.changeList(instrument_list_pos, openedInstrFromActive);
	mtHaptic.start(15,150,0x01,56);

	strcpy(buttonFunctionLabels[buttonFunctInstrumentList], "Instrument ");
	strcat(buttonFunctionLabels[buttonFunctInstrumentList], mtProject.instrument[openedInstrumentIndex].name);

	if(mtProject.sampleBank.samples_count > 0)
	{
		int8_t sample_index = mtProject.instrument[openedInstrumentIndex].sampleIndex;
		if(sample_index >= 0)
		{
			waitToLoadSpectrum(); // przeladuj spectrum tlyko jelsi posiada sampla
			strcpy(buttonFunctionLabels[buttonFunctSampleList], mtProject.sampleBank.sample[sample_index].file_name);
		}
	}

	labelsChanged = 1;
	if(sampleListEnabled) sampleListChanged = 2;
}

void cMtInstrumentEditor::selectAddInstrument(int16_t value)
{
	if(inActiveInstrumentIndex + value < 0) inActiveInstrumentIndex = 0;
	else if(inActiveInstrumentIndex + value > inActiveInstrumentsCount-1) inActiveInstrumentIndex  = inActiveInstrumentsCount-1;
	else inActiveInstrumentIndex += value;

	mtHaptic.start(15,150,0x01,56);

	mtDisplay.changeList(instrument_list_pos, inActiveInstrumentIndex);
}

void cMtInstrumentEditor::addInstrument(uint8_t value)
{
	if(value == 1)
	{
		if(mtProject.instruments_count < INSTRUMENTS_MAX)
		{
			openedInstrFromActive = 0;
			inActiveInstrumentsCount = 0;
			for(uint8_t i = 0; i < INSTRUMENTS_MAX; i++)
			{
				if(mtProject.instrument[i].isActive == 0)
				{
					if(i<9) mtProject.instrument[i].name[0] = (i+1)%10 + 48;
					else
					{
						mtProject.instrument[i].name[0] = (((i+1)-(i+1)%10)/10) + 48;
						mtProject.instrument[i].name[1] = (i+1)%10 + 48;
					}

					instrumentNames[inActiveInstrumentsCount] = mtProject.instrument[i].name;
					activeInstruments[inActiveInstrumentsCount] = i;
					//if(i == openedInstrumentIndex) openedInstrFromActive = inActiveInstr;
					inActiveInstrumentsCount++;
				}
			}

			inActiveInstrumentIndex = 0;
			instrumentListMode = 1;
			instrumentListEnabled = 1;
			instrumentListChanged = 2;
		}
	}
}

void cMtInstrumentEditor::importInstrument(uint8_t value)
{

}

void cMtInstrumentEditor::createInstrument(uint8_t value)
{
	if(value == 1)
	{
		fileManager.addInstrumentToProject(activeInstruments[inActiveInstrumentIndex]);

		startExisting(activeInstruments[inActiveInstrumentIndex]);

		instrumentListEnabled = 0;  // ustaiwnie na 0 zeby pokazalo liste zamiast ukryc
		showInstrumentList(1);
	}
}

void cMtInstrumentEditor::cancelCreateInstrument(uint8_t value)
{
	if(value == 1)
	{
		instrumentListEnabled = 0;  // ustaiwnie na 0 zeby pokazalo liste zamiast ukryc
		showInstrumentList(1);
	}
}

void cMtInstrumentEditor::removeInstrument()
{
	//fileManager.removeInstrument(openedInstrumentIndex);

	uint8_t open_first = 0;
	for(uint8_t i = 0; i < INSTRUMENTS_MAX; i++)
	{
		if(mtProject.instrument[i].isActive == 1)
		{
			open_first = i;
			break;
		}
	}

	startExisting(open_first); // otworz pierwszy aktywny

	instrumentListEnabled = 0;  // ustaiwnie na 0 zeby pokazalo liste zamiast ukryc
	showInstrumentList(1);
}

// ----------------------------------------------------------------------------------------------

void cMtInstrumentEditor::selectSample(int16_t value)
{
	if(editorInstrument->sampleIndex + value < 0) editorInstrument->sampleIndex  = 0;
	else if(editorInstrument->sampleIndex + value > mtProject.sampleBank.samples_count-1) editorInstrument->sampleIndex  = mtProject.sampleBank.samples_count-1;
	else editorInstrument->sampleIndex += value;

	mtDisplay.changeList(sample_list_pos, editorInstrument->sampleIndex);

	mtHaptic.start(15,150,0x01,56);

	waitToLoadSpectrum();
	lastChangedPoint = 0;
}


void cMtInstrumentEditor::addSample(uint8_t value)
{
	if(value == 1) padsChange(1, interfacePadSampleBank, 0);
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
			if(mtProject.sampleBank.samples_count == 0)
			{

			}

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
			if(mtProject.instruments_count > 0)
			{
				openedInstrFromActive = 0;
				uint8_t activeInstr = 0;
				for(uint8_t i = 0; i < INSTRUMENTS_MAX; i++)
				{
					if(mtProject.instrument[i].isActive)
					{
						instrumentNames[activeInstr] = mtProject.instrument[i].name;
						activeInstruments[activeInstr] = i;
						if(i == openedInstrumentIndex) openedInstrFromActive = activeInstr;
						activeInstr++;
					}
				}
			}

			instrumentListMode = 0;
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

}

void cMtInstrumentEditor::play(uint8_t value)
{
	if(value == 1)
	{
		//eventFunct(mtInstrumentEditorEventPadPress, &interfacePadStop, 0, 0);
		sequencer.stop();

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

		instrumentPlayer[0].noteOn(openedInstrumentIndex, playNote, -1);
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


void cMtInstrumentEditor::playInstrumentByPad(uint8_t type, int8_t pad, int8_t velocity)
{
	sequencer.stop();

	if(type == 1)
	{
		//uint8_t note = mtPadBoard.convertPadToNote(pad);
		//if(note > 48) note = 48;
		//editorInstrument->tune = note;
		mtPadBoard.startInstrument(pad, openedInstrumentIndex,-1);

	}
	else if(type == 0)
	{
		mtPadBoard.stopInstrument(pad);
	}




}

