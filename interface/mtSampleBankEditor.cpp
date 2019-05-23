
#include "display.h"
#include "AnalogInputs.h"

#include "mtAudioEngine.h"
#include "mtInterfaceDefs.h"
#include "mtFileManager.h"
#include "mtPadsBacklight.h"

#include "mtSampleBankEditor.h"


cMtSampleBankEditor mtSampleBankEditor;
extern int16_t sdram_sampleBank[4*1024*1024];

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtSampleBankEditor::update()
{
	if(!refreshModule) return;
	refreshModule = 0;

	//-----------------------------------------------------
	if(labelsChanged)
	{
		if(labelsChanged == 2)
		{

			for(uint8_t i = 0; i<5; i++)
			{
				strControlProperties prop;
				prop.text = (char*)"Test";
				prop.style = 	(controlStyleShow |/* controlStyleFont2 | controlStyleBackground |*/ controlStyleCenterX);
				prop.x = (800/5)*i+(800/10);
				prop.y = 5;
				prop.w = 800/5;
				prop.h = 25;

				if(potControls[i] == nullptr) potControls[i] = display.createControl<cLabel>(&prop);
			}

			for(uint8_t i = 0; i<5; i++)
			{
				strControlProperties prop;
				prop.text = (char*)"Test";
				prop.style = 	(controlStyleShow | /*controlStyleCenterY |*/ controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
				prop.x = (800/5)*i+(800/10);
				prop.y = 450;
				prop.w = 800/5-10;
				prop.h = 30;

				if(buttonControls[i] == nullptr) buttonControls[i] = display.createControl<cLabel>(&prop);
			}
		}

		labelsChanged = 0;

		updateButtonsFunctions();
		updatePotsFunctions();

		processLabels();
	}
	//-----------------------------------------------------
	if(spectrumChanged)
	{
		if(!spectrumEnabled)
		{
			spectrumChanged = 0;
			//mtDisplay.setSpectrum(0);
			//mtDisplay.setSpectrumPoints(0);

			refreshModule = 1;
			return;
		}

		if(spectrumChanged == 2)
		{
			////mtDisplay.setSpectrum(1);
			//mtDisplay.setSpectrumPoints(1);

			refreshModule = 1;
			labelsChanged = 1;
		}

		spectrumChanged = 0;
		pointsChanged = 1;

		processSpectrum();

		//mtDisplay.changeSpectrum(&spectrum);

	}
	//-----------------------------------------------------
	if(pointsChanged)
	{
		processPoints();
		//mtDisplay.changeSpectrumPoints(&spectrum);
	}
	//-----------------------------------------------------
	if(samplesListChanged)
	{
		labelsChanged = 1;
		refreshModule = 1;

		if(!samplesListEnabled)
		{
			samplesListChanged = 0;
			display.destroyControl(samplesListControl);
			samplesListControl = nullptr;
			//mtDisplay.setList(samples_list_pos, 0, 0, 0, 0, 0);
			return;
		}

		if(samplesListChanged == 2) // pokaz liste
		{
			if(mtProject.sampleBank.samples_count > 0)
			{
				uint8_t activeSample = 0;
				for(uint8_t i = 0; i < SAMPLES_COUNT; i++)
				{
					if(mtProject.sampleBank.sample[i].loaded)
					{
						strcpy(&samplesNames[activeSample][0], mtProject.sampleBank.sample[i].file_name);
						ptrSamplesNames[activeSample] = &samplesNames[activeSample][0];
						samplesIndex[activeSample] = i;
						activeSample++;
					}
				}

				sampleListPos = 0;
				samplesCount = activeSample;

				//mtDisplay.setList(samples_list_pos, samples_list_pos, 2, 0, ptrSamplesNames, mtProject.sampleBank.samples_count);

				samplesList.start = 0;
				samplesList.length = mtProject.sampleBank.samples_count;
				samplesList.linesCount = 5;
				samplesList.data = ptrSamplesNames;

				strControlProperties prop;
				prop.style = 	(controlStyleShow );//| controlStyleFont2 | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
				prop.x = (800/5)*3;
				prop.y = 35;
				prop.w = (800/5)*2;
				prop.h = 25;
				prop.data = &samplesList;
				if(samplesListControl == nullptr) samplesListControl = display.createControl<cList>(&prop);
				display.refreshControl(samplesListControl);

			}
			else
			{
				ptrSamplesNames[0] = buttonFunctionLabels[0]; // przypisanie pustego

				samplesList.start = 0;
				samplesList.length = 1;
				samplesList.linesCount = 5;
				samplesList.data = ptrSamplesNames;

				strControlProperties prop;
				prop.style = 	(controlStyleShow );//| controlStyleFont2 | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
				prop.x = (800/5)*3;
				prop.y = 35;
				prop.w = (800/5)*2;
				prop.h = 25;
				prop.data = &samplesList;
				if(samplesListControl == nullptr) samplesListControl = display.createControl<cList>(&prop);
				//display.setControlData(control1,  &projectList);
				display.refreshControl(samplesListControl);
				//mtDisplay.setList(samples_list_pos, samples_list_pos, 2, 0, ptrSamplesNames, 1 );
			}
		}
		//processSamples();
		samplesListChanged = 0;
	}
	//-----------------------------------------------------
	if(filesListChanged)
	{
		labelsChanged = 1;
		refreshModule = 1;

		if(!filesListEnabled)
		{
			filesListChanged = 0;
			//mtDisplay.setList(files_list_pos, 0, 0, 0, 0, 0);
			return;
		}

		if(filesListChanged == 2) // pokaz liste
		{
			listOnlyDirAndWavFromActualPath();

			getSelectedFileType();

			//mtDisplay.setList(files_list_pos, files_list_pos, 2, selectedLocation, filesNames, locationFilesCount);
		}

		filesListChanged = 0;
	}
	//-----------------------------------------------------
	if(slotListChanged)
	{
		labelsChanged = 1;
		refreshModule = 1;

		if(!slotListEnabled)
		{
			slotListChanged = 0;
			//mtDisplay.setList(slot_list_index, 0, 0, 0, 0, 0);
			return;
		}

		if(slotListChanged == 2) // pokaz liste
		{
			listSampleSlots();

			//mtDisplay.setList(slot_list_index, files_list_pos, 2, selectedSlot, ptrSlotNames, SAMPLES_COUNT);
		}

		slotListChanged = 0;
	}
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtSampleBankEditor::start()
{
	samplesListChanged = 2;
	samplesListEnabled = 1;

	filesListEnabled = 0;
	slotListEnabled = 0;
	spectrumEnabled = 0;
	filesListChanged = 1;
	slotListChanged = 1;
	spectrumChanged = 1;
	labelsChanged = 2;

	if(spectrumEnabled) loadSamplesBank();

	refreshModule = 1;
}

void cMtSampleBankEditor::stop()
{
	display.destroyControl(filesListControl);
	display.destroyControl(samplesListControl);
	display.destroyControl(slotsListControl);

	slotsListControl = nullptr;
	samplesListControl = nullptr;
	filesListControl = nullptr;

	for(uint8_t i = 0; i<5; i++)
	{
		display.destroyControl(potControls[i]);
		display.destroyControl(buttonControls[i]);
		potControls[i] = nullptr;
		buttonControls[i] = nullptr;
	}

	samplesListEnabled = 0;
	filesListEnabled = 0;
	slotListEnabled = 0;
	spectrumEnabled = 0;


	if(spectrumEnabled) loadSamplesBank();

	if(playMode != playModeStop)
	{
		stopPlaying();
	}
}


void cMtSampleBankEditor::startRecorder()
{
	samplesListEnabled = 0;
	filesListEnabled = 0;
	slotListEnabled = 0;
	samplesListChanged = 1;
	filesListChanged = 1;
	slotListChanged = 1;

	spectrumEnabled = 1;
	spectrumChanged = 2;

	refreshModule = 1;
}



void cMtSampleBankEditor::processSpectrum()
{
	if(recordingStatus == recordingStatusClear)
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
	uint32_t resolution;

	switch(lastChangedPoint)
	{
		case 0: zoomPosition = startPoint; break;
		case 1: zoomPosition = stopPoint;  break;

		default: zoomPosition = startPoint; break;
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

		uint32_t offset = ((float)zoomPosition/MAX_16BIT) * recorder.getLength();
		sampleData = recorder.getAddress() + offset;
		resolution = (((float)zoomWidth/MAX_16BIT) * recorder.getLength() ) / 480;
	}
	else
	{
		offset_pixel = 0;
		zoomWidth = MAX_16BIT;
		zoomStart = 0;
		zoomEnd = MAX_16BIT;

		// TODO
		sampleData = mtProject.sampleBank.sample[0].address;
		resolution = mtProject.sampleBank.sample[0].length / 480;
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


void cMtSampleBankEditor::processPoints()
{
	spectrum.pointsType = singleShot;

	if(startPoint >= zoomStart && startPoint <= zoomEnd)
	{
		spectrum.startPoint = ((startPoint-zoomStart) * 479) / zoomWidth;
	}
	else spectrum.startPoint = -1;

	if(stopPoint >= zoomStart && stopPoint <= zoomEnd)
	{
		spectrum.endPoint = ((stopPoint-zoomStart) * 479) / zoomWidth;
	}
	else spectrum.endPoint = -1;
}

void cMtSampleBankEditor::saveRecording()
{

}

void cMtSampleBankEditor::clearRecording()
{

	recordingStatus = recordingStatusClear;
	refreshModule = 1;
}

void cMtSampleBankEditor::trimRecording()
{

}


void cMtSampleBankEditor::changeStartPoint(int16_t value)
{

}

void cMtSampleBankEditor::changeStopPoint(int16_t value)
{

}

void cMtSampleBankEditor::changeZoom(int16_t value)
{

}


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
uint8_t cMtSampleBankEditor::padsChange(uint8_t type, uint8_t n, uint8_t velo)
{

	if(n == interfacePadPlay)
	{
		sequencer.stop();
		play(type);

		return 0;
	}
	if(n == interfacePadStop && recordingStatus > 0)
	{
		if(recordingStatus == recordingStatusRecording)
		{
			padsBacklight.stopBlink(interfacePadRec);
			recorder.stopRecording();
			recordingStatus = recordingStatusRecorded;
		}
		else if(recordingStatus == recordingStatusRecorded) recorder.stop();

		refreshModule = 1;
		labelsChanged = 1;
		spectrumChanged = 1;

		return 0;
	}
	if(n == interfacePadRec && recordingStatus == recordingStatusClear)
	{
		recordingStatus = recordingStatusRecording;

		padsBacklight.startBlink(31, interfacePadRec);
		sequencer.stop();
		recorder.startRecording(sdram_sampleBank);

		refreshModule = 1;
		labelsChanged = 1;
		spectrumChanged = 1;
		return 0;
	}

			if(n==16)
	        {
	            recorder.startRecording(sdram_sampleBank);

	        }
	        else if(n==17)
	        {
	            recorder.stopRecording();
	        }
	        else if(n==18)
	        {
	            recorder.play(0, MAX_16BIT);
	        }
	        else if(n==19)
	        {
	            recorder.stop();
	        }
	        else if(n==20)
	        {
	            recorder.save();
	        }



	if(type == 1)
	{
		switch(n)
		{
		case interfacePadPlay                 :    sequencer.play();    break;
		case interfacePadStop                 :    sequencer.stop();    break;
		case interfacePadProjectEditor        :
		case interfacePadInstrumentEditor     :
		case interfacePadConfig               :
		case interfacePadSettings             :

			stop();
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);

		break;

		case interfacePadRecorder			:	startRecorder(); 	   break;
		case interfacePadSampleBank			:	start();			   break;


/*
		case interfacePadNotes       :  setActualEditedStepParam(stepParamNote);   break;
		case interfacePadInstr       :  setActualEditedStepParam(stepParamInstr);   break;
		case interfacePadVelocity    :  setActualEditedStepParam(stepParamVolume);   break;
		case interfacePadFx          :  setActualEditedStepParam(stepParamFx);   break;
		case interfacePadFxParams    :  setActualEditedStepParam(stepParamFxParams);   break;
		case interfacePadUp          :  moveActualStep(0);   break;
		case interfacePadLeft        :  moveActualStep(3);   break;
		case interfacePadDown        :  moveActualStep(2);   break;
		case interfacePadRight       :  moveActualStep(1);   break;
*/
		default: break;
		}

	}

	return 0;
}

void cMtSampleBankEditor::buttonChange(uint8_t button, uint8_t value)
{
	if(value == 1)
	{
		switch(buttonFunctions[button])
		{
		case buttonFunctNone				:	break;
		case buttonFunctImportSample		:	importSample(mtSampleTypeWaveFile);		break;
		case buttonFunctImportWavetable		:	importSample(mtSampleTypeWavetable);	break;
		case buttonFunctRemoveSample		:	removeSample();		break;
		case buttonFunctRenameSample		:	renameSample();		break;

		case buttonFunctBrowseSelectSample	:	browseSelectSample();	break;
		case buttonFunctBrowseOpenFolder	:	browseOpenFolder();	break;
		case buttonFunctBrowseBack			:	browseBack();		break;
		case buttonFunctBrowseCancel		:	browseCancel();		break;

		case buttonFunctSelectSampleSlot	:	browseSelectSlot();	break;


		case buttonFunctSaveRecording		:	saveRecording();	break;
		case buttonFucntTrimRecording		:	trimRecording();	break;
		case buttonClearRecording			:	clearRecording();	break;



		default: break;
		}
	}

	refreshModule = 1;
}


void cMtSampleBankEditor::potChange(uint8_t pot, int16_t value)
{

	switch(potFunctions[pot])
	{
		case potFunctNone					:	break;
		case potFunctChangeSamplesListPos	:	changeSampleListPos(value);	break;
		case potFunctChangeFileListPos		:	changeFilesListPos(value);	break;
		case potFunctChangeSlotListPos		:	changeSlotsListPos(value);	break;

		case potFunctChangeStartPoint		:	changeStartPoint(value);	break;
		case potFunctChangeStopPoint		:	changeStopPoint(value);		break;
		case potFunctChangeZoom				:	changeZoom(value);			break;

		default: break;
	}

	refreshModule = 1;
}

void cMtSampleBankEditor::seqButtonChange(uint8_t type, uint8_t x, uint8_t y)
{
/*

	if(type == 1)
	{
		//if(y > 0)
			showStep(x,y);
	}
*/
}


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtSampleBankEditor::processLabels()
{

	for(uint8_t i = 0; i < 5; i++)
	{
		switch(buttonFunctions[i])
		{


		default: break;
		}
	}

}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

// BUTTONS

void cMtSampleBankEditor::setButtonLabel(uint8_t function, char* label)
{
	uint8_t i = 0;
	buttonFunctionLabels[function][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		buttonFunctionLabels[function][i] = label[i];
		i++;
	}
	buttonFunctionLabels[function][i] = 0;

	//mtDisplay.changeButtonsLabels(buttonLabels);
}


void cMtSampleBankEditor::updateButtonsFunctions()
{
	setButtonFunction(0, buttonFunctNone);
	setButtonFunction(1, buttonFunctNone);
	setButtonFunction(2, buttonFunctNone);
	setButtonFunction(3, buttonFunctNone);
	setButtonFunction(4, buttonFunctNone);

//--------------------------------------------------------

	//setButtonFunction(0, buttonFunctRenameSample);


	if(filesListEnabled)
	{
		if(selectedFileType == 0) setButtonFunction(0, buttonFunctBrowseOpenFolder);
		else setButtonFunction(0, buttonFunctBrowseSelectSample);
		setButtonFunction(1, buttonFunctBrowseCancel);

	}
	else if(slotListEnabled)
	{
		setButtonFunction(0, buttonFunctSelectSampleSlot);
		setButtonFunction(1, buttonFunctBrowseCancel);
	}
	else if(spectrumEnabled)
	{
		if(recordingStatus == recordingStatusRecorded)
		{
			setButtonFunction(0, buttonFunctSaveRecording);
			setButtonFunction(1, buttonFucntTrimRecording);
			setButtonFunction(1, buttonClearRecording);
		}
	}
	else
	{
		setButtonFunction(0, buttonFunctImportSample);
		setButtonFunction(1, buttonFunctImportWavetable);

		setButtonFunction(3, buttonFunctRenameSample);
		setButtonFunction(4, buttonFunctRemoveSample);
	}

//--------------------------------------------------------

	buttonLabels[0] = (char *)&buttonFunctionLabels[buttonFunctions[0]][0];
	buttonLabels[1] = (char *)&buttonFunctionLabels[buttonFunctions[1]][0];
	buttonLabels[2] = (char *)&buttonFunctionLabels[buttonFunctions[2]][0];
	buttonLabels[3] = (char *)&buttonFunctionLabels[buttonFunctions[3]][0];
	buttonLabels[4] = (char *)&buttonFunctionLabels[buttonFunctions[4]][0];

	display.setControlText(buttonControls[0], buttonLabels[0]);
	display.setControlText(buttonControls[1], buttonLabels[1]);
	display.setControlText(buttonControls[2], buttonLabels[2]);
	display.setControlText(buttonControls[3], buttonLabels[3]);
	display.setControlText(buttonControls[4], buttonLabels[4]);

	display.refreshControl(buttonControls[0]);
	display.refreshControl(buttonControls[1]);
	display.refreshControl(buttonControls[2]);
	display.refreshControl(buttonControls[3]);
	display.refreshControl(buttonControls[4]);
}


void cMtSampleBankEditor::setButtonFunction(uint8_t number, uint8_t function)
{
	buttonFunctions[number] = function;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// POTS

void cMtSampleBankEditor::setPotsLabel(uint8_t function, char* label)
{
	uint8_t i = 0;
	potFunctionLabels[function][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		potFunctionLabels[function][i] = label[i];
		i++;
	}
	potFunctionLabels[function][i] = 0;

	//mtDisplay.changePotsLabels(potLabels);
}



void cMtSampleBankEditor::updatePotsFunctions()
{
	setPotFunction(0, potFunctNone);
	setPotFunction(1, potFunctNone);
	setPotFunction(2, potFunctNone);
	setPotFunction(3, potFunctNone);
	setPotFunction(4, potFunctNone);

//--------------------------------------------------------



	if(filesListEnabled)
	{
		setPotFunction(0, potFunctChangeFileListPos);
		setPotFunction(3, potFunctChangeSamplesListPos);
	}
	else if(slotListEnabled)
	{
		setPotFunction(0, potFunctChangeSlotListPos);
		setPotFunction(3, potFunctChangeSamplesListPos);
	}
	else if(spectrumEnabled)
	{
		setPotFunction(0, potFunctChangeStartPoint);
		setPotFunction(2, potFunctChangeZoom);
		setPotFunction(4, potFunctChangeStopPoint);
	}
	else
	{
		setPotFunction(3, potFunctChangeSamplesListPos);
	}


//--------------------------------------------------------

	potLabels[0] = (char *)&potFunctionLabels[potFunctions[0]][0];
	potLabels[1] = (char *)&potFunctionLabels[potFunctions[1]][0];
	potLabels[2] = (char *)&potFunctionLabels[potFunctions[2]][0];
	potLabels[3] = (char *)&potFunctionLabels[potFunctions[3]][0];
	potLabels[4] = (char *)&potFunctionLabels[potFunctions[4]][0];


	display.setControlText(potControls[0], potLabels[0]);
	display.setControlText(potControls[1], potLabels[1]);
	display.setControlText(potControls[2], potLabels[2]);
	display.setControlText(potControls[3], potLabels[3]);
	display.setControlText(potControls[4], potLabels[4]);

	display.refreshControl(potControls[0]);
	display.refreshControl(potControls[1]);
	display.refreshControl(potControls[2]);
	display.refreshControl(potControls[3]);
	display.refreshControl(potControls[4]);
}


void cMtSampleBankEditor::setPotFunction(uint8_t number, uint8_t function)
{
	potFunctions[number] = function;
	AnalogInputs.setPotResolution(number, potFuncRes[function]);
	AnalogInputs.setPotAcceleration(number, potFuncAcc[function]);
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################


void cMtSampleBankEditor::importSample(uint8_t type)
{
	sampleType = type;

	strcpy(filePath,"/");
	dirLevel = 0;
	selectedLocation = 0;

	filesListChanged = 2;
	filesListEnabled = 1;

}

void cMtSampleBankEditor::removeSample()
{
	fileManager.deleteSample(samplesIndex[sampleListPos]);


}

void cMtSampleBankEditor::renameSample()
{

}

void cMtSampleBankEditor::browseSelectSample()
{
	filesListChanged = 1;
	filesListEnabled = 0;

	slotListChanged = 2;
	slotListEnabled = 1;

	refreshModule = 1;
}

void cMtSampleBankEditor::browseSelectSlot()
{
	if(selectedSlot > SAMPLES_MAX) browseCancel();
	fileManager.importSampleToProject(filePath, &locationFilesList[selectedLocation][0],
			&locationFilesList[selectedLocation][0], selectedSlot, -1, sampleType);


	loadSamplesBank();

	slotListChanged = 1;
	slotListEnabled = 0;

	samplesListChanged = 2;

	refreshModule = 1;
}


void cMtSampleBankEditor::browseOpenFolder()
{

	if(dirLevel == 0)
	{
		strcpy(filePath,&locationFilesList[selectedLocation][0]);
		dirLevel++;
	}
	else
	{
		if(selectedLocation == 0)
		{
			*strrchr(filePath, '/') = 0;
			dirLevel--;
			if(dirLevel == 0) 	strcpy(filePath,"/");
		}
		else
		{
			strcat(filePath,&locationFilesList[selectedLocation][0]);
			dirLevel++;
		}
	}



	selectedLocation = 0;

	filesListChanged = 2;
	filesListEnabled = 1;

	refreshModule = 1;
}

void cMtSampleBankEditor::browseBack()
{

}

void cMtSampleBankEditor::browseCancel()
{
	filesListChanged = 1;
	filesListEnabled = 0;
	slotListChanged = 1;
	slotListEnabled = 0;

	refreshModule = 1;
}




//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtSampleBankEditor::changeSampleListPos(int16_t value)
{
	if(sampleListPos + value < 0) sampleListPos  = 0;
	else if(sampleListPos + value > samplesCount-1) sampleListPos  = samplesCount-1;
	else sampleListPos += value;


	//mtDisplay.changeList(samples_list_pos, sampleListPos);


	refreshModule = 1;
}

void cMtSampleBankEditor::changeFilesListPos(int16_t value)
{
	if(selectedLocation + value < 0) selectedLocation  = 0;
	else if(selectedLocation + value > locationFilesCount-1) selectedLocation  = locationFilesCount-1;
	else selectedLocation += value;

	getSelectedFileType();

	//mtDisplay.changeList(files_list_pos, selectedLocation);

	labelsChanged = 1;

//	filesListChanged = 1;
	refreshModule = 1;
}

void cMtSampleBankEditor::changeSlotsListPos(int16_t value)
{
	if(selectedSlot + value < 0) selectedSlot  = 0;
	else if(selectedSlot + value > SAMPLES_MAX) selectedSlot  = SAMPLES_MAX;
	else selectedSlot += value;

	//mtDisplay.changeList(slot_list_index, selectedSlot);


//	filesListChanged = 1;
	refreshModule = 1;
}


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################


void cMtSampleBankEditor::listOnlyDirAndWavFromActualPath()
{

	sdLocation.close();
	sdLocation.open(filePath, O_READ);

	if(dirLevel == 0)
	{
		locationFilesCount = sdLocation.createFilesList(0,locationFilesList, files_list_length_max);
	}
	else
	{
		strcpy(&locationFilesList[0][0], "/..");
		locationFilesCount = sdLocation.createFilesList(1,locationFilesList, files_list_length_max-1);
	}


	sdLocation.close();


	uint8_t foundFilesCount = 0;
	for(uint8_t i = 0; i < locationFilesCount; i++)
	{
		if(locationFilesList[i][0] == '/' || isWavFile(&locationFilesList[i][0]))	//tylko jesli folder albo plik wav
		{
			strcpy(&locationFilesList[foundFilesCount][0],&locationFilesList[i][0]);
			foundFilesCount++;
		}
	}


	locationFilesCount = foundFilesCount;

	for(uint8_t i = 0; i < locationFilesCount; i++)
	{
		filesNames[i] = &locationFilesList[i][0];
	}
}

uint8_t cMtSampleBankEditor::isWavFile(char* fileName)
{
	uint8_t endPos = 0;
	char temp_name[32];
	strcpy(temp_name, fileName);

	while(temp_name[endPos] != 0 && endPos < 19)
	{
		if(temp_name[endPos] > 96 && temp_name[endPos] < 123) temp_name[endPos] = temp_name[endPos] - 32;
		endPos++;
	}

	endPos--;

	if(temp_name[endPos] == 'V' && temp_name[endPos-1] == 'A' && temp_name[endPos-2] == 'W' && temp_name[endPos-3] == '.')
	{
		return 1;
	}

	return 0;
}

void cMtSampleBankEditor::getSelectedFileType()
{
	if(locationFilesList[selectedLocation][0] == '/') selectedFileType = 0;
	else selectedFileType = 1;

}

void cMtSampleBankEditor::listSampleSlots()
{
	for(uint8_t i = 0; i < SAMPLES_COUNT; i++)
	{
		if(i<9)
		{
			slotNames[i][0] = (i+1)%10 + 48;
			slotNames[i][1] = '.';
			slotNames[i][2] = ' ';
			slotNames[i][3] = 0;
		}
		else
		{
			slotNames[i][0] = (((i+1)-(i)%10)/10) + 48;
			slotNames[i][1] = (i+1)%10 + 48;
			slotNames[i][2] = '.';
			slotNames[i][3] = ' ';
			slotNames[i][4] = 0;
		}

		if(mtProject.sampleBank.sample[i].loaded)
		{
			strcat(&slotNames[i][0], mtProject.sampleBank.sample[i].file_name);
		}

		ptrSlotNames[i] = &slotNames[i][0];
	}

}
void cMtSampleBankEditor::play(uint8_t type)
{
	if(spectrumEnabled)
	{
		if(recordingStatus == recordingStatusRecording)
		{
			padsBacklight.stopBlink(interfacePadRec);
			recorder.stopRecording();
			recordingStatus = recordingStatusRecorded;
		}
		else if(recordingStatus == recordingStatusRecorded) recorder.play(startPoint, stopPoint);
		refreshModule = 1;
		labelsChanged = 1;
		spectrumChanged = 1;
	}
	else
	{
		if(type == 1)
		{
			if(filesListEnabled) playSdFile();
			else playSampleFromBank();
		}
		else
		{
			stopPlaying();
		}
	}
}

void cMtSampleBankEditor::playSdFile()
{
	if(!isWavFile(&locationFilesList[selectedLocation][0])) return;

	char file_path[255];

	strcpy(file_path, filePath);
	strcat(file_path, &locationFilesList[selectedLocation][0]);

	playMode = playModeSdFile;

	engine.prevSdConnect();

	playSdWav.play(file_path);

}

void cMtSampleBankEditor::playSampleFromBank()
{
	playMode = playModeSampleBank;

	instrumentPlayer[0].noteOnforPrev(mtProject.sampleBank.sample[samplesIndex[sampleListPos]].address,
									  mtProject.sampleBank.sample[samplesIndex[sampleListPos]].length);
}


void cMtSampleBankEditor::stopPlaying()
{
	if(playMode == playModeSdFile)
	{
		playSdWav.stop();
		engine.prevSdDisconnect();
	}
	else if(playMode == playModeSampleBank)
	{
		instrumentPlayer[0].noteOff();
	}

	playMode = playModeStop;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

uint8_t cMtSampleBankEditor::loadSamplesBank()
{
	//zaladowanie banku sampli
	char currentPatch[PATCH_SIZE];

	int32_t size;
	mtProject.sampleBank.used_memory = 0;

	mtProject.sampleBank.sample[0].address = sdram_sampleBank;
	mtProject.sampleBank.samples_count = 0;

	for(uint8_t i = 0; i < SAMPLES_COUNT; i++)
	{
		if(fileManager.currentProjectPatch != NULL)
		{
			memset(currentPatch,0,PATCH_SIZE);
			strcpy(currentPatch,fileManager.currentProjectPatch);
			strcat(currentPatch,"/samples/");
			strcat(currentPatch,mtProject.sampleBank.sample[i].file_name);
		}


		if(mtProject.sampleBank.sample[i].type == mtSampleTypeWavetable)
		{

			//size = loadWavetable(mtProject.sampleBank.sample[i].file_name, mtProject.sampleBank.sample[i].address, &mtProject.sampleBank.sample[i].wavetable_window_size);

			//size = loadFullWavetableSerum("DirtySaw",mtProject.sampleBank.sample[i].address);

			size = fmLoadWavetable(currentPatch, mtProject.sampleBank.sample[i].address, &mtProject.sampleBank.sample[i].wavetable_window_size);

		}
		else
		{
			size = fmLoadSample(currentPatch, mtProject.sampleBank.sample[i].address);
		}


		if(size > 0)
		{
			mtProject.sampleBank.used_memory += size*2;
			mtProject.sampleBank.sample[i].loaded = 1;
			mtProject.sampleBank.sample[i].length = size;

			mtProject.sampleBank.samples_count++;
		}
		else
		{
			mtProject.sampleBank.sample[i].loaded = 0;
			mtProject.sampleBank.sample[i].length = 0;
			mtProject.sampleBank.sample[i].file_name[0] = '-';
			mtProject.sampleBank.sample[i].file_name[1] = 'e';
			mtProject.sampleBank.sample[i].file_name[2] = 'm';
			mtProject.sampleBank.sample[i].file_name[3] = 'p';
			mtProject.sampleBank.sample[i].file_name[4] = 't';
			mtProject.sampleBank.sample[i].file_name[5] = 'y';
			mtProject.sampleBank.sample[i].file_name[6] = '-';
			mtProject.sampleBank.sample[i].file_name[7] = 0;
			size = 0;
			//return 2; // blad ladowania wave
		}

		if(i+1 < SAMPLES_COUNT)
		{
			mtProject.sampleBank.sample[i+1].address = mtProject.sampleBank.sample[i].address+size;
		}
		if(mtProject.sampleBank.used_memory > mtProject.sampleBank.max_memory) return 1; // out of memory
	}

	return 0;
}



