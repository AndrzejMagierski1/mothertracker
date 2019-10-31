#include "mtCommonBuffer.h"

extern uint8_t sdram_writeLoadBuffer[32768];


uint8_t *const writeLoadBufferPointer = sdram_writeLoadBuffer;

uint8_t *getWriteLoadBufferPointer()
{
	return writeLoadBufferPointer;
}
