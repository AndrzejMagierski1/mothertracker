#ifndef SOURCE_MTEXPORTERWAV_H_
#define SOURCE_MTEXPORTERWAV_H_

#include "Audio.h"

enum struct exportStatus
{
	exportFinished,
	exportDuring
};
class mtExporterWAV
{
	public:
		void startExport(char * patch);
		void finishExport();
		uint8_t getStatus();
		void  getFinishAction();
	private:
		exportStatus status;
};

extern mtExporterWAV exporterWAV;

#endif
