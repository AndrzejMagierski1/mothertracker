

#include "mtPadBoard.h"

#include "samplePlayback.h"


cSamplePlayback samplePlayback;
static cSamplePlayback* SP = &samplePlayback;




static  uint8_t functPlayAction();
static  uint8_t functStopAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);



void cSamplePlayback::update()
{
	if(refreshSpectrum)
	{
		spectrum.loadProgress;
	}





}

void cSamplePlayback::start(uint32_t options)
{
	moduleRefresh = 1;

	//--------------------------------------------------------------------

	mtPadBoard.setPadNotes(mtProject.values.padBoardScale,
			mtProject.values.padBoardNoteOffset,
			mtProject.values.padBoardRootNote);

	mtPadBoard.configureInstrumentPlayer(mtProject.values.padBoardMaxVoices);

	if(mtProject.sampleBank.samples_count == 0)
	{
		//strcpy(buttonFunctionLabels[buttonFunctSampleList], "No samples");
	}
	if(mtProject.instruments_count == 0)
	{
		//strcpy(buttonFunctionLabels[buttonFunctInstrumentList], "No instruments");

		//askToCreateNewInstr();
		return;
	}


	// utworzenie listy tylko aktywnych instryumentow w otwartym projeckie
	openedInstrumentIndex = -1;
	openedInstrFromActive = 0;
	uint8_t activeInstr = 0;
	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		if(mtProject.instrument[i].isActive)
		{
			if(openedInstrumentIndex == -1) openedInstrumentIndex = i; // tymczasowo przypisz pierwszy aktywny jako otwarty
			instrumentNames[activeInstr] = mtProject.instrument[i].name;
			activeInstruments[activeInstr] = i;
			if(i == mtProject.values.lastUsedInstrument) // jesli znaleziono wybrany jako aktywny
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
	else if(openedInstrumentIndex != mtProject.values.lastUsedInstrument) // jesli wybrany nie jest aktywny
	{
		// przypisano juz wczesniej pierwszy aktywny jako otwierany (openedInstrumentIndex = i;	// zapisz otwarty jako wybrany)

	}

	//openedInstrumentIndex = openedInstrFromActive;
	editorInstrument = &mtProject.instrument[openedInstrumentIndex];

//--------------------------------------------------------------------


	// inicjalizacja kontrolek

	for(uint8_t i = 0; i<8; i++)
	{
		strControlProperties prop2;
		prop2.text = (char*)"";
		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 450;
		prop2.w = 800/8-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);
	}




	strControlProperties prop;
//	prop.text = (char*)"";
//	prop.style = 	(controlStyleShow );//| controlStyleFont2 | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
	prop.x = 0;
	prop.y = 0;
	prop.w = 800;
	prop.h = 400;
	prop.data = &spectrum;
	if(spectrumControl == nullptr)  spectrumControl = display.createControl<cSpectrum>(&prop);
	//display.refreshControl(hTrackControl);






	// ustawienie funkcji
	FM->clearAllButtons();
	FM->clearAllPots();

	FM->setButtonObj(interfaceButton17, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton13, buttonPress, functSwitchModule);


	showDefaultScreen();

	//typedef void (cProjectEditor::*funct1) (void);
	//funct1 = &cProjectEditor::functOpenProject;
	//(this->*funct1)();

}

void cSamplePlayback::stop()
{
	display.destroyControl(spectrumControl);
	spectrumControl = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;
	}

	FM->clearAllButtons();
	FM->clearAllPots();

}

void cSamplePlayback::showDefaultScreen()
{

	processSpectrum();


	//lista
	display.setControlShow(spectrumControl);
	display.refreshControl(spectrumControl);

	// bottom labels
	display.setControlText(bottomLabel[0], "Start");
	display.setControlText(bottomLabel[1], "Loop Start");
	display.setControlText(bottomLabel[2], "Loop End");
	display.setControlText(bottomLabel[3], "End");
	display.setControlText(bottomLabel[4], "- Zoom");
	display.setControlText(bottomLabel[5], "+ Zoom");
	display.setControlText(bottomLabel[6], "Play Mode");
	//display.setControlText(bottomLabel[7], "End");

	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}

	display.synchronizeRefresh();

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButton8, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButton9, buttonPress, functStopAction);
	FM->setButtonObj(interfaceButton10, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButton30, buttonPress, functLeft);
	FM->setButtonObj(interfaceButton32, buttonPress, functRight);
	FM->setButtonObj(interfaceButton26, buttonPress, functUp);
	FM->setButtonObj(interfaceButton31, buttonPress, functDown);



	//FM->setPotObj(interfacePot0, (uint16_t*)(&trackerPattern.part), 0, 744, 5, patternControl);


	FM->setPotObj(interfacePot0, functEncoder, nullptr);



}
//==============================================================================================================



void cSamplePlayback::processSpectrum()
{

	//refreshSpectrum = 1;


	// uwaga tu wazna kolejnosc + do sprawdzenia
	if(openedInstrumentIndex < 0 || mtProject.instrument[openedInstrumentIndex].sampleIndex < 0)
	{
		for(uint16_t i = 0; i < 800; i++)
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

		float resolution = windowSize / 800.0;

		int16_t up = 0;
		int16_t low = 0;
		float step = 0;

		for(uint16_t i = 0; i < 800; i++)
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
			offset_pixel = ((zoomPosition-zoomStart) * 799) / zoomWidth;
		}
		else if(zoomEnd > MAX_16BIT)
		{
			zoomEnd = MAX_16BIT;
			zoomStart = MAX_16BIT-zoomWidth;
			offset_pixel = ((zoomPosition-zoomStart) * 799) / zoomWidth;
		}
		else
		{
			offset_pixel = ((zoomPosition-zoomStart) * 799) / zoomWidth;
		}


		uint32_t offset = ((float)zoomPosition/MAX_16BIT) * mtProject.sampleBank.sample[editorInstrument->sampleIndex].length;

		sampleData = mtProject.sampleBank.sample[editorInstrument->sampleIndex].address + offset;

		resolution = (((float)zoomWidth/MAX_16BIT) * mtProject.sampleBank.sample[editorInstrument->sampleIndex].length ) / 800;


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
		resolution = mtProject.sampleBank.sample[editorInstrument->sampleIndex].length / 800;
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


	for(uint16_t i = offset_pixel; i < 800; i++)
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




//==============================================================================================================


static  uint8_t functEncoder(int16_t value)
{



	//display.refreshControl(PTE->patternControl);

	return 1;
}





static  uint8_t functLeft()
{

	return 1;
}


static  uint8_t functRight()
{


	return 1;
}


static  uint8_t functUp()
{

	return 1;
}


static  uint8_t functDown()
{


	return 1;
}




static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == 0) sequencer.play();

	return 1;
}


static  uint8_t functStopAction()
{
	if(sequencer.getSeqState() == 1)
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


