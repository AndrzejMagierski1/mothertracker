#ifndef SOURCE_MTEXPORTERWAV_H_
#define SOURCE_MTEXPORTERWAV_H_

#include "Audio.h"
#include "SD.h"

#define SEND_BUF_SIZE 2560

enum struct exportStatus
{
	exportFinished,
	exportDuring
} ;

void startExport(char * patch);
void finishExport();
void updateExport();
uint8_t getExportStatus();


#endif
