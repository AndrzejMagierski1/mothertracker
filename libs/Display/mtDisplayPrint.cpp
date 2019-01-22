
#include <stdint.h>

#include "mtDisplay.h"


//---------------------------------------------------------------------------
void cMtDisplay::dl_load_print_main()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(displayColors.bgColor);
	API_CLEAR(1,1,1);

	uint8_t first_line = (totalLinesPrint < MAX_PRINT_LINES ? 0 : (lastPrintLine < MAX_PRINT_LINES-1 ? lastPrintLine+1 : 0));
	uint8_t max_lines = (totalLinesPrint >= MAX_PRINT_LINES ? MAX_PRINT_LINES : totalLinesPrint);

	for(uint8_t i = 0; i < max_lines; i++)
	{
		API_COLOR(displayColors.fontPrint);
		API_CMD_TEXT(3,(i*20)+3, MT_GPU_RAM_FONT1_HANDLE, 0, &text[first_line][0]);

		first_line++;
		if(first_line == MAX_PRINT_LINES) first_line  = 0;
	}

    API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();



}

//---------------------------------------------------------------------------


void cMtDisplay::print(const char * s)
{
#ifdef DEBUG

	uint8_t j = 0;
	for(uint16_t i = 0; i < 256; i++)
	{
		if(text[lastPrintLine][i] == 0 && i < 255 && s[j] != 0)
		{
			text[lastPrintLine][i] = s[j];
			text[lastPrintLine][i+1] = 0;
			j++;
		}
	}

	printShow();

#endif
}

void cMtDisplay::print(int n)
{
#ifdef DEBUG

	uint8_t sign=0;

	if (n < 0)
	{
		sign = '-';
		n = -n;
	}
	return printNumber(n, 10, sign);

#endif
}

void cMtDisplay::print(unsigned int n)
{
#ifdef DEBUG

	return printNumber(n, 10, 0);

#endif
}

void cMtDisplay::printNumber(unsigned long n, uint8_t base, uint8_t sign)
{
#ifdef DEBUG
	char buf[34];
	uint8_t digit, i;

	// TODO: make these checks as inline, since base is
	// almost always a constant.  base = 0 (BYTE) should
	// inline as a call directly to write()
	if (base == 0)
	{
		return print((char)n);
	}
	else if (base == 1)
	{
		base = 10;
	}


	if (n == 0)
	{
		buf[sizeof(buf) - 1] = 0;
		buf[sizeof(buf) - 2] = '0';
		i = sizeof(buf) - 2;
	} else
	{
		buf[sizeof(buf) - 1] = 0;
		i = sizeof(buf) - 2;
		while (1)
		{
			digit = n % base;
			buf[i] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
			n /= base;
			if (n == 0) break;
			i--;
		}
	}
	if (sign) {
		i--;
		buf[i] = '-';
	}
	print(buf+i);
#endif
}


void cMtDisplay::println(const char * s)
{
#ifdef DEBUG

	print(s);
	println();

#endif
}

void cMtDisplay::println(int n)
{
#ifdef DEBUG

	print(n);
	println();

#endif
}

void cMtDisplay::println()
{
#ifdef DEBUG

	if(totalLinesPrint < 10) totalLinesPrint++;
	lastPrintLine++;
	if(lastPrintLine >= MAX_PRINT_LINES) lastPrintLine = 0;
	text[lastPrintLine][0] = 0;

#endif
}

void cMtDisplay::printClear()
{
#ifdef DEBUG

	totalLinesPrint = 0;
	text[0][0] = 0;
	lastPrintLine = 0;

#endif
}

void cMtDisplay::printShow()
{
#ifdef DEBUG

	screenRefresh = 1;
	printTimer = 0;
	if(screenMode != mtDisplayModePrint) modeSave = screenMode;
	screenMode = mtDisplayModePrint;


#endif
}


void mtPrint(const char *s)
{
	mtDisplay.print(s);
}
void mtPrintln(const char * s)
{
	mtDisplay.println(s);
}
void mtPrint(int n)
{
	mtDisplay.print(n);
}
void mtPrint(unsigned int n)
{
	mtDisplay.print(n);
}
void mtPrintln(int n)
{
	mtDisplay.print(n);
	mtDisplay.println();
}
void mtPrintln(unsigned int n)
{
	mtDisplay.print(n);
	mtDisplay.println();
}
void mtPrintln()
{
	mtDisplay.println();
}
void mtPrintShow()
{
	mtDisplay.printShow();
}
void mtPrintClear()
{
	mtDisplay.printClear();
}






