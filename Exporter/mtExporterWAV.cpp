#include "mtExporterWAV.h"
#include "sdram.h"
#include "mtSequencer.h"

__NOINIT(EXTERNAL_RAM) int16_t exportBuffer1[SEND_BUF_SIZE];
__NOINIT(EXTERNAL_RAM) int16_t exportBuffer2[SEND_BUF_SIZE];

mtExporter exporter;

void setOnLastExportStep()
{
	switch(exporter.type)
	{
		case  mtExporter::exportType::song : 																								break;
		case  mtExporter::exportType::songStems : 		 																					break;
		case  mtExporter::exportType::pattern : 		exporter.patternExporter.setOnLastStep();											break;
		case  mtExporter::exportType::patternStems : 	exporter.patternStemsExporter.trackExporter.localPatternExporter.setOnLastStep();	break;
		default: break;
	}
}

void mtExporter::begin()
{
	sequencer.setOnPatternEnd(setOnLastExportStep);
}

void mtExporter::update()
{
	switch(type)
	{
		case exportType::song : 											break;
		case exportType::songStems : 		 								break;
		case exportType::pattern : 		patternExporter.update();			break;
		case exportType::patternStems : patternStemsExporter.update();		break;
		default: break;
	}
}

void mtExporter::setExportType(exportType t)
{
	type = t;
}

void mtExporter::start(char * path, exportType t)
{
	setExportType(t);
	switch(t)
	{
		case exportType::song : 													break;
		case exportType::songStems : 		 										break;
		case exportType::pattern : 			patternExporter.start(path);			break;
		case exportType::patternStems : 	patternStemsExporter.start(path);		break;
		default: break;
	}
}
