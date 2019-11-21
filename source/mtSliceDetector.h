#ifndef SOURCE_MTSLICEDETECTOR_H_
#define SOURCE_MTSLICEDETECTOR_H_

#include "mtStructs.h"

constexpr uint16_t LOCAL_BUFFER_SIZE = 10240;
constexpr uint16_t INSTANT_BUFFER_SIZE = 1024;
extern uint32_t measureBuffer[INSTANT_BUFFER_SIZE];

class mtSliceDetector
{
public:

void start(int16_t * addr, uint32_t len, int16_t ** sliceTab);
void update();
uint8_t getState();
uint8_t getProgress();
private:

uint32_t calcInstantAverage(uint32_t * buf);
uint32_t calcLocalAverage(uint32_t * buf);
uint32_t calcLocalVariance(uint32_t * buf, uint32_t average);
enum struct detectState
{
	ended,
	inProgress
};

detectState state = detectState::ended;
uint32_t position = 0;																	// pozycja w samplu
uint8_t averagePositionHead = 0;														// pozycja aktualnego bufora chwilowego
int16_t * address = nullptr;															// adres sampla
int16_t ** slice = nullptr;																// adres tablicy sliców, która jest tablicą wskaźników do adresów w samplu
uint32_t length = 0;																	// dlugosc sampla
uint32_t * const measureBuff = measureBuffer;											// stały wskaźnik do bufora w externalRam ( żeby zachowywał się tak samo jak adres tablicy)
uint32_t instantAverageBuffer[LOCAL_BUFFER_SIZE/INSTANT_BUFFER_SIZE];					// tablica średnich (akumulacja srednich buforow chwilowych)
uint32_t localAverage;																	// srednia lokalna
uint32_t localVariance;																	// lokalna wariancja
float sensCoeff;																		// współczynnik czułośc wykrywania
uint8_t currentSlice = 0;

};

extern mtSliceDetector sliceDetector;

#endif /* SOURCE_MTSLICEDETECTOR_H_ */
