
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
	uint8_t getStatus() 	{ return status; }
	uint8_t getProgress() 	{ return progress; }

	//-------------------------------------------------
	bool loadProjectFromWorkspace();
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


	// metody wewnetrzne
	bool readProjectFile(const char * name, strMtProject * proj);


	void clearWorkspace();
	void updateOpenWorkspaceProject();


	// misc
	bool createWorkspaceDirs();


};


extern cFileManager newFileManager;


#endif /* FILEMANAGER_FILEMANAGER_H_ */
