


#include "SD.h"

#include <stdio.h>

#include "cserial.h"

static char print_string[100];

//size_t println(const char s[]);

bool SdFile::open(const char* path, uint8_t oflag)
{
	close();

	//---------------------------------------------
	int opened_files_count = get_lock_count();
	if(opened_files_count > 1)
	{
		sprintf(print_string, "%d file open: %s", opened_files_count, path);
		cpplog_println(print_string);
		cppserial_println(print_string);
	}
	//---------------------------------------------

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

bool SdFile::close()
{
	FRESULT error = f_close(file);

	if(file != nullptr)
	{
		delete file;
		file = nullptr;
	}

	if(error==FR_INVALID_OBJECT)
	{
		return false;
	}
	else if(error)
	{
		reportSdError("close - failed", error);
		return false;
	}

	//---------------------------------------------
	int opened_files_count = get_lock_count();
	if(opened_files_count > 1)
	{
		sprintf(print_string, "%d file close: %s", opened_files_count, path_for_report);
		cpplog_println(print_string);
		cppserial_println(print_string);
	}
	//---------------------------------------------

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
