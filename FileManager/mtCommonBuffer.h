#ifndef FILEMANAGER_MTCOMMONBUFFER_H_
#define FILEMANAGER_MTCOMMONBUFFER_H_

#include <stdint.h>

constexpr uint32_t COMMON_BUFFER_SIZE = 32768;

uint8_t *getWriteLoadBufferPointer();

#endif /* FILEMANAGER_MTCOMMONBUFFER_H_ */
