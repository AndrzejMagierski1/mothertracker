
#include "SD.h"

// FILE_SYSTEM = 1 for FAT16/FAT32
//               2 for exFAT,
//               3 for FAT16/FAT32 and exFAT.
#define FILE_SYSTEM 3

#if FILE_SYSTEM == 1
SdFat sd;
FatFile file;
#elif FILE_SYSTEM == 2
SdExFat sd;
ExFatFile file;
#elif FILE_SYSTEM == 3
SdFs SD;
//FsVolume volume;
//FsFile root;
#endif
