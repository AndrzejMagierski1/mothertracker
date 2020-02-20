
#ifndef FILEMANAGER_FILEMANAGER_H_
#define FILEMANAGER_FILEMANAGER_H_

#include <stdint.h>


const uint8_t FILEMANAGER_DEBUGLOG =  1;


enum fileManagerStatus
{
	fmIdle = 0,
	fmExploring,
	fmCopying,
	fmLoading,
	fmLoadingProject,
	fmLoadingPattern,
	fmLoadingInstrument,
	fmSaving,
	fmSavingProject,
	fmSavingPattern,
	fmSavingInstrument,

};

enum fileManagerOperation
{
	fmNoOperation = 0,
	fmOpenWorkspaceProject,
};




class cFileManager
{
public:
	void update();

	//-------------------------------------------------
	uint8_t getStatus() 	{ return currentOperation; }
	uint8_t getProgress() 	{ return progress; }

	//-------------------------------------------------
	bool openProjectFromWorkspace();
	bool loadProject(char* name);

	bool createNewProjectInWorkspace();

	uint8_t createEmptyTemplateProject();





private:
	// na cele zewnetrzne (popupy itp)
	uint8_t status;
	uint8_t progress;

	// wewnetrzne stany maszynki
	uint8_t currentOperation;
	uint8_t currentOperationStep;


	strProjectFile* projectFile;

	// metody wewnetrzne
	void throwError(uint8_t source);
	void moveToNextOperationStep();


	// workspace
	void clearWorkspace();
	void updateOpenWorkspaceProject();
	bool createWorkspaceDirs();
	bool createNewEmptyProjectInWorkspace();

	// project
	void loadProjectFromWorkspace();
	bool loadProjectFormFileStruct(strMtProject * project, strProjectFile* pFile);

	void writeProjectFile(const char* name, strMtProject* proj);
	bool readProjectFile(const char* name, strMtProject* proj);
	void getDefaultProject(struct strMtProject* source);
	void getDefaultSong(struct strSong* source);
	void getDefaultValues(struct strMtValues* source);

	// pattern
	bool loadPatternFormFileStruct(uint8_t* pattern, uint8_t* patternFile);

	bool writePatternFile(const char* filePath, uint8_t* sourcePattern);
	bool readPatternFile(const char* filePath, uint8_t* destPattern);
	bool saveActualPattern(const char* path, uint8_t index);
	bool loadPattern(const char* path, uint8_t index);
	bool loadTrack(uint8_t pattIndex, uint8_t trackIndex);
	void loadPatternFromWorkspace(uint8_t index);

	// instrument
	void getDefaultInstrument(struct strInstrument* source);


	//loader

};




extern cFileManager newFileManager;


#endif /* FILEMANAGER_FILEMANAGER_H_ */
