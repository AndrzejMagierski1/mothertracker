#include "mtProjectEditor.h"

#include "mtDisplay.h"
#include "AnalogInputs.h"

#include "mtHardware.h"
#include "SD.h"
#include "sdram.h"

#include "spi_interrupt.h"
#include "sdram.h"

#include "mtFileManager.h"

#include "mtInterfaceDefs.h"


cMtProjectEditor mtProjectEditor;


strMtProject mtProject;

__NOINIT(EXTERNAL_RAM) int16_t sdram_sampleBank[4*1024*1024];


void cMtProjectEditor::update()
{
	if(!refreshModule) return;
	refreshModule = 0;


	//-----------------------------------------------------
	if(moduleStart)
	{
		moduleStart = 0;
		labelsChanged = 2;


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
			if(browseLocationType == browseLocationTypeOpenProject)
			{
				listOnlyFolderNames("/Projects/");
			}
			else if(browseLocationType == browseLocationTypeOpenTemplate)
			{
				listOnlyFolderNames("/Templates/");
			}

			mtDisplay.setList(files_list_pos, files_list_pos, 2, 0, filesNames, locationFilesCount);
		}

		filesListChanged = 0;
	}
	//-----------------------------------------------------
	if(editNameChanged)
	{
		labelsChanged = 1;
		refreshModule = 1;

		if(!editNameEnabled)
		{
			editNameChanged = 0;
			mtDisplay.setTextEdit(0, 0, 0, 0, 0);
			return;
		}

		if(editNameChanged == 2) // pokaz liste
		{
			if(editNameType == editNameTypeSaveProject)
			{

			}

			mtDisplay.setTextEdit(10, 20, 172, editName,editLabel);

		}

		editNameChanged = 0;
	}



}

void cMtProjectEditor::exploreActualLocation()
{
	sdLocation.close();
	sdLocation.open(filePath, O_READ); //"/"
	locationFilesCount = sdLocation.createFilesList(0,locationFilesList, files_list_length_max);
	sdLocation.close();

}


void cMtProjectEditor::listOnlyFolderNames(const char* folder)
{
	sdLocation.close();
	sdLocation.open(folder, O_READ); //"/"
	locationFilesCount = sdLocation.createFilesList(0,locationFilesList, files_list_length_max);
	sdLocation.close();


	uint8_t foundProjectsCount = 0;

	for(uint8_t i = 0; i < locationFilesCount; i++)
	{
		if(locationFilesList[i][0] == '/')	//tylko jesli folder
		{
			strcpy(filePath,"/Projects");
			strcat(filePath,&locationFilesList[i][0]); //doklej nazwe folderu

			sdLocation.open(filePath, O_READ);

			if(sdLocation.exists("project.bin"))	//tylko jesli w folderze jest plik projektu
			{
				strcpy(&locationFilesList[foundProjectsCount][0],&locationFilesList[i][1]);

				foundProjectsCount++;
			}


			sdLocation.close();
		}
	}


	locationFilesCount = foundProjectsCount;

	for(uint8_t i = 0; i < locationFilesCount; i++)
	{
		filesNames[i] = &locationFilesList[i][0];
	}

}


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################


void cMtProjectEditor::start(uint8_t mode)
{

	switch(mode)
	{
	case mtProjectStartModeDoNothing :
	{

		break;
	}
	case mtProjectStartModeOpenLast   :
	{
		uint8_t result = loadLastProject();
		if(result)
		{
			// jesli nie mozna zaladowac ostatneigo projektu
			// to poinformuj o tym interfejs
			eventFunct(mtPriojectEditorEventLoadLastProjFailed, &result,0,0);
		}
		break;
	}
	case mtProjectStartModeOpenProject:
	{

		break;
	}
	case mtProjectStartModeNewProject :
	{

		break;
	}
	default: break;
	}




	moduleStart = 1;
	refreshModule = 1;


}


void cMtProjectEditor::stop()
{


	mtDisplay.setList(0, 0, 0, 0, 0, 0);
	mtDisplay.setList(1, 0, 0, 0, 0, 0);
	mtDisplay.setList(2, 0, 0, 0, 0, 0);
	mtDisplay.setList(3, 0, 0, 0, 0, 0);
	mtDisplay.setList(4, 0, 0, 0, 0, 0);

	mtDisplay.setTextEdit(0, 0, 0, 0, 0);

}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

uint8_t cMtProjectEditor::padsChange(uint8_t type, uint8_t n, uint8_t velo)
{
	if(type == 1)
	{
		if(n == interfacePadPlay || n == interfacePadStop)
		{
			eventFunct(mtPriojectEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadInstrumentEditor)
		{
			stop();
			eventFunct(mtPriojectEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadConfig)
		{
			stop();
			eventFunct(mtPriojectEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadSampleBank)
		{
			stop();
			eventFunct(mtPriojectEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadSettings)
		{
			stop();
			eventFunct(mtPriojectEditorEventPadPress, &n, 0, 0);
		}


	}

	return 0;
}

void cMtProjectEditor::buttonChange(uint8_t button, uint8_t value)
{
	switch(buttonFunctions[button])
	{
	case buttonFunctNone  				: 							break;
	case buttonFunctNewProject			:	newProject(value);		break;
	case buttonFunctOpenProject			:	openProject(value);		break;
	case buttonFunctSaveProject			:	saveProject(value);		break;
	case buttonFunctCopyProject			:	copyProject(value);		break;
	case buttonFunctBrowseOpenSave  	:	browseOpenSave(value);	break;
	case buttonFunctBrowseOpen  		:	browseOpen(value);     	break;
	case buttonFunctBrowseCancel		:	browseCancel(value);  	break;
	case buttonFunctSaveProjectAs		:	saveProjectAs(value);  	break;


	default: break;
	}

	refreshModule = 1;
}

void cMtProjectEditor::potChange(uint8_t pot, int16_t value)
{
	switch(potFunctions[pot])
	{
	case potFunctNone					:	 	break;
	case potFunctChangeProjectsList 	:	changeProjectsListPos(value);	break;
	case potFunctChangeTemplatesList	:	changeTemplatesListPos(value);	break;

	default: break;
	}

	refreshModule = 1;
}

void cMtProjectEditor::seqButtonChange(uint8_t type, uint8_t x, uint8_t y)
{


}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################


void cMtProjectEditor::processLabels()
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
// BUTTONS

void cMtProjectEditor::setButtonLabel(uint8_t function, char* label)
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


void cMtProjectEditor::updateButtonsFunctions()
{
	setButtonFunction(0, buttonFunctNone);
	setButtonFunction(1, buttonFunctNone);
	setButtonFunction(2, buttonFunctNone);
	setButtonFunction(3, buttonFunctNone);
	setButtonFunction(4, buttonFunctNone);

//--------------------------------------------------------

	if(filesListEnabled)
	{
		if(browseLocationType == browseLocationTypeOpenTemplate)
		{
			setButtonFunction(0, buttonFunctBrowseOpenSave);
		}
		else if(browseLocationType == browseLocationTypeOpenProject)
		{
			setButtonFunction(0, buttonFunctBrowseOpen);
		}

		setButtonFunction(1, buttonFunctBrowseCancel);
	}
	else if(editNameEnabled)
	{
		if(editNameType == editNameTypeSaveProject)
		{
			setButtonFunction(0, buttonFunctSaveProjectAs);
		}
		setButtonFunction(1, buttonFunctBrowseCancel);
	}
	else
	{
		setButtonFunction(0, buttonFunctNewProject);
		setButtonFunction(1, buttonFunctOpenProject);


		setButtonFunction(4, buttonFunctSaveProject);
	}

//--------------------------------------------------------

	buttonLabels[0] = (char *)&buttonFunctionLabels[buttonFunctions[0]][0];
	buttonLabels[1] = (char *)&buttonFunctionLabels[buttonFunctions[1]][0];
	buttonLabels[2] = (char *)&buttonFunctionLabels[buttonFunctions[2]][0];
	buttonLabels[3] = (char *)&buttonFunctionLabels[buttonFunctions[3]][0];
	buttonLabels[4] = (char *)&buttonFunctionLabels[buttonFunctions[4]][0];

	mtDisplay.changeButtonsLabels(buttonLabels);
}


void cMtProjectEditor::setButtonFunction(uint8_t number, uint8_t function)
{
	buttonFunctions[number] = function;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// POTS

void cMtProjectEditor::setPotsLabel(uint8_t function, char* label)
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



void cMtProjectEditor::updatePotsFunctions()
{
	setPotFunction(0, potFunctNone);
	setPotFunction(1, potFunctNone);
	setPotFunction(2, potFunctNone);
	setPotFunction(3, potFunctNone);
	setPotFunction(4, potFunctNone);

//--------------------------------------------------------

	if(filesListEnabled)
	{
		if(browseLocationType == browseLocationTypeOpenProject)
		{
			setPotFunction(0, potFunctChangeProjectsList);
		}
		else if(browseLocationType == browseLocationTypeOpenTemplate)
		{
			setPotFunction(0, potFunctChangeTemplatesList);
		}


	}
	else
	{



	}


//--------------------------------------------------------

	potLabels[0] = (char *)&potFunctionLabels[potFunctions[0]][0];
	potLabels[1] = (char *)&potFunctionLabels[potFunctions[1]][0];
	potLabels[2] = (char *)&potFunctionLabels[potFunctions[2]][0];
	potLabels[3] = (char *)&potFunctionLabels[potFunctions[3]][0];
	potLabels[4] = (char *)&potFunctionLabels[potFunctions[4]][0];

	mtDisplay.changePotsLabels(potLabels);
}


void cMtProjectEditor::setPotFunction(uint8_t number, uint8_t function)
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

void cMtProjectEditor::newProject(uint8_t value)
{
	if(value == 1)
	{
		if(!filesListEnabled)
		{
			filesListEnabled = 1;
			filesListChanged = 2;
		}

		browseLocationType = browseLocationTypeOpenTemplate;
		refreshModule = 1;


	}
}

void cMtProjectEditor::openProject(uint8_t value)
{
	if(value == 1)
	{
		if(!filesListEnabled)
		{
			filesListEnabled = 1;
			filesListChanged = 2;
		}

		browseLocationType = browseLocationTypeOpenProject;
		refreshModule = 1;
	}
}

void cMtProjectEditor::saveProject(uint8_t value)
{
	if(value == 1)
	{

		fileManager.saveProject();


	}
}

void cMtProjectEditor::copyProject(uint8_t value)
{
	if(value == 1)
	{

	}
}

void cMtProjectEditor::browseOpenSave(uint8_t value)
{
	if(value == 1)
	{
		if(filesListEnabled)
		{
			filesListEnabled = 0;
			filesListChanged = 1;
		}
		browseLocationType = browseLocationTypeNone;

		if(!editNameEnabled)
		{
			editNameEnabled = 1;
			editNameChanged = 2;
		}
		editNameType = editNameTypeSaveProject;
		refreshModule = 1;
	}
}

void cMtProjectEditor::browseOpen(uint8_t value)
{
	if(value == 1)
	{

//		for(uint8_t i=0; i < INSTRUMENTS_COUNT; i++)
//		{
//			mtProject.mtProjectRemote.instrumentFile[i].index = -1;
//		}
//		for(uint8_t i=0; i < SAMPLES_COUNT; i++)
//		{
//			mtProject.mtProjectRemote.sampleFile[i].index = -1;
//		}
//		for(uint8_t i=0; i < PATTERNS_COUNT; i++)
//		{
//			mtProject.mtProjectRemote.patternFile[i].index = -1;
//		}

		fileManager.openProject(&locationFilesList[selectedLocation][0],projectTypeUserMade);
		loadSamplesBank();

//		fileManager.importSampleToProject(NULL,"c.WAV","1.WAV",0,mtSampleTypeWaveFile);

//		fileManager.createNewProject("Project_Test1");
//		fileManager.importSampleToProject(NULL,"1.WAV","1.WAV",0,0,mtSampleTypeWaveFile);
//		fileManager.importSampleToProject(NULL,"2.WAV","2.WAV",1,1,mtSampleTypeWaveFile);
//		fileManager.importSampleToProject(NULL,"3.WAV","3.WAV",2,2,mtSampleTypeWaveFile);
//		fileManager.importSampleToProject(NULL,"4.WAV","4.WAV",3,3,mtSampleTypeWaveFile);
//		fileManager.importSampleToProject(NULL,"5.WAV","5.WAV",4,4,mtSampleTypeWaveFile);
//		fileManager.importSampleToProject(NULL,"6.WAV","6.WAV",5,5,mtSampleTypeWaveFile);
//		fileManager.importSampleToProject(NULL,"7.WAV","7.WAV",6,6,mtSampleTypeWaveFile);
//		fileManager.importSampleToProject(NULL,"8.WAV","8.WAV",7,7,mtSampleTypeWaveFile);
//
//		fileManager.importSampleToProject(NULL,"11.WAV","11.WAV",8,8,mtSampleTypeWavetable);
//		fileManager.importSampleToProject(NULL,"12.WAV","12.WAV",9,9,mtSampleTypeWavetable);
//		fileManager.importSampleToProject(NULL,"13.WAV","13.WAV",10,10,mtSampleTypeWavetable);
//		fileManager.importSampleToProject(NULL,"14.WAV","14.WAV",11,11,mtSampleTypeWavetable);
//		fileManager.importSampleToProject(NULL,"15.WAV","15.WAV",12,12,mtSampleTypeWavetable);
//		fileManager.importSampleToProject(NULL,"16.WAV","16.WAV",13,13,mtSampleTypeWavetable);
//		fileManager.importSampleToProject(NULL,"17.WAV","17.WAV",14,14,mtSampleTypeWavetable);
//		fileManager.importSampleToProject(NULL,"18.WAV","18.WAV",15,15,mtSampleTypeWavetable);

//		fileManager.saveProject();



		browseCancel(1);
		refreshModule = 1;
	}
}


void cMtProjectEditor::saveProjectAs(uint8_t value)
{
	if(value == 1)
	{

		fileManager.openProject(&locationFilesList[selectedLocation][0],projectTypeExample);
		loadSamplesBank();
		fileManager.saveAsProject(editName);
		fileManager.openProject(editName,projectTypeUserMade);
		loadSamplesBank();

		browseCancel(1);
		refreshModule = 1;
	}
}

void cMtProjectEditor::browseCancel(uint8_t value)
{
	if(value == 1)
	{
		if(filesListEnabled)
		{
			filesListEnabled = 0;
			filesListChanged = 1;
		}
		browseLocationType = browseLocationTypeNone;

		if(editNameEnabled)
		{
			editNameEnabled = 0;
			editNameChanged = 1;
		}
		editNameType = editNameTypeNone;

		refreshModule = 1;
	}
}





void cMtProjectEditor::changeProjectsListPos(int16_t value)
{
	if(selectedLocation + value < 0) selectedLocation  = 0;
	else if(selectedLocation + value > locationFilesCount-1) selectedLocation  = locationFilesCount-1;
	else selectedLocation += value;

	mtDisplay.changeList(files_list_pos, selectedLocation);

	filesListChanged = 1;
	refreshModule = 1;
}

void cMtProjectEditor::changeTemplatesListPos(int16_t value)
{
	if(selectedLocation + value < 0) selectedLocation  = 0;
	else if(selectedLocation + value > locationFilesCount-1) selectedLocation  = locationFilesCount-1;
	else selectedLocation += value;

	mtDisplay.changeList(files_list_pos, selectedLocation);

	filesListChanged = 1;
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





//------------------------------------------------------------------------------
uint8_t cMtProjectEditor::readProjectConfig()
{
	// parametry sampli ==============================================

	// trzeba czytac z pliku projektu zawierajacego opis banku sampli
	// pod jaki index tablicy sampli 0-32 zapisywac dany sampel
	// teraz domyslnie zajmowane 0-7


/*
	for(uint8_t i=0; i < INSTRUMENTS_COUNT; i++)
	{
		mtProject.mtProjectRemote.instrumentFile[i].index = -1;
	}
	for(uint8_t i=0; i < SAMPLES_COUNT; i++)
	{
		mtProject.mtProjectRemote.sampleFile[i].index = -1;
	}
	for(uint8_t i=0; i < PATTERNS_COUNT; i++)
	{
		mtProject.mtProjectRemote.patternFile[i].index = -1;
	}



//	for(uint8_t i = 0; i < 8; i++) // max do 9
//	{
//		mtProject.sampleBank.sample[i].type = mtSampleTypeWaveFile;//;
//		mtProject.sampleBank.sample[i].file_name[0] = i+49;
//		mtProject.sampleBank.sample[i].file_name[1] = '.';
//		mtProject.sampleBank.sample[i].file_name[2] = 'w';
//		mtProject.sampleBank.sample[i].file_name[3] = 'a';
//		mtProject.sampleBank.sample[i].file_name[4] = 'v';
//		mtProject.sampleBank.sample[i].file_name[5] = 0;
//
//		mtProject.sampleBank.sample[i].wavetable_window_size = 1024;
//	}

*/
/*
	for(uint8_t i = 0; i < 8; i++) // max do 9
	{
		mtProject.sampleBank.sample[i+8].type = mtSampleTypeWavetable;
		mtProject.sampleBank.sample[i+8].file_name[0] = 49;
		mtProject.sampleBank.sample[i+8].file_name[1] = i+49;
		mtProject.sampleBank.sample[i+8].file_name[2] = '.';
		mtProject.sampleBank.sample[i+8].file_name[3] = 'w';
		mtProject.sampleBank.sample[i+8].file_name[4] = 'a';
		mtProject.sampleBank.sample[i+8].file_name[5] = 'v';
		mtProject.sampleBank.sample[i+8].file_name[6] = 0;
		mtProject.sampleBank.sample[i+8].wavetable_window_size = 1024;
	}

*/

//	mtProject.sampleBank.sample[1].wavetable_window_size = 1024;
//	mtProject.sampleBank.sample[1].type = mtSampleTypeWaveFile;


	// parametry instrumentow ========================================

	//strcpy(fileManager.currentProjectPatch,"Projects/Project_001");


	mtProject.instruments_count = 8;

	for(uint8_t i = 0; i < mtProject.instruments_count; i++)
	{
		mtProject.instrument[i].sampleIndex = i;
		mtProject.instrument[i].wavetableCurrentWindow = 0;

		mtProject.instrument[i].playMode = 1;

		mtProject.instrument[i].startPoint = 0;
		mtProject.instrument[i].loopPoint1 = 10000;
		mtProject.instrument[i].loopPoint2 = 20000;
		mtProject.instrument[i].endPoint = SAMPLE_POINT_POS_MAX;

		mtProject.instrument[i].envelope[envAmp].delay = 0;

		mtProject.instrument[i].envelope[envAmp].attack = 0;
		mtProject.instrument[i].envelope[envAmp].hold = 0;
		mtProject.instrument[i].envelope[envAmp].decay = 0;
		mtProject.instrument[i].envelope[envAmp].sustain = 1.0;
		mtProject.instrument[i].envelope[envAmp].release = 1000;
		mtProject.instrument[i].envelope[envAmp].amount = 1.0;
		mtProject.instrument[i].envelope[envAmp].enable = envelopeOn;

		mtProject.instrument[i].envelope[envFilter].delay = 0;
		mtProject.instrument[i].envelope[envFilter].attack = 3000;
		mtProject.instrument[i].envelope[envFilter].hold = 0;
		mtProject.instrument[i].envelope[envFilter].decay = 0;
		mtProject.instrument[i].envelope[envFilter].sustain = 1.0;
		mtProject.instrument[i].envelope[envFilter].release = 1000;
		mtProject.instrument[i].envelope[envFilter].amount = 1.0;
		mtProject.instrument[i].envelope[envFilter].enable = envelopeOff;

		mtProject.instrument[i].cutOff = 1.0;
		mtProject.instrument[i].filterEnable = filterOff;
		mtProject.instrument[i].filterType = lowPass;
		mtProject.instrument[i].resonance = 0;
		mtProject.instrument[i].panning = 50;
		mtProject.instrument[i].glide = 10000;
		mtProject.instrument[i].volume = 100;
		mtProject.instrument[i].tune = 0;
		mtProject.instrument[i].fineTune = 0;

		if(i >= 10)
		{
			mtProject.instrument[i].name[0] = (i+1)/10 + 48;
			mtProject.instrument[i].name[1] = (i+1)%10 + 48;
			mtProject.instrument[i].name[2] = 0;
		}
		else
		{
			mtProject.instrument[i].name[0] = (i+1)%10 + 48;
			mtProject.instrument[i].name[1] = 0;
		}

	}

	mtProject.instrument[0].startPoint = 19000;
	mtProject.instrument[0].loopPoint1 = 19000; //PRO
	mtProject.instrument[0].loopPoint2 = 29000;

	mtProject.instrument[0].lfo[lfoA].rate=600;
	mtProject.instrument[0].lfo[lfoA].wave=0;
	mtProject.instrument[0].lfo[lfoA].amount=4095;
	mtProject.instrument[0].lfo[lfoA].sync=0;
	mtProject.instrument[0].lfo[lfoA].enable=lfoOff;



	// parametry paternu ========================================

	return 0;
}

//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::loadSamplesBank()
{
	//zaladowanie banku sampli
	char currentPatch[PATCH_SIZE];

	int32_t size;
	mtProject.sampleBank.used_memory = 0;

	mtProject.sampleBank.sample[0].address = sdram_sampleBank;
	mtProject.sampleBank.samples_count = 0;

	for(uint8_t i = 0; i < SAMPLES_MAX; i++)
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

		if(i+1 < SAMPLES_MAX)
		{
			mtProject.sampleBank.sample[i+1].address = mtProject.sampleBank.sample[i].address+size;
		}
		if(mtProject.sampleBank.used_memory > mtProject.sampleBank.max_memory) return 1; // out of memory
	}

	return 0;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::loadLastProject()
{
	if(readProjectConfig())
	{
		mtPrintln("loading config file failed!");
		return 1;
	}
/*
	if(loadSamplesBank())
	{
		mtPrintln("loading samples failed!");
		return 2;
	}
	else
	{
//		mtPrint(mtProject.sampleBank.samples_count);
//		mtPrintln(" samples loaded successfully");
//		mtPrint("sample memory used: ");
//		mtPrint( int((mtProject.sampleBank.used_memory * 100 ) / mtProject.sampleBank.max_memory));
//		mtPrintln(" %");
	}
*/




	return 0;
}


//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::isProjectLoaded()
{


	return 0;
}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------



int32_t loadSample(const char *filename, int16_t * buf)
{
	strWavFileHeader sampleHead;
	uint16_t bufferLength=0;
	uint32_t accBufferLength=0;
//	uint32_t * bufStart;
	int16_t buf16[256];
	FsFile wavfile;

	//__disable_irq();

	//bufStart = (uint32_t*)buf;
	//buf+=2;

	wavfile = SD.open(filename);
	wavfile.read(&sampleHead, 44);

	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 1  || sampleHead.bitsPerSample != 16  || sampleHead.sampleRate != 44100 )
	{
		wavfile.close();
//		__enable_irq();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
			mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		return -1;
	}
	else
	{
		if(hardwareTest)
		{
			Serial.println("load WAV header to SDRAM succesfull");
			mtPrint("load WAV header to SDRAM succesfull");
		}
	}
/*
	if(sampleHead.numChannels == 1) sampleLength = sampleHead.subchunk2Size;
	else if(sampleHead.numChannels == 2) sampleLength = sampleHead.subchunk2Size/2;
	else
	{
		wavfile.close();
//		__enable_irq();
		return -2;
	}
*/
	if(sampleHead.numChannels == 1)
	{
		while ( wavfile.available() )
		{
			bufferLength = wavfile.read(buf16, 512);

			accBufferLength += bufferLength;

			for(int i=0; i< 256; i++)
			{
				if(bufferLength <= i ) *buf=0;
				else *buf=buf16[i];
				buf++;
			}
		}
	}
	else if (sampleHead.numChannels == 2)
	{
		while (wavfile.available() )
		{

			bufferLength = wavfile.read(buf16, 512);
			//Serial.println(bufferLength);
			accBufferLength += bufferLength;
			for(int i=0; i< 256; i+=2)
			{
				if(bufferLength <= i ) *buf=0;
				else *buf=buf16[i];
				buf++;
			}

		}
	}

	wavfile.close();

//	*bufStart = (accBufferLength/4);
	if(sampleHead.numChannels == 1)
	{
		accBufferLength = accBufferLength/2;
	}
	else if(sampleHead.numChannels == 2)
	{
		accBufferLength = accBufferLength/4;
	}


	return accBufferLength;
}

int32_t loadWavetableStandard(const char *filename, int16_t * buf)
{
	strWavFileHeader sampleHead;
	uint16_t bufferLength=0;
	int16_t buf16[256];
	int16_t buf16_1024[1024];
	int16_t windowDivider=0;
	int16_t currentWave[1024];
	int16_t nextWave[1024];

	FsFile wavfile;

	uint16_t numberOfWindows=0;
	uint16_t missingWindows=0;
	float windowsControl=0;
	float windowsCounter=0;
	uint16_t buforCounter=0;

	wavfile = SD.open(filename);
	wavfile.read(&sampleHead, 44);



	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 1  || sampleHead.bitsPerSample != 16  || sampleHead.sampleRate != 44100 )
	{
		wavfile.close();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
			mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		return -1;
	}
	else
	{
		if(hardwareTest)
		{
			Serial.println("load WAV header to SDRAM succesfull");
			mtPrint("load WAV header to SDRAM succesfull");
		}
	}

	numberOfWindows= (sampleHead.subchunk2Size/2)/STANDARD_WAVETABLE_WINDOW_LEN;
	if(numberOfWindows == STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		if(sampleHead.numChannels == 1)
		{
			while ( wavfile.available() )
			{
				bufferLength = wavfile.read(buf16, 512);
				for(int i=0; i< 256; i++)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=buf16[i];
					buf++;
				}
			}
		}
		else if (sampleHead.numChannels == 2)
		{
			while (wavfile.available() )
			{
				bufferLength = wavfile.read(buf16, 512);
				for(int i=0; i< 256; i+=2)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=buf16[i];
					buf++;
				}
			}
		}

		wavfile.close();

	}

	else if(numberOfWindows > STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		;
	}
	else if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		missingWindows=STANDARD_WAVETABLE_WINDOWS_NUMBER-numberOfWindows;
		windowsControl=(float)STANDARD_WAVETABLE_WINDOWS_NUMBER/(missingWindows+1);

		if(sampleHead.numChannels == 1)
		{
			while ( wavfile.available() )
			{
				if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
				{
					bufferLength=wavfile.read(buf16_1024, 2048);
					for(int i=0; i< 1024; i++)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=buf16_1024[i];
						buf++;
						currentWave[i]=buf16_1024[i];
					}
					buforCounter++;
				}
				else
				{
					bufferLength=wavfile.read(nextWave, 2048);
					windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);

					for(uint8_t i=0;i<windowDivider;i++)
					{
						for(uint16_t j=0; j< 1024; j++)
						{
							*buf=currentWave[j]+(((nextWave[j]-currentWave[j])/(windowDivider+1))*i);
							buf++;
						}
					}
					for(int i=0; i< 1024; i++)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=nextWave[i];
						buf++;

						currentWave[i]=nextWave[i];
					}
					buforCounter++;
					buforCounter+=(windowDivider);
					windowsCounter+=(windowDivider*windowsControl);
				}
			}
		}
		else if (sampleHead.numChannels == 2)
		{
			// nie trzeba ale zostawiam jakieś bałagany
			/*while ( wavfile.available() )
			{
				if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
				{
					bufferLength=wavfile.read(buf16_2048, 4096);
					for(int i=0; i< 2048; i+=2)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=buf16_2048[i];
						buf++;
						currentWave_2048[i]=buf16_2048[i];
					}
					buforCounter++;
				}
				else
				{
					bufferLength=wavfile.read(buf16_2048, 4096);
					for(int i=0; i< 2048; i+=2)
					{
						if(bufferLength <= i ) nextWave_2048[i]=0;
						else nextWave_2048[i]=buf16_2048[i];
					}
					windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);

					for(uint8_t i=0;i<windowDivider;i++)
					{
						for(uint16_t j=0; j< 1024; j++)
						{
							*buf=currentWave_2048[j]+(((nextWave_2048[j]-currentWave_2048[j])/(windowDivider+1))*i);
							buf++;
						}
					}
					for(int i=0; i< 1024; i++)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=nextWave_2048[i];
						buf++;

						currentWave_2048[i]=nextWave_2048[i];
					}
					buforCounter++;
					buforCounter+=(windowDivider);
					windowsCounter+=(windowDivider*windowsControl);
				}
			}*/
		}

		wavfile.close();
	}

	return STANDARD_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
}


int32_t loadWavetableSerum(const char *filename, int16_t * buf)
{
	//strSerumWavetableFileHeader sampleHead;
	strWavFileHeader sampleHead;
	uint16_t bufferLength=0;

	float bufFloat[2048];
	float currentWave[2048];
	float nextWave[2048];

	int16_t windowDivider=0;
	uint16_t numberOfWindows=0;
	uint16_t missingWindows=0;
	float windowsControl=0;
	float windowsCounter=0;
	uint16_t buforCounter=0;


	int16_t currentInt=0;
	int16_t nextInt=0;
	FsFile wavfile;



	wavfile = SD.open(filename);
	readHeader(&sampleHead,&wavfile);



	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 3  || sampleHead.bitsPerSample != 32  || sampleHead.sampleRate != 44100)
	{
		wavfile.close();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
			mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		return -1;
	}
	else
	{
		if(hardwareTest)
		{
			Serial.println("load WAV header to SDRAM succesfull");
			mtPrint("load WAV header to SDRAM succesfull");
		}
	}

	numberOfWindows= (sampleHead.subchunk2Size/4)/SERUM_WAVETABLE_WINDOW_LEN;
	if(numberOfWindows == STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{

		while ( wavfile.available() )
		{
			bufferLength = wavfile.read(bufFloat, 8192);

			for(int i=0; i< 2048; i++)
			{
				if(bufferLength <= i ) *buf=0;
				else *buf=fmap(bufFloat[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,MIN_SIGNED_16BIT,MAX_SIGNED_16BIT);
				buf++;
			}
		}

		wavfile.close();
	}

	else if(numberOfWindows > STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		;
	}
	else if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		missingWindows=STANDARD_WAVETABLE_WINDOWS_NUMBER-numberOfWindows;
		windowsControl=(float)STANDARD_WAVETABLE_WINDOWS_NUMBER/(missingWindows+1);


		while ( wavfile.available() )
		{
			if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
			{
				bufferLength=wavfile.read(bufFloat, 8192);
				for(int i=0; i< 2048; i++)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=fmap(bufFloat[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
					buf++;
					currentWave[i]=bufFloat[i];
				}
				buforCounter++;
			}
			else
			{
				bufferLength=wavfile.read(nextWave, 8192);
				windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);

				for(uint8_t i=0;i<windowDivider;i++)
				{
					for(uint16_t j=0; j< 2048; j++)
					{
						currentInt=fmap(currentWave[j],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
						nextInt=fmap(nextWave[j],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
						*buf=currentInt+(((nextInt-currentInt)/(windowDivider+1))*i);
						buf++;
					}
				}
				for(int i=0; i< 2048; i++)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=fmap(nextWave[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);;
					buf++;

					currentWave[i]=nextWave[i];
				}
				buforCounter++;
				buforCounter+=(windowDivider);
				windowsCounter+=(windowDivider*windowsControl);
			}
		}
		wavfile.close();


	}

	return SERUM_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
}


uint8_t getMiddleWindow(uint16_t cnt, float windowsCnt, float windowsControl)
{
	uint8_t localCounter=0;
	while(cnt >= (uint16_t)windowsCnt)
	{
		localCounter++;
		cnt++;
		windowsCnt+=windowsControl;
	}
	return localCounter;

}

int16_t fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
	  return (int16_t)( (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void readHeader(strWavFileHeader* header, FsFile * wavfile)
{
	uint8_t tab[200];
	uint8_t headerSize=0;
	wavfile->read(tab,200);
	header->chunkId = *( (uint32_t *)(&tab[0]) );
	header->chunkSize = *( (uint32_t *)(&tab[4]) );
	header->format = *( (uint32_t *)(&tab[8]) );
	for(int i=12;i<200;i++)
	{
		if((tab[i] == 'f') && (tab[i+1] == 'm') && (tab[i+2] == 't') && (tab[i+3] == ' '))
		{
			header->subchunk1Id = *((uint32_t *)(&tab[i]));
			header->subchunk1Size = *((uint32_t *)(&tab[i+4]));
			header->AudioFormat = *((uint16_t *)(&tab[i+8]));
			header->numChannels = *((uint16_t *)(&tab[i+10]));
			header->sampleRate = *((uint32_t *)(&tab[i+12]));
			header->byteRate = *((uint32_t *)(&tab[i+16]));
			header->blockAlign = *((uint16_t *)(&tab[i+20]));
			header->bitsPerSample = *((uint16_t *)(&tab[i+22]));
		}

		if((tab[i] == 'd') && (tab[i+1] == 'a') && (tab[i+2] == 't') && (tab[i+3] == 'a'))
		{
			header->subchunk2Id = *((uint32_t *)(&tab[i]));
			header->subchunk2Size = *((uint32_t *)(&tab[i+4]));
			headerSize=i+8;
			wavfile->seek(headerSize);
			break;
		}

	}
}

int32_t loadWavetable(const char *filename, int16_t * buf ,uint16_t * windowSize)
{
	strWavFileHeader sampleHead;
	FsFile wavfile;
	int32_t size=-1;

	wavfile = SD.open(filename);
	readHeader(&sampleHead,&wavfile);
	wavfile.close();
	if(sampleHead.AudioFormat == 1)
	{
		size=loadWavetableStandard(filename,buf);
		*windowSize = STANDARD_WAVETABLE_WINDOW_LEN;
	}
	else if(sampleHead.AudioFormat == 3)
	{
		size=loadWavetableSerum(filename,buf);
		*windowSize = SERUM_WAVETABLE_WINDOW_LEN;
	}
	return size;

}
/*int32_t loadFullWavetableSerum(const char *baseName, int16_t * buf)
{
	strWavFileHeader sampleHead;
	uint8_t tabSize=0;
	uint16_t bufferLength=0;
	float currentWave[2048];
	FsFile wavfile;
	while( *(baseName+tabSize) != 0 )
	{
		tabSize++;
	}
	tabSize+=9;


	char name[tabSize];

	for(uint8_t i=0;i< tabSize; i++)
	{
		if(i< (tabSize-9)) name[i]=baseName[i];
		else if(i== (tabSize -1 ) ) name[i] = 0;
		else if(i== (tabSize -2 ) ) name[i] = 'V';
		else if(i== (tabSize -3 ) ) name[i] = 'A';
		else if(i== (tabSize -4 ) ) name[i] = 'W';
		else if(i== (tabSize -5 ) ) name[i] = '.';
		else if(i== (tabSize -6 ) ) name[i] = '1';
		else if(i== (tabSize -9 ) ) name[i] = '_';
		else name[i] ='0';
	}

	for(uint16_t a=0; a < STANDARD_WAVETABLE_WINDOWS_NUMBER;a++)
	{
		wavfile = SD.open(name);
		wavfile.read(&sampleHead,44);

		bufferLength = wavfile.read(currentWave, 8192);

		for(int j=0; j< SERUM_WAVETABLE_WINDOW_LEN; j++)
		{
			if(bufferLength <= j ) *buf=0;
			else *buf=fmap(currentWave[j],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,MIN_SIGNED_16BIT,MAX_SIGNED_16BIT);
			buf++;
		}
		wavfile.close();

		if(name[tabSize - 6 ] < '9') name[tabSize - 6 ]++;
		else if(name[tabSize - 6 ] == '9')
		{
			name[tabSize - 6 ] = '0';
			if( name[tabSize - 7 ] < '9') name[tabSize - 7 ]++;
			else if(name[tabSize - 7 ] == '9')
			{
				name[tabSize - 7 ]= '0';
				name[tabSize - 8]++;
			}
		}
	}

	return STANDARD_WAVETABLE_WINDOWS_NUMBER* SERUM_WAVETABLE_WINDOW_LEN;

}*/

void loadFullWavetableSerum(const char *baseName)
{
	strWavFileHeader sampleHead;
	uint8_t tabSize=0;
	uint8_t newTabSize=0;
	float currentWave[2048];
	FsFile wavfile;
	FsFile result;
	while( *(baseName+tabSize) != 0 )
	{
		tabSize++;
	}
	newTabSize=tabSize+5;
	tabSize+=9;

	char newName[newTabSize];
	char name[tabSize];

	for(uint8_t i=0;i< (newTabSize - 5); i++)
	{
		newName[i]=baseName[i];
	}
	newName[newTabSize -1] = 0;
	newName[newTabSize -2] = 'v';
	newName[newTabSize -3] = 'a';
	newName[newTabSize -4] = 'w';
	newName[newTabSize -5] = '.';

	if(SD.exists(newName)) return;
	for(uint8_t i=0;i< tabSize; i++)
	{
		if(i< (tabSize-9)) name[i]=baseName[i];
		else if(i== (tabSize -1 ) ) name[i] = 0;
		else if(i== (tabSize -2 ) ) name[i] = 'V';
		else if(i== (tabSize -3 ) ) name[i] = 'A';
		else if(i== (tabSize -4 ) ) name[i] = 'W';
		else if(i== (tabSize -5 ) ) name[i] = '.';
		else if(i== (tabSize -6 ) ) name[i] = '1';
		else if(i== (tabSize -9 ) ) name[i] = '_';
		else name[i] ='0';
	}


	result = SD.open(newName,FILE_WRITE);
	for(uint16_t a=0; a < STANDARD_WAVETABLE_WINDOWS_NUMBER;a++)
	{

		if(!a)
		{
			wavfile = SD.open(name);
			wavfile.read(&sampleHead,44);

			sampleHead.subchunk2Size=STANDARD_WAVETABLE_WINDOWS_NUMBER* SERUM_WAVETABLE_WINDOW_LEN*4;
			result.write(&sampleHead,44);
		}
		else
		{
			wavfile = SD.open(name);
			wavfile.read(&sampleHead,44);
		}


		wavfile.read(currentWave, 8192);
		result.write(currentWave, 8192);


		wavfile.close();

		if(name[tabSize - 6 ] < '9') name[tabSize - 6 ]++;
		else if(name[tabSize - 6 ] == '9')
		{
			name[tabSize - 6 ] = '0';
			if( name[tabSize - 7 ] < '9') name[tabSize - 7 ]++;
			else if(name[tabSize - 7 ] == '9')
			{
				name[tabSize - 7 ]= '0';
				name[tabSize - 8]++;
			}
		}
	}
	result.close();


}
