#ifndef SOURCE_MTSLICEMANAGER_H_
#define SOURCE_MTSLICEMANAGER_H_
#include "mtStructs.h"

class mtSliceManager
{
public:

void addSlice(strInstrument * instr);
void removeSlice(strInstrument * instr, uint8_t n);
void adjustSlice(strInstrument * instr, uint8_t n, int32_t val);
void autoSlice(strInstrument * instr);

void getAutoSliceProgress();
void getAutoSliceState();
private:

};

extern mtSliceManager sliceManager;


#endif /* SOURCE_MTSLICEMANAGER_H_ */
