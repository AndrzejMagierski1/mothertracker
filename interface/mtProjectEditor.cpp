

#include "mtDisplay.h"
#include "AnalogInputs.h"

#include "mtHardware.h"
#include "SD.h"
#include "sdram.h"

#include "spi_interrupt.h"
#include "sdram.h"

#include "mtFileManager.h"

#include "mtInterfaceDefs.h"


#include "mtProjectEditor.h"

#include "mtInterface.h"

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
				listOnlyFolderNames("/Templates");
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
	strcpy(filePath, folder);
	strcat(filePath,"/");
	sdLocation.close();
	sdLocation.open(folder, O_READ); //"/"
	locationFilesCount = sdLocation.createFilesList(0,locationFilesList, files_list_length_max);
	sdLocation.close();


	uint8_t foundProjectsCount = 0;

	for(uint8_t i = 0; i < locationFilesCount; i++)
	{
		if(locationFilesList[i][0] == '/')	//tylko jesli folder
		{
			strcpy(filePath, folder);
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
	case buttonFunctCreateNewTemplate   :	createNewTemplate(value);  	break;

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

			setButtonFunction(4, buttonFunctCreateNewTemplate);

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

		//fileManager.saveProject();
		fileManager.addInstrumentToProject(9);

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

		fileManager.openProject(&locationFilesList[selectedLocation][0],projectTypeUserMade);
		loadSamplesBank();

		browseCancel(1);
		refreshModule = 1;
	}
}



void cMtProjectEditor::saveProjectAs(uint8_t value)
{
	if(value == 1)
	{
		fileManager.openProject(&locationFilesList[selectedLocation][0],projectTypeExample);
		//loadSamplesBank();
		fileManager.saveAsProject(editName);
		fileManager.openProject(editName,projectTypeUserMade);
		loadSamplesBank();

		browseCancel(1);
		refreshModule = 1;
	}
}

void cMtProjectEditor::createNewTemplate(uint8_t value)
{
	if(value == 1)
	{
		// utworz nowy czysty projekt w templates
		fileManager.createEmptyTemplateProject((char*)"New");

		//
		filesListEnabled = 1;
		filesListChanged = 2;

		browseLocationType = browseLocationTypeOpenTemplate;
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

/*

	if(readProjectConfig())
	{
		mtPrintln("loading config file failed!");
		return 1;
	}

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

