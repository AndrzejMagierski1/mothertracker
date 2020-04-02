
#ifndef FILEMANAGER_FILEMANAGER_H_
#define FILEMANAGER_FILEMANAGER_H_

#include <stdint.h>


//extern int16_t sdram_sampleBank[4 * 1024 * 1024];


const uint8_t FILEMANAGER_DEBUGLOG =  1;


enum fileManagerStatus
{
	fmIdle = 0,
	fmBrowsingSamples,
	fmBrowsingProjects,
	fmBrowsingMods,
	fmBrowsingFirmwares,
	fmImportingSamplesToWorkspace,
	fmCopyingInstrumentsInWorkspace,
	fmDeleteingInstruments,
	fmPreviewSampleFromSd,
	fmLoadingProjectfromWorkspace,
	fmLoadingProjectFromProjects,
	fmImportingMod,
	fmSavingProjectToWorkspace,
	fmSavingProjectToProjects,
	fmLoadingPatternFromWorkspace,
	fmExportingSoundSong,
	fmExportingSoundSongStems,
	fmExportingSoundPattern,
	fmExportingSoundPatternStems,
	fmExportingSoundRenderSelection,
	fmSavingRecordedSound,
	fmSavingImportingRecordedSound,
	fmImportingSampleFromSampleEditor,


	fmLoadEnd,
	fmSaveEnd,
	fmBrowseSamplesEnd,
	fmBrowseProjectsEnd,
	fmBrowseModsEnd,
	fmBrowseFirmwaresEnd,
	fmImportSamplesEnd,
	fmCopyingInstrumentsEnd,
	fmDeleteInstrumentsEnd,
	fmLoadPatternEnd,
	fmExportSoundEnd,
	fmSaveRecordedSoundEnd,
	fmImportSampleFromSampleEditorEnd,



	fmError,
	fmLoadError,
	fmImportModError,
	fmSaveError,
	fmCopyError,
	fmBrowseSamplesError,
	fmBrowseProjectsError,
	fmBrowseModsError,
	fmBrowseFirmwaresError,
	fmImportSamplesError,
	fmCopyingInstrumentsError,
	fmDeleteInstrumentsError,
	fmLoadPatternError,
	fmExportSoundError,
	fmSaveRecordedError,
	fmImportSampleFromSampleEditorError,

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
	fmBrowseMods, 				//7
	fmBrowseFirmwares, 			//8

	fmImportSamplesToWorkspace,	//8
	fmCopyInstrumentsInWorkspace,//9
	fmPreviewSamplesFromSD,		//10
	fmDeleteInstruments,		//11

	fmLoadWorkspacePattern,		//12

	fmExportSound,				//13
	fmSaveRecordedSound,		//14

	fmImportSampleFromSampleEditor, // 15


	fmImportModFile,			//13


};


enum enImportModMode
{
	importModFiletype_notSupported,
	importModFiletype_mod,
	importModFiletype_it,
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

	void clearStatus() 	{ if(status >= fmLoadEnd) status = fmIdle; }

	// flagi zmian
	void clearChangeFlags();
	bool isProjectChanged();
	void setAllChangeFlags();

	//getery
	bool projectExist(char* name);
	uint8_t getBrowsedFilesList(char*** list, uint32_t** memoryList);
	uint8_t getProjectsList(char*** list);
	uint8_t getModsList(char*** list);
	uint8_t getFirmwaresList(char*** list);

	uint8_t getLoadedPatternIndex();

	//setery
	void setProjectStructChanged();
	void setPatternStructChanged(uint8_t pattern);
	void setInstrumentStructChanged(uint8_t instrument);

	//-------------------------------------------------
	// metody glowne
	bool openProjectFromWorkspace();
	bool openProjectFromProjects(uint8_t index);
	bool importModAfterLoadNewProject(uint8_t index);
	bool saveProjectToWorkspace(bool forceSaveAll = false);
	bool saveProjectToProjects(char* projectNameToSave = nullptr);
	bool importSamplesToProject(uint8_t fileFrom, uint8_t fileTo, uint8_t instrumentSlot);
	bool copyInstrumentsInWorkspace(uint8_t copyInstrSrc, uint8_t copyInstrCount, uint8_t copyInstrDest);

	bool browseSdCard(uint8_t* index);
	bool browseProjects();
	bool browseMods();
	bool browseFirmwares();

	bool previevSamplefromSD(uint8_t index);
	bool stopPrevievSamplefromSD();

	bool deleteInstruments(uint8_t instrumentSlotFrom, uint8_t instrumentSlotTo);

	bool loadWorkspacePattern(uint8_t index);

	bool deleteProject(uint8_t index);
	bool deleteMod(uint8_t index);

	bool exportSound(uint8_t mode);
	bool exportSoundCancel();
	bool saveRecordedSound(char* fileName, int8_t importSlot);

	bool importSampleFromSampleEditor(int16_t* memoryAddres, uint32_t length, uint8_t instrumentSlot);

	// to chyba trzeba zoptymalizowac/wrzucic w petle \/
	bool createNewProjectInWorkspace();

	// to na pozniej \/

	bool loadWorkspacePatternNow(uint8_t index);
	bool saveWorkspacePatternNow(uint8_t index);
	bool loadPerformanceTrackToBuffer(uint8_t pattIndex, uint8_t trackIndex);


	uint8_t getNextSongPattern();
	uint8_t getSongPattern(uint8_t pos);
	void setSongPos(uint8_t pos);
	uint8_t resetToFirstSongPattern();
	bool switchNextPatternInSong();
	void copySongTracks(char *currentProjectPath, uint8_t src, uint8_t dest, uint8_t trackStartSrc, uint8_t trackStartDest, uint8_t tracksNum);
	void deleteTracks(char *currentProjectPath, uint8_t src, uint8_t trackStartSrc, uint8_t tracksNum);

	bool updatePatternBitmask(uint8_t patternNum);
	bool updatePatternBitmask(uint8_t index, uint8_t* sourcePattern);
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
		uint8_t pattern[PATTERN_INDEX_MAX+1];
	} changesFlags;


	//SdDir* sdLocation;
	//SdFile* wavfile;

	// metody wewnetrzne ------------------------------------
	void throwError(uint8_t source);
	void showWarning(uint8_t source);
	void report(const char* text, uint8_t value = 0);
	void report(const char* text, const char* text2);
	void moveToNextOperationStep();
	void moveToPrevOperationStep();
	void skipNextOperationStep();
	void calcTotalMemoryToTransfer();
	void calcActualMemoryTransfered();

	// glowne update / workspace ------------------------------------
	void updateLoadProjectFromWorkspace();
	void updateImportModFile();
	void updateSaveProjectToWorkspace();
	void updateCopyProjectsToWorkspace();
	void updateCopyWorkspaceToProjects();
	void updateBrowseSamples();
	void updateBrowseProjects();
	void updateBrowseMods();
	void updateBrowseFirmwares();
	void updateImportSamplesToWorkspace();
	void updateCopyInstrumentsInWorkspace();
	void updateDeleteInstruments();
	void updateLoadWorkspacePattern();
	void updateExportSound();
	void updateSaveRecordedSound();
	void updateImportSampleFromSampleEditor();

	void autoSaveProjectToWorkspace();

	void stopOperationWithError(uint8_t error);

	void clearWorkspace();
	bool createWorkspaceDirs();
	bool createNewEmptyProjectInWorkspace();

	void clearProjectChangedFlag();
	void clearPatternChanged(uint8_t pattern);
	void clearInstrumentChanged(uint8_t instrument);

	//init/finisz ------------------------------------
	void loadProjectFromWorkspaceInit();
	void loadProjectFromWorkspaceFinish();
	void saveProjectToWorkspaceInit();
	void saveProjectToWorkspaceContinue();
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
	uint8_t loadedPattern = 0;
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
	uint8_t saveInstrument(uint8_t idx);
	void copyInstruments();
	void createEmptyInstrumentInWorkspace();
	void deleteInstrumentsFromWorkspace();


	bool loadInstrumentFormFileStruct(strInstrument* instrument, strInstrumentFile* instrumentFile);
	bool writeInstrumentToFileStruct(strInstrument* instrument, strInstrumentFile* instrumentFile);

	void calcFirstSlotToMoveInMemory(uint8_t calcStartSlot);
	void findLastActiveInstrumentBeforeCurrent();
	bool continueInstrumentProcess();

	void instrumentThrowError();
	void setDefaultActiveInstrument(struct strInstrument* targetInstrument);
	void getEmptyInstrument(struct strInstrument* source);
	uint32_t calcWorkspaceInstrumentsSize();

	uint8_t copySrcSlot;
	uint8_t copyDestSlot;
	uint8_t copySlotsCount;

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
	void moveMemory(uint8_t* memoryStart, uint8_t* memoryEnd, int32_t memoryOffset);

	void importSamplesToWorkspaceInit();
	void importSamplesToWorkspaceContinue();
	void importSamplesToWorkspaceFinish();

	void copyInstrumentsInWorkspaceInit();
	void copyInstrumentsInWorkspaceContinue();
	void copyInstrumentsInWorkspaceFinish();

	void deleteInstrumentsFromWorkspaceFinish();

	uint32_t getWaveSizeIfValid(const char *filename);
	uint32_t calcWorkspaceSamplesSize();
	uint32_t getActualSampleMemoryLoaded();
	void sampleThrowError();


	uint8_t sampleInProgress = 0;
	uint8_t currentSample = 0;
	uint32_t currentSampleSamplesCount = 0; // ilosc probek!!! (int16)
	uint8_t deleteEndSample;

	uint32_t newSamplesSize = 0;
	uint32_t oldSamplesSize = 0;

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

	void browseModsLocation();
	uint8_t modsListLength = 0;
	char* modsList[list_length_max];

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


	// export sound
	void exportSoundEnd();
	void exportSoundGetStatus();
	uint8_t getExportProgress();

	// recording
	void saveRecording();
	void saveRecordingEnd();
	char* getRecordingFileName();
	void saveRecordedSoundFinish();
	uint8_t recordingImportSlot;
	// preview sample

	// sample edior
	void importSampleFromSampleEditorInit();
	void importSampleFromSampleEditorFinish();
	uint8_t* importFromSampleEditorAddress;
	uint32_t importFromSampleEditorLength;
    

	enImportModMode  checkImportFileType();

    
	/// IMPORT MOD

	void importModFile_Init();
	void importModFile_GetInstrumentData();
	void importModFile_SaveInstrument();

	void importModFile_SongInit();
	void importModFile_Patterns();
	void importModFile_ImportWave();
	void importModFile_WriteWave();
	void importModFile_Finish();
	void importModFile_Error();

	int8_t periodToNote(uint16_t period);
	void printNote(uint8_t note);

	bool importModFileAfterNewProject = 0;
	static const uint8_t modSampleNameSize = 22;


	uint8_t importModFileType = importModFiletype_mod;

	struct strImportFileDataCommon
	{
		char filename[255];	// tylko nazwa
		char path[255];			// cala sciezka

	} impFileData;

	struct strImportModFile
	{
		uint8_t instrumentsCount = 31;
		uint8_t instrumentsActualIndex = 0;
		uint8_t channelsCount = 4;

		uint8_t patternsMax = 1;
		uint8_t patternsActualIndex = 0;

		const uint8_t sampleInfoSize = 30;
		const uint8_t songInfoSize = 1 + 1 + 128 + 4;

		const uint16_t patternSize = 1024;

		uint8_t sampleActualIndex = 0;
		uint8_t waveWriteFlag = 0;
		int16_t *waveSrcPtr;

		uint32_t modImport_saveLength = 0;

	} modFileData;

	struct strModFileData
	{

		char sampleName[modSampleNameSize];
		uint16_t sampleLengthInWords = 0;
		uint8_t finetune = 0;

		/*
		 *  Value:  Finetune:
		 0        0
		 1       +1
		 2       +2
		 3       +3
		 4       +4
		 5       +5
		 6       +6
		 7       +7
		 8       -8
		 9       -7
		 A       -6
		 B       -5
		 C       -4
		 D       -3
		 E       -2
		 F       -1

		 */

		//Range is $00-$40, or 0-64 decimal.
		uint8_t volume = 0;

		/*
		 Repeat point for sample 1. Stored as number of words offset
		 from start of sample. Multiply by two to get offset in bytes.
		 */
		uint16_t repeatPointInWords = 0;

		/*
		 Repeat Length for sample 1. Stored as number of words in
		 loop. Multiply by two to get replen in bytes.
		 */
		uint16_t repeatLengthInWords = 0;

	} modSampleData;
	/*
	 * 'M.K', '4CHN',
	 '6CHN','8CHN','FLT4','FLT8.
	 */

	struct strModSong
	{
		uint8_t length = 0;
		uint8_t oldByte = 127;
		uint8_t playlist[128] { 0 };
		uint8_t theFourLetters[4] { 0 };

	} modSong;
	/// IMPORT MOD KONIEC

	// import IT
	void importItFile_Init();
	void importItFile_ProcessHeader();
	void importItFile_ProcessSong();
	void importItFile_ProcessOffsets();

	void importItFile_ProcessInstruments();
	void importItFile_OpenSample();
	void importItFile_InitPattern();
	void importItFile_ProcessPattern(uint32_t patternOffset,
										uint16_t length,
										uint16_t rows);


	void importItFile_setPattern(uint8_t index, uint8_t length);
	void importItFile_setStep(uint8_t step,
						uint8_t track,
						uint8_t note,
						uint8_t instrument,
						uint8_t fx,
						uint8_t fxVal);
	void importItFile_savePattern();






	uint32_t getInstrumentOffset(uint8_t index);
	uint32_t getSampleOffset(uint8_t index);
	uint32_t getPatternOffset(uint8_t index);
	uint32_t getFileVariable(uint32_t subFileOffset,
								uint32_t offset,
								uint8_t varSize);





	// import IT koniec
	//song


	// debug Log
	void saveDebugLogToSd();

};




extern cFileManager newFileManager;
#endif /* FILEMANAGER_FILEMANAGER_H_ */
