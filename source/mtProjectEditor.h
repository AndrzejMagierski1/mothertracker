
#ifndef MT_PROJECT_MANAGER_H
#define MT_PROJECT_MANAGER_H

#include <stdint.h>
#include "mtStructs.h"


#define COMMANDS_PROJECT_EDITOR_MAX 32

enum mtProjectEditorCommands
{
	ProjEditCommandNone,
	ProjEditCommandOpenLastProject,

	ProjEditCommandCount
};

enum mtProjectEditorEvents
{
	ProjEditEventNone,
	ProjEditEventLoadLastProjFailed,

	ProjEditEventCount
};

//=====================================================================
class cMtProjectEditor
{
public:
	void update();
	void startProject();


	uint8_t readProjectConfig();
	uint8_t loadSamplesBank();
	uint8_t loadLastProject();
	uint8_t isProjectLoaded();



private:

	uint8_t commandsToDo[ProjEditCommandCount];


};


//=====================================================================
extern cMtProjectEditor mtProjectEditor;

void projectEditorEvent(uint8_t event, void* param);
int32_t loadSdWavToMemory(const char *filename, int16_t * buf);



#endif
