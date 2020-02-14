


#include "SD.h"




bool SdFile::open(const char* path, uint8_t oflag)
{
	close();

	file = new FIL;
	FRESULT error = f_open(file, path, oflag);
	if (error)
	{
		if (error == FR_EXIST)
		{
			reportError("open - file exist", error);
		}
		else if (error == FR_TOO_MANY_OPEN_FILES)
		{
			reportError("open error - too many files opened", error);
			close();
			return false;
		}
		else
		{
			reportError("open - failed", error);
			close();
			return false;
		}
	}
	return true;
}
