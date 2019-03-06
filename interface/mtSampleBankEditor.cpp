
#include "mtDisplay.h"
#include "AnalogInputs.h"

#include "mtAudioEngine.h"
#include "mtInterfaceDefs.h"
#include "mtFileManager.h"


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
	if(moduleStart)
	{
		moduleStart = 0;
		labelsChanged = 2;
		samplesListChanged = 2;

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
	if(samplesListChanged)
	{
		labelsChanged = 1;
		refreshModule = 1;

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

				mtDisplay.setList(samples_list_pos, samples_list_pos, 2, 0, ptrSamplesNames, mtProject.sampleBank.samples_count);
			}
			else
			{
				ptrSamplesNames[0] = buttonFunctionLabels[0]; // przypisanie pustego
				mtDisplay.setList(samples_list_pos, samples_list_pos, 2, 0, ptrSamplesNames, 1 );
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
			mtDisplay.setList(files_list_pos, 0, 0, 0, 0, 0);
			return;
		}

		if(filesListChanged == 2) // pokaz liste
		{
			listOnlyDirAndWavFromActualPath();

			getSelectedFileType();

			mtDisplay.setList(files_list_pos, files_list_pos, 2, selectedLocation, filesNames, locationFilesCount);
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
			mtDisplay.setList(slot_list_index, 0, 0, 0, 0, 0);
			return;
		}

		if(slotListChanged == 2) // pokaz liste
		{
			listSampleSlots();

			mtDisplay.setList(slot_list_index, files_list_pos, 2, selectedSlot, ptrSlotNames, SAMPLES_COUNT);
		}

		slotListChanged = 0;
	}
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtSampleBankEditor::start()
{
	moduleStart = 1;
	refreshModule = 1;



}

void cMtSampleBankEditor::stop()
{
	mtDisplay.setList(samples_list_pos, 0, 0, 0, 0, 0);
	mtDisplay.setList(files_list_pos, 0, 0, 0, 0, 0);
	filesListEnabled = 0;

	if(playMode != playModeStop)
	{
		stopPlaying();
	}
	//filesListChanged = 1;
}

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
//#########################################################################################################
//#########################################################################################################
uint8_t cMtSampleBankEditor::padsChange(uint8_t type, uint8_t n, uint8_t velo)
{

	if(n == interfacePadPlay)
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

	if(type == 1)
	{
		if(n == interfacePadInstrumentEditor)
		{
			stop();
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadProjectEditor)
		{
			stop();
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadConfig)
		{
			stop();
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadSettings)
		{
			stop();
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadPlay)
		{

		}
		else if(n == interfacePadStop)
		{
			stopPlaying();
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);
		}
		else if(n==8)
		{
			recorder.startRecording(sdram_sampleBank);

		}
		else if(n==9)
		{
			recorder.stopRecording();
		}
		else if(n==10)
		{
			recorder.play(0, MAX_16BIT);
		}
		else if(n==11)
		{
			recorder.stop();
		}
		else if(n==12)
		{
			recorder.save();
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

			size = loadWavetable(currentPatch, mtProject.sampleBank.sample[i].address, &mtProject.sampleBank.sample[i].wavetable_window_size);

		}
		else
		{
			size = loadSample(currentPatch, mtProject.sampleBank.sample[i].address);
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

	mtDisplay.changeButtonsLabels(buttonLabels);
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

	mtDisplay.changeButtonsLabels(buttonLabels);
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

	mtDisplay.changePotsLabels(potLabels);
}



void cMtSampleBankEditor::updatePotsFunctions()
{
	setPotFunction(0, potFunctNone);
	setPotFunction(1, potFunctNone);
	setPotFunction(2, potFunctNone);
	setPotFunction(3, potFunctNone);
	setPotFunction(4, potFunctNone);

//--------------------------------------------------------

	setPotFunction(3, potFunctChangeSamplesListPos);

	if(filesListEnabled)
	{
		setPotFunction(0, potFunctChangeFileListPos);
	}
	else if(slotListEnabled)
	{
		setPotFunction(0, potFunctChangeSlotListPos);
	}


//--------------------------------------------------------

	potLabels[0] = (char *)&potFunctionLabels[potFunctions[0]][0];
	potLabels[1] = (char *)&potFunctionLabels[potFunctions[1]][0];
	potLabels[2] = (char *)&potFunctionLabels[potFunctions[2]][0];
	potLabels[3] = (char *)&potFunctionLabels[potFunctions[3]][0];
	potLabels[4] = (char *)&potFunctionLabels[potFunctions[4]][0];

	mtDisplay.changePotsLabels(potLabels);
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


	mtDisplay.changeList(samples_list_pos, sampleListPos);


	refreshModule = 1;
}

void cMtSampleBankEditor::changeFilesListPos(int16_t value)
{
	if(selectedLocation + value < 0) selectedLocation  = 0;
	else if(selectedLocation + value > locationFilesCount-1) selectedLocation  = locationFilesCount-1;
	else selectedLocation += value;

	getSelectedFileType();

	mtDisplay.changeList(files_list_pos, selectedLocation);

	labelsChanged = 1;

//	filesListChanged = 1;
	refreshModule = 1;
}

void cMtSampleBankEditor::changeSlotsListPos(int16_t value)
{
	if(selectedSlot + value < 0) selectedSlot  = 0;
	else if(selectedSlot + value > SAMPLES_MAX) selectedSlot  = SAMPLES_MAX;
	else selectedSlot += value;

	mtDisplay.changeList(slot_list_index, selectedSlot);


//	filesListChanged = 1;
	refreshModule = 1;
}
