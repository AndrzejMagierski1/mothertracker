
#ifndef FILEMANAGER_FILEMANAGER_H_
#define FILEMANAGER_FILEMANAGER_H_

#include <stdint.h>


const uint8_t FILEMANAGER_DEBUGLOG =  1;


enum fileManagerStatus
{
	fmIdle = 0,
	fmExploring,
	fmCopying,
	fmLoading,
	fmLoadingProjectfromWorkspace,
	fmLoadingProjectFromProjects,
	fmLoadingInstrument,
	fmSaving,
	fmSavingProject,
	fmSavingPattern,
	fmSavingInstrument,


	fmLoadEnd,
	fmError,
	fmLoadError,
	fmCopyError,
};

enum fileManagerOperation
{
	fmNoOperation = 0,
	fmOpenWorkspaceProject,
	fmCopyProjectToWorkspace,
};




class cFileManager
{
public:
	void update();

	//-------------------------------------------------
	uint8_t getStatus() 	{ return status; }
	uint8_t getProgress();

	void clearStatus() 	{ status = fmIdle; }

	// flagi zmian
	void clearChangeFlags();
	bool isProjectChanged();

	void setProjectStructChanged();
	void setPatternStructChanged(uint8_t pattern);
	void setInstrumentStructChanged(uint8_t instrument);

	//-------------------------------------------------
	// metody glowne
	bool openProjectFromWorkspace();
	bool openProjectFromProjects(uint8_t index);

	bool createNewProjectInWorkspace();

	uint8_t  getProjectsList(char*** list);



	//-------------------------------------------------
	// globalne
	char currentProjectPatch[PATCH_SIZE-PROJECT_NAME_SIZE];
	char currentProjectName[PROJECT_NAME_SIZE] = {0};
	static const uint8_t files_list_length_max = 100;
	char* projectsNames[files_list_length_max];

private:
	// na cele zewnetrzne (popupy itp)
	uint8_t status;
	uint8_t progress;

	// wewnetrzne stany maszynki itp
	uint8_t currentOperation;
	uint8_t currentOperationStep;
	char currentCopySrcPath[PATCH_SIZE];
	char currentCopyDestPath[PATCH_SIZE];

	// do obliczania progresu
	uint32_t totalMemoryToTranfser;
	uint32_t actualMemoryTransfered;

	//flagi zmian w aktywnym projeckie
	uint8_t projectChangeFlag;
	uint8_t instrumentIsChangedFlag[INSTRUMENTS_COUNT];
	uint8_t patternIsChangedFlag[PATTERN_INDEX_MAX];

	// metody wewnetrzne
	void throwError(uint8_t source);
	void moveToNextOperationStep();
	void calcTotalMemoryToTransfer();
	void calcActualMemoryTransfered();

	// glowne/workspace
	void updateOpenWorkspaceProject();
	void updateCopyProjectToWorkspace();

	void clearWorkspace();
	bool createWorkspaceDirs();
	bool createNewEmptyProjectInWorkspace();

	//init/finisz
	void loadWorkspaceProjectInit();
	void loadWorkspaceProjectFinish();
	void copyProjectToWorkspaceInit();
	void copyProjectToWorkspaceFinish();

	// project
	void loadProjectFromWorkspace();
	bool loadProjectFormFileStruct(strMtProject * project, strProjectFile* pFile);

	void copyProjectToWorkspace();

	void writeProjectFile(const char* name, strMtProject* proj);
	bool readProjectFile(const char* name, strMtProject* proj);
	void getDefaultProject(struct strMtProject* source);
	void getDefaultSong(struct strSong* source);
	void getDefaultValues(struct strMtValues* source);

	// pattern
	void loadPatternFromWorkspace(uint8_t index);
	bool loadPatternFormFileStruct(uint8_t* pattern, uint8_t* patternFile);

	void copyPaternsToWorkspace();
	void continuePatternProcess();

	bool writePatternFile(const char* filePath, uint8_t* sourcePattern);
	bool readPatternFile(const char* filePath, uint8_t* destPattern);
	bool saveActualPattern(const char* path, uint8_t index);
	bool loadPattern(const char* path, uint8_t index);
	bool loadTrack(uint8_t pattIndex, uint8_t trackIndex);

	uint16_t currentPattern = 0;

	// instrument
	void loadInstrumentsFromWorkspace();
	bool loadInstrumentFormFileStruct(strInstrument* instrument, strInstrumentFile* instrumentFile);
	void continueInstrumentProcess();

	void copyInstrumentsToWorkspace();

	void instrumentThrowError();
	void getDefaultInstrument(struct strInstrument* source);

	uint32_t calcWorkspaceInstrumentsSize();

	uint8_t currentInstrument = 0;

	//samples
	void loadSamplesFromWorkspace();
	void startSampleLoad();
	void continueSampleLoad();
	void completeLoadedSampleStruct();
	uint32_t getWaveSizeIfValid(const char *filename);
	uint32_t calcWorkspaceSamplesSize();
	uint32_t getActualSampleMemoryLoaded();
	void sampleThrowError();

	void copySamplesToWorkspace();
	void continueSampleProcess();

	uint8_t sampleLoadPhase = 0;
	uint8_t currentSample = 0;
	uint32_t currentSampleSamplesCount = 0; // ilosc probek!!! (int16)


	//loader

};




extern cFileManager newFileManager;


#endif /* FILEMANAGER_FILEMANAGER_H_ */
