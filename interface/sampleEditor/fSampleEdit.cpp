#include "sampleEditor/sampleEditor.h"
#include "core/graphicProcessing.h"

cSampleEditor sampleEditor;

void cSampleEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	if(mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX)
	{
		mtProject.values.lastUsedInstrument = 0;
	}


	editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];

	if(editorInstrument->isActive)
	{
		spectrumParams.address = editorInstrument->sample.address;
		spectrumParams.length = editorInstrument->sample.length;
	}
	else
	{
		spectrumParams.address = nullptr;
		spectrumParams.length = 0;
	}

	GP.processSpectrum( &spectrumParams, &zoom, &spectrumData);

	display.setControlShow(spectrum);
	display.refreshControl(spectrum);

}

void cSampleEditor::update()
{

}

void cSampleEditor::stop()
{

}

