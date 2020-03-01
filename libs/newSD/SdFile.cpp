


#include "SD.h"

#include <stdio.h>

char print_string[100];



bool SdFile::open(const char* path, uint8_t oflag)
{
	close();

	file = new FIL;
	path_for_report = path;
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


int32_t SdFile::read(void* buf, uint32_t count)
{
	UINT read = 0;
	FRESULT error = f_read(file,buf,count,&read);
	if (error)
	{
		sprintf(print_string, "read error: %s", path_for_report);
		reportSdError(print_string, error);
		return -1;
	}
	return read;
}

int32_t SdFile::write(const void* buf, uint32_t count)
{
	UINT written = 0;
	FRESULT error = f_write(file,buf,count,&written);
	if (error)
	{
		sprintf(print_string, "write error: %s", path_for_report);
		reportSdError(print_string, error);
		return -1;
	}
	return written;
}
