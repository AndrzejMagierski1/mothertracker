
#include <stdint.h>

#include "mtDisplay.h"
#include "mtSampleEditor.h"

//#include "Arduino.h"

cMtSampleEditor mtSampleEditor;

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtSampleEditor::begin()
{


}


void cMtSampleEditor::update()
{
	if(SampleEditorMode == mtSampleEditorModeDisabled) return;
	if(!refreshSampleEditor) return;
	refreshSampleEditor = 0;

	if(SampleEditorMode == mtSampleEditorModeWaveEdit)
	{
		if(SampleEditorStart)
		{
			SampleEditorStart = 0;
			mtDisplay.setMode(mtDisplayModeSampleEditor);

			sampleViewStartLast = 0;
			sampleViewStopLast = 0;

			readFileInfo();
			sampleViewStart = 0;
			sampleViewStop = monoSamplesCount;//  wymusza odwiezenie widoku spectrum
		}

		if(sampleViewStart != sampleViewStartLast || sampleViewStop != sampleViewStopLast)
		{
			processSpectrum();

			mtDisplay.setMode(mtDisplayModeSampleEditor);
		}



	}
}


void cMtSampleEditor::start(char * file_path)
{
	uint8_t file_name_pos = 0;
	for(uint8_t i = 0; i < 256; i++)
	{
		if(file_path[i] == 0) break;
		if(file_path[i] == '/') file_name_pos = i+1;
		filePath[i] = file_path[i];
	}

	for(uint8_t i = file_name_pos; i < 256; i++)
	{
		if(file_path[i] == 0) break;
		fileName[i] = file_path[i];
	}

	SampleEditorMode = mtSampleEditorModeWaveEdit;
	SampleEditorStart = 1;
	refreshSampleEditor = 1;
}


void cMtSampleEditor::buttonPress(uint8_t button)
{
	switch(buttonFunction[button])
	{
	case mtSampleEditorButtonFunctionNone  	: 	break;
	case mtSampleEditorButtonFunctionPlay  	:	break;
	case mtSampleEditorButtonFunctionStop  	:	break;
	case mtSampleEditorButtonFunctionLoop  	: 	break;

	default: break;
	}
	refreshSampleEditor = 1;
}


void cMtSampleEditor::potChange(uint8_t pot, int16_t value)
{
	switch(pot)
	{
	case 0:
	{


		break;
	}
	case 2:
	{

		break;
	}

	default: break;
	}



	refreshSampleEditor = 1;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// spectrum processing


void cMtSampleEditor::processSpectrum()
{





}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// wave file processing


void cMtSampleEditor::readFileInfo()
{
	sampleFile.close();
	sampleFile.open(filePath,O_READ);

	sampleFile.read((char*)&sampleDesc, 44);
	sampleFile.close();

	monoSamplesCount = (sampleDesc.subchunk2Size - 8) / sampleDesc.numChannels;
}


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// BUTTONS

void cMtSampleEditor::setButtonLabel(uint8_t number, char * label)
{
	uint8_t i = 0;
	buttonLabels[number][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		buttonLabels[number][i] = label[i];
		i++;
	}
	buttonLabels[number][i] = 0;
}


void cMtSampleEditor::updateButtons()
{
	setButton(0, mtSampleEditorButtonFunctionNone);
	setButton(1, mtSampleEditorButtonFunctionNone);
	setButton(2, mtSampleEditorButtonFunctionNone);
	setButton(3, mtSampleEditorButtonFunctionNone);
	setButton(4, mtSampleEditorButtonFunctionNone);

	if(playMode == 0)
	{
		setButton(0, mtSampleEditorButtonFunctionPlay);
	}
	else
	{
		setButton(0, mtSampleEditorButtonFunctionStop);
	}

	if(loopMode == 0)
	{
		setButton(1, mtSampleEditorButtonFunctionLoop);
	}
	else
	{
		setButton(1, mtSampleEditorButtonFunctionLoop);
	}

	setButtonLabel(0, (char *)&mtSampleEditorButtonsLabels[buttonFunction[0]][0]);
	setButtonLabel(1, (char *)&mtSampleEditorButtonsLabels[buttonFunction[1]][0]);
	setButtonLabel(2, (char *)&mtSampleEditorButtonsLabels[buttonFunction[2]][0]);
	setButtonLabel(3, (char *)&mtSampleEditorButtonsLabels[buttonFunction[3]][0]);
	setButtonLabel(4, (char *)&mtSampleEditorButtonsLabels[buttonFunction[4]][0]);

	mtDisplay.setSampleEditorLabels(&buttonLabels[0][0]);
}


void cMtSampleEditor::setButton(uint8_t number, uint8_t function)
{
	buttonFunction[number] = function;
}

