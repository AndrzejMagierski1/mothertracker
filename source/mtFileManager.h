#ifndef SOURCE_MTFILEMANAGER_H_
#define SOURCE_MTFILEMANAGER_H_

#include "mtStructs.h"
#include "mtProjectEditor.h"

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
	///todo: jest mozliwosc ze beda potem prywatne
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
#endif

