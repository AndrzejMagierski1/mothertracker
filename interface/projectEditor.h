#ifndef INTERFACE_PROJECTEDITOR_H_
#define INTERFACE_PROJECTEDITOR_H_


#include <modulesBase.h>
#include "SD.h"

enum mtProjectStartMode
{
	mtProjectStartModeDoNothing,
	mtProjectStartModeOpenLast,
	mtProjectStartModeOpenProject,
	mtProjectStartModeNewProject,

};





class cProjectEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cProjectEditor() {}
	virtual ~cProjectEditor() {}

//-------------------------------------------------------------

	void showDefaultScreen();
	void showProjectsList();
	void showTemplatesList();


	void setDefaultScreenFunct();

//-------------------------------------------------------------

	uint8_t loadSamplesBank();


//-------------------------------------------------------------

	strList projectList;

	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl fileListControl;

	uint32_t topLabelColors[3] =
	{
			0x3F3F3F, // tekst
			0x000000, // tło
			0x3F3F3F, // ramka
	};


	uint32_t projectOptions = 0;



	void listOnlyFolderNames(const char* folder);

	FsFile sdLocation;
	uint8_t selectedLocation;
	uint16_t locationFilesCount;
	static const uint8_t files_list_length_max = 100;
	char locationFilesList[files_list_length_max][20];
	char *filesNames[files_list_length_max];



};





extern cProjectEditor projectEditor;




#endif /* INTERFACE_PROJECTEDITOR_H_ */