
#ifndef LIBS_NEWSD_SD_H_
#define LIBS_NEWSD_SD_H_


#include <stdint.h>



#include "ff.h"

//#define SD_FILE_WRITE  ( FA_READ | FA_WRITE | FA_CREATE_NEW | FA_OPEN_APPEND )
#define SD_FILE_WRITE  ( FA_READ | FA_WRITE | FA_CREATE_ALWAYS )
#define  O_RDONLY FA_READ
#define  O_READ FA_READ

#define FILE_WRITE  ( FA_READ | FA_WRITE | FA_CREATE_ALWAYS )



class SdFile;



class SdCard
{
public:

	bool init();
	void pinsInit();


	bool exists(const char* path)
	{
		if(f_stat (path,nullptr ) == FR_OK) return true;
		return false;
	}
	bool mkdir(uint8_t hidden, const char *path, bool pFlag = true);
	SdFile open(const char* path, uint8_t oflag = FA_READ);
	bool remove(const char* path);
	bool rmdir(const char* path) { return remove(path); }

	uint32_t clusterCount();
	uint16_t sectorsPerCluster();
	uint32_t freeClusterCount();
};




class SdFile
{
public:

	bool open(const char* path, uint8_t oflag = FA_READ);


	uint16_t createFilesList(uint8_t start_line, char list[][40], uint8_t list_length, uint8_t chooseFilter = 0)
	{
		uint16_t count = start_line;

		return (count-start_line);
	}

	uint16_t createFilesListShort(uint8_t start_line, char * list,uint8_t list_length,uint8_t nameLength)
	{
		uint16_t count = start_line;

		return count;
	}

	int read(void* buf, uint32_t count)
	{
		UINT read = 0;
		FRESULT error = f_read(file,buf,count,&read);
		if (error)
		{
			return -1;
		}
		return read;
	}

	int64_t write(const void* buf, uint32_t count)
	{
		UINT written = 0;
		FRESULT error = f_write(file,buf,count,&written);
		if (error)
		{
			return -1;
		}
		return written;
	}

	uint32_t write(uint8_t b) {return write(&b, 1);}

	bool seek(uint32_t pos)
	{
		FRESULT error = f_lseek(file, pos);
		if (error)
		{
			return false;
		}
		return true;
	}

	bool seekCur(int32_t offset)
	{
		FRESULT error = f_lseek(file, f_tell(file) + offset);
		if (error)
		{
			return false;
		}
		return true;
	}

	bool close()
	{
		if (f_close(file))
		{
			delete file;
			file = nullptr;
			return false;
		}

		delete file;
		file = nullptr;
		return true;
	}


	void println()
	{

	}

	void println(const char* text)
	{

	}

	void print(const char* text)
	{

	}

	void printf(const char *format, ...)
	{

	}

	operator bool()
	{
		return isOpen();
	}

	bool isOpen()
	{
		return file != nullptr;
	}

	uint32_t available()
	{
	    return isOpen() ? f_size(file) - f_tell(file) : 0;
	}

	bool openNext(SdFile* parent,  uint8_t oflag = FA_READ)
	{


		return true;
	}

	bool isDirectory()
	{
		return is_directory > 0;
	}

	bool isDir()
	{
		return is_directory > 0;
	}


	uint64_t size()
	{
		return f_size(file);
	}

	uint8_t getName(char* buf, uint8_t max_length)
	{

		return 0;
	}


	bool exists(const char* child)
	{

		return true;
	}



private:

	FIL* file = nullptr;
	DIR* directory = nullptr;

	uint8_t file_state = 0;
	uint8_t is_directory = 0;
};





extern SdCard SD;


#endif /* LIBS_NEWSD_SD_H_ */
