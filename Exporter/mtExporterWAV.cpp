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
		case  mtExporter::exportType::pattern : 		exporter.patternExporter.setOnLastStep();											break;
		case  mtExporter::exportType::patternStems : 	exporter.patternStemsExporter.trackExporter.localPatternExporter.setOnLastStep();	break;
		default: break;
	}
}

void setOnLastExportStepInSong()
{
	switch(exporter.type)
	{
		case  mtExporter::exportType::song : 			exporter.songExporter.setOnLastStep();												break;
		case  mtExporter::exportType::songStems : 		exporter.songStemsExporter.trackExporter.localSongExporter.setOnLastStep();			break;
		default: break;
	}
}

void mtExporter::begin()
{
	sequencer.setOnPatternEnd(setOnLastExportStep);
	sequencer.setOnSongEnd(setOnLastExportStepInSong);
}

void mtExporter::update()
{
	switch(type)
	{
		case exportType::song : 		songExporter.update();				break;
		case exportType::songStems : 	songStemsExporter.update();	 		break;
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
		case exportType::song : 			songExporter.start(path);				break;
		case exportType::songStems : 		songStemsExporter.start(path);	 		break;
		case exportType::pattern : 			patternExporter.start(path);			break;
		case exportType::patternStems : 	patternStemsExporter.start(path);		break;
		default: break;
	}
}

uint8_t mtExporter::getProgress()
{
	switch(type)
	{
		case exportType::song : 			songExporter.getProgress();				break;
		case exportType::songStems : 		songStemsExporter.getProgress();		break;
		case exportType::pattern : 			patternExporter.getProgress();			break;
		case exportType::patternStems : 	patternStemsExporter.getProgress();		break;
		default: break;
	}
}
uint8_t mtExporter::getState()
{
	switch(type)
	{
		case exportType::song : 			songExporter.getStatus();				break;
		case exportType::songStems : 		songStemsExporter.getStatus();			break;
		case exportType::pattern : 			patternExporter.getStatus();			break;
		case exportType::patternStems : 	patternStemsExporter.getStatus();		break;
		default: break;
	}
}

uint8_t mtExporter::getStemsTrack()
{
	switch(type)
	{
		case exportType::songStems : 		songStemsExporter.getTrack(); 			break;
		case exportType::patternStems : 	patternStemsExporter.getTrack();		break;
		default: break;
	}
}
