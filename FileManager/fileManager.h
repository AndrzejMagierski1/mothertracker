
#ifndef FILEMANAGER_FILEMANAGER_H_
#define FILEMANAGER_FILEMANAGER_H_

#include <stdint.h>


const uint8_t FILEMANAGER_DEBUGLOG =  1;


enum fileManagerStatus
{
	fmIdle = 0,
	fmBrowsingSamples,
	fmBrowsingProjects,
	fmBrowsingFirmwares,
	fmImportingSamplesToWorkspace,
	fmDeleteingInstruments,
	fmPreviewSampleFromSd,
	fmLoadingProjectfromWorkspace,
	fmLoadingProjectFromProjects,
	fmSavingProjectToWorkspace,
	fmSavingProjectToProjects,
	fmLoadingPatternFromWorkspace,

	fmLoadEnd,
	fmSaveEnd,
	fmBrowseSamplesEnd,
	fmBrowseProjectsEnd,
	fmBrowseFirmwaresEnd,
	fmImportSamplesEnd,
	fmDeleteInstrumentsEnd,
	fmLoadPatternEnd,

	fmError,
	fmLoadError,
	fmSaveError,
	fmCopyError,
	fmBrowseSamplesError,
	fmBrowseProjectsError,
	fmBrowseFirmwaresError,
	fmImportSamplesError,
	fmDeleteInstrumentsError,
	fmLoadPatternError,

};

enum fileManagerOperation
{
	fmNoOperation = 0,
	fmLoadWorkspaceProject,		//1
	fmSaveWorkspaceProject,		//2
	fmCopyProjectsToWorkspace,	//3
	fmCopyWorkspaceToProjects,	//4

	fmBrowseSamples, 			//5
	fmBrowseProjects, 			//6
	fmBrowseFirmwares, 			//7

	fmImportSamplesToWorkspace,	//8
	fmPreviewSamplesFromSD,		//9
	fmDeleteInstruments,		//10

	fmLoadWorkspacePattern,		//11


};


struct strProjectFile;
struct strInstrumentFile;
class SdDir;
class SdFile;

extern SdDir sdLocation;
extern SdFile wavfile;

class cFileManager
{
public:
	void update();

	//-------------------------------------------------
	inline uint8_t getStatus() 	{ return status; }
	uint8_t getProgress();

	void clearStatus() 	{ status = fmIdle; }

	// flagi zmian
	void clearChangeFlags();
	bool isProjectChanged();
	void setAllChangeFlags();

	//getery
	bool projectExist(char* name);
	uint8_t getBrowsedFilesList(char*** list, uint32_t** memoryList);
	uint8_t getProjectsList(char*** list);
	uint8_t getFirmwaresList(char*** list);

	//setery
	void setProjectStructChanged();
	void setPatternStructChanged(uint8_t pattern);
	void setInstrumentStructChanged(uint8_t instrument);

	//-------------------------------------------------
	// metody glowne
	bool openProjectFromWorkspace();
	bool openProjectFromProjects(uint8_t index);
	bool saveProjectToWorkspace(bool forceSaveAll = false);
	bool saveProjectToProjects(char* projectNameToSave = nullptr);
	bool importSamplesToProject(uint8_t fileFrom, uint8_t fileTo, uint8_t instrumentSlot);

	bool browseSdCard(uint8_t* index);
	bool browseProjects();
	bool browseFirmwares();

	bool previevSamplefromSD(uint8_t index);
	bool stopPrevievSamplefromSD();

	bool deleteInstruments(uint8_t instrumentSlotFrom, uint8_t instrumentSlotTo);

	bool loadWorkspacePattern(uint8_t index);

	bool deleteProject(uint8_t index);


	// to chyba trzeba zoptymalizowac/wrzucic w petle \/
	bool createNewProjectInWorkspace();

	// to na pozniej \/

	bool loadWorkspacePatternNow(uint8_t index);
	bool saveWorkspacePatternNow(uint8_t index);
	bool loadTrack(uint8_t pattIndex, uint8_t trackIndex);

	uint8_t getNextSongPattern();
	uint8_t getSongPattern(uint8_t pos);
	void setSongPos(uint8_t pos);
	uint8_t resetToFirstSongPattern();
	bool switchNextPatternInSong();
	void copySongTracks(char *currentProjectPath, uint8_t src, uint8_t dest, uint8_t trackStartSrc, uint8_t trackStartDest, uint8_t tracksNum);
	void deleteTracks(char *currentProjectPath, uint8_t src, uint8_t trackStartSrc, uint8_t tracksNum);

	void updatePatternBitmask(uint8_t patternNum);
	void updatePatternBitmask(uint8_t index, uint8_t* sourcePattern);
	void storePatternUndoRevision();
	void storeSongUndoRevision(uint8_t index);
	void undoSongPattern();
	void undoPattern();
	void redoPattern();

	//-------------------------------------------------
	// globalne
	char* getCurrentProjectName() { return currentProjectName; }
	char* getCurrentProjectPath() { return currentProjectPatch; }

private:
	// na cele zewnetrzne (popupy itp)
	uint8_t status;
	uint8_t progress;

	// wewnetrzne stany maszynki itp
	uint8_t currentOperation;
	uint8_t currentOperationStep;
	char currentCopySrcPath[PATCH_SIZE];
	char currentCopyDestPath[PATCH_SIZE];
	bool forceOperation;

	// stringi
	char currentProjectPatch[PATCH_SIZE-PROJECT_NAME_SIZE];
	char currentProjectName[PROJECT_NAME_SIZE] = {0};
	char projectNamefromProjectFile[PROJECT_NAME_SIZE] = {0};

	// do obliczania progresu
	uint32_t totalMemoryToTranfser;
	uint32_t actualMemoryTransfered;

	//flagi zmian w aktywnym projeckie
	struct strChengesFlags
	{
		uint8_t project;
		uint8_t instrument[INSTRUMENTS_COUNT];
		uint8_t pattern[PATTERN_INDEX_MAX];
	} changesFlags;


	//SdDir* sdLocation;
	//SdFile* wavfile;

	// metody wewnetrzne ------------------------------------
	void throwError(uint8_t source);
	void report(const char* text, uint8_t value = 0);
	void moveToNextOperationStep();
	void skipNextOperationStep();
	void calcTotalMemoryToTransfer();
	void calcActualMemoryTransfered();

	// glowne update / workspace ------------------------------------
	void updateLoadProjectFromWorkspace();
	void updateSaveProjectToWorkspace();
	void updateCopyProjectsToWorkspace();
	void updateCopyWorkspaceToProjects();
	void updateBrowseSamples();
	void updateBrowseProjects();
	void updateBrowseFirmwares();
	void updateImportSamplesToWorkspace();
	void updateDeleteInstruments();
	void updateLoadWorkspacePattern();

	void autoSaveProjectToWorkspace();

	void stopOperationWithError(uint8_t error);

	void clearWorkspace();
	bool createWorkspaceDirs();
	bool createNewEmptyProjectInWorkspace();

	//init/finisz ------------------------------------
	void loadProjectFromWorkspaceInit();
	void loadProjectFromWorkspaceFinish();
	void saveProjectToWorkspaceInit();
	void saveProjectToWorkspaceFinish();

	void copyProjectsToWorkspaceInit();
	void copyProjectsToWorkspaceFinish();
	void copyWorkspaceToProjectsInit();
	void copyWorkspaceToProjectsFinish();

	// project ------------------------------------
	void loadProjectFileFromWorkspace();
	void saveProjectFileToWorkspace();
	void copyProjectFile();

	bool loadProjectFileFormFileStruct(strMtProject* project, strProjectFile* pFile);
	bool writeProjectFileToFileStruct(strMtProject* project, strProjectFile* pFile);

	void writeProjectFile(const char* name, strMtProject* proj);
	bool readProjectFile(const char* name, strMtProject* proj);
	void getDefaultProject(struct strMtProject* source);
	void getDefaultSong(struct strSong* source);
	void getDefaultValues(struct strMtValues* source);

	// pattern ------------------------------------
	void loadPatternFromWorkspace(uint8_t index);
	void savePatternToWorkspace();
	void copyPaterns();

	bool loadPatternFormFileStruct(uint8_t* pattern, uint8_t* patternFile);
	bool writePatternToFileStruct(uint8_t* pattern, uint8_t* patternFile);

	bool continuePatternProcess();

	void loadPatternFromWorkspaceFinish();

	bool writePatternFile(const char* filePath, uint8_t* sourcePattern);
	bool readPatternFile(const char* filePath, uint8_t* destPattern);
	bool saveActualPattern(const char* path, uint8_t index);
	bool loadPattern(const char* path, uint8_t index);

	uint16_t currentPattern = 0;

	// instrument ------------------------------------
	void loadInstrumentsFromWorkspace();
	void saveInstrumentsToWorkspace();
	void copyInstruments();
	void createEmptyInstrumentInWorkspace(uint8_t slot, char* name);
	void deleteInstrumentsFromWorkspace();


	bool loadInstrumentFormFileStruct(strInstrument* instrument, strInstrumentFile* instrumentFile);
	bool writeInstrumentToFileStruct(strInstrument* instrument, strInstrumentFile* instrumentFile);

	void calcFirstSlotToMoveInMemory(uint8_t calcStartSlot);
	void setCurrentInstrumentToFirstActiveAfterCurrent();
	void findLastActiveInstrumentBeforeCurrent();
	bool continueInstrumentProcess();

	void instrumentThrowError();
	void setDefaultActiveInstrument(struct strInstrument* targetInstrument);
	void getEmptyInstrument(struct strInstrument* source);
	uint32_t calcWorkspaceInstrumentsSize();

	uint8_t currentInstrument = 0;
	uint8_t lastActiveInstrument = 0; // do przydzielania pamieci
	uint8_t deleteEndInstrument;

	//samples ------------------------------------
	void loadSamplesFromWorkspace();
	void saveSamplesToWorkspace();
	void copySamples();
	void deleteSamplesFromWorkspace();

	void completeLoadedSampleStruct(bool success);

	void startSampleLoad();

	void moveSampleMemory();

	void importSamplesToWorkspaceInit();
	void importSamplesToWorkspaceContinue();
	void importSamplesToWorkspaceFinish();

	void deleteInstrumentsFromWorkspaceFinish();

	uint32_t getWaveSizeIfValid(const char *filename);
	uint32_t calcWorkspaceSamplesSize();
	uint32_t getActualSampleMemoryLoaded();
	void sampleThrowError();


	uint8_t sampleInProgress = 0;
	uint8_t currentSample = 0;
	uint32_t currentSampleSamplesCount = 0; // ilosc probek!!! (int16)
	uint8_t deleteEndSample;

	uint32_t importSamplesSize = 0;

	uint8_t importCurrentFile;
	int8_t importStartSlot;
	int8_t importEndSlot;
	int8_t firstSlotToMoveInMemory;





	// browser
	void browseProjectsLocation();
	static const uint8_t list_length_max = 100;
	uint8_t projectsListLength = 0;
	char* projectsList[list_length_max];

	void browseFirmwaresLocation();
	uint8_t firmwaresListLength = 0;
	char* firmwaresList[list_length_max];

	void browseCurrentLocation();
	void listOnlyFolderNames();
	void processDirFileSizes();
	void listOnlyWavFromActualPath();
	void browseFinish();
	void goUpInActualPath();

	uint8_t explorerListLength = 0;
	char *explorerList[list_length_max];
	char explorerCurrentPath[255] = {'/',0};
	uint8_t explorerDirLevel = 0;

	uint8_t listWavStage = 0;


	static const uint8_t PREVIOUS_POSITION_LIFO = 20;
	uint8_t explorerPositionTable[PREVIOUS_POSITION_LIFO];
	uint8_t explorerCurrentPosition;

	uint8_t openCalcStart = 0;
	uint8_t openCalcEnd = 0;
	uint8_t openCurrentPos =0;

	uint32_t currentFolderMemoryFileUsage[list_length_max];


	// preview sample







//patern undo



	//song



};




extern cFileManager newFileManager;
#endif /* FILEMANAGER_FILEMANAGER_H_ */
