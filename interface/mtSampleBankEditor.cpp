
#include "mtDisplay.h"
#include "AnalogInputs.h"

#include "mtAudioEngine.h"
#include "mtInterfaceDefs.h"

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
				for(uint8_t i = 0; i < SAMPLES_MAX; i++)
				{
					samplesNames[i] = mtProject.sampleBank.sample[i].file_name;
				}

				mtDisplay.setList(samples_list_pos, samples_list_pos, 2, 0, samplesNames, mtProject.sampleBank.samples_count);
			}
			else
			{
				samplesNames[0] = buttonFunctionLabels[0]; // przypisanie pustego
				mtDisplay.setList(samples_list_pos, samples_list_pos, 2, 0, samplesNames, 1 );
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
	while(fileName[endPos] != 0 && endPos < 19)
	{
		if(fileName[endPos] > 96 && fileName[endPos] < 123) fileName[endPos] = fileName[endPos] - 32;
		endPos++;
	}

	endPos--;

	if(fileName[endPos] == 'V' && fileName[endPos-1] == 'A' && fileName[endPos-2] == 'W' && fileName[endPos-3] == '.')
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

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
uint8_t cMtSampleBankEditor::padsChange(uint8_t type, uint8_t n, uint8_t velo)
{
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
		else if(n == interfacePadPlay || n == interfacePadStop)
		{
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);
		}
		else if(n==8)
		{
			recorder.startRecording("nazwa.wav", sdram_sampleBank);

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
			recorder.save("\0","nagrywka.wav");
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
		case buttonFunctImportSample		:	importSample();		break;
		case buttonFunctRemoveSample		:	removeSample();		break;
		case buttonFunctRenameSample		:	renameSample();		break;

		case buttonFunctBrowseSelectSample	:	browseSelectSample();	break;
		case buttonFunctBrowseOpenFolder	:	browseOpenFolder();	break;
		case buttonFunctBrowseBack			:	browseBack();		break;
		case buttonFunctBrowseCancel		:	browseCancel();		break;



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
	else
	{
		setButtonFunction(3, buttonFunctImportSample);
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

void cMtSampleBankEditor::importSample()
{
	strcpy(filePath,"/");
	dirLevel = 0;
	selectedLocation = 0;

	filesListChanged = 2;
	filesListEnabled = 1;

}

void cMtSampleBankEditor::removeSample()
{

}

void cMtSampleBankEditor::renameSample()
{

}

void cMtSampleBankEditor::browseSelectSample()
{

}

void cMtSampleBankEditor::browseOpenFolder()
{

}

void cMtSampleBankEditor::browseBack()
{

}

void cMtSampleBankEditor::browseCancel()
{
	filesListChanged = 1;
	filesListEnabled = 0;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtSampleBankEditor::changeSampleListPos(int16_t value)
{

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


