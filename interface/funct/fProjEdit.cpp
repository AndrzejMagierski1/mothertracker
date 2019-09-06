


#include <projectEditor.h>
#include "mtFileManager.h"
#include "mtAudioEngine.h"
#include "mtLED.h"

enum valueMapDirecion
{
	valueMapDirectionLeft,
	valueMapDirectionRight,
	valueMapDirectionUp,
	valueMapDirectionDown
};

constexpr char smallKeyboard[KEYBOARD_SIZE] =
{
		 	 '1', '2', '3' ,'4' ,'5' ,'6' ,'7' ,'8' ,'9' ,'0' , 0 ,
		 	 'q', 'w', 'e' ,'r' ,'t' ,'y' ,'u' ,'i' ,'o' ,'p' ,'-' ,'+',
			 'a', 's', 'd' ,'f' ,'g' ,'h' ,'j' ,'k' ,'l' ,'@' , 1 ,
			 'z', 'x', 'c' ,'v' ,'b' ,'n' ,'m' , ' '
};

constexpr char bigKeyboard[KEYBOARD_SIZE] =
{
			 '1', '2', '3' ,'4' ,'5' ,'6' ,'7' ,'8' ,'9' ,'0' , 0 ,
		 	 'Q', 'W', 'E' ,'R' ,'T' ,'Y' ,'U' ,'I' ,'O' ,'P' ,'-' ,'+',
			 'A', 'S', 'D' ,'F' ,'G' ,'H' ,'J' ,'K' ,'L', '@', 1 ,
			 'Z', 'X', 'C' ,'V' ,'B' ,'N' ,'M' , ' '
};

//constexpr uint8_t valueMap[4][42] = przechodzenie przez sciany
//{
//		{
//			10,0,1,2,3,4,5,6,7,8,9,
//			22,11,12,13,14,15,16,17,18,19,20,21,
//			33,23,24,25,26,27,28,29,30,31,32,
//			41,34,35,36,37,38,39,40
//		},
//
//		{
//			1,2,3,4,5,6,7,8,9,10,0,
//			12,13,14,15,16,17,18,19,20,21,22,11,
//			24,25,26,27,28,29,30,31,32,33,23,
//			35,36,37,38,39,40,41,34
//		},
//
//		{
//			34,35,36,37,38,39,40,41,41,41,41,
//			0,1,2,3,4,5,6,7,8,9,10,10,
//			11,12,13,14,15,16,17,18,19,20,21,
//			23,24,25,26,27,28,29,30,
//		},
//
//		{
//			11,12,13,14,15,16,17,18,19,20,21,
//			23,24,25,26,27,28,29,30,31,32,33,33,
//			34,35,36,37,38,39,40,41,41,41,41,
//			0,1,2,3,4,5,6,7
//		},
//};

constexpr uint8_t valueMap[4][42] =
{
		{
			0,0,1,2,3,4,5,6,7,8,9,
			11,11,12,13,14,15,16,17,18,19,20,21,
			23,23,24,25,26,27,28,29,30,31,32,
			34,34,35,36,37,38,39,40
		},

		{
			1,2,3,4,5,6,7,8,9,10,10,
			12,13,14,15,16,17,18,19,20,21,22,22,
			24,25,26,27,28,29,30,31,32,33,33,
			35,36,37,38,39,40,41,41
		},

		{
			0,1,2,3,4,5,6,7,8,9,10,
			0,1,2,3,4,5,6,7,8,9,10,10,
			11,12,13,14,15,16,17,18,19,20,21,
			23,24,25,26,27,28,29,30,
		},

		{
			11,12,13,14,15,16,17,18,19,20,21,
			23,24,25,26,27,28,29,30,31,32,33,33,
			34,35,36,37,38,39,40,41,41,41,41,
			34,35,36,37,38,39,40,41
		},
};

constexpr uint8_t valueMapPads[48] =
{
	0,1,2,3,4,5,6,7,8,9,10,10,
	11,12,13,14,15,16,17,18,19,20,21,22,
	23,24,25,26,27,28,29,30,31,32,33,33,
	34,35,36,37,38,39,40,41,41,41,41,41
};

constexpr uint8_t keyPositionToPads[42] =
{
	0,1,2,3,4,5,6,7,8,9,10,
	12,13,14,15,16,17,18,19,20,21,22,23,
	24,25,26,27,28,29,30,31,32,33,34,
	36,37,38,39,40,41,42,43
};

constexpr uint8_t BACKSPACE_PAD_1 = 10;
constexpr uint8_t BACKSPACE_PAD_2 = 11;

constexpr uint8_t CAPS_LOCK_PAD_1 = 34;
constexpr uint8_t CAPS_LOCK_PAD_2 = 35;

constexpr uint8_t SPACE_PAD_1 = 43;
constexpr uint8_t SPACE_PAD_2 = 44;
constexpr uint8_t SPACE_PAD_3 = 45;
constexpr uint8_t SPACE_PAD_4 = 46;
constexpr uint8_t SPACE_PAD_5 = 47;

constexpr uint8_t F_PAD = 27;

constexpr uint8_t J_PAD = 30;

cProjectEditor projectEditor;
cProjectEditor* PE = &projectEditor;

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

uint8_t functShowProjectsList();
uint8_t functShowTemplatesList();
uint8_t functCancelList();
uint8_t functSaveProject();
uint8_t functOpenProject();
uint8_t functOpenTemplate();
uint8_t functCreateNewTemplate();

static uint8_t functEnterName();
static uint8_t functSwitchModule(uint8_t button);

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();
static  uint8_t functConfirmKey();

void cProjectEditor::update()
{
	if(projectOptions > 0)
	{
		switch(projectOptions)
		{
			case mtProjectStartModeOpenLast:
			{
				listOnlyFolderNames("/Projects/");

				fileManager.openProject(&locationFilesList[selectedLocation][0],projectTypeUserMade);

				loadProjectValues();

				functSwitchModule(interfaceButtonPattern);
				break;
			}
			default: break;
		}

		projectOptions = 0;
	}
}

void cProjectEditor::start(uint32_t options)
{

	selectedLocation = 0;

	if(options == mtProjectStartModeOpenLast)
	{
		moduleRefresh = 1;

		projectOptions = mtProjectStartModeOpenLast;

		return;
	}

	FM->setPadsGlobal(functPads);

	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonEnvelopes, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);


	showDefaultScreen();
	setDefaultScreenFunct();



	//typedef void (cProjectEditor::*funct1) (void);
	//funct1 = &cProjectEditor::functOpenProject;
	//(this->*funct1)();

}

void cProjectEditor::stop()
{
	moduleRefresh = 0;
	projectOptions = 0;

}

void cProjectEditor::setDefaultScreenFunct()
{
	//funkcje
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();
	PE->keyboardActiveFlag = 0;
/*
	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	FM->setButtonObj(interfaceButtonShift, functShift);
	FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);
*/
	FM->setButtonObj(interfaceButton0, buttonPress, functShowTemplatesList);
	FM->setButtonObj(interfaceButton1, buttonPress, functShowProjectsList);
	FM->setButtonObj(interfaceButton4, buttonPress, functSaveProject);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

}
//==============================================================================================================
//==============================================================================================================

uint8_t cProjectEditor::loadProjectValues()
{
	engine.setHeadphonesVolume(mtProject.values.volume);
	engine.setReverbRoomsize(mtProject.values.reverbRoomSize);
	engine.setReverbDamping(mtProject.values.reverbDamping);
	engine.setLimiterAttack(mtProject.values.limiterAttack);
	engine.setLimiterRelease(mtProject.values.limiterRelease);
	engine.setLimiterTreshold(mtProject.values.limiterTreshold);

}

//==============================================================================================================
//==============================================================================================================
//==============================================================================================================

uint8_t functShowProjectsList()
{
	PE->listOnlyFolderNames("/Projects/");


	PE->showProjectsList();


// funkcje
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	PE->FM->clearAllPots();

	PE->FM->setPotObj(interfacePot0, &PE->selectedLocation, 0, PE->locationFilesCount-1, 1, PE->fileListControl);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functOpenProject);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList);


	return 1;
}


uint8_t functShowTemplatesList()
{
	PE->listOnlyFolderNames("/Templates/");


	PE->showTemplatesList();




// funkcje
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	PE->FM->clearAllPots();

	PE->FM->setPotObj(interfacePot0, &PE->selectedLocation, 0, PE->locationFilesCount-1, 1, PE->fileListControl);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functEnterName);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList);
	PE->FM->setButtonObj(interfaceButton4, buttonPress, functCreateNewTemplate);

	return 1;
}

uint8_t functCancelList()
{
	PE->showDefaultScreen();
	PE->setDefaultScreenFunct();
	return 1;
}

uint8_t functSaveProject()
{
	fileManager.saveProject();

	return 1;
}

uint8_t functOpenProject()
{

	fileManager.openProject(&PE->locationFilesList[PE->selectedLocation][0],projectTypeUserMade);
//
//			for(uint8_t i=0; i < 8; i++)
//			{
//				mtProject.mtProjectRemote.instrumentFile[i].index = -1;
//			}
//			for(uint8_t i=0; i < 8; i++)
//			{
//				mtProject.mtProjectRemote.sampleFile[i].index = -1;
//			}


/*
	fileManager.createNewProject("Project_Test1");

	fileManager.importSampleToProject(NULL,"1.WAV","1.WAV",0,0,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"2.WAV","2.WAV",1,1,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"3.WAV","3.WAV",2,2,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"4.WAV","4.WAV",3,3,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"5.WAV","5.WAV",4,4,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"6.WAV","6.WAV",5,5,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"7.WAV","7.WAV",6,6,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"8.WAV","8.WAV",7,7,mtSampleTypeWaveFile);


	fileManager.saveProject();
*/



	//PE->eventFunct(mtProjectEditorEventLoadSampleBank, 0, 0, 0);


	PE->loadProjectValues();


	PE->showDefaultScreen();
	PE->setDefaultScreenFunct();

	return 1;
}

uint8_t functOpenTemplate()
{
	fileManager.openProject(&PE->locationFilesList[PE->selectedLocation][0],projectTypeExample);
	//loadSamplesBank();
	if(fileManager.saveAsProject(PE->name) == 2) Serial.println("Zrobic proceduere"); //todo: zrobic procedure
	fileManager.openProject(PE->name, projectTypeUserMade);
	//loadSamplesBank();
	//PE->eventFunct(mtProjectEditorEventLoadSampleBank, 0, 0, 0);

	PE->showDefaultScreen();
	PE->setDefaultScreenFunct();

	return 1;
}

uint8_t functCreateNewTemplate()
{
	fileManager.createEmptyTemplateProject((char*)"New");
	functShowTemplatesList();

	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{

	PE->eventFunct(eventSwitchModule,PE,&button,0);

	return 1;
}
//======================================================================================================================


void cProjectEditor::listOnlyFolderNames(const char* folder)
{
	char filePath[256] = {0};
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

static uint8_t functEnterName()
{

	char localPatch[128];
	uint16_t cnt=1;
	char cntBuf[5];


	do
	{
	   memset(cntBuf,0,5);
	   sprintf(cntBuf, "%d", cnt);
	   strcpy(PE->name,"New Project");
	   strcat(PE->name,cntBuf);

	   strcpy(localPatch,"Projects/");
	   strcat(localPatch, PE->name);

	   cnt++;
	   if(cnt > 9999)
	   {
		   memset(PE->name,0,33);
		   break;
	   }
	} while(SD.exists(localPatch));

	PE->editPosition = strlen(PE->name);
	PE->keyboardPosition = BACKSPACE_PAD_1;
	PE->lastPressedPad = BACKSPACE_PAD_1;
	leds.setLED(BACKSPACE_PAD_1, 1, 31);
	leds.setLED(BACKSPACE_PAD_2, 1, 31);


	PE->showEnterNameKeyboard();
	PE->keyboardActiveFlag = 1;


// funkcje
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	PE->FM->clearAllPots();

//	PE->FM->setPotObj(interfacePot0, &PE->selectedLocation, 0, PE->locationFilesCount-1, 1, PE->fileListControl);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functOpenTemplate);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList);
	PE->FM->setButtonObj(interfaceButton4, buttonPress, functCreateNewTemplate);
	PE->FM->setButtonObj(interfaceButtonEnter, buttonPress, functConfirmKey);


	return 1;
}

static  uint8_t functLeft()
{
	if(PE->keyboardActiveFlag)
	{
		PE->keyboardPosition = valueMap[valueMapDirectionLeft][PE->keyboardPosition];
		PE->showKeyboard();
		return 1;
	}
	return 0;
}
static  uint8_t functRight()
{
	if(PE->keyboardActiveFlag)
	{
		PE->keyboardPosition = valueMap[valueMapDirectionRight][PE->keyboardPosition];
		PE->showKeyboard();
		return 1;
	}
	return 0;
}
static  uint8_t functUp()
{
	if(PE->keyboardActiveFlag)
	{
		PE->keyboardPosition = valueMap[valueMapDirectionUp][PE->keyboardPosition];
		PE->showKeyboard();
		return 1;
	}
	return 0;
}
static  uint8_t functDown()
{
	if(PE->keyboardActiveFlag)
	{
		PE->keyboardPosition = valueMap[valueMapDirectionDown][PE->keyboardPosition];
		PE->showKeyboard();
		return 1;
	}
	return 0;
}

static uint8_t functConfirmKey()
{
	if(PE->keyboardActiveFlag)
	{
		if(PE->editPosition > 31) return 1;

		//****************************************************ledy
		if(PE->lastPressedPad == BACKSPACE_PAD_1 || PE->lastPressedPad == BACKSPACE_PAD_2) //backspace
		{
			leds.setLED(BACKSPACE_PAD_1, 0, 0);
			leds.setLED(BACKSPACE_PAD_2, 0, 0);
		}
		else if(PE->lastPressedPad == CAPS_LOCK_PAD_1 || PE->lastPressedPad == CAPS_LOCK_PAD_2) //capslock
		{
			if(PE->keyboardShiftFlag)
			{
				leds.setLED(CAPS_LOCK_PAD_1, 1, 10);
				leds.setLED(CAPS_LOCK_PAD_2, 1, 10);
			}
			else
			{
				leds.setLED(CAPS_LOCK_PAD_1, 0, 0);
				leds.setLED(CAPS_LOCK_PAD_2, 0, 0);
			}

		}
		else if(PE->lastPressedPad >= SPACE_PAD_1 && PE->lastPressedPad <= SPACE_PAD_5) //space
		{
			for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
			{
				leds.setLED(i, 0, 0);
			}
		}
		else
		{
			if(PE->lastPressedPad != F_PAD && PE->lastPressedPad != J_PAD) leds.setLED(PE->lastPressedPad,0,0);
			else leds.setLED(PE->lastPressedPad,1,10);
		}


		PE->lastPressedPad = keyPositionToPads[PE->keyboardPosition];

		if(keyPositionToPads[PE->keyboardPosition] == BACKSPACE_PAD_1 || keyPositionToPads[PE->keyboardPosition] == BACKSPACE_PAD_2) //backspace
		{
			leds.setLED(BACKSPACE_PAD_1, 1, 31);
			leds.setLED(BACKSPACE_PAD_2, 1, 31);
		}
		else if(keyPositionToPads[PE->keyboardPosition] == CAPS_LOCK_PAD_1 || keyPositionToPads[PE->keyboardPosition] == CAPS_LOCK_PAD_2) //capslock
		{
			leds.setLED(CAPS_LOCK_PAD_1, 1, 31);
			leds.setLED(CAPS_LOCK_PAD_2, 1, 31);
		}
		else if(keyPositionToPads[PE->keyboardPosition] >= SPACE_PAD_1 && keyPositionToPads[PE->keyboardPosition] <=SPACE_PAD_5) //space
		{
			for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
			{
				leds.setLED(i, 1, 31);
			}
		}
		else
		{
			leds.setLED(keyPositionToPads[PE->keyboardPosition],1,31);
		}
		//////////////////////////////////////
		if(smallKeyboard[PE->keyboardPosition] > 1)
		{
			if(PE->editPosition == 31) return 1;
			uint8_t localNameLen = strlen(PE->name);
			if(PE->editPosition < localNameLen)
			{
				for(uint8_t i = localNameLen; i >= PE->editPosition ; i-- )
				{
					PE->name[i+1] = PE->name[i];
				}
			}

			PE->name[PE->editPosition] = PE->keyboardShiftFlag ? bigKeyboard[PE->keyboardPosition] : smallKeyboard[PE->keyboardPosition];

			PE->editPosition++;
		}
		else if(smallKeyboard[PE->keyboardPosition] == 0)
		{
			if(PE->editPosition == 0 ) return 1;

			PE->name[PE->editPosition-1] = 0;
			PE->editPosition--;


		}
		else if(smallKeyboard[PE->keyboardPosition] == 1)
		{
			PE->keyboardShiftFlag = ! PE->keyboardShiftFlag;
			PE->showKeyboard();

		}

		PE->showKeyboardEditName();
		return 1;
	}
	return 0;
}

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if((state == 1) || (state == 2))
	{
		if(PE->keyboardActiveFlag)
		{
			if(PE->lastPressedPad == BACKSPACE_PAD_1 || PE->lastPressedPad == BACKSPACE_PAD_2) //backspace
			{
				leds.setLED(BACKSPACE_PAD_1, 0, 0);
				leds.setLED(BACKSPACE_PAD_2, 0, 0);
			}
			else if(PE->lastPressedPad == CAPS_LOCK_PAD_1 || PE->lastPressedPad == CAPS_LOCK_PAD_2) //capslock
			{
				if(PE->keyboardShiftFlag)
				{
					leds.setLED(CAPS_LOCK_PAD_1, 1, 10);
					leds.setLED(CAPS_LOCK_PAD_2, 1, 10);
				}
				else
				{
					leds.setLED(CAPS_LOCK_PAD_1, 0, 0);
					leds.setLED(CAPS_LOCK_PAD_2, 0, 0);
				}

			}
			else if(PE->lastPressedPad >= SPACE_PAD_1 && PE->lastPressedPad <= SPACE_PAD_5) //space
			{
				for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
				{
					leds.setLED(i, 0, 0);
				}
			}
			else
			{
				if(PE->lastPressedPad != F_PAD && PE->lastPressedPad != J_PAD) leds.setLED(PE->lastPressedPad,0,0);
				else leds.setLED(PE->lastPressedPad,1,10);
			}


			PE->lastPressedPad = pad;

			if(pad == BACKSPACE_PAD_1 || pad == BACKSPACE_PAD_2) //backspace
			{
				leds.setLED(BACKSPACE_PAD_1, 1, 31);
				leds.setLED(BACKSPACE_PAD_2, 1, 31);
			}
			else if(pad == CAPS_LOCK_PAD_1 || pad == CAPS_LOCK_PAD_2) //capslock
			{
				leds.setLED(CAPS_LOCK_PAD_1, 1, 31);
				leds.setLED(CAPS_LOCK_PAD_2, 1, 31);
			}
			else if(pad >= SPACE_PAD_1 && pad <=SPACE_PAD_5) //space
			{
				for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
				{
					leds.setLED(i, 1, 31);
				}
			}
			else
			{
				leds.setLED(pad,1,31);
			}

			PE->keyboardPosition = valueMapPads[pad];


			if(PE->editPosition > 31) return 1;
			if(smallKeyboard[PE->keyboardPosition] > 1)
			{
				if(PE->editPosition == 31) return 1;
				uint8_t localNameLen = strlen(PE->name);
				if(PE->editPosition < localNameLen)
				{
					for(uint8_t i = localNameLen; i >= PE->editPosition ; i-- )
					{
						PE->name[i+1] = PE->name[i];
					}
				}

				PE->name[PE->editPosition] = PE->keyboardShiftFlag ? bigKeyboard[PE->keyboardPosition] : smallKeyboard[PE->keyboardPosition];
				PE->name[PE->editPosition + 1] = 0;
				PE->editPosition++;
			}
			else if(smallKeyboard[PE->keyboardPosition] == 0)
			{
				if(PE->editPosition == 0 ) return 1;

				PE->name[PE->editPosition-1] = 0;
				PE->editPosition--;


			}
			else if(smallKeyboard[PE->keyboardPosition] == 1)
			{
				PE->keyboardShiftFlag = ! PE->keyboardShiftFlag;
//				PE->showKeyboard();
			}
			PE->showKeyboard();
			PE->showKeyboardEditName();
			return 1;
		}

		return 1;
	}
	else if(state == 0)
	{
		if(PE->keyboardActiveFlag)
		{

		}
	}




	return 1;
}


