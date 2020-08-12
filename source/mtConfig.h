#ifndef SOURCE_MTCONFIG_H_
#define SOURCE_MTCONFIG_H_




void eepromUpdate(bool force);


void readConfigOnce();

void forceSaveConfig();
void saveConfig();
void saveConfigAsap();
void readConfig();



uint8_t readStartState();
//void saveStartState();
void saveStartState(uint8_t state);

void checkConfig();
void recoverConfigFromOldVerison();

void readSdConfig();
void executeSdConfig(char* buff);


#endif /* SOURCE_MTCONFIG_H_ */
