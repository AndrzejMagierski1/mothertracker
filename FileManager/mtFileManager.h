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

//struct strPatternFile
//{
//
//	strProjectFileHeader patternHeader;
//
//	// > tutaj w pliku jest sekwencja <
//	uint32_t crc;
//
//};

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

	uint8_t openProject(char * name, uint8_t type);
	void importProject(char* sourceProjectPatch,char* name, char* newName);
	uint8_t saveAsProject(char* name);
	void saveProject();
	uint8_t createNewProject(char * name);

	/////////////////////////////////////////////////////////////////////////
	uint8_t loadPattern(uint8_t index);
	uint8_t savePattern(uint8_t index);
	/////////////////////////////////////////////////////////////////////////

	uint8_t assignSampleToInstrument(char* filePatch, char* name,int8_t instrumentIndex, uint8_t type  = mtSampleTypeWaveFile);
	void importInstrumentToProject(char* projectPatch, char* name, int8_t index);
	void importPatternToProject(char* filePatch, char* name, int8_t index);
	void createEmptyTemplateProject(char * name);
//	void addInstrumentToProject (int8_t index);
	void addPatternToProject (int8_t index);
	void deleteSample(int8_t index);
	void deleteInstrument(int8_t index);
	void deletePattern(int8_t index);
	uint8_t getNextSongPattern();
	uint8_t resetToFirstSongPattern();
	void switchNextPatternInSong();
	void refreshPatternView();
//	uint8_t isSongMode();

	void update();

	friend class cProjectEditor;
	SamplesLoader samplesLoader;
	SamplesImporter samplesImporter;
	char currentProjectPatch[PATCH_SIZE-PROJECT_NAME_SIZE];
private:
	void formatSDCard();


	void copySample(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName);
	void copyPattern(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName);
	void writeInstrumentFile(char * name, strInstrument * instr);
	uint8_t readInstrumentFile(char * name, strInstrument * instr);
	uint8_t writePatternFile(char * name);
	uint8_t readPatternFile(char * name);
	void writeProjectFile(char * name,strMtProjectRemote * proj);
	uint8_t readProjectFile(char * name, strMtProjectRemote * proj);

	uint8_t currentCopyStatusFlag;
	uint8_t lastCopyStatusFlag;
	char currentProjectName[PROJECT_NAME_SIZE];
//	uint8_t currentPattern;
};


extern FileManager fileManager;

extern int16_t sdram_effectsBank[4*1024*1024];
extern int16_t sdram_sampleBank[4*1024*1024];

#endif

