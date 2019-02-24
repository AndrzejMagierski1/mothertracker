
#ifndef MT_PROJECT_MANAGER_H
#define MT_PROJECT_MANAGER_H

#include <stdint.h>
#include "mtStructs.h"
#include "SD.h"
#include "mtFileManager.h"

#include <FastCRC.h>
#include <FastCRC_cpu.h>
#include <FastCRC_tables.h>

#define COMMANDS_PROJECT_EDITOR_MAX 32

enum mtProjectStartMode
{
	mtProjectStartModeDoNothing,
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

	//edycja nazw---------------------------------------------
	enum
	{
		editNameTypeNone,
		editNameTypeSaveProject,
		editNameTypeSaveTemplate,
	};



	uint8_t editNameType;
	uint8_t editNameEnabled = 0;
	char editName[20] = {"Untitled"};
	char editLabel[32] = {"Eneter name for new project:"};

	//lista plikow/folderow---------------------------------------------
	enum
	{
		browseLocationTypeNone,
		browseLocationTypeSaveProject,
		browseLocationTypeSaveTemplate,
		browseLocationTypeOpenProject,
		browseLocationTypeOpenTemplate,
	};

	static const uint8_t files_list_length_max = 100;
	const uint8_t files_list_pos = 0;
	uint8_t filesListEnabled = 0;
	uint8_t browseLocationType;

	char *filesNames[128];
	char filePath[256];
	char fileName[32];

	uint8_t selectedLocation;
	char locationFilesList[files_list_length_max][20];
	uint16_t locationFilesCount;

	FsFile sdLocation;

	void exploreActualLocation();
	void listOnlyFolderNames(const char* folder);


	//funkcje przyciskow---------------------------------------------
	void newProject(uint8_t value);
	void openProject(uint8_t value);
	void saveProject(uint8_t value);
	void copyProject(uint8_t value);
	void browseOpenSave(uint8_t value);
	void browseOpen(uint8_t value);
	void browseCancel(uint8_t value);
	void saveProjectAs(uint8_t value);


	//funkcje potow
	void changeProjectsListPos(int16_t value);
	void changeTemplatesListPos(int16_t value);


	uint8_t	refreshModule = 0;
	uint8_t	moduleStart = 0;


//========================================================
	// odswiezanie elementow modulu

	uint8_t labelsChanged;
	uint8_t filesListChanged;
	uint8_t editNameChanged;





//=======================================================================

//przyciski  ------------------------------------------------------------
	enum
	{
		buttonFunctNone,
		buttonFunctNewProject,
		buttonFunctOpenProject,
		buttonFunctSaveProject,
		buttonFunctCopyProject,
		buttonFunctBrowseOpenSave,
		buttonFunctBrowseOpen,
		buttonFunctBrowseCancel,
		buttonFunctSaveProjectAs,




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
		"Select",
		"Open",
		"Cancel",
		"Create"




	};



//potencjometry  ---------------------------------------------------------

	enum
	{
		potFunctNone,
		potFunctChangeProjectsList,
		potFunctChangeTemplatesList,
		potFunctEditText,


		//-------------------------------
		potFunctCount
	};

	char *potLabels[5];
	uint8_t potFunctions[5];

	char potFunctionLabels[potFunctCount][20] =
	{
		{0},
		"Select project:",
		"Select template:",


	};

	const uint16_t potFuncRes[potFunctCount] =
	{
			100, // potFunctionNone,
			20,
			20,

	};

	const uint8_t potFuncAcc[potFunctCount] =
	{
			3, // potFunctionNone,
			1,
			1,

	};

};

//=====================================================================
extern cMtProjectEditor mtProjectEditor;


int32_t loadSample(const char *filename, int16_t * buf);
int32_t loadWavetable(const char *filename, int16_t * buf);
int32_t loadWavetableStandard(const char *filename, int16_t * buf);
int32_t loadWavetableSerum(const char *filename, int16_t * buf);
//int32_t loadFullWavetableSerum(const char *filename, int16_t * buf);
void loadFullWavetableSerum(const char *baseName);
uint8_t getMiddleWindow(uint16_t cnt, float windowsCnt, float windowsControl);
int16_t fmap(float x, float in_min, float in_max, float out_min, float out_max);
int32_t loadWavetable(const char *filename, int16_t * buf ,uint16_t * windowSize);
void readHeader(strWavFileHeader* header, FsFile * wavfile);


#endif
