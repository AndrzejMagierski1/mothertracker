#ifndef SOURCE_MTEXPORTERWAV_H_
#define SOURCE_MTEXPORTERWAV_H_

#include "mtSongExporter.h"
#include "mtSongStemsExporter.h"

static constexpr uint32_t SEND_BUF_SIZE = 20 * 1024;

extern int16_t exportBuffer1[SEND_BUF_SIZE];
extern int16_t exportBuffer2[SEND_BUF_SIZE];


class mtExporter
{

public:
	enum struct exportType
	{
		song,
		songStems,
		pattern,
		patternStems
	};
	void start(char * path, exportType t);
	void update();
	friend void setOnLastExportStep();
private:
	void setExportType(exportType t);
	exportType type;
	mtSongStemsExporter songStemsExporter;
	mtSongExporter songExporter;
};

extern mtExporter exporter;




#endif
