

#ifndef LIBS_NEWSD_NEWSD_H_
#define LIBS_NEWSD_NEWSD_H_


#include <stdint.h>

int8_t sdCardInit();


extern "C"
{

void cardDetectPinInterruptHandler(uint8_t state);

}






















#endif /* LIBS_NEWSD_NEWSD_H_ */
