#ifndef EXPORTER_MTPATTERNEXPORTER_H_
#define EXPORTER_MTPATTERNEXPORTER_H_




#include "wavHeaderReader.h"
//#include "Audio.h"
#include "SD.h"

#include "Arduino.h"
extern int16_t exportBuffer1[];
extern int16_t exportBuffer2[];


class mtPatternExporter
{

public:
	void start(char * path);
	void finishReceiving();
	void finishSave();
	void updateReceiving();
	void updateSave();
	void cancel();
	uint8_t getStatus();
	void setOnLastStep();
	void ignoreDelay() {	isIgnoredDelay = true;	};
	void clearIgnoreDelay() {	isIgnoredDelay = false;	};

	enum struct exportStatus
	{
		exportFinished,
		exportFinishedReceiving,
		exportDuring
	};

	uint8_t getProgress();

	friend class mtSongExporter;
	friend class mtRenderWAV;
private:
	void switchBuffer();
	void swapBuffer(int16_t ** a, int16_t ** b);
	void refreshReceiving();
	void refreshSave();
	int16_t * recBuf = nullptr;
	int16_t * sendBuf = nullptr;
	int16_t * lastSendBuf = nullptr;
	int16_t * const buf1 = exportBuffer1;
	int16_t * const buf2 = exportBuffer2;
	strWavFileHeader header;
	exportStatus status;
	uint32_t byteRecorded;
	SdFile wavExport;
	uint32_t position;
	volatile bool requiredSave;
	uint8_t lastStep = 0;
	uint32_t microsTime;
	uint32_t localTime;
	uint32_t packageLR;
	bool headerIsNotSaved = false;
	int16_t * const packageL = (int16_t *) (&packageLR);
	int16_t * const packageR = (int16_t *) ((int16_t *)&packageLR + 1);
	bool duringUpdateReceive;
	bool isIgnoredDelay;
	const float sampleRateCorrectionCoefficientFraction = 44117.0f/44100.0f - 1.0f;
	float sampleRateCorrectionSum;
};



#endif /* EXPORTER_MTPATTERNEXPORTER_H_ */
