
//#include "Arduino.h"

#include "FT812.h"

#include "spectrumControl.h"

void Number2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, int16_t number);
void String2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);

static uint32_t defaultColors[] =

{
	0xFFFFFF, // linie
	0xFFFFFF, // numery wierszy
	0xFFFFFF, // nuta
	0xFFFFFF, // instrument
	0xFFFFFF, // volume
	0xFFFFFF, // effekt
	0x3F0F00, // zaznaczenie
};


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cSpectrum::cSpectrum(strControlProperties* properties)
{
	colorsCount = 7;
	colors = defaultColors;

	refreshStep =  0;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		width = 0;
		height = 0;
		spectrum = nullptr;
		this->style = 0;
		return;
	}

	spectrum = (strTrackerSpectrum*)properties->data;
	posX = properties->x;
	posY = properties->y;
	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}


cSpectrum::~cSpectrum()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cSpectrum::setStyle(uint32_t style)
{
	this->style = style;
}

void cSpectrum::setText(char* text)
{

}

void cSpectrum::setValue(int value)
{

}

void cSpectrum::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cSpectrum::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cSpectrum::setData(void* data)
{
	spectrum = (strTrackerSpectrum*)data;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cSpectrum::update()
{
	API_LIB_BeginCoProListNoCheck();
	API_CMD_DLSTART();

	switch(refreshStep)
	{
	case 0: refresh1(); break;
	case 1: refresh2(); break;
	default: refreshStep = 0; break;
	}

	API_LIB_EndCoProList();

	return 0;
}

uint8_t cSpectrum::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();


	switch(refreshStep)
	{
	case 0: API_CMD_MEMCPY(address, 		 		RAM_DL, dlOffset); break;
	case 1: API_CMD_MEMCPY(address+ramPartSize[0], 	RAM_DL, dlOffset); break;
	default: break;
	}

	ramPartSize[refreshStep] = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

    refreshStep++;
    if(refreshStep > 1)
    {
    	refreshStep = 0;
    	return 0;
    }

	return 1;
}

uint8_t cSpectrum::append(uint32_t address)
{

	API_CMD_APPEND(address, ramPartSize[0]);
	API_CMD_APPEND(address+ ramPartSize[0], ramPartSize[1]);


	return 0;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cSpectrum::refresh1()
{
	API_COLOR(colors[0]);
	API_LINE_WIDTH(8);


	//uint16_t half = height/2;
	uint16_t center = posY+height/2;

	if(spectrum == nullptr)
	{
		API_BEGIN(LINES);
		API_VERTEX2F(posX, center);
		API_VERTEX2F(posX+width-1, center);
	}
	else if(spectrum->spectrumType == 1)
	{
		//jedna lamana linia
		API_BEGIN(LINE_STRIP);

		for(uint16_t i = 0; i < width-1; i++)
		{
			if(spectrum->upperData[i] > 0) API_VERTEX2F( posX+i, center-spectrum->upperData[i]);
			else if(spectrum->lowerData[i] < 0) API_VERTEX2F( posX+i, center-spectrum->lowerData[i]);
			else API_VERTEX2F( posX+i, center);
		}
	}
	else if(spectrum->spectrumType == 0)
	{
		API_BEGIN(LINE_STRIP);

		uint16_t length = (width>300) ? 300 : width;

		uint8_t odd = 0;

		for(uint16_t i = 0; i < length; i++)
		{
			if(odd) API_VERTEX2F( posX+i, center-spectrum->upperData[i]);
			else API_VERTEX2F( posX+i, center-spectrum->lowerData[i]);
			odd = !odd;

		//	Serial.println(half-spectrum->upperData[i]);
		}
	}
	else if(spectrum->spectrumType == 3)
	{
		//elementsState.waitSpinner = 1;
	}
	API_END();
}

//------------------------------------------------------------------------------------------
void cSpectrum::refresh2()
{
	API_COLOR(colors[0]);
	API_LINE_WIDTH(8);


	if(spectrum == nullptr)
	{

	}
	else if(spectrum->spectrumType == 1)
	{

	}
	else if(spectrum->spectrumType == 0 && width > 400)
	{
		API_BEGIN(LINES);

		uint16_t center = posY+height/2;

		uint8_t odd = 0;

		uint16_t width_left = width-300;

		if(width_left > 0)
		{
			for(uint16_t i = 0; i < width_left; i++)
			{
				if(odd) API_VERTEX2F( posX+i+299, center-spectrum->upperData[i+299]);
				else API_VERTEX2F( posX+i+299, center-spectrum->lowerData[i+299]);
				odd = !odd;
			}
		}
	}
	API_END();
}