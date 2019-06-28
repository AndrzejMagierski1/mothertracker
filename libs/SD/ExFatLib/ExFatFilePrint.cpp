/**
 * Copyright (c) 20011-2017 Bill Greiman
 * This file is part of the SdFs library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "ExFatFile.h"
#include "upcase.h"
#include "../common/DebugMacros.h"
#include "../common/PrintTemplates.h"
#include "ExFatVolume.h"
//-----------------------------------------------------------------------------
size_t ExFatFile::printFileSize(print_t* pr) {
  uint64_t n = m_validLength;
  char buf[21];
  char *str = &buf[sizeof(buf) - 1];
  char *bgn = str - 12;
  *str = '\0';
  do {
    uint64_t m = n;
    n /= 10;
    *--str = m - 10*n + '0';
  } while (n);
  while (str > bgn) {
    *--str = ' ';
  }
  return pr->write(str);
}
//-----------------------------------------------------------------------------
size_t ExFatFile::printCreateDateTime(print_t* pr) {
  DirFile_t* df = reinterpret_cast<DirFile_t*>
                 (m_vol->dirCache(&m_dirPos, FsCache::CACHE_FOR_READ));
  if (!df) {
    DBG_FAIL_MACRO;
    goto fail;
  }
  return fsPrintDateTime(pr, getLe32(df->createTime));
fail:
  return 0;
}
//-----------------------------------------------------------------------------
size_t ExFatFile::printModifyDateTime(print_t* pr) {
  DirFile_t* df = reinterpret_cast<DirFile_t*>
                 (m_vol->dirCache(&m_dirPos, FsCache::CACHE_FOR_READ));
  if (!df) {
    DBG_FAIL_MACRO;
    goto fail;
  }
  return fsPrintDateTime(pr, getLe32(df->modifyTime));
fail:
  return 0;
}
//------------------------------------------------------------------------------
void ExFatFile::ls(print_t* pr) {
  ExFatFile file;
  rewind();
  while (file.openNext(this, O_READ)) {
    if (!file.isHidden()) {
      file.printName(pr);
      if (file.isDir()) {
        pr->write('/');
      }
      pr->write('\r');
      pr->write('\n');
    }
    file.close();
  }
}
//------------------------------------------------------------------------------
void ExFatFile::ls(print_t* pr, uint8_t flags, uint8_t indent) {
  ExFatFile file;
  rewind();
  while (file.openNext(this, O_READ)) {
    // indent for dir level
    if (!file.isHidden() || (flags & LS_A)) {
      for (uint8_t i = 0; i < indent; i++) {
        pr->write(' ');
      }
      if (flags & LS_DATE) {
        file.printModifyDateTime(pr);
        pr->write(' ');
      }
      if (flags & LS_SIZE) {
        file.printFileSize(pr);
        pr->write(' ');
      }
      file.printName(pr);
      if (file.isDir()) {
        pr->write('/');
      }
      pr->write('\r');
      pr->write('\n');
      if ((flags & LS_R) && file.isDir()) {
        file.ls(pr, flags, indent + 2);
      }
    }
    file.close();
  }
}

//TODO


uint16_t ExFatFile::createFilesList(uint8_t start_line, char list[][20], uint8_t list_length, uint8_t chooseFilter)
{
	uint16_t count = start_line;
	uint8_t n = 0;
	ExFatFile file;
	rewind();
	while (file.openNext(this, O_READ))
	{
		n = 0;

		if (!file.isHidden())
		{
			switch(chooseFilter)
			{
			case 0:
				if (file.isDir())
				{
					list[count][0] = '/';
					n = 1;
				}

				file.getName(&list[count][n], 20-n);

				count++;
				break;
			case 1:
				if (file.isDir())
				{
					list[count][0] = '/';
					n = 1;
					file.getName(&list[count][n], 20-n);
					count++;
				}
				break;
			case 2:
				if (!file.isDir())
				{
					file.getName(&list[count][n], 20-n);
					uint8_t localLength = strlen(&list[count][n]);

					if(((list[count][localLength - 1] == 'V') || (list[count][localLength - 1] == 'v'))  && ((list[count][localLength - 2] == 'A') ||(list[count][localLength - 2] == 'a')) && ((list[count][localLength - 3] == 'W') || (list[count][localLength - 3] == 'w')) && (list[count][localLength - 4] == '.'))
					{
						struct strWavFileHeader
						{
							uint32_t chunkId;			//0
							uint32_t chunkSize;
							uint32_t format;

							uint32_t subchunk1Id;		//12
							uint32_t subchunk1Size;
							uint16_t AudioFormat;		//20
							uint16_t numChannels;		//22
							uint32_t sampleRate;		//24
							uint32_t byteRate;			//28
							uint16_t blockAlign;		//32
							uint16_t bitsPerSample;		//34

							uint32_t subchunk2Id;		//36
							uint32_t subchunk2Size;		//40

						} localHeader;
						file.read(&localHeader,44);


						if( (localHeader.sampleRate != 44100) || ((localHeader.AudioFormat != 1) && (localHeader.AudioFormat != 3) )  || ((localHeader.bitsPerSample != 16) && (localHeader.bitsPerSample != 32) ) ) memset(&list[count][n],0,localLength);
						else count++;
					}


				}
				break;

			}


		}
		file.close();

		//list[count][w] = 0;

		if(count >= list_length) break;
	}

	return count;
}

//------------------------------------------------------------------------------
int ExFatFile::printf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  return vfprintf(this, fmt, ap);
}
//------------------------------------------------------------------------------
int ExFatFile::mprintf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  return vmprintf(this, fmt, ap);
}
#if ENABLE_ARDUINO_FEATURES
//------------------------------------------------------------------------------
int ExFatFile::mprintf(const __FlashStringHelper *ifsh, ...) {
  va_list ap;
  va_start(ap, ifsh);
  return vmprintf(this, ifsh, ap);
}
#endif  // ENABLE_ARDUINO_FEATURES

