#ifndef SOURCE_MTFILEMANAGER_H_
#define SOURCE_MTFILEMANAGER_H_

#include <stdint.h>
#include "SD.h"

//parametry przetwarzanego pliku
struct strWavFileHeader
{
	uint32_t chunkId;			//0
	uint32_t chunkSize;
	uint32_t format;

	uint32_t subchunk1Id;		//12
	uint32_t subchunk1Size;
	uint16_t AudioFormat;		//20
	uint16_t numChannels;		//22
	uint32_t sampleRate;		//24
	uint32_t byteRate;			//28
	uint16_t blockAlign;		//32
	uint16_t bitsPerSample;		//34

	uint32_t subchunk2Id;		//36
	uint32_t subchunk2Size;		//40

};


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

struct strPatternFile
{
	struct strPatternDataAndHeader
	{
		strProjectFileHeader patternHeader;
		Sequencer::strPattern pattern;

	} patternDataAndHeader;

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

	uint8_t openProject(char * name, uint8_t type);
	void importProject(char* sourceProjectPatch,char* name, char* newName);
	void saveAsProject(char* name);
	void saveProject();
	void importSampleToProject(char* filePatch, char* name, char* newName, int8_t sampleIndex,int8_t instrumentIndex, uint8_t type);
	void importInstrumentToProject(char* filePatch, char* name, char* newName, int8_t index);
	void importPatternToProject(char* filePatch, char* name, char* newName, int8_t index);
	void createEmptyTemplateProject(char * name);

	///todo: jest mozliwosc ze beda potem prywatne
	void formatSDCard();
	void createNewProject(char * name);
	void copySample(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName);
	void copyPattern(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName);
	void writeInstrumentFile(char * name, strInstrument * instr);
	uint8_t readInstrumentFile(char * name, strInstrument * instr);
	void writePatternFile(char * name);
	uint8_t readPatternFile(char * name);
	void writeProjectFile(char * name,strMtProjectRemote * proj);
	uint8_t readProjectFile(char * name, strMtProjectRemote * proj);

	friend class cMtProjectEditor;
private:

	char currentProjectPatch[PATCH_SIZE];
	char currentProjectName[PROJECT_NAME_SIZE];
	uint8_t currentPattern;
};


extern FileManager fileManager;




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

