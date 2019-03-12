#ifndef SOURCE_MTCONFIG_H_
#define SOURCE_MTCONFIG_H_


#include "mtStructs.h"


void saveConfig(int16_t address, strMtConfig * config);
void readConfig(int16_t address, strMtConfig * config);

void checkConfig(strMtConfig * config);


#endif /* SOURCE_MTCONFIG_H_ */
