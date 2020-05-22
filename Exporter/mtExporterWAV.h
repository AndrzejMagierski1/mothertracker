#ifndef SOURCE_MTEXPORTERWAV_H_
#define SOURCE_MTEXPORTERWAV_H_

#include <mtPatternExporter.h>
#include <mtPatternStemsExporter.h>
#include "mtSongExporter.h"
#include "mtSongStemsExporter.h"
#include "mtRenderWAV.h"

static constexpr uint32_t SEND_BUF_SIZE = 100 * 1024;



enum enExportType
{
	exportSong = 0,
	exportSongStems,
	exportPattern,
	exportPatternStems,
	exportRenderSelection,


	exportTypesCount
};



class mtExporter
{

public:

	void start(char * path, uint8_t t);
	void begin();
	void updateReceiving();
	void updateSave();

	void cancel();
	uint8_t getProgress();
	uint8_t getState();
	uint8_t getStemsTrack();
	uint32_t getRenderLength();
	friend void setOnLastExportStep();
	friend void setOnLastExportStepInSong();
private:
	void setExportType(uint8_t t);
	uint8_t type;
	mtPatternStemsExporter patternStemsExporter;
	mtPatternExporter patternExporter;
	mtSongExporter	songExporter;
	mtSongStemsExporter songStemsExporter;
	mtRenderWAV renderExporter;
};

extern mtExporter exporter;




#endif
