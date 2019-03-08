
#include "EEPROM.h"

#include "mtConfig.h"


elapsedMillis save_delay;

void saveConfig(int16_t address, strMtConfig * config)
{
	if(save_delay > 1000)
	{
		save_delay = 0;
		EEPROM.put(address, config);
	}

}

void readConfig(int16_t address, strMtConfig * config)
{
	EEPROM.get(address, config);

	checkConfig(config);
}


void checkConfig(strMtConfig * config)
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

/*
	mtConfig.audioCodecConfig.inSelect = 0;
	mtConfig.audioCodecConfig.outSelect = 1;

	mtConfig.audioCodecConfig.inputGain = 35; // 0-63

	mtConfig.audioCodecConfig.mutedHeadphone = 0;

	mtConfig.audioCodecConfig.mutedLineOut = 0;

	mtConfig.audioCodecConfig.lineInLeft = 0; // 0-15
	mtConfig.audioCodecConfig.lineInRight = 0; // 0-15

	mtConfig.audioCodecConfig.lineOutLeft = 15; // 13-31
	mtConfig.audioCodecConfig.lineOutRight = 15; //13-31
*/
	mtConfig.audioCodecConfig.changeFlag = 1;
}
