#ifndef SOURCE_MTCONFIG_H_
#define SOURCE_MTCONFIG_H_

#include "EEPROM.h"
#include "mtStructs.h"

void saveConfig(int16_t address, strMtConfig * config);
void readConfig(int16_t address, strMtConfig * config);



#endif /* SOURCE_MTCONFIG_H_ */
