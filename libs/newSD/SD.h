
#ifndef LIBS_NEWSD_SD_H_
#define LIBS_NEWSD_SD_H_


#include <stdint.h>
#include <string.h>


#include "ff.h"
#include "wavHeaderReader.h"


//#define SD_FILE_WRITE  ( FA_READ | FA_WRITE | FA_CREATE_NEW | FA_OPEN_APPEND )
#define SD_FILE_WRITE  ( FA_READ | FA_WRITE | FA_CREATE_ALWAYS )
#define  O_RDONLY FA_READ
#define  O_READ FA_READ

#define FILE_WRITE  ( FA_READ | FA_WRITE | FA_CREATE_ALWAYS )


void reportError(const char* text, uint16_t value);


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

	bool open(const char* path, uint8_t oflag = FA_READ)
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


	int read(void* buf, uint32_t count)
	{
		UINT read = 0;
		FRESULT error = f_read(file,buf,count,&read);
		if (error)
		{
			reportError("read - failed", error);
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
			reportError("write - failed", error);
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
			reportError("seek - failed", error);
			return false;
		}
		return true;
	}

	bool seekCur(int32_t offset)
	{
		FRESULT error = f_lseek(file, f_tell(file) + offset);
		if (error)
		{
			reportError("seekCur - failed", error);
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
			reportError("close - failed", error);
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


	uint8_t file_state = 0;
	uint8_t is_directory = 0;
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
			reportError("dir open - failed", error);
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
			reportError("dir close - failed", error);
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

	bool readItem(char* path, uint8_t* isDir)
	{
		FILINFO fno;
		FRESULT error = f_readdir(directory, &fno);
		if(error)
		{
			reportError("read dir item - failed", error);
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

		strncpy(path, fno.fname, 32);
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

	// filter: 0-all, 1-folders only, 2-supported wav only // always ignore hidden files // max_used_memory must be higher than 255
	uint16_t createFilesList(uint8_t start_line, char** list, uint8_t list_length, uint16_t max_used_memory, uint8_t chooseFilter = 0)
	{
		for(uint8_t i = 0; i<list_length; i++) // wyczyszczenie uzytej wczesniej pamieci
		{
			delete list[i];
		}

		uint8_t n = start_line;

		if(max_used_memory < 255) return n;

		char wav_file[255];
		uint16_t memory_used = 0;
		FILINFO fno;
		FRESULT error;
		SdFile local_file;

		f_readdir(directory, nullptr);

		while (1)
		{
			if (n >= list_length) break;
			if (memory_used > max_used_memory-255) break;

			error = f_readdir(directory, &fno);
			if(error)
			{
				reportError("create list - read dir item - failed", error);
				break;
			}
			else if(!fno.fname[0]) // koniec folderu
			{
				break;
			}

			if (fno.fattrib & AM_HID) // ukryty
			{
				continue;
			}

	        if (fno.fattrib & AM_DIR) // folder
	        {
	        	uint8_t len = strlen(fno.fname)+2;
	        	list[n] = new char[len];

	        	strcpy(list[n], "/");
	        	strcat(list[n], fno.fname);

	        	memory_used += len;
	        }
	        else // plik
	        {
	        	if(chooseFilter == 2) // filtrowanie .wav
	        	{
		        	uint8_t wav_len = strlen(fno.fname);
		        	if(wav_len<5) continue;

					if(((fno.fname[wav_len - 1] != 'V') && (fno.fname[wav_len - 1] != 'v'))
					|| ((fno.fname[wav_len - 2] != 'A') && (fno.fname[wav_len - 2] != 'a'))
					|| ((fno.fname[wav_len - 3] != 'W') && (fno.fname[wav_len - 3] != 'w'))
					||  (fno.fname[wav_len - 4] != '.')) continue;

					if(strlen((dir_path)+wav_len+2) > 255) continue;
		        	strcpy(wav_file, dir_path);
		        	strcat(wav_file, "/");
		        	strcat(wav_file, fno.fname);

	        		if(local_file.open(wav_file))
	        		{
						strWavFileHeader localHeader;
						readHeader(&localHeader, &local_file);
						local_file.close();

						if ((localHeader.sampleRate != 44100)
						|| ((localHeader.AudioFormat != 1) && (localHeader.AudioFormat != 3))
						|| ((localHeader.bitsPerSample != 16) && (localHeader.bitsPerSample != 24) && (localHeader.bitsPerSample != 32)))
							continue;
					}
	        		else continue;
	        	}

	        	uint8_t len = strlen(fno.fname)+1;
	        	list[n] = new char[len];
	        	strcpy(list[n], fno.fname);
	        	memory_used += len;
	        }

	        n++;
		}

		reportError("create list - memory used ", memory_used);

		return n;
	}

	uint16_t createFilesListShort(uint8_t start_line, char * list,uint8_t list_length,uint8_t nameLength)
	{
		uint16_t count = start_line;

		return count;
	}



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
