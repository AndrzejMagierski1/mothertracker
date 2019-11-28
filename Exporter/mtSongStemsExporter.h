#ifndef EXPORTER_MTSONGSTEMSEXPORTER_H_
#define EXPORTER_MTSONGSTEMSEXPORTER_H_

#include "mtPatternStemsExporter.h"
#include "mtSongExporter.h"

class mtSongTrackExporter: public mtPatternTrackExporter
{
public:
	void start(char * path, uint8_t track_n);
	void update();
	uint8_t getStatus();
	friend void setOnLastExportStepInSong();
	mtSongExporter localSongExporter;
private:

	uint8_t currentStatus;
	uint8_t lastStatus;
	uint8_t currentTrack;

};

class mtSongStemsExporter
{
public:
	void start(char * path);
	void update();
	uint8_t getStatus();
	uint8_t getProgress();
	uint8_t getTrack();
	void cancel();

	friend void setOnLastExportStepInSong();
private:
	uint16_t songLength;
	mtSongTrackExporter trackExporter;
	char folderPath[PATCH_SIZE - 30];
	uint8_t currentTrack = 0;
	uint8_t currentTrackState = 0;
	uint8_t lastTrackState = 0;
	uint8_t status = 0;
};

#endif /* EXPORTER_MTSONGSTEMSEXPORTER_H_ */
