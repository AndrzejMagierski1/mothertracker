


#include "SD.h"

#include <stdio.h>

char print_string[100];

bool SdFile::open(const char* path, uint8_t oflag)
{
	close();

	file = new FIL;
	FRESULT error = f_open(file, path, oflag);
	if (error)
	{
		sprintf(print_string, "open error: %s", path);
		reportSdError(print_string, error);
		close();
		return false;
	}
	return true;
}
