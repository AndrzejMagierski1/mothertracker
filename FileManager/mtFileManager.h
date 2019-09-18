#ifndef SOURCE_MTFILEMANAGER_H_
#define SOURCE_MTFILEMANAGER_H_

#include <stdint.h>
#include "SD.h"
#include "mtStructs.h"
#include "wavHeaderReader.h"
#include "patternEditor.h"
#include "mtSamplesLoader.h"
#include "mtSamplesImporter.h"

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
	uint8_t saveAsProject(char* name);
	void saveProject();
	uint8_t createNewProject(char * name);
	void createEmptyTemplateProject(char * name);

//**************************************************************************************************************************
//************************************************ FileManagerInstrument****************************************************
	void setStart(uint8_t startPoint);
	uint8_t assignSampleToInstrument(char* filePatch, char* name,int8_t instrumentIndex, uint8_t type  = mtSampleTypeWaveFile);
	void setAutoLoadFlag();
	void clearAutoLoadFlag();
	void importInstrumentToProject(char* projectPatch, char* name, int8_t index);
	void deleteSample(int8_t index);
	void deleteInstrument(int8_t index);
	SamplesLoader samplesLoader;
	SamplesImporter samplesImporter;
//**************************************************************************************************************************
//************************************************ FileManagerPattern*******************************************************
	uint8_t loadPattern(uint8_t index);
	uint8_t savePattern(uint8_t index);
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
//**************************************************************************************************************************
	friend class cProjectEditor;

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
	char currentProjectName[PROJECT_NAME_SIZE];
	uint8_t endImportSampleFlag = 0;
	uint8_t autoLoadFlag = 1;
//**************************************************************************************************************************
//************************************************ FileManagerInstrument****************************************************
	uint8_t currentCopyStatusFlag;
	uint8_t lastCopyStatusFlag;
//**************************************************************************************************************************
//************************************************ FileManagerPattern*******************************************************
	void copyPattern(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName);
//**************************************************************************************************************************



};


extern FileManager fileManager;

extern int16_t sdram_effectsBank[4*1024*1024];
extern int16_t sdram_sampleBank[4*1024*1024];

#endif

