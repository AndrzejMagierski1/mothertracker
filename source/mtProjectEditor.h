
#ifndef MT_PROJECT_MANAGER_H
#define MT_PROJECT_MANAGER_H

#include <stdint.h>
#include "mtStructs.h"
#include "SD.h"


#define COMMANDS_PROJECT_EDITOR_MAX 32

enum mtProjectStartMode
{
	mtProjectStartModeDonNothing,
	mtProjectStartModeOpenLast,
	mtProjectStartModeOpenProject,
	mtProjectStartModeNewProject,

};


enum mtProjectEditorEvents
{
	mtPriojectEditorEventNone,
	mtPriojectEditorEventLoadLastProjFailed,
	mtPriojectEditorEventPadPress,

	ProjEditEventCount
};

//=====================================================================
class cMtProjectEditor
{
public:
	void update();
	void start(uint8_t mode);
	void stop();

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

	void processLabels();


	void setButtonLabel(uint8_t number, char * label);
	void updateButtonsFunctions();
	void setButtonFunction(uint8_t number, uint8_t function);

	void setPotsLabel(uint8_t number, char * label);
	void updatePotsFunctions();
	void setPotFunction(uint8_t number, uint8_t function);

	void (*eventFunct)(uint8_t, void*, void*, void*);

	// elementy modulu

	//lista plikow/folderow
	enum
	{
		browseLocationTypeSave,
		browseLocationTypeOpen,
	};

	const uint8_t file_list_pos = 0;
	uint8_t filesListEnabled = 0;

	char *filesNames[128];
	char filePath[256];
	char fileName[32];

	uint8_t browseLocationType;
	uint8_t locationLevel;
	char locationFilesList[100][20];
	uint16_t locationFilesCount;

	FsFile sdLocation;

	void exploreActualLocation();



	//funkcje przyciskow
	void newProject(uint8_t value);
	void openProject(uint8_t value);
	void saveProject(uint8_t value);
	void copyProject(uint8_t value);
	void browseSave(uint8_t value);
	void browseOpen(uint8_t value);
	void browseCancel(uint8_t value);


	//funkcje potow




	uint8_t	refreshModule = 0;
	uint8_t	moduleStart = 0;


//========================================================
	// odswiezanie elementow modulu

	uint8_t labelsChanged;
	uint8_t filesListChanged;





//=======================================================================

//przyciski  ------------------------------------------------------------
	enum
	{
		buttonFunctNone,
		buttonFunctNewProject,
		buttonFunctOpenProject,
		buttonFunctSaveProject,
		buttonFunctCopyProject,
		buttonFunctBrowseSave,
		buttonFunctBrowseOpen,
		buttonFunctBrowseCancel,



		//-------------------------------
		buttonFunctCount
	};

	char *buttonLabels[5];
	uint8_t buttonFunctions[5];

	char buttonFunctionLabels[buttonFunctCount][20] =
	{
		{0},
		"New project",
		"Open project",
		"Save project",
		"Copy project",
		"Save",
		"Open",
		"Cancel",




	};



//potencjometry  ---------------------------------------------------------

	enum
	{
		potFunctNone,




		//-------------------------------
		potFunctCount
	};

	char *potLabels[5];
	uint8_t potFunctions[5];

	char potFunctionLabels[potFunctCount][20] =
	{
		{0},


	};

	const uint16_t potFuncRes[potFunctCount] =
	{
			100, // potFunctionNone,

	};

	const uint8_t potFuncAcc[potFunctCount] =
	{
			3, // potFunctionNone,

	};

};

//=====================================================================
extern cMtProjectEditor mtProjectEditor;


int32_t loadSample(const char *filename, int16_t * buf);
int32_t loadWavetable(const char *filename, int16_t * buf);
int32_t loadWavetableStandard(const char *filename, int16_t * buf);
int32_t loadWavetableSerum(const char *filename, int16_t * buf);
int32_t loadFullWavetableSerum(const char *filename, int16_t * buf);
uint8_t getMiddleWindow(uint16_t cnt, float windowsCnt, float windowsControl);
int16_t fmap(float x, float in_min, float in_max, float out_min, float out_max);
int32_t loadWavetable(const char *filename, int16_t * buf ,uint16_t * windowSize);
void readHeader(strWavFileHeader* header, FsFile * wavfile);


#endif
