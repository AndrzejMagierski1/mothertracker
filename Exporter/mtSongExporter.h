#ifndef EXPORTER_MTSONGEXPORTER_H_
#define EXPORTER_MTSONGEXPORTER_H_

#include "mtPatternExporter.h"

class mtSongExporter: public mtPatternExporter
{
public:
	void start(char * path);
private:
	uint8_t currentPatternInSong;
	uint8_t songLength;
};



#endif /* EXPORTER_MTSONGEXPORTER_H_ */
