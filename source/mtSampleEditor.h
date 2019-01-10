#ifndef MT_SAMPLE_EDITOR_H
#define MT_SAMPLE_EDITOR_H

#include "SD.h"
#include <stdint.h>


enum enumMtSampleEditorMode
{
	mtSampleEditorModeDisabled,
	mtSampleEditorModeWaveEdit,
};

enum enumMtSampleEditorButtonFunction
{
	mtSampleEditorButtonFunctionNone,
	mtSampleEditorButtonFunctionPlay,
	mtSampleEditorButtonFunctionStop,
	mtSampleEditorButtonFunctionLoop,


	//-------------------------------
	mtSampleEditorButtonFunctionCount
};

const char mtSampleEditorButtonsLabels[mtSampleEditorButtonFunctionCount][20] =
{
		{0},
		{'P','l','a','y'},
		{'S','t','o','p'},
		{'L','o','o','p'},

};



class cMtSampleEditor
{
public:
	void begin();
	void update();
	void start(char * file_path);
	void buttonPress(uint8_t button);
	void potChange(uint8_t pot, int16_t value);

private:
	void processSpectrum();
	void readFileInfo();

	void setButtonLabel(uint8_t number, char * label);
	void updateButtons();
	void setButton(uint8_t number, uint8_t function);

	uint8_t	SampleEditorMode = mtSampleEditorModeDisabled;
	uint8_t	refreshSampleEditor = 0;
	uint8_t	SampleEditorStart = 0;
	uint8_t	editMode = 0;

	char filePath[256];
	char fileName[32];

	// parametry edytora
	uint8_t loopMode;
	uint8_t playMode;

	uint32_t startCursor;
	uint32_t stopCursor;
	uint32_t loopCursor;


	//parametry pliku w [próbkach]
	uint32_t sampleViewStart;
	uint32_t sampleViewStop;
	uint32_t sampleViewStartLast;
	uint32_t sampleViewStopLast;
	uint32_t monoSamplesCount;

	//parametry rysowania spektrum
	struct strSpectrum
	{
		uint16_t baseLength;
		uint16_t actualLength;
		uint8_t data[480];
		uint16_t start;
		uint16_t end;

	} spectrum;


	//parametry przetwarzanego pliku
	struct strSampleFile
	{
		uint32_t chunkId;
		uint32_t chunkSize;
		uint32_t format;

		uint32_t subchunk1Id;
		uint32_t subchunk1Size;
		uint16_t AudioFormat;
		uint16_t numChannels;
		uint32_t sampleRate;
		uint32_t byteRate;
		uint16_t blockAlign;
		uint16_t bitsPerSample;

		uint32_t subchunk2Id;
		uint32_t subchunk2Size;

	} sampleDesc;

	FsFile sampleFile;


	//przyciski w edytorze
	char buttonLabels[5][20];
	uint8_t buttonFunction[5];

};



extern cMtSampleEditor mtSampleEditor;

#endif
