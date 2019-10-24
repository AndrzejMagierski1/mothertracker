#ifndef FILEMANAGER_MTWAVETABLELOADER_H_
#define FILEMANAGER_MTWAVETABLELOADER_H_

#include "SD.h"
#include "mtStructs.h"
#include "wavHeaderReader.h"




class WavetableLoader
{
public:
//	void update();
//	uint32_t start(const char *filename, int16_t * buf);
//	void stop();
//	uint8_t getState();
//	uint8_t getProgress();
uint8_t isWavetable(const char *filename);
private:

//	void stop();

//	enum struct loadState
//	{
//		loadEnded,
//		loadInProgress
//	};
//
//	loadState state;
//
	strWavFileHeader sampleHead;
//	uint16_t bufferLength=0;
//
//	float bufFloat[2048];
//	float currentWave[2048];
//	float nextWave[2048];
//
//	int16_t windowDivider=0;
//	uint16_t numberOfWindows=0;
//	uint16_t missingWindows=0;
//	float windowsControl=0;
//	float windowsCounter=0;
//	uint16_t bufferCounter=0;
//	uint8_t middleWindowFlag;
//	uint8_t currentMiddleWindow;
//
//	int16_t currentInt=0;
//	int16_t nextInt=0;
	FsFile wavfile;
//
//	uint32_t sampleCounter;
//	int16_t * destBuf;

};


//class WavetableLoader::WavetableStandardLoader
//{
//public:
//	uint32_t start(const char *filename, int16_t * buf);
//	void update();
//	uint8_t getCurrentState();
//	uint8_t getProgress();
//private:
//
//	void stop();
//
//	enum struct loadState
//	{
//		loadEnded,
//		loadInProgress
//	};
//
//	loadState state;
//
//	strWavFileHeader sampleHead;
//	uint16_t bufferLength=0;
//	int16_t buf16_1024[1024];
//	int16_t windowDivider=0;
//	int16_t currentWave[1024];
//	int16_t nextWave[1024];
//
//	FsFile wavfile;
//
//	uint16_t numberOfWindows=0;
//	uint16_t missingWindows=0;
//	float windowsControl=0;
//	float windowsCounter=0;
//	uint16_t bufferCounter=0;
//	uint8_t middleWindowFlag;
//	uint8_t currentMiddleWindow;
//
//	uint32_t sampleCounter;
//	int16_t * destBuf;
//};
//
//class WavetableLoader::WavetableSerumLoader
//{
//public:
//	uint32_t start(const char *filename, int16_t * buf);
//	void update();
//	uint8_t getCurrentState();
//	uint8_t getProgress();
//private:
//	void stop();
//
//	enum struct loadState
//	{
//		loadEnded,
//		loadInProgress
//	};
//
//	loadState state;
//
//	strWavFileHeader sampleHead;
//	uint16_t bufferLength=0;
//
//	float bufFloat[2048];
//	float currentWave[2048];
//	float nextWave[2048];
//
//	int16_t windowDivider=0;
//	uint16_t numberOfWindows=0;
//	uint16_t missingWindows=0;
//	float windowsControl=0;
//	float windowsCounter=0;
//	uint16_t bufferCounter=0;
//	uint8_t middleWindowFlag;
//	uint8_t currentMiddleWindow;
//
//	int16_t currentInt=0;
//	int16_t nextInt=0;
//	FsFile wavfile;
//
//	uint32_t sampleCounter;
//	int16_t * destBuf;
//};
//
//
//
//int32_t loadWavetableStandard(const char *filename, int16_t * buf);
//int32_t loadWavetableSerum(const char *filename, int16_t * buf);
////int32_t loadFullWavetableSerum(const char *filename, int16_t * buf);
//void loadFullWavetableSerum(const char *baseName);
//uint8_t getMiddleWindow(uint16_t cnt, float windowsCnt, float windowsControl);
//int16_t fmap(float x, float in_min, float in_max, float out_min, float out_max);
//int32_t fmLoadWavetable(const char *filename, int16_t * buf ,uint16_t * windowSize);






#endif /* FILEMANAGER_MTWAVETABLELOADER_H_ */
