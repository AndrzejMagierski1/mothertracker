#ifndef FILEMANAGER_MTWAVETABLELOADER_H_
#define FILEMANAGER_MTWAVETABLELOADER_H_

#include "SD.h"
#include "mtStructs.h"
#include "wavHeaderReader.h"

class WavetableLoader
{
public:
	void update();
	uint8_t start(const char *filename, int16_t * buf);
	uint32_t stop();
	int32_t fmLoadWavetable(const char *filename, int16_t * buf ,uint16_t * windowSize);
private:

};

int32_t loadWavetableStandard(const char *filename, int16_t * buf);
int32_t loadWavetableSerum(const char *filename, int16_t * buf);
//int32_t loadFullWavetableSerum(const char *filename, int16_t * buf);
void loadFullWavetableSerum(const char *baseName);
uint8_t getMiddleWindow(uint16_t cnt, float windowsCnt, float windowsControl);
int16_t fmap(float x, float in_min, float in_max, float out_min, float out_max);
int32_t fmLoadWavetable(const char *filename, int16_t * buf ,uint16_t * windowSize);

#endif /* FILEMANAGER_MTWAVETABLELOADER_H_ */
