#ifndef SOURCE_ARCANOIDROUNDCREATOR_H_
#define SOURCE_ARCANOIDROUNDCREATOR_H_

#include "Arcanoid.h"

void placeBlock(uint8_t blocknum,uint16_t x_axis,uint16_t y_axis,uint16_t blockLength, uint8_t blocklife);
void placeBlockRow(uint8_t startBlock,uint8_t blocksToCreate,uint16_t x_axis,uint16_t y_axis,uint16_t blockLength,uint16_t blockslife);


#endif
