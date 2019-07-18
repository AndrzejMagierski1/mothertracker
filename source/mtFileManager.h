#ifndef SOURCE_MTFILEMANAGER_H_
#define SOURCE_MTFILEMANAGER_H_

#include <stdint.h>
#include "SD.h"
#include "mtStructs.h"

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

enum loaderStateType
{
	loaderStateTypeEnded,
	loaderStateTypeInProgress
};
//**********************************************************************WAVE LOADER***********************************************************************************//
constexpr uint16_t BUFOR_COUNT = 10;

class WaveLoader
{
public:
	void update();
	uint32_t start(const char *filename, int16_t * buf);
	uint8_t stop();
	uint8_t getState();
	uint8_t getStopStatus();
	uint32_t getInfoAboutWave(const char *filename);
	uint8_t getCurrentWaveProgress();
	uint32_t getCurrentWaveLoadedMemory();
	void setStopStatus(uint8_t s);
private:
	strWavFileHeader sampleHead;
	FsFile wavfile;
	uint8_t state;
	uint32_t accBufferLength;
	int16_t * currentAddress;
	int8_t stopFlag = - 1;

};
//******************************************************************WAVETABLE LOADER**********************************************************************************//

class WavetableLoader
{
public:
	void update();
	uint8_t start(const char *filename, int16_t * buf);
	uint32_t stop();
	int32_t fmLoadWavetable(const char *filename, int16_t * buf ,uint16_t * windowSize);
private:

};

//*******************************************************************SAMPLES LOADER***********************************************************************************//


class SamplesLoader
{
public:
//	uint8_t loadSamplesMemory();
	void update();
	void start(uint8_t startIndex);
	uint8_t getMemoryUsageChangeFlag();
	void clearMemoryUsageChangeFlag();
	uint8_t getLoadChangeFlag();
	void clearLoadChangeFlag();
	uint8_t getCurrentProgress();
	uint8_t getStateFlag();
	WaveLoader waveLoader;
	WavetableLoader wavetableLoader;
private:

	uint8_t currentIndex;
	uint8_t state;
	uint8_t lastIndex;
	int32_t currentSize;
	uint8_t memoryUsageChange = 0;
	uint8_t loadedFlagChange = 0;
	uint32_t sizeAllFiles;
	uint32_t currentLoadSize = 0;
	uint32_t currentStepLoadSize = 0;

};

//********************************************************************************************************************************************************************//

class FileManager
{

public:

	uint8_t openProject(char * name, uint8_t type);
	void importProject(char* sourceProjectPatch,char* name, char* newName);
	uint8_t saveAsProject(char* name);
	void saveProject();
	uint8_t createNewProject(char * name);
	uint8_t importSampleToProject(char* filePatch, char* name,int8_t instrumentIndex, uint8_t type = mtSampleTypeWaveFile);
	void importInstrumentToProject(char* projectPatch, char* name, int8_t index);
	void importPatternToProject(char* filePatch, char* name, int8_t index);
	void createEmptyTemplateProject(char * name);
	void addInstrumentToProject (int8_t index);
	void addPatternToProject (int8_t index);
	void deleteSample(int8_t index);
	void deleteInstrument(int8_t index);
	void deletePattern(int8_t index);
	void update();

	friend class cProjectEditor;
	SamplesLoader samplesLoader;
	char currentProjectPatch[PATCH_SIZE];
private:
	void formatSDCard();


	void copySample(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName);
	void copyPattern(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName);
	void writeInstrumentFile(char * name, strInstrument * instr);
	uint8_t readInstrumentFile(char * name, strInstrument * instr);
	void writePatternFile(char * name);
	uint8_t readPatternFile(char * name);
	void writeProjectFile(char * name,strMtProjectRemote * proj);
	uint8_t readProjectFile(char * name, strMtProjectRemote * proj);


	char currentProjectName[PROJECT_NAME_SIZE];
	uint8_t currentPattern;
};


extern FileManager fileManager;

extern int16_t sdram_effectsBank[4*1024*1024];
extern int16_t sdram_sampleBank[4*1024*1024];



int32_t loadWavetableStandard(const char *filename, int16_t * buf);
int32_t loadWavetableSerum(const char *filename, int16_t * buf);
//int32_t loadFullWavetableSerum(const char *filename, int16_t * buf);
void loadFullWavetableSerum(const char *baseName);
uint8_t getMiddleWindow(uint16_t cnt, float windowsCnt, float windowsControl);
int16_t fmap(float x, float in_min, float in_max, float out_min, float out_max);
int32_t fmLoadWavetable(const char *filename, int16_t * buf ,uint16_t * windowSize);
void readHeader(strWavFileHeader* header, FsFile * wavfile);

#endif

