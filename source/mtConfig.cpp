
#include "EEPROM.h"
#include "SD.h"
#include <display.h>


#include "mtStructs.h"


#include "mtConfig.h"

void firmwareVersionChange();
void memoryStructureChange();

extern uint32_t patternTrackerSelectionColor;
extern uint32_t patternTrackerColors[8];

elapsedMillis save_delay;

void saveConfig()
{
	if(save_delay > 1000)
	{
		save_delay = 0;
		EEPROM.put(CONFIG_EEPROM_ADRESS, &mtConfig);
	}

}

void forceSaveConfig()
{
	EEPROM.put(CONFIG_EEPROM_ADRESS, mtConfig);
}

void readConfig()
{
	EEPROM.get(CONFIG_EEPROM_ADRESS, mtConfig);

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



	// VALUES CHECK


	if(mtConfig.values.padsLightBack > 31)	mtConfig.values.padsLightBack = PADS_LIGHT_BACK_DEFAULT;
	if(mtConfig.values.padsLightFront > 31)	mtConfig.values.padsLightFront = PADS_LIGHT_FRONT_DEFAULT;
	if(mtConfig.values.padsLightBackWeek > 31)	mtConfig.values.padsLightFront = PADS_LIGHT_BACK_DEFAULT/2;


// TODO dodac zapis spowrotem do configu jesli np przy sprawdzaniu wazne dane sie nie zgadzały
// TODO
// TODO
}



void firmwareVersionChange()
{

	Serial.println("Firmware changed!");

}

void memoryStructureChange()
{

	Serial.println("MEMO_STRUCT changed!");

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
		FsFile fConfig;
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


