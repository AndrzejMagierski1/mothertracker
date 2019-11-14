#ifndef EXPORTER_MTSONGEXPORTER_H_
#define EXPORTER_MTSONGEXPORTER_H_




#include "wavHeaderReader.h"
//#include "Audio.h"
#include "SD.h"

#include "Arduino.h"
extern int16_t exportBuffer1[];
extern int16_t exportBuffer2[];



class mtSongExporter
{

public:
	void start(char * path);
	void finish();
	void update();
	uint8_t getStatus();
	void setOnLastStep();

	enum struct exportStatus
	{
		exportFinished,
		exportDuring
	};
private:

	void switchBuffer();
	void refresh();
	int16_t * recBuf = nullptr;
	int16_t * sendBuf = nullptr;
	int16_t * const buf1 = exportBuffer1;
	int16_t * const buf2 = exportBuffer2;
	strWavFileHeader header;
	exportStatus status;
	uint32_t byteRecorded;
	FsFile wavExport;
	uint16_t position;
	uint8_t lastStep = 0;

	uint32_t packageLR;
	int16_t * const packageL = (int16_t *) (&packageLR);
	int16_t * const packageR = (int16_t *) ((int16_t *)&packageLR + 1);
};



#endif /* EXPORTER_MTSONGEXPORTER_H_ */
