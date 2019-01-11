
#ifndef MT_PROJECT_MANAGER_H
#define MT_PROJECT_MANAGER_H

#include <stdint.h>
#include "mtStructs.h"


//=====================================================================
class cMtProjectEditor
{
public:
	uint8_t readProjectConfig();
	uint8_t loadSamplesBank();
	uint8_t loadProject();
	uint8_t isProjectLoaded();



private:





};


//=====================================================================
extern cMtProjectEditor mtProjectEditor;


int32_t loadSdWavToMemory(const char *filename, int16_t * buf);



#endif
