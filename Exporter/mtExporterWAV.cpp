#include "mtExporterWAV.h"
#include "sdram.h"
#include "mtSequencer.h"

__NOINIT(EXTERNAL_RAM) int16_t exportBuffer1[SEND_BUF_SIZE];
__NOINIT(EXTERNAL_RAM) int16_t exportBuffer2[SEND_BUF_SIZE];

mtExporter exporter;

IntervalTimer updateExportTimer;

void updateReceivingExport()
{
	exporter.updateReceiving();
}

void setOnLastExportStep()
{
	switch(exporter.type)
	{
		case  exportPattern : 			exporter.patternExporter.setOnLastStep();											break;
		case  exportPatternStems : 		exporter.patternStemsExporter.trackExporter.localPatternExporter.setOnLastStep();	break;
		case  exportRenderSelection :  	exporter.renderExporter.setOnLastStep();
		default: break;
	}
}

void setOnLastExportStepInSong()
{
	switch(exporter.type)
	{
		case  exportSong : 				exporter.songExporter.setOnLastStep();												break;
		case  exportSongStems : 		exporter.songStemsExporter.trackExporter.localSongExporter.setOnLastStep();			break;
		default: break;
	}
}


void mtExporter::begin()
{
	sequencer.setOnPatternEnd(setOnLastExportStep);
	sequencer.setOnSongEnd(setOnLastExportStepInSong);

	updateExportTimer.begin(updateReceivingExport,1500);
	updateExportTimer.priority(255);
}

void mtExporter::update()
{
	switch(type)
	{
		case exportSong : 				songExporter.update();				break;
		case exportSongStems : 			songStemsExporter.update();	 		break;
		case exportPattern : 			patternExporter.update();			break;
		case exportPatternStems :	 	patternStemsExporter.update();		break;
		case exportRenderSelection : 	renderExporter.update();			break;
		default: break;
	}
}

void mtExporter::updateReceiving()
{
	switch(type)
	{
		case exportSong : 				songExporter.updateReceiving();		break;
		case exportSongStems : 			songStemsExporter.update();	 		break;
		case exportPattern : 			patternExporter.updateReceiving();	break;
		case exportPatternStems :	 	patternStemsExporter.update();		break;
		case exportRenderSelection : 	renderExporter.update();			break;
		default: break;
	}
}
void mtExporter::updateSave()
{
	switch(type)
	{
		case exportSong : 				songExporter.updateSave();			break;
		case exportSongStems : 			songStemsExporter.update();	 		break;
		case exportPattern : 			patternExporter.updateSave();		break;
		case exportPatternStems :	 	patternStemsExporter.update();		break;
		case exportRenderSelection : 	renderExporter.update();			break;
		default: break;
	}
}

void mtExporter::setExportType(uint8_t t)
{
	type = t;
}

void mtExporter::start(char * path, uint8_t t)
{
	setExportType(t);
	switch(t)
	{
		case exportSong : 				songExporter.start(path);				break;
		case exportSongStems : 			songStemsExporter.start(path);	 		break;
		case exportPattern : 			patternExporter.start(path);			break;
		case exportPatternStems : 		patternStemsExporter.start(path);		break;
		case exportRenderSelection :	renderExporter.start(path);				break;
		default: break;
	}
}

uint8_t mtExporter::getProgress()
{
	switch(type)
	{
		case exportSong : 				return songExporter.getProgress();			break;
		case exportSongStems : 			return songStemsExporter.getProgress();		break;
		case exportPattern : 			return patternExporter.getProgress();		break;
		case exportPatternStems : 		return patternStemsExporter.getProgress();	break;
		case exportRenderSelection :	return renderExporter.getProgress();		break;
		default: break;
	}
	return 0;
}

uint8_t mtExporter::getState()
{
	switch(type)
	{
		case exportSong : 				return songExporter.getStatus();			break;
		case exportSongStems : 			return songStemsExporter.getStatus();		break;
		case exportPattern : 			return patternExporter.getStatus();			break;
		case exportPatternStems : 		return patternStemsExporter.getStatus();	break;
		case exportRenderSelection :   return renderExporter.getStatus();			break;
		default: break;
	}
	return 0;
}

uint8_t mtExporter::getStemsTrack()
{
	switch(type)
	{
		case exportSongStems : 		return songStemsExporter.getTrack(); 		break;
		case exportPatternStems : 	return patternStemsExporter.getTrack();		break;
		default: break;
	}
	return 0;
}

void mtExporter::cancel()
{
	switch(type)
	{
		case exportSong : 				songExporter.cancel();				break;
		case exportSongStems : 			songStemsExporter.cancel();			break;
		case exportPattern : 			patternExporter.cancel();			break;
		case exportPatternStems : 		patternStemsExporter.cancel();		break;
		case exportRenderSelection : 	renderExporter.cancel();			break;
		default: break;
	}
}
