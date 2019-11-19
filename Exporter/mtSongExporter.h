#ifndef EXPORTER_MTSONGEXPORTER_H_
#define EXPORTER_MTSONGEXPORTER_H_

#include "mtPatternExporter.h"

class mtSongExporter: public mtPatternExporter
{
public:
	void start(char * path);
	uint8_t getProgress();
private:
	uint16_t songLength;
};



#endif /* EXPORTER_MTSONGEXPORTER_H_ */
