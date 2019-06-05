#ifndef INTERFACE_SAMPLEIMPORTER_H_
#define INTERFACE_SAMPLEIMPORTER_H_


#include <modulesBase.h>
#include "SD.h"




class cSampleImporter: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cSampleImporter() {}
	virtual ~cSampleImporter() {}

	void showDefaultScreen();
	void showFolderTree();
	void showFilesTree();
	void activateLabelsBorder();

	strList folderList;
	strList fileList;
	strList instrumentList;

	hControl topLabel[4];
	hControl bottomLabel[4];

	hControl folderListControl;
	hControl fileListControl;
	hControl instrumentListControl;

	uint8_t selectedLabel = 0;

	void setDefaultScreenFunct();


	uint8_t changeFolderSelection(int16_t value);
	uint8_t changeFileSelection(int16_t value);


//--------------------------------------------------------------
	FsFile sdLocation;
	static const uint8_t list_length_max = 100;
	char actualPath[255] = {0};
	uint8_t dirLevel;

	void BrowseFolder();
	void SelectFile();
	void goUpInActualPath();

// foldery
	void listOnlyFolderNames(char* path);

	uint16_t locationFolderCount;
	char locationFolderList[list_length_max][20];
	char *folderNames[list_length_max];
	uint8_t selectedFolder;
	//char *folderPath;

// pliki wave w wybranym folderze

	void listOnlyWaveNames(char* folder);
	void listOnlyWavFromActualPath();
	uint8_t isWavFile(char* fileName);

	uint16_t locationFileCount;
	char locationFileList[list_length_max][20];
	char *fileNames[list_length_max];
	uint8_t selectedFile;

//--------------------------------------------------------------



};





extern cSampleImporter sampleImporter;




#endif /* INTERFACE_SAMPLEIMPORTER_H_ */
