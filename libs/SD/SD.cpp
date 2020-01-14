
#include "SD.h"


#if FILE_SYSTEM == 1
SdFat SD;
FatFile file;
#elif FILE_SYSTEM == 2
SdExFat sd;
ExFatFile file;
#elif FILE_SYSTEM == 3
SdFs SD;
//FsVolume volume;
//FsFile root;
#endif
