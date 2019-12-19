#ifndef SOURCE_MTFILEMANAGER_H_
#define SOURCE_MTFILEMANAGER_H_

#include <stdint.h>
#include "SD.h"
#include "mtStructs.h"
#include "wavHeaderReader.h"
#include "patternEditor.h"
#include "mtSamplesLoader.h"
#include "mtSamplesImporter.h"
#include "mtSamplesCopyier.h"


struct strProjectFileHeader
{
	char id_file[2];
	uint16_t type;
	char version[4];
	char id_data[4];
	uint16_t size;
};

struct strInstrumentFile
{
	struct strInstrumentDataAndHeader
	{
		strProjectFileHeader instrHeader;
		strInstrument instrument;

	} instrumentDataAndHeader;

	uint32_t crc;
};

struct strProjectFile
{
	struct strProjectDataAndHeader
	{
		strProjectFileHeader projectHeader;
		strMtProjectRemote project;
	} projectDataAndHeader;

	uint32_t crc;
};

typedef enum
{
	sSavePrepare,
	sSavePatterns,
	sSaveInstrumentFiles,
	sSaveSamples,
	sSaveEppromConfig,
	sSavingDone,

}saving_status_t;

typedef enum
{
	sLoadPrepare,
	sLoadClearWorkspace,
	sLoadInstrumentFiles,
	sLoadPatterns,
	sLoadSamples,
	sLoadInstrumentsToMem,
	sLoadDone,

}loading_status_t;

typedef enum
{
	sWspLoadPrepare,
	sWspLoadInstumentFiles,
	sWspLoadInstrumentsToMem,
	sWspLoadDone,

}loadingFromWrsp_status_t;

typedef enum
{
	sDeletePrepare,
	sDeleteAlll,
	sDeleteDone,
}deleting_status_t;

typedef struct
{
	uint8_t stage;
	uint8_t currPatternIdx;
	uint8_t currInstrumentFileIdx;
	uint8_t currSampleIdx;
	uint8_t nextSampleFlag;
	uint8_t forcedInstrReload;

}save_load_handle_t;

typedef struct
{
	char deleteSourcePath[255];
	FsFile roots[10];
	uint8_t lastRoot;
	uint8_t addedLength[10];
	uint8_t lastAddedLengthNum;

	uint8_t isDone;

}delete_handle_t;


class FileManager
{

public:
//************************************************ FileManagerProject*******************************************************

	uint8_t openProjectStart(char * name, uint8_t type);
	void refreshProjectOpening();

	uint8_t loadProjectFromWorkspaceStart();
	void refreshLoadProjectFromWorkspace();

	void deleteProjectStart(const char *projectName);
	void refreshDeleting();

	uint8_t prepareSaveAs(char *name, uint8_t type);
	void startSaveProject();
	void refreshSaveProject();

	void createEmptyTemplateProject(char * name);

	void autoSaveProject();

	void autoSaveWorkspace(uint8_t forcedWorkspaceSave);

//**************************************************************************************************************************
//************************************************ FileManagerInstrument****************************************************
	void setStart(uint8_t startPoint);
	uint8_t assignSampleToInstrument(char* filePatch, char* name,int8_t instrumentIndex, uint8_t type  = mtSampleTypeWaveFile);
	void setAutoLoadFlag();
	void clearAutoLoadFlag();
	void importInstrumentToProject(char* projectPatch, char* name, int8_t index);
	void deleteSample(int8_t index);
	void deleteInstrument(int8_t index);
	void saveInstrument(int8_t index);
	void setLoadLength(uint8_t filesNum);
	uint8_t getActiveInstrumentsInProject(char * name);
	SamplesLoader samplesLoader;
	SamplesImporter samplesImporter;
	SamplesCopyier samplesCopyier;
//**************************************************************************************************************************
//************************************************ FileManagerPattern*******************************************************
	uint8_t loadPattern(uint8_t index);
	uint8_t loadTrack(uint8_t pattIndex, uint8_t trackIndex);
	void setLoadPattern(uint8_t index);
//	void setTrackToLoad(uint8_t pattIndex, uint8_t trackIndex);
	void handleLoadPattern();

	void copySongTracks(char *currentProjectPath, uint8_t src, uint8_t dest, uint8_t trackStartSrc, uint8_t trackStartDest, uint8_t tracksNum);
	void deleteTracks(char *currentProjectPath, uint8_t src, uint8_t trackStartSrc, uint8_t tracksNum);

	uint8_t savePattern(uint8_t index);
	void storePatternUndoRevision();
	void undoPattern();
	void redoPattern();
	void importPatternToProject(char* filePatch, char* name, int8_t index);
	void deletePattern(int8_t index);
	uint8_t getNextSongPattern();
	uint8_t getSongPattern(uint8_t pos);
	void  setSongPos(uint8_t pos);
	uint8_t resetToFirstSongPattern();
	void addPatternToProject (int8_t index);
	bool switchNextPatternInSong();
	void setPatternChangeFlag(uint8_t num);
	void setInstrumentChangeFlag(uint8_t num);
//	void refreshPatternView();

//**************************************************************************************************************************
//************************************************ FileManagerCore**********************************************************
	void update();
	uint8_t getEndImportSampleFlag();
	void clearEndImportSampleFlag();
	char currentProjectPatch[PATCH_SIZE-PROJECT_NAME_SIZE];
	char currentProjectName[PROJECT_NAME_SIZE];
	char currentProjectNameOpenTemplate[PROJECT_NAME_SIZE];
//**************************************************************************************************************************
//******************************************************ChangeFlags*********************************************************
	uint8_t configIsChangedFlag;
	elapsedMillis configChangedRefresh;
	uint8_t instrumentIsChangedFlag[48];
	//uint8_t instrumentForcedSaveFlag;
	elapsedMillis instrumentRefresh;
	uint8_t patternIsChangedFlag[PATTERN_INDEX_MAX];
	elapsedMillis patternRefresh;
//**************************************************************************************************************************
	friend class cProjectEditor;

	enum
	{
		saveAsChecking,
		saveAsOverwrite
	};

	uint8_t loadingInProgress = 0;
	uint8_t savingInProgress = 0;
	uint8_t deletingInProgress = 0;

	uint8_t getSavingStatus();
	uint8_t getLoadingStatus();
	uint8_t getDeletingStatus();


	void getDefaultSong(struct strSong *source);
	void getDefaultInstrument(struct strInstrument *source);
	void getDefaultProject(struct strMtProject *source);
	void getDefaultValues(struct strMtValues *source);

private:
//************************************************ FileManagerCore**********************************************************
	void formatSDCard();
	void copySample(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName);
	void writeInstrumentFile(char * name, strInstrument * instr);
	uint8_t readInstrumentFile(char * name, strInstrument * instr);
	uint8_t writePatternFile(char * name, uint8_t *sourcePattern);
	uint8_t readPatternFile(char * name, uint8_t *patternDest);
	void 	writeProjectFile(char * name, strMtProject *proj);
	uint8_t readProjectFile(char * name, strMtProject * proj);
	uint8_t endImportSampleFlag = 0;
	uint8_t autoLoadFlag = 1;
	uint8_t saveProjectFlag = 0;
	uint8_t currentSaveWave = 0;
	uint8_t openWorkspaceCreateFlag = 0;
	uint8_t saveAsFlag = 0;
	uint8_t samplesCopyierCurrentState = 0;
	uint8_t lastCopyierCurrentState = 0;
	uint8_t loadLength = 0;
	uint32_t allCopyingFileSizeOpen = 0;
	uint32_t currentCopyingSizeOpen = 0;

	//uint32_t allCopyingFileSizeSave = 0;
	uint32_t currentCopyingSizeSave = 0;
//**************************************************************************************************************************
//************************************************ FileManagerInstrument****************************************************
	uint8_t currentCopyStatusFlag;
	uint8_t lastCopyStatusFlag;
//**************************************************************************************************************************
//************************************************ FileManagerPattern*******************************************************
	void copyPattern(char* srcProjectPatch, uint8_t src_idx, char * dstProjectPatch, uint8_t dst_idx);
//**************************************************************************************************************************




	save_load_handle_t saveHandle;
	save_load_handle_t loadHandle;

	save_load_handle_t loadFromWorkspaceHandle;
	void refreshSaveInstrumentFiles();
	void refreshSaveSamples();
	void refreshSavePatterns();
	void saveEeprom();
	void finishSaving();

	void refreshLoadingInstrumentFiles();
	void refreshLoadingPatterns();
	void refreshLoadingSamples();
	void refreshLoadingInstrToMem();
	void refreshClearWorkspace();

	void refreshLoadInstrFilesFromWorkspace();
	void refreshLoadingInstrToMemFromWorkspace();

	void moveToNextStage(save_load_handle_t *handle);
	void refreshClearDir(const char* dirToClear, save_load_handle_t *handle);



	//uint8_t deleteBuffer[512];
	//char deletePath[200];
	//uint8_t deleteCurrPositiionInDir;
	delete_handle_t deleteHandle;
	void recursiveRemoveProject(FsFile *source);

	char deleteProjectName[PROJECT_NAME_SIZE];


	void updatePatternBitmask(uint8_t patternNum);


};


extern FileManager fileManager;

extern int16_t sdram_effectsBank[15*256*1024]; // 7.5 MB
extern int16_t sdram_sampleBank[15*256*1024];

#endif

