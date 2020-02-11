
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

	uint8_t exists(const char* path)
	{


		return 0;
	}

	uint8_t mkdir(uint8_t hidden, const char *path, bool pFlag = true)
	{

		return 0;
	}

	SdFile open(const char* path, uint8_t oflag = FA_READ);


	bool remove(const char* path)
	{

		return true;
	}

	bool rmdir(const char* path)
	{

		return true;
	}


	uint32_t clusterCount() {return 0;}
	uint8_t sectorsPerCluster() {return 0;}
	int32_t freeClusterCount() {return 0;}


};




class SdFile
{
public:

	bool open(const char* path, uint8_t oflag = FA_READ)
	{

		return true;
	}

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
		return 0;
	}

	uint32_t write(const void* buf, uint32_t count)
	{
		return 0;
	}

	uint32_t write(uint8_t b) {return write(&b, 1);}

	uint8_t seek(uint32_t pos)
	{

		return 0;
	}

	uint8_t seekCur(int64_t offset)
	{
		return 0;
	}

	uint8_t close()
	{

		return 0;
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
		return file_state;
	}

	bool available()
	{
		return file_state > 0;
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
		return file->obj.objsize;
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

	FIL* file;
	uint8_t file_state = 0;
	uint8_t is_directory = 0;
};





typedef SdFile ExFatFile;
typedef SdFile FatFile;



extern SdCard SD;


#endif /* LIBS_NEWSD_SD_H_ */
