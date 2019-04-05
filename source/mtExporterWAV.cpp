#include "mtExporterWAV.h"


mtExporterWAV exporterWAV;

void mtExporterWAV::startExport(char * patch)
{
	AudioOutputI2S::startExport(patch);
	sequencer.play();
	status = exportStatus::exportDuring;
}

void mtExporterWAV::finishExport()
{
	AudioOutputI2S::finishExport();
	status = exportStatus::exportFinished;
}

uint8_t mtExporterWAV::getStatus()
{
	return (uint8_t)status;
}

