#ifndef SOURCE_ARCANOIDLEVELS_H_
#define SOURCE_ARCANOIDLEVELS_H_

#include "Arcanoid.h"
#include "Arduino.h"


typedef uint8_t (*level_pntrs[])(void);



uint8_t load_level(uint8_t levelnum);






#endif
