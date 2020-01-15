
#include "EEPROM.h"
#include "SD.h"
#include <display.h>


#include "mtStructs.h"

#include "debugLog.h"
#include "mtConfig.h"

void firmwareVersionChange();
void memoryStructureChange();
void eepromStructureChange();


extern uint32_t patternTrackerSelectionColor;
extern uint32_t patternTrackerColors[8];

elapsedMillis save_delay;
elapsedMicros save_micros;

void saveConfig()
{
	if(save_delay > 1000)
	{

		forceSaveConfig();
	}

}

void forceSaveConfig()
{
	debugLog.addLine("eeprom save");

	save_micros = 0;
	EEPROM.put(CONFIG_EEPROM_ADRESS, &mtConfig);
	debugLog.addText(" czas: ");
	debugLog.addValue(save_micros);
	debugLog.forceRefresh();
}

uint8_t readStartState()
{
	uint8_t startState = 1;

	save_micros = 0;

	EEPROM.get(sizeof(mtConfig), startState);

	uint32_t czas = save_micros;
	debugLog.addLine(" start state read: ");
	debugLog.addValue(czas);
	debugLog.forceRefresh();

	return startState;
}

void saveStartState()
{
	uint8_t startState = 0;

	save_micros = 0;

	EEPROM.put(sizeof(mtConfig), startState);

	debugLog.addLine("start state save: ");
	debugLog.addValue(save_micros);
	debugLog.forceRefresh();

}

void readConfig()
{
//	debugLog.addLine("eeprom read");
//	save_micros = 0;

	EEPROM.get(CONFIG_EEPROM_ADRESS, mtConfig);

//	debugLog.addText(" czas: ");
//	debugLog.addValue(save_micros);
//	debugLog.forceRefresh();

	checkConfig();

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

		EEPROM.put(CONFIG_EEPROM_ADRESS, mtConfig);
	}

	if (mtConfig.firmware.eepromStructVer != EEPROM_STRUCT_VER)
	{
		eepromStructureChange();

		mtConfig.firmware.eepromStructVer = EEPROM_STRUCT_VER;
		EEPROM.put(CONFIG_EEPROM_ADRESS, mtConfig);
	}

	if (mtConfig.firmware.memoryStructVer != MEMORY_STRUCT_VER)
	{
		memoryStructureChange();

		mtConfig.firmware.memoryStructVer = MEMORY_STRUCT_VER;
		EEPROM.put(CONFIG_EEPROM_ADRESS, mtConfig);
	}



}


uint8_t onceReaded = 1;

void readConfigOnce()
{
	if(onceReaded)
	{
		onceReaded = 0;
		readConfig();
	}

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





	mtConfig.startup.lastProjectName[PROJECT_NAME_SIZE-1] = 0;
	if(mtConfig.startup.startMode >= interfaceCommandsCount) mtConfig.startup.startMode = interfaceOpenLastProject;

	if(mtConfig.startup.powerState > powerStateRun)
	{
		mtConfig.startup.powerState = powerStateSleep;
	}


	// VALUES CHECK

	if(mtConfig.values.padsLightBack > 31)	mtConfig.values.padsLightBack = PADS_LIGHT_BACK_DEFAULT;
	if(mtConfig.values.padsLightFront > 31)	mtConfig.values.padsLightFront = PADS_LIGHT_FRONT_DEFAULT;
	if(mtConfig.values.padsLightBackWeek > 31)	mtConfig.values.padsLightBackWeek = PADS_LIGHT_BACK_DEFAULT/2;

	if(mtConfig.arcanoidHighestScore > 20000)
	{
		mtConfig.arcanoidHighestScore = 0;
	}

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

	if(mtConfig.midi.transportIn > 3)
	{
		mtConfig.midi.transportIn = 0;
	}

	if(mtConfig.midi.transportOut > 3)
	{
		mtConfig.midi.transportIn = 0;
	}

	if(mtConfig.general.brightness > 2)
	{
		mtConfig.general.brightness = 2;
	}

	if(mtConfig.general.patternDiv > 15)
	{
		mtConfig.general.patternDiv = 3; //4
	}

	if(mtConfig.general.radioRegion > 3)
	{
		mtConfig.general.radioRegion = 0;
	}

	if(mtConfig.general.radioRegion > 3)
	{
		mtConfig.general.radioRegion = 0;
	}

	// interface ----------------------------------------
	if(mtConfig.interface.fxPopupDescription > 1)
	{
		mtConfig.interface.fxPopupDescription = 0;
	}

	// debug ----------------------------------------
	if(mtConfig.debug.debugLogState > 1)
	{
#ifdef DEBUG
		mtConfig.debug.debugLogState = 1;
#else
		mtConfig.debug.debugLogState = 0;
#endif
	}


// TODO dodac zapis spowrotem do configu jesli np przy sprawdzaniu wazne dane sie nie zgadzały
// TODO
// TODO
}

void resetConfig()
{

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



	mtConfig.startup.lastProjectName[0] = 0;
	mtConfig.startup.startMode = interfaceOpenLastProject;

	mtConfig.startup.powerState = powerStateSleep;

	// VALUES
	mtConfig.values.padsLightBack = PADS_LIGHT_BACK_DEFAULT;
	mtConfig.values.padsLightFront = PADS_LIGHT_FRONT_DEFAULT;
	mtConfig.values.padsLightBackWeek = PADS_LIGHT_BACK_DEFAULT/2;


	mtConfig.arcanoidHighestScore = 0;


	for(uint8_t i = 0; i < 10; i++)
	{
		mtConfig.midi.ccOut[i] = 0;
	}

	mtConfig.midi.clkIn = 0;
	mtConfig.midi.clkOut = 0;
	mtConfig.midi.transportIn = 0;
	mtConfig.midi.transportIn = 0;


	mtConfig.general.brightness = 2;
	mtConfig.general.patternDiv = 3; //4
	mtConfig.general.radioRegion = 0;
	mtConfig.general.radioRegion = 0;


	// interface ----------------------------------------
	mtConfig.interface.fxPopupDescription = 0;

	// debug ----------------------------------------
#ifdef DEBUG
	mtConfig.debug.debugLogState = 1;
#else
	mtConfig.debug.debugLogState = 0;
#endif
}

void firmwareVersionChange()
{

	Serial.println("Firmware changed!");

}

void memoryStructureChange()
{

	Serial.println("MEMO_STRUCT changed!");

}


void eepromStructureChange()
{

	Serial.println("Eeprom struct changed!");


	resetConfig();

}

//===============================================================================================================
//====================================         SD CONFIG         ================================================
//===============================================================================================================


const uint8_t sd_comannds_count = 7;
uint8_t buffSize = 255;

void readSdConfig()
{


	if(SD.exists("/mtconfig.txt"))
	{
		SdFile fConfig;
		char buff[buffSize+1];

		fConfig = SD.open("/mtconfig.txt", FILE_READ);
		buffSize = fConfig.read(buff, buffSize);
		//fConfig.write(&CE->firmwareNamesList[CE->firmwareSelect][0], 13);
		fConfig.close();

		executeSdConfig(buff);
	}






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
			else if(strcmp(cmd,"oproject") == 0)
			{
				mtConfig.startup.startMode = interfaceOpenLastProject;

				if(mtConfig.startup.lastProjectName[0] == 0 || mtConfig.startup.lastProjectName[0] == 255)
				{
					strcpy(mtConfig.startup.lastProjectName,param);
				}
			}

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


