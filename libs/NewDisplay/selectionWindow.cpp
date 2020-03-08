
#include "FT812.h"

#include "displayControls.h"
#include "selectionWindowControl.h"



static uint32_t defaultColors[] =
{
	0xFFFFFF, // ramka okna
	0x0a0a0a, // tło okna
	0x000000, // ramka przyciskow
	0x0a0a0a,  // tło przyciskow
	0x000000, // tekst przyciski
	0x333333, // zaznaczenie ramka
	0x0000FF, // tło zaznaczenie
	0x020202, // tekst zaznaczenia
	0x000000  // tekst
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cSelectionWindow::cSelectionWindow(strControlProperties* properties): cDisplayControl(controlIdSelectionWindow)
{
	colorsCount = 9;
	colors = defaultColors;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
		value = 0;
		width = 0;
		height = 0;
		style = 0;
		return;
	}

	posX = properties->x;
	posY = properties->y;

	text = properties->text;
	value = properties->value;

	width = properties->w;
	height = properties->h;

	data = (strSelectWindowData *)properties->data;

	setStyle(properties->style);
}

cSelectionWindow::~cSelectionWindow()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cSelectionWindow::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	fontHight = fonts[textFont].height;
	textFont =  fonts[textFont].handle;
}

void cSelectionWindow::setText(char* text)
{
	this->text = text;
}

void cSelectionWindow::setValue(int value)
{
	this->value = value;
}

void cSelectionWindow::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cSelectionWindow::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cSelectionWindow::setData(void* data)
{
	this->data = (strSelectWindowData *)data;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cSelectionWindow::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

    uint16_t frameWidth = width;
    uint16_t frameX = posX; //(width > 800/8) ? posX+5 : posX + width/2;

    uint16_t frameHeight = height;
    uint16_t frameY = posY;

    uint16_t button1Width = (5*width)/14;
    uint16_t button1X = posX+ (width/14);

    uint16_t button1Height = height/4 ;
    uint16_t button1Y = posY + ((5*height)/8);

    uint16_t button2Width = (5*width)/14;
    uint16_t button2X = posX+ ((4*width)/7);

    uint16_t button2Height = height/4 ;
    uint16_t button2Y = posY + ((5*height)/8);

    uint16_t textX = posX + (width/2);
    uint16_t textY = posY + ((2*height)/7);

    uint16_t textSecondX = posX + (width/2);
    uint16_t textSecondY = posY + ((2*height)/7) + fontHight;

    uint16_t textButton1X = button1X + (button1Width/2);
    uint16_t textButton1Y = button1Y + (button1Height/2);

    uint16_t textButton2X = button2X + (button2Width/2);
    uint16_t textButton2Y = button2Y + (button2Height/2);







	if(value == 0 || value == 1)
	{
//****************************************************** main window
		API_COLOR(colors[0]);

		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(frameX+1, frameY+1);
		API_VERTEX2F(frameX+frameWidth-1, frameY+frameHeight-1);
		API_END();

		API_COLOR(colors[1]);

		API_LINE_WIDTH(16);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(frameX, frameY);
		API_VERTEX2F(frameX+frameWidth, frameY);
		API_VERTEX2F(frameX+frameWidth, frameY+frameHeight);
		API_VERTEX2F(frameX, frameY+frameHeight);
		API_VERTEX2F(frameX, frameY);
		API_END();
//***********************************************************
//****************************************************** button 1
		if(value == 0) API_COLOR(colors[5]);
		else API_COLOR(colors[2]);

		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(button1X+1, button1Y+1);
		API_VERTEX2F(button1X+button1Width-1, button1Y+button1Height-1);
		API_END();


		if(value == 0) API_COLOR(colors[6]);
		else API_COLOR(colors[3]);

		API_LINE_WIDTH(16);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(button1X, button1Y);
		API_VERTEX2F(button1X+button1Width, button1Y);
		API_VERTEX2F(button1X+button1Width, button1Y+button1Height);
		API_VERTEX2F(button1X, button1Y+button1Height);
		API_VERTEX2F(button1X, button1Y);
		API_END();


//***********************************************************
//****************************************************** button 2
		if(value == 0) API_COLOR(colors[2]);
		else API_COLOR(colors[5]);

		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(button2X+1, button2Y+1);
		API_VERTEX2F(button2X+button2Width-1, button2Y+button2Height-1);
		API_END();

		if(value == 0) API_COLOR(colors[3]);
		else API_COLOR(colors[6]);

		API_LINE_WIDTH(16);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(button2X, button2Y);
		API_VERTEX2F(button2X+button2Width, button2Y);
		API_VERTEX2F(button2X+button2Width, button2Y+button2Height);
		API_VERTEX2F(button2X, button2Y+button2Height);
		API_VERTEX2F(button2X, button2Y);
		API_END();
//***********************************************************
//****************************************************** main text
		API_COLOR(colors[8]);

		API_CMD_TEXT(textX, textY, textFont, OPT_CENTER, text);
		API_CMD_TEXT(textSecondX, textSecondY, textFont, OPT_CENTER, data->secondLine);
//***********************************************************
//****************************************************** text button 1

		if(value == 0) API_COLOR(colors[7]);
		else API_COLOR(colors[4]);
		API_CMD_TEXT(textButton1X, textButton1Y, textFont, OPT_CENTER,data->textButton1);
//***********************************************************
//****************************************************** text button 2

		if(value == 0) API_COLOR(colors[4]);
		else API_COLOR(colors[7]);
		API_CMD_TEXT(textButton2X, textButton2Y, textFont, OPT_CENTER, data->textButton2);
	}

	//API_COLOR(colors[0]);

	//if(style & controlStyleShowValue) API_CMD_NUMBER(posX+data->xValue, posY+5+data->yValue, textFont, data->styleValue, value);


    API_LIB_EndCoProList();


	return 0;
}

uint8_t cSelectionWindow::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cSelectionWindow::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
