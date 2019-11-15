#ifndef SOURCE_MTEXPORTERWAV_H_
#define SOURCE_MTEXPORTERWAV_H_

#include <mtPatternExporter.h>
#include <mtPatternStemsExporter.h>

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
	void begin();
	void update();
	uint8_t getProgress();
	uint8_t getState();
	uint8_t getStemsTrack();
	friend void setOnLastExportStep();
private:
	void setExportType(exportType t);
	exportType type;
	mtPatternStemsExporter patternStemsExporter;
	mtPatternExporter patternExporter;
};

extern mtExporter exporter;




#endif
