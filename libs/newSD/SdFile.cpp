


#include "SD.h"





bool SdFile::open(const char* path, uint8_t oflag)
{

	file = new FIL;

	FRESULT error = f_open(file, path, oflag);
	if (error)
	{
		if (error == FR_EXIST)
		{
		   // PRINTF("File exists.\r\n");
		}
		else
		{
		   // PRINTF("Open file failed.\r\n");
			return false;
		}
	}
	return true;
}
