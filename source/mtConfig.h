#ifndef SOURCE_MTCONFIG_H_
#define SOURCE_MTCONFIG_H_




void readConfigOnce();

void forceSaveConfig();
void saveConfig();
void readConfig();

void checkConfig();


void readSdConfig();
void executeSdConfig(char* buff);


#endif /* SOURCE_MTCONFIG_H_ */
