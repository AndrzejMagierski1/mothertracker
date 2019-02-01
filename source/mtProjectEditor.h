
#ifndef MT_PROJECT_MANAGER_H
#define MT_PROJECT_MANAGER_H

#include <stdint.h>
#include "mtStructs.h"


#define COMMANDS_PROJECT_EDITOR_MAX 32

enum mtProjectEditorCommands
{
	ProjEditCommandNone,
	ProjEditCommandOpenLastProject,

};



//=====================================================================
class cMtProjectEditor
{
public:
	void update();
	void command(uint8_t comm);

	uint8_t readProjectConfig();
	uint8_t loadSamplesBank();
	uint8_t loadLastProject();
	uint8_t isProjectLoaded();



private:

	uint8_t commandsBuffor[COMMANDS_PROJECT_EDITOR_MAX];

	uint8_t lastComandIndex = 0;
	uint8_t waitingComandsCount = 0;

	uint8_t lastCommand = ProjEditCommandNone;




};


//=====================================================================
extern cMtProjectEditor mtProjectEditor;


int32_t loadSdWavToMemory(const char *filename, int16_t * buf);



#endif
