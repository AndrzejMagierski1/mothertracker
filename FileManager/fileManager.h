
#ifndef FILEMANAGER_FILEMANAGER_H_
#define FILEMANAGER_FILEMANAGER_H_

#include <stdint.h>


enum fileManagerStatus
{
	fmIdle,
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





class cFileManager
{
public:

	uint8_t getStatus() 	{ return status; }
	uint8_t getProgress() 	{ return progress; }


	uint8_t loadProjectFromWorkspace();
	uint8_t loadProject(char* name);


	uint8_t createEmptyTemplateProject();









private:
	uint8_t status;
	uint8_t progress;


};





#endif /* FILEMANAGER_FILEMANAGER_H_ */
