#ifndef EXPORTER_MTSONGSTEMSEXPORTER_H_
#define EXPORTER_MTSONGSTEMSEXPORTER_H_

#include "mtSongExporter.h"

class mtSongTrackExporter
{
public:
	void start(char * path, uint8_t track_n);
	void update();
	uint8_t getStatus();
	friend void setOnLastExportStep();
private:

	void setSoloTrack(uint8_t n);
	void clearSoloTrack(uint8_t n);
	uint8_t currentStatus;
	uint8_t lastStatus;
	uint8_t currentTrack;
	mtSongExporter localSongExporter;

};


class mtSongStemsExporter
{
public:
	void start(char * path);
	void update();
	uint8_t getStatus();

	friend void setOnLastExportStep();
private:
	mtSongTrackExporter trackExporter;
	char folderPath[50];
	uint8_t currentTrack = 0;
	uint8_t currentTrackState = 0;
	uint8_t lastTrackState = 0;
	uint8_t status = 0;
};




#endif /* EXPORTER_MTSONGSTEMSEXPORTER_H_ */
