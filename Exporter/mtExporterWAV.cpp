#include "mtExporterWAV.h"
#include "sdram.h"

__NOINIT(EXTERNAL_RAM) int16_t exportBuffer1[SEND_BUF_SIZE];
__NOINIT(EXTERNAL_RAM) int16_t exportBuffer2[SEND_BUF_SIZE];

mtExporter exporter;

void setOnLastExportStep()
{


	switch(exporter.type)
	{
		case  mtExporter::exportType::song : 			exporter.songExporter.setOnLastStep();											break;
		case  mtExporter::exportType::songStems : 		exporter.songStemsExporter.trackExporter.localSongExporter.setOnLastStep(); 	break;
		case  mtExporter::exportType::pattern : 	break;
		case  mtExporter::exportType::patternStems : break;
		default: break;
	}
}

void mtExporter::update()
{
	switch(type)
	{
		case exportType::song : 			songExporter.update();			break;
		case exportType::songStems : 		songStemsExporter.update(); 	break;
		case exportType::pattern : 	break;
		case exportType::patternStems : break;
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
		case exportType::song : 			songExporter.start(path);			break;
		case exportType::songStems : 		songStemsExporter.start(path); 		break;
		case exportType::pattern : 	break;
		case exportType::patternStems : break;
		default: break;
	}
}
