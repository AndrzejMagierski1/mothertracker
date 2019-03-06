#include "mtConfig.h"


void saveConfig(int16_t address, strMtConfig * config)
{
	uint16_t len=sizeof(*config);
	uint8_t * confAddr=(uint8_t *) config;

	for(uint16_t i=0;i<len;i++)
	{
		EEPROM.write(address + i, *confAddr++);
	}
}

void readConfig(int16_t address, strMtConfig * config)
{
	uint16_t len=sizeof(*config);
	uint8_t * confAddr = (uint8_t *) config;

	for(uint16_t i=0;i<len;i++)
	{
		*confAddr++=EEPROM.read(address+i);
	}
}

