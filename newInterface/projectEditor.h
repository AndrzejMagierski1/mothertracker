#ifndef NEWINTERFACE_PROJECTEDITOR_H_
#define NEWINTERFACE_PROJECTEDITOR_H_


#include "modulesBase.h"

#include "SD.h"

enum mtProjectStartMode
{
	mtProjectStartModeDoNothing,
	mtProjectStartModeOpenLast,
	mtProjectStartModeOpenProject,
	mtProjectStartModeNewProject,

};


enum mtProjectEditorEvents
{
	mtPriojectEditorEventNone,
	mtPriojectEditorEventLoadLastProjFailed,
	mtPriojectEditorEventPadPress,
	mtProjectEditorEventLoadSampleBank,

	ProjEditEventCount
};




class cProjectEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	cProjectEditor() {}
	~cProjectEditor() {}

	void showDefaultScreen();


	strList projectList;

	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl fileListControl;


	typedef void (cProjectEditor::*FUNC1) (void);

	//void (cProjectEditor::*funct1)(void) = nullptr;
	FUNC1 funct1;




	void listOnlyFolderNames(const char* folder);
	uint8_t selectedLocation;
	static const uint8_t files_list_length_max = 100;
	char locationFilesList[files_list_length_max][20];
	uint16_t locationFilesCount;
	FsFile sdLocation;
	uint8_t filesListEnabled = 0;
	uint8_t browseLocationType;
	char *filesNames[128];
	char filePath[256];
	char fileName[32];



};





extern cProjectEditor projectEditor;




#endif /* NEWINTERFACE_PROJECTEDITOR_H_ */
