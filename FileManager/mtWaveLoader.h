#ifndef FILEMANAGER_MTWAVELOADER_H_
#define FILEMANAGER_MTWAVELOADER_H_

#include "SD.h"
#include "mtStructs.h"
#include "wavHeaderReader.h"

constexpr uint16_t BUFFER_COUNT = 1;



class WaveLoader
{
public:
	void update();
	uint32_t start(const char *filename, int16_t * buf);
	uint8_t stop();
	uint8_t getState();
	uint8_t getStopStatus();
	uint32_t getInfoAboutWave(const char *filename);
	uint8_t getCurrentWaveProgress();
	uint32_t getCurrentWaveLoadedMemory();
	void setStopStatus(uint8_t s);
private:
	strWavFileHeader sampleHead;
	FsFile wavfile;
	uint8_t state;
	uint32_t accBufferLength;
	int16_t * currentAddress;
	int8_t stopFlag = - 1;

};


extern WaveLoader externalWaveLoader;



#endif /* FILEMANAGER_MTWAVELOADER_H_ */
