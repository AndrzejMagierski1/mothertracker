
#include "EEPROM.h"
//#include "newSd.h"
#include <display.h>


#include "mtStructs.h"

#include "performanceMode/performanceEngine.h"

#include "debugLog.h"
#include "mtConfig.h"

void firmwareVersionChange();
void memoryStructureChange();
void eepromStructureChange();


extern uint32_t patternTrackerSelectionColor;
extern uint32_t patternTrackerColors[8];

elapsedMillis save_delay;

static uint8_t saveFlag = 0;
static uint8_t saveAsapFlag = 0;



static const uint8_t eepromStructsCount = 9;
static uint8_t* eepromStructsPtr[eepromStructsCount]
{
		(uint8_t*)&mtConfig.audioCodecConfig,
        (uint8_t*)&mtConfig.firmware,
        (uint8_t*)&mtConfig.midi,
        (uint8_t*)&mtConfig.values,
        (uint8_t*)&mtConfig.general,
        (uint8_t*)&mtConfig.interface,
        (uint8_t*)&mtConfig.metronome,
        (uint8_t*)&mtConfig.debug,
        (uint8_t*)&mtConfig.common,


};

static uint16_t eepromStructsSizes[]
{
		sizeof(mtConfig.audioCodecConfig),
		sizeof(mtConfig.firmware),
		sizeof(mtConfig.midi),
		sizeof(mtConfig.values),
		sizeof(mtConfig.general),
		sizeof(mtConfig.interface),
		sizeof(mtConfig.metronome),
		sizeof(mtConfig.debug),
		sizeof(mtConfig.common),
};


void eepromUpdate(bool force)
{
	if(saveFlag  && ( save_delay > 5000 || force || saveAsapFlag))
	{
		saveAsapFlag = 0;
		saveFlag = 0;
		forceSaveConfig();
	}
	if( save_delay > 5000) save_delay = 0;
}


void saveConfig()
{
	saveFlag = 1;
}
void saveConfigAsap()
{
	saveFlag = 1;
	saveAsapFlag = 1;
}

void forceSaveConfig()
{

	strcpy(mtConfig.header.id, "EEPROM");
	mtConfig.header.id[6] = 0;
	mtConfig.header.version = EEPROM_STRUCT_VER;
	mtConfig.header.totalSize = sizeof(mtConfig);


	mtConfig.audioCodecConfig.size = sizeof(mtConfig.audioCodecConfig);
	mtConfig.firmware.size = sizeof(mtConfig.firmware);
	mtConfig.midi.size = sizeof(mtConfig.midi);
	mtConfig.values.size = sizeof(mtConfig.values);
	mtConfig.general.size = sizeof(mtConfig.general);
	mtConfig.interface.size = sizeof(mtConfig.interface);
	mtConfig.metronome.size = sizeof(mtConfig.metronome);
	mtConfig.debug.size = sizeof(mtConfig.debug);
	mtConfig.common.size = sizeof(mtConfig.common);



//	uint16_t writedSize = 0;
//
//	for(uint8_t i = 0; writedSize < mtConfig.header.totalSize-1; i++)
//	{
//
//
//		// nadanie aktulanej wielkosci podstruktury
//		memcpy(eepromStructsPtr[i], &eepromStructsSizes[i], 2);
//
//		writedSize += eepromStructsSizes[i];
//	}



	EEPROM.put(CONFIG_EEPROM_ADRESS +( (uint32_t)&mtConfig.header -  (uint32_t)&mtConfig) , 			mtConfig.header);
	EEPROM.put(CONFIG_EEPROM_ADRESS +( (uint32_t)&mtConfig.audioCodecConfig -  (uint32_t)&mtConfig) , 	mtConfig.audioCodecConfig);
	EEPROM.put(CONFIG_EEPROM_ADRESS +( (uint32_t)&mtConfig.firmware -  (uint32_t)&mtConfig) , 			mtConfig.firmware);
	EEPROM.put(CONFIG_EEPROM_ADRESS +( (uint32_t)&mtConfig.midi -  (uint32_t)&mtConfig) , 				mtConfig.midi);
	EEPROM.put(CONFIG_EEPROM_ADRESS +( (uint32_t)&mtConfig.values -  (uint32_t)&mtConfig) , 			mtConfig.values);
	EEPROM.put(CONFIG_EEPROM_ADRESS +( (uint32_t)&mtConfig.general -  (uint32_t)&mtConfig) , 			mtConfig.general);
	EEPROM.put(CONFIG_EEPROM_ADRESS +( (uint32_t)&mtConfig.interface -  (uint32_t)&mtConfig) , 			mtConfig.interface);
	EEPROM.put(CONFIG_EEPROM_ADRESS +( (uint32_t)&mtConfig.metronome -  (uint32_t)&mtConfig) , 			mtConfig.metronome);
	EEPROM.put(CONFIG_EEPROM_ADRESS +( (uint32_t)&mtConfig.debug -  (uint32_t)&mtConfig) , 				mtConfig.debug);
	EEPROM.put(CONFIG_EEPROM_ADRESS +( (uint32_t)&mtConfig.common -  (uint32_t)&mtConfig) , 			mtConfig.common);



}


//###############################################################
//						START STATE
//###############################################################
uint8_t readStartState()
{
	uint8_t startState = 1;

	EEPROM.readData(CONFIG_EEPROM_ADRESS, &mtConfig.header, sizeof(mtConfig.header));

	if(strcmp(mtConfig.header.id, "EEPROM") == 0) // czytaj nowa pozycje start_state tylko jesli nowy config
	{
		EEPROM.get(START_STATE_EEPROM_ADRESS, startState);
	}

	return startState;
}

void saveStartState(uint8_t state)
{
	if(START_STATE_SAVE)
	{
		EEPROM.put(START_STATE_EEPROM_ADRESS, state);
	}
}

//###############################################################
//
//###############################################################
void readConfig()
{

	EEPROM.readData(CONFIG_EEPROM_ADRESS, &mtConfig.header, sizeof(mtConfig.header));

	if(strcmp(mtConfig.header.id, "EEPROM") != 0) // nie rozpoznana zawartosc eepromu
	{
		// mozliwe ze w eepromie jest stary config wiec dokonaj konwersji
		recoverConfigFromOldVerison();

		// teraz napraw zawartosc jesli bylo inaczej i byl tam po prostu syf
		checkConfig();
	}
	else // normalny odczyt configu w nowej wersji
	{

		uint16_t offsetToRead, sizeToRead, sizeFromEeprom;

		uint16_t configMinSize = mtConfig.header.totalSize < sizeof(mtConfig)
								? mtConfig.header.totalSize
								: sizeof(mtConfig);

		// pierwsza pozycja do odczytu podstruktury
		offsetToRead = CONFIG_EEPROM_ADRESS + ((uint32_t)eepromStructsPtr[0] - (uint32_t)&mtConfig); // start+header

		for(uint8_t i = 0; i < eepromStructsCount; i++)
		{
			// odczyt zapisanej wielksoci podstruktury
			EEPROM.readData(offsetToRead, &sizeFromEeprom, 2);

			// ustalenie ilosci danych do odczytania
			sizeToRead = sizeFromEeprom < eepromStructsSizes[i]
						? sizeFromEeprom
						: eepromStructsSizes[i];

			// odczyt
			EEPROM.readData(offsetToRead, eepromStructsPtr[i], sizeToRead);

			// nadanie aktulanej wielkosci podstruktury - to jest przy zapisie
			//memcpy(eepromStructsPtr[i], &eepromStructsSizes[i], 2);

			// kolejna pozycja do odczytu podstruktury
			offsetToRead += sizeFromEeprom;


			if(offsetToRead >= configMinSize-1)
			{
				break;
			}
		}

		// napraw zawartosc
		checkConfig();
	}


	if ((mtConfig.firmware.ver_1 != FV_VER_1) ||
	    (mtConfig.firmware.ver_2 != FV_VER_2)||
	    (mtConfig.firmware.ver_3 != FV_VER_3)||
	    (mtConfig.firmware.beta != FV_BETA))
	{
		firmwareVersionChange();

		mtConfig.firmware.ver_1		= FV_VER_1;
		mtConfig.firmware.ver_2		= FV_VER_2;
		mtConfig.firmware.ver_3		= FV_VER_3;
		mtConfig.firmware.beta 		= FV_BETA;
	}

	if (mtConfig.firmware.eepromStructVer != EEPROM_STRUCT_VER)
	{
		eepromStructureChange();

		mtConfig.firmware.eepromStructVer = EEPROM_STRUCT_VER;
	}



	forceSaveConfig();
}



void checkConfig()
{

	if(mtConfig.audioCodecConfig.headphoneVolume < MASTER_VOLUME_MIN)
		mtConfig.audioCodecConfig.headphoneVolume = MASTER_VOLUME_MIN;
	else if(mtConfig.audioCodecConfig.headphoneVolume > MASTER_VOLUME_MAX)
		mtConfig.audioCodecConfig.headphoneVolume = MASTER_VOLUME_MAX;

	if(mtConfig.audioCodecConfig.inSelect >= inputSelectCount)
		mtConfig.audioCodecConfig.inSelect = inputSelectMic;
	if(mtConfig.audioCodecConfig.outSelect >= outputSelectCount)
		mtConfig.audioCodecConfig.outSelect = outputSelectHeadphones;

	if(mtConfig.audioCodecConfig.inputGain > 63)
		mtConfig.audioCodecConfig.inputGain = INPUT_MIC_GAIN_DEFAULT;

	if(mtConfig.audioCodecConfig.mutedHeadphone > 1)
			mtConfig.audioCodecConfig.mutedHeadphone = 0;
	if(mtConfig.audioCodecConfig.mutedLineOut > 1)
			mtConfig.audioCodecConfig.mutedLineOut = 0;

	if(mtConfig.audioCodecConfig.lineInLeft > 15)
			mtConfig.audioCodecConfig.lineInLeft = LINE_IN_SENS_LEVEL_DEFAULT;
	if(mtConfig.audioCodecConfig.lineInRight > 15)
			mtConfig.audioCodecConfig.lineInRight = LINE_IN_SENS_LEVEL_DEFAULT;

	if(mtConfig.audioCodecConfig.lineOutLeft > 31)
			mtConfig.audioCodecConfig.lineOutLeft = LINE_OUT_LEVEL_DEFAULT;
	else if(mtConfig.audioCodecConfig.lineOutLeft < 13)
			mtConfig.audioCodecConfig.lineOutLeft = LINE_OUT_LEVEL_DEFAULT;

	if(mtConfig.audioCodecConfig.lineOutRight > 31)
			mtConfig.audioCodecConfig.lineOutRight = LINE_OUT_LEVEL_DEFAULT;
	else if(mtConfig.audioCodecConfig.lineOutRight < 13)
			mtConfig.audioCodecConfig.lineOutRight = LINE_OUT_LEVEL_DEFAULT;

	if(mtConfig.audioCodecConfig.volume > MASTER_VOLUME_MAX) mtConfig.audioCodecConfig.volume = MASTER_VOLUME_MAX;


//	mtConfig.startup.lastProjectName[PROJECT_NAME_SIZE-1] = 0;
//	if(mtConfig.startup.startMode >= interfaceCommandsCount) mtConfig.startup.startMode = interfaceOpenLastProject;
//
//	if(mtConfig.startup.powerState > powerStateRun)
//	{
//		mtConfig.startup.powerState = powerStateSleep;
//	}


	//  ----------------------------------------

	if(mtConfig.common.arcanoidHighestScore > 20000)
	{
		mtConfig.common.arcanoidHighestScore = 0;
	}

	// midi ----------------------------------------
	for(uint8_t i = 0; i < 10; i++)
	{
		if(mtConfig.midi.ccOut[i] > 126)
		{
			mtConfig.midi.ccOut[i] = 0;
		}
	}

	if(mtConfig.midi.clkIn > 2)
	{
		mtConfig.midi.clkIn = 0;
	}

	if(mtConfig.midi.clkOut > 3)
	{
		mtConfig.midi.clkOut = 0;
	}

	if(mtConfig.midi.transportIn > 2)
	{
		mtConfig.midi.transportIn = 0;
	}

	if(mtConfig.midi.transportOut > 3)
	{
		mtConfig.midi.transportOut = 0;
	}

	if(mtConfig.midi.notesInMode > 3)
	{
		mtConfig.midi.notesInMode = 0;
	}

	if(mtConfig.midi.notesOutMode > 3)
	{
		mtConfig.midi.notesOutMode = 0;
	}

	if(mtConfig.midi.notesInChannel > 16)
	{
		mtConfig.midi.notesInChannel = 0;
	}

	if(mtConfig.midi.notesOutChannel > 15)
	{
		mtConfig.midi.notesOutChannel = 0;
	}


	// VALUES CHECK  --------------------
	if(mtConfig.values.padsLightBack > 31)	mtConfig.values.padsLightBack = PADS_LIGHT_BACK_DEFAULT;
	if(mtConfig.values.padsLightFront > 31)	mtConfig.values.padsLightFront = PADS_LIGHT_FRONT_DEFAULT;
	if(mtConfig.values.padsLightBackWeek > 31)	mtConfig.values.padsLightBackWeek = PADS_LIGHT_BACK_DEFAULT/2;


	if(mtConfig.values.padBoardScale > 39)	 	mtConfig.values.padBoardScale = 0;
	if(mtConfig.values.padBoardNoteOffset > 12)	 mtConfig.values.padBoardNoteOffset = 12;
	if(mtConfig.values.padBoardRootNote > 62)	 mtConfig.values.padBoardRootNote = 36;
	if(mtConfig.values.padBoardMaxVoices > 8)	 mtConfig.values.padBoardMaxVoices = 8;


	// performance mode i inne
	for(uint8_t i = 0; i<8; i++)
	{
		// paterny na trakach w performance mode
		if(mtConfig.values.perfTracksPatterns[i] < 1 || mtConfig.values.perfTracksPatterns[i] > 255)
		{
			mtConfig.values.perfTracksPatterns[i] = 1;
		}
	}

	for(uint8_t i = 0; i<12; i++)
	{
		//if(mtConfig.values.perfFxPlaces[i] > performance.getFxCount()-1)
		if(mtConfig.values.perfFxPlaces[i] > 18-1)
		{
			//mtConfig.values.perfFxPlaces[i] = (i+1 < performance.getFxCount()) ? i+1 : 0;
			mtConfig.values.perfFxPlaces[i] = (i+1 < 18) ? i+1 : 0;
		}

		if(mtConfig.values.perfSelectedValues[i] > 3) mtConfig.values.perfSelectedValues[i] = 0;

		if(mtConfig.values.perfFxValues[i][0] != 0)
			mtConfig.values.perfFxValues[i][0] = 0;
		if(mtConfig.values.perfFxValues[i][1] > 255 || mtConfig.values.perfFxValues[i][1] < -255)
			mtConfig.values.perfFxValues[i][1] = 0;
		if(mtConfig.values.perfFxValues[i][2] > 255 || mtConfig.values.perfFxValues[i][2] < -255)
			mtConfig.values.perfFxValues[i][2] = 0;
		if(mtConfig.values.perfFxValues[i][3] > 255 || mtConfig.values.perfFxValues[i][3] < -255)
			mtConfig.values.perfFxValues[i][3] = 0;
	}



	// general ----------------------------------------
	if(mtConfig.general.dispBrightness > 2)
	{
		mtConfig.general.dispBrightness = 2;
	}

	if(mtConfig.general.padsBrightness > 2)
	{
		mtConfig.general.padsBrightness = 2;
	}

	if(mtConfig.general.patternDiv > 15)
	{
		mtConfig.general.patternDiv = 3; // =>> 4
	}

	if(mtConfig.general.radioRegion > 2)
	{
		mtConfig.general.radioRegion = 0;
	}

	if(mtConfig.general.mtpState > 0)
	{
		mtConfig.general.mtpState = 0;
	}

	if(mtConfig.general.recOptions > recOptions_max) 	mtConfig.general.recOptions = 0;
	if(mtConfig.general.performanceSource > 1) 			mtConfig.general.performanceSource = 0;

	if(mtConfig.general.padBoardScale > 39) 			mtConfig.general.padBoardScale = 0;
	if(mtConfig.general.padBoardNoteOffset > 11) 		mtConfig.general.padBoardNoteOffset = 11;
	if(mtConfig.general.padBoardRootNote > 25) 			mtConfig.general.padBoardRootNote = 12;
	//if(mtConfig.general.padBoardMaxVoices > 8) 		mtConfig.general.padBoardMaxVoices = 8;
	if(mtConfig.general.antialiasingEnable > 1) 		mtConfig.general.antialiasingEnable = 0;


	if(mtConfig.general.limiterMode > 1)  				mtConfig.general.limiterMode = 0;

	// interface ----------------------------------------
	if(mtConfig.interface.fxPopupDescription > 1)
	{
		mtConfig.interface.fxPopupDescription = 0;
	}

	// metornome ----------------------------------------
	if(mtConfig.metronome.state > 1) mtConfig.metronome.state = 0;
	if(mtConfig.metronome.preRoll > 1) mtConfig.metronome.preRoll = 0;
	if(mtConfig.metronome.timeSignatureNumerator > 11) mtConfig.metronome.timeSignatureNumerator = 0;
	if(mtConfig.metronome.volume > 100) mtConfig.metronome.volume = 50;


	// debug ----------------------------------------
	if(mtConfig.debug.debugLogState > 0)
	{
		mtConfig.debug.debugLogState = 0;
	}


	// common ----------------------------------------
	if(mtConfig.common.equalSliceNumber < 1) mtConfig.common.equalSliceNumber = 8;
	else if(mtConfig.common.equalSliceNumber > 48) mtConfig.common.equalSliceNumber = 8;



// TODO dodac zapis spowrotem do configu jesli np przy sprawdzaniu wazne dane sie nie zgadzały
// TODO
// TODO
}

void resetConfig()
{
	// startup ----------------------------------------
//	mtConfig.startup.lastProjectName[0] = 0;
//	mtConfig.startup.startMode = interfaceOpenLastProject;
//	mtConfig.startup.powerState = powerStateSleep;

	// interface ----------------------------------------
	mtConfig.audioCodecConfig.headphoneVolume = MASTER_VOLUME_DEFAULT;
	mtConfig.audioCodecConfig.inSelect = inputSelectMic;
	mtConfig.audioCodecConfig.outSelect = outputSelectHeadphones;
	mtConfig.audioCodecConfig.inputGain = INPUT_MIC_GAIN_DEFAULT;
	mtConfig.audioCodecConfig.mutedHeadphone = 0;
	mtConfig.audioCodecConfig.mutedLineOut = 0;
	mtConfig.audioCodecConfig.lineInLeft = LINE_IN_SENS_LEVEL_DEFAULT;
	mtConfig.audioCodecConfig.lineInRight = LINE_IN_SENS_LEVEL_DEFAULT;
	mtConfig.audioCodecConfig.lineOutLeft = LINE_OUT_LEVEL_DEFAULT;
	mtConfig.audioCodecConfig.lineOutRight = LINE_OUT_LEVEL_DEFAULT;
	mtConfig.audioCodecConfig.volume = MASTER_VOLUME_DEFAULT;

	// midi ----------------------------------------
	for(uint8_t i = 0; i < 10; i++)
	{
		mtConfig.midi.ccOut[i] = 0;
	}

	mtConfig.midi.clkIn = 0;
	mtConfig.midi.clkOut = 0;
	mtConfig.midi.transportIn = 0;
	mtConfig.midi.transportOut = 0;
	mtConfig.midi.notesInMode = 0;
	mtConfig.midi.notesInChannel = 0;
	mtConfig.midi.notesOutMode = 0;
	mtConfig.midi.notesOutChannel = 0;

	// values ----------------------------------------
	mtConfig.values.padsLightBack = PADS_LIGHT_BACK_DEFAULT;
	mtConfig.values.padsLightFront = PADS_LIGHT_FRONT_DEFAULT;
	mtConfig.values.padsLightBackWeek = PADS_LIGHT_BACK_DEFAULT/2;
	mtConfig.values.padBoardScale = 0;
	mtConfig.values.padBoardNoteOffset = 12;
	mtConfig.values.padBoardRootNote = 36;
	mtConfig.values.padBoardMaxVoices = 8;

	for(uint32_t i = 0; i < 8; i++)
	{
		mtConfig.values.perfTracksPatterns[i] = 1;
		mtConfig.values.perfTracksState[i] = 0;
	}

	for(uint32_t i = 0; i < 12; i++)
	{
		mtConfig.values.perfSelectedValues[i] = 0;
		mtConfig.values.perfFxPlaces[i] = (i+1);
		mtConfig.values.perfFxValues[i][0] = 0;
		mtConfig.values.perfFxValues[i][1] = 0;
		mtConfig.values.perfFxValues[i][2] = 0;
		mtConfig.values.perfFxValues[i][3] = 0;
	}

	// general ----------------------------------------
	mtConfig.general.dispBrightness = 2;
	mtConfig.general.padsBrightness = 2;
	mtConfig.general.patternDiv = 3; // czyli 4
	mtConfig.general.radioRegion = 0;
	mtConfig.general.mtpState = 0;

	mtConfig.general.recOptions = 0;
	mtConfig.general.performanceSource = 0;
	mtConfig.general.padBoardScale = 0;
	mtConfig.general.padBoardNoteOffset = 11;
	mtConfig.general.padBoardRootNote = 12;
	//mtConfig.general.padBoardMaxVoices = 8;
	mtConfig.general.antialiasingEnable = 1;
	mtConfig.general.limiterMode = 0;


	// interface ----------------------------------------
	mtConfig.interface.fxPopupDescription = 0;

	// metornome ----------------------------------------
	mtConfig.metronome.state = 0;
	mtConfig.metronome.preRoll = 0;
	mtConfig.metronome.timeSignatureNumerator = 3;
	mtConfig.metronome.timeSignatureDenominator = 3;
	mtConfig.metronome.volume = 50;



	// debug ----------------------------------------
	mtConfig.debug.debugLogState = 0;

	// common ----------------------------------------
	 mtConfig.common.equalSliceNumber = 8;


	// game ----------------------------------------
	mtConfig.common.arcanoidHighestScore = 0;
}



uint16_t sizes[10];

// odczytanie eepromu w starej konwencji i wstawienie do nowej
void recoverConfigFromOldVerison()
{

	EEPROM.readData(36, 	((uint8_t*)&mtConfig.audioCodecConfig)+2	,13);
	EEPROM.readData(52, 	((uint8_t*)&mtConfig.firmware)+2			,5);
	EEPROM.readData(57, 	((uint8_t*)&mtConfig.midi)+2				,18);
	EEPROM.readData(76, 	((uint8_t*)&mtConfig.values)+2				,151);
	EEPROM.readData(228, 	((uint8_t*)&mtConfig.general)+2				,10);
	EEPROM.readData(238, 	((uint8_t*)&mtConfig.interface)+2			,1);
	EEPROM.readData(239, 	((uint8_t*)&mtConfig.metronome)+2			,5);
	EEPROM.readData(244, 	((uint8_t*)&mtConfig.debug)+2				,1);
	EEPROM.readData(245, 	((uint8_t*)&mtConfig.common)+2				,1);

	sizes[0] = sizeof(mtConfig.audioCodecConfig);
	sizes[1] = sizeof(mtConfig.firmware);
	sizes[2] = sizeof(mtConfig.midi);
	sizes[3] = sizeof(mtConfig.values);
	sizes[4] = sizeof(mtConfig.general);
	sizes[5] = sizeof(mtConfig.interface);
	sizes[6] = sizeof(mtConfig.metronome);
	sizes[7] = sizeof(mtConfig.debug);
	sizes[8] = sizeof(mtConfig.common);



	strcpy(mtConfig.header.id, "EEPROM");
	mtConfig.header.id[6] = 0;
	mtConfig.header.version = EEPROM_STRUCT_VER;
	mtConfig.header.totalSize = sizeof(mtConfig);


	// ustaw nowe parametry
	mtConfig.audioCodecConfig.volume = MASTER_VOLUME_DEFAULT;
	mtConfig.general.antialiasingEnable = 1;

}


void firmwareVersionChange()
{

	mtConfig.general.mtpState = 0; //xxx po zmianie firmwaru wymusza wylaczenie mtp
	//Serial.println("Firmware changed!");

}



void eepromStructureChange()
{

	//Serial.println("Eeprom structure changed!");


	//resetConfig();

}

//===============================================================================================================
//====================================         SD CONFIG         ================================================
//===============================================================================================================


const uint8_t sd_comannds_count = 7;
uint8_t buffSize = 255;

void readSdConfig()
{
//	if(SD.exists("/mtconfig.txt"))
//	{
//		SdFile fConfig;
//		char buff[buffSize+1];
//
//		fConfig = SD.open("/mtconfig.txt", FILE_READ);
//		buffSize = fConfig.read(buff, buffSize);
//		//fConfig.write(&CE->firmwareNamesList[CE->firmwareSelect][0], 13);
//		fConfig.close();
//
//		executeSdConfig(buff);
//	}
}


void executeSdConfig(char* buff)
{
	buff[buffSize] = 0;

	for(uint8_t i = 0; i<sd_comannds_count; i++)
	{
		char* equal = strchr(buff, '=');
		if(equal)
		{
			char cmd[12], param[32];
			strncpy(cmd,buff,equal-buff);
			cmd[(equal-buff<12)?equal-buff:11] = 0;

			char* lineEnd = strchr(equal, '\r');
			if(lineEnd == nullptr)
			{
				char* lineEnd = strchr(equal, '\n');
				if(lineEnd == nullptr)
				{
					lineEnd = strchr(equal, 0);
					if(lineEnd == nullptr) return;
				}
			}

			equal+=1;
			strncpy(param, equal, lineEnd-equal);
			param[(lineEnd-equal<32)?lineEnd-equal:31] = 0;

			//-------------------------------------------------------------------
			if(strcmp(cmd,"rotate") == 0)
			{
				char value = param[0]-48;
				if(value >= 0 && value <= 7) display.setRotate(value);
			}
/*			else if(strcmp(cmd,"oproject") == 0)
			{
				mtConfig.startup.startMode = interfaceOpenLastProject;

				if(mtConfig.startup.lastProjectName[0] == 0 || mtConfig.startup.lastProjectName[0] == 255)
				{
					strcpy(mtConfig.startup.lastProjectName,param);
				}
			}
*/

			//--------------------------------

			else if(strcmp(cmd,"notecolor") == 0)
			{
				uint32_t temp = strtol(param,NULL,16);
				if(temp < 0xffffff)
				{
					patternTrackerColors[2] = temp;
				}
			}
			else if(strcmp(cmd,"instrcolor") == 0)
			{
				uint32_t temp = strtol(param,NULL,16);
				if(temp < 0xffffff)
				{
					patternTrackerColors[3] = temp;
				}
			}
			else if(strcmp(cmd,"volcolor") == 0)
			{
				uint32_t temp = strtol(param,NULL,16);
				if(temp < 0xffffff)
				{
					patternTrackerColors[4] = temp;
				}
			}
			else if(strcmp(cmd,"fxcolor") == 0)
			{
				uint32_t temp = strtol(param,NULL,16);
				if(temp < 0xffffff)
				{
					patternTrackerColors[5] = temp;
				}
			}
			else if(strcmp(cmd,"selcolor") == 0)
			{
				uint32_t temp = strtol(param,NULL,16);
				if(temp < 0xffffff)
				{
					patternTrackerSelectionColor = temp;
				}
			}




			//-------------------------------------------------------------------

			for(uint8_t i = 0; i<10; i++)
			{
				if(*lineEnd == '\n' || *lineEnd == '\r' ) lineEnd++;
				else break;
			}

			buff = lineEnd;
		}
		else
		{
			break;
		}
	}



}


