
#ifndef LIBS_NEWSD_SD_H_
#define LIBS_NEWSD_SD_H_


#include <stdint.h>
#include <string.h>


#include "ff.h"
#include "wavHeaderReader.h"

#ifdef SDK_ALIGN
#undef SDK_ALIGN
#endif
#define SDK_ALIGN(var, alignbytes) var __attribute__((aligned(alignbytes)))

//#define SD_FILE_WRITE  ( FA_READ | FA_WRITE | FA_CREATE_NEW | FA_OPEN_APPEND )
#define  SD_FILE_WRITE  ( FA_READ | FA_WRITE | FA_CREATE_ALWAYS )
#define  O_RDONLY FA_READ
#define  O_READ FA_READ

#define FILE_WRITE  ( FA_READ | FA_WRITE | FA_CREATE_ALWAYS )


void reportSdError(const char* text, uint16_t value);

class SdFile;



class SdCard
{
public:

	bool init();
	void stop();
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
	bool removeDirWithFiles(const char* path);

	uint32_t clusterCount();
	uint16_t sectorsPerCluster();
	uint32_t freeClusterCount();

private:
	FRESULT delete_node(TCHAR* path, UINT sz_buff, FILINFO* fno);
};




class SdFile
{
public:

	bool open(const char* path, uint8_t oflag = FA_READ);



	int32_t read(void* buf, uint32_t count)
	{
		UINT read = 0;
		FRESULT error = f_read(file,buf,count,&read);
		if (error)
		{
			reportSdError("read - failed", error);
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
			reportSdError("write - failed", error);
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
			reportSdError("seek - failed", error);
			return false;
		}
		return true;
	}

	bool seekCur(int32_t offset)
	{
		FRESULT error = f_lseek(file, f_tell(file) + offset);
		if (error)
		{
			reportSdError("seekCur - failed", error);
			return false;
		}
		return true;
	}

	bool close()
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
		return true;
	}


	void println()
	{
		print("\n");
	}

	void println(const char* text)
	{
		print(text);
		println();
	}

	void print(const char* text)
	{
		f_puts(text, file);
	}

	void printf(const char *format, int value)
	{
		f_printf(file, format, value);
	}

	void printf(const char *format, int value1, int value2)
	{
		f_printf(file, format, value1, value2);
	}

	void printf(const char *format, int value1, int value2, int value3)
	{
		f_printf(file, format, value1, value2, value3);
	}

	void printf(const char *format, char* str, int value)
	{
		f_printf(file, format, str, value);
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

//	bool openNext(SdFile* parent,  uint8_t oflag = FA_READ)
//	{
//
//
//		return true;
//	}
//	bool isDirectory()
//	{
//		return is_directory > 0;
//	}
//
//	bool isDir()
//	{
//		return is_directory > 0;
//	}


	uint64_t size()
	{
		return f_size(file);
	}

/*	uint8_t getName(char* buf, uint8_t max_length)
	{
		FILINFO fno;
		FRESULT error = f_stat("file.txt", &fno);

		//strncpy( );
		return max_length;
	}
*/






private:
//	~SdFile()
//	{
//		close();
//	}

	FIL* file = nullptr;

};



class SdDir
{
public:

	bool open(const char* path, uint8_t oflag = FA_READ)
	{
		close();

		directory = new DIR;
		FRESULT error =  f_opendir(directory, path);
		if (error)
		{
			reportSdError("dir open - failed", error);
			close();
			return false;
		}

		dir_path = new char[strlen(path)+1];
		strcpy(dir_path, path);
		return true;
	}

	bool close()
	{
		FRESULT error = f_closedir(directory);

		if(directory != nullptr)
		{
			delete directory;
			directory = nullptr;
		}

		if(dir_path != nullptr)
		{
			delete dir_path;
			dir_path = nullptr;
		}


		if(error==FR_INVALID_OBJECT)
		{
			return false;
		}
		else if(error)
		{
			reportSdError("dir close - failed", error);
			return false;
		}
		return true;
	}

	operator bool()
	{
		return isOpen();
	}

	bool isOpen()
	{
		return directory != nullptr;
	}

	bool readItem(char* path, uint8_t* isDir, uint32_t* size)
	{
		FILINFO fno;
		FRESULT error = f_readdir(directory, &fno);
		if(error)
		{
			reportSdError("read dir item - failed", error);
			return false;
		}
		else if(!fno.fname[0]) // koniec folderu
		{
			return false;
		}

        if (fno.fattrib & AM_DIR)
        {
        	*isDir = 1;
        }
        else
        {
        	*isDir = 0;
        }

        if(size != nullptr) *size = fno.fsize;

		strcpy(path, fno.fname);
		return true;
	}

/*
	bool exists(const char* filename)
	{
		FRESULT error = f_stat("file.txt", nullptr);
		if(error)
		{
			return false;
		}
		return true;
	}
*/

	uint16_t createFilesList(uint8_t start_line, char** list, uint8_t list_length, uint16_t max_used_memory, uint8_t chooseFilter = 0);
	uint16_t createProjectsList(char** list, uint8_t list_length, uint16_t max_used_memory);


private:
//	~SdDir()
//	{
//		close();
//	}

	DIR* directory = nullptr;
	char* dir_path = nullptr;

};








extern SdCard SD;


#endif /* LIBS_NEWSD_SD_H_ */
