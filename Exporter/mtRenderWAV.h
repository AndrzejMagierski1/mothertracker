#ifndef EXPORTER_MTRENDERWAV_H_
#define EXPORTER_MTRENDERWAV_H_
#include "mtPatternExporter.h"

class mtRenderWAV : public mtPatternExporter
{
public:
	void start(char * path);
	void cancel();
	uint8_t getProgress();
	uint32_t getLength();
private:
//	int16_t * recBuf = nullptr;
//	int16_t * sendBuf = nullptr;
//	int16_t * const buf1 = exportBuffer1;
//	int16_t * const buf2 = exportBuffer2;
//	uint32_t byteRecorded;
//	SdFile wavExport;
//	uint8_t lastStep = 0;
};



#endif /* EXPORTER_MTRENDERWAV_H_ */
