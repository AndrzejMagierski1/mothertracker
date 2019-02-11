
#ifndef MT_PROJECT_MANAGER_H
#define MT_PROJECT_MANAGER_H

#include <stdint.h>
#include "mtStructs.h"
#include "SD.h"


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
	mtPriojectEditorEventPadPress,

	ProjEditEventCount
};

//=====================================================================
class cMtProjectEditor
{
public:
	void update();
	void startProject();

	uint8_t padsChange(uint8_t type, uint8_t n, uint8_t velo);
	void buttonChange(uint8_t button, uint8_t value);
	void potChange(uint8_t pot, int16_t value);
	void seqButtonChange(uint8_t type, uint8_t x, uint8_t y);

	uint8_t readProjectConfig();
	uint8_t loadSamplesBank();
	uint8_t loadLastProject();
	uint8_t isProjectLoaded();

	void setEventFunct(void (*func)(uint8_t, void*, void*, void*)) { eventFunct = func; };

private:

	uint8_t commandsToDo[ProjEditCommandCount];

	void (*eventFunct)(uint8_t, void*, void*, void*);

};


//=====================================================================
extern cMtProjectEditor mtProjectEditor;


int32_t loadSample(const char *filename, int16_t * buf);
int32_t loadWavetable(const char *filename, int16_t * buf);
int32_t loadWavetableStandard(const char *filename, int16_t * buf);
int32_t loadWavetableSerum(const char *filename, int16_t * buf);
uint8_t getMiddleWindow(uint16_t cnt, float windowsCnt, float windowsControl);
int16_t fmap(float x, float in_min, float in_max, float out_min, float out_max);
void readHeader(strWavFileHeader* header, FsFile * wavfile);


#endif
