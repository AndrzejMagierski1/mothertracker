#ifndef NEWINTERFACE_PATTERNEDITOR_H_
#define NEWINTERFACE_PATTERNEDITOR_H_


#include "modulesBase.h"

#include "SD.h"



enum mtPatternEditorEvents
{
	mtPatternEditEventNone,
	mtPatternEditEventLoadLastProjFailed,
	mtPatternEditEventPadPress,
	mtPatternEditventLoadSampleBank,

	mtPatternEditEventCount
};




class cPatternEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	cPatternEditor() {}
	~cPatternEditor() {}

	void showDefaultScreen();


	strList projectList;

	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl fileListControl;










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





extern cPatternEditor patternEditor;




#endif /* NEWINTERFACE_PATTERNEDITOR_H_ */
