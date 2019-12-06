#ifndef EXPORTER_MTPATTERNSTEMSEXPORTER_H_
#define EXPORTER_MTPATTERNSTEMSEXPORTER_H_

#include "mtPatternExporter.h"
#include "mtStructs.h"

class mtPatternTrackExporter
{
public:
	void start(char * path, uint8_t track_n);
	void update();
	uint8_t getStatus();
	void clearSoloTrack(uint8_t n);
	friend void setOnLastExportStep();
	mtPatternExporter localPatternExporter;
protected:
	void setSoloTrack(uint8_t n);
	uint8_t currentStatus;
	uint8_t lastStatus;
	uint8_t currentTrack;

};


class mtPatternStemsExporter
{
public:
	void start(char * path);
	void update();
	void cancel();
	uint8_t getStatus();
	uint8_t getProgress();
	uint8_t getTrack();

	friend void setOnLastExportStep();
private:
	mtPatternTrackExporter trackExporter;
	char folderPath[PATCH_SIZE - 15];
	uint8_t currentTrack = 0;
	uint8_t currentTrackState = 0;
	uint8_t lastTrackState = 0;
	uint8_t status = 0;
};




#endif /* EXPORTER_MTPATTERNSTEMSEXPORTER_H_ */
