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





class FileManager
{

public:
//************************************************ FileManagerProject*******************************************************
	uint8_t openProject(char * name, uint8_t type);
	void importProject(char* sourceProjectPatch,char* name, char* newName);

	uint8_t prepareSaveAs(char *name, uint8_t type);
	void startSaveAsProject(char *name);

	uint8_t saveAsProject(char* name);
//	void saveProject();
	void startSaveProject();
	uint8_t getSaveProjectState();
	uint8_t getOpenProjectState();
	uint8_t getOpenProjectStateProgress();
	uint8_t getSaveProjectStateProgress();
	uint8_t createNewProject(char * name);
	void createEmptyTemplateProject(char * name);
	uint8_t loadProjectFromWorkspace();
	void autoSaveProject();

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
	void setLoadPattern(uint8_t index);
	void handleLoadPattern();

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
	void switchNextPatternInSong();
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
	uint8_t instrumentForcedSaveFlag;
	elapsedMillis instrumentRefresh;
	uint8_t patternIsChangedFlag;
	elapsedMillis patternRefresh;
//**************************************************************************************************************************
	friend class cProjectEditor;

	enum
	{
		saveAsChecking,
		saveAsOverwrite
	};


private:
//************************************************ FileManagerCore**********************************************************
	void formatSDCard();
	void copySample(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName);
	void writeInstrumentFile(char * name, strInstrument * instr);
	uint8_t readInstrumentFile(char * name, strInstrument * instr);
	uint8_t writePatternFile(char * name);
	uint8_t readPatternFile(char * name);
	void writeProjectFile(char * name,strMtProjectRemote * proj);
	uint8_t readProjectFile(char * name, strMtProjectRemote * proj);
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

	uint32_t allCopyingFileSizeSave = 0;
	uint32_t currentCopyingSizeSave = 0;
//**************************************************************************************************************************
//************************************************ FileManagerInstrument****************************************************
	uint8_t currentCopyStatusFlag;
	uint8_t lastCopyStatusFlag;
//**************************************************************************************************************************
//************************************************ FileManagerPattern*******************************************************
	void copyPattern(char* srcProjectPatch, uint8_t src_idx, char * dstProjectPatch, uint8_t dst_idx);
//**************************************************************************************************************************




};


extern FileManager fileManager;

extern int16_t sdram_effectsBank[15*256*1024]; // 7.5 MB
extern int16_t sdram_sampleBank[15*256*1024];

#endif

