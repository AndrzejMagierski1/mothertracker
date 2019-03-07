
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

	mtConfig.audioCodecConfig.inSelect = 0;
	mtConfig.audioCodecConfig.outSelect = 0;

	//TODO
	// ustawnianie reszty paramtrow min. do ngrywania przez mikforon potrzebne to jest

	mtConfig.audioCodecConfig.changeFlag = 1;

}
