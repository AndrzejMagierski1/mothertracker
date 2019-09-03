
#include "EEPROM.h"

#include "mtStructs.h"


#include "mtConfig.h"

void firmwareVersionChange();
void memoryStructureChange();


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






}



void firmwareVersionChange()
{

	Serial.println("Firmware changed!");

}

void memoryStructureChange()
{

	Serial.println("MEMO_STRUCT changed!");

}
