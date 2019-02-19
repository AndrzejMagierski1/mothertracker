#ifndef SOURCE_MTFILEMANAGER_H_
#define SOURCE_MTFILEMANAGER_H_

#include "mtStructs.h"
#include "mtProjectEditor.h"

class FileManager
{

public:

	uint8_t openProject(char * name);
	void createNewProject(char* patch, char * name);
	void importSampleToProject( char* filePatch, char* name, int8_t index, uint8_t type);
	///todo: jest mozliwosc ze beda potem prywatne
	void writeInstrumentFile(char * name, strInstrument * instr);
	uint8_t readInstrumentFile(char * name, strInstrument * instr);
	void writePatternFile(char * name);
	uint8_t readPatternFile(char * name);
	void writeProjectFile(char * name,strMtProjectRemote *proj);
	uint8_t readProjectFile(char * name, strMtProjectRemote * proj);

	friend class cMtProjectEditor;
private:

	char currentProjectPatch[PATCH_SIZE];
	char currentProjectName[PROJECT_NAME_SIZE];
};


extern FileManager fileManager;
#endif

