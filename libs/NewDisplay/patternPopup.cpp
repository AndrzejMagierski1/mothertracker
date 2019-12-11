
#include "FT812.h"

#include "displayControls.h"
#include "patternPopupControl.h"

#include "list.h"
#include "barControl.h"
#include "commonControls.h"


static uint32_t defaultColors[] =
{
	0xFFFFFF, // tekst
	0x0a0a0a, // tło
	one_true_red, // ramka
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cPatternPopup::cPatternPopup(strControlProperties* properties)
{
	colorsCount = 3;
	colors = defaultColors;

	refreshStep =  0;

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

	setStyle(properties->style);
	setData(properties->data);




}

cPatternPopup::~cPatternPopup()
{
	for(uint8_t i = 0; i<popup->columnsCount ; i++)
	{
		if(column[i] != nullptr) delete column[i];
		column[i] = nullptr;
	}
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cPatternPopup::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cPatternPopup::setText(char* text)
{
	this->text = text;
}

void cPatternPopup::setValue(int value)
{
	this->value = value;
}

void cPatternPopup::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cPatternPopup::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cPatternPopup::setData(void* data)
{
	popup = (strPatternPopup*)data;

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cPatternPopup::update()
{





	switch(refreshStep)
	{
	case 0: refresh1(); break;
	case 1: refresh2(); break;
	case 2: refresh3(); break;
	case 3: refresh4(); break;
	case 4: refresh5(); break;
	case 5: refresh6(); break;

	default: refreshStep = 0; break;
	}

	return 0;
}

uint8_t cPatternPopup::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();


	switch(refreshStep)
	{
	case 0: API_CMD_MEMCPY(address, 		 		RAM_DL, dlOffset); break;
	case 1: API_CMD_MEMCPY(address+ramPartSize[0], 	RAM_DL, dlOffset); break;
	case 2: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1], 	RAM_DL, dlOffset); break;
	case 3: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1]+ramPartSize[2], 	RAM_DL, dlOffset); break;
	case 4: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1]+ramPartSize[2]+ramPartSize[3], 	RAM_DL, dlOffset); break;
	case 5: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1]+ramPartSize[2]+ramPartSize[3]+ramPartSize[4], 	RAM_DL, dlOffset); break;
	default: break;
	}

	ramPartSize[refreshStep] = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

    refreshStep++;
    if(refreshStep > 5)
    {
    	refreshStep = 0;
        return selfRefresh;
    	//return 0;
    }

	return 1;
}


uint8_t cPatternPopup::append(uint32_t address)
{
	if(ramPartSize[0] > 0) API_CMD_APPEND(address, ramPartSize[0]);
	if(ramPartSize[1] > 0) API_CMD_APPEND(address+ ramPartSize[0],  ramPartSize[1]);
	if(ramPartSize[2] > 0) API_CMD_APPEND(address+ ramPartSize[0] + ramPartSize[1],  ramPartSize[2]);
	if(ramPartSize[3] > 0) API_CMD_APPEND(address+ ramPartSize[0] + ramPartSize[1] + ramPartSize[2],  ramPartSize[3]);
	if(ramPartSize[4] > 0) API_CMD_APPEND(address+ ramPartSize[0] + ramPartSize[1] + ramPartSize[2] + ramPartSize[3],  ramPartSize[4]);
	if(ramPartSize[5] > 0) API_CMD_APPEND(address+ ramPartSize[0] + ramPartSize[1] + ramPartSize[2] + ramPartSize[3] + ramPartSize[4], ramPartSize[5]);

	return 0;
}

void cPatternPopup::refresh1()
{

    uint16_t bg_width = 0;
    // wyliczanie szerokosci tla
    for(uint8_t i = 0; i<popup->columnsCount; i++)
    {
    	if(popup->column[i].options & columnOptionDoubleWide) bg_width += width*2;
    	else bg_width += width;
    }


    // wyliczanie wspolrzednych
	if(style & controlStyleCenterX)
	{
		x = posX - (bg_width)/2;
	}
	else x = posX;

	if(style & controlStyleCenterY)
	{
		y = posY - (height)/2;
	}
	else y = posY;


	if(style & controlStyleCreateSubcontrols)
	{
		style &= ~(controlStyleCreateSubcontrols);


		for(uint8_t i = 0; i<popup->columnsCount ; i++)
		{
			if(column[i] != nullptr) delete column[i];
			column[i] = nullptr;
		}


		if(popup != nullptr)
		{
			strControlProperties prop;

			for(uint8_t i = 0; i<popup->columnsCount ; i++)
			{
				if(popup->column[i].type == 1)
				{
					prop.x = x + width*(i) + 2;
					prop.y = (y + height/2);
					if(popup->column[i].options & columnOptionDoubleWide) prop.w = width*2 - 4;
					else prop.w = width-4;
					prop.h = 25;
					prop.data = popup->column[i].data;
					prop.value = popup->column[i].value;
					//prop.text = popup->column[i].text;
					prop.style = controlStyleShow | controlStyleCenterY;

					column[i] = new cList(&prop);
				}
				else if (popup->column[i].type == 2)
				{

					prop.colors = popupColumnLabelColors;
					prop.x = x + width*(i) + width /2;
					prop.y = (y + height/2);
					if(popup->column[i].options & columnOptionDoubleWide) prop.w = width*2 - 4;
					else prop.w = width-4;
					prop.h = 25;
					//prop.data = popup->column[i].data;
					//prop.value = popup->column[i].value;
					prop.text = popup->column[i].text;
					prop.style = controlStyleShow | controlStyleCenterY | controlStyleCenterX | controlStyleFont2;

					column[i] = new cLabel(&prop);

				}
				else if (popup->column[i].type == 3)
				{

				}

			}
		}


	}





	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();


	//tlo pop
	API_COLOR(colors[1]);

	//API_COLOR_A(128);
	API_BLEND_FUNC(SRC_ALPHA, ZERO);
	//API_BLEND_FUNC(DST_ALPHA , ZERO);
	API_LINE_WIDTH(16);
	API_BEGIN(RECTS);
	API_VERTEX2F(x, y);
	API_VERTEX2F(x+bg_width, y+height);
	API_END();
	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);


	// naglowek
	API_COLOR(colors[0]);
	API_CMD_TEXT(posX, y+15, textFont, textStyle, text);


	// labele
	for(uint8_t i = 0; i<popup->columnsCount ; i++)
	{
		int16_t border_x = (x + width*i) + 3;
		int16_t border_y = y+height-30 + 1;
		int16_t border_w = width - 6;
		if(popup->column[i].options & columnOptionDoubleWide) border_w = width*2 - 6;

		int16_t border_h = 30 - 3;


		API_COLOR(colors[0]);

		if(style & controlStyleRoundedBorder) API_LINE_WIDTH(32);
		else API_LINE_WIDTH(16);

		API_BEGIN(RECTS);
		API_VERTEX2F(border_x , border_y);
		API_VERTEX2F(border_x+border_w , border_y+border_h);
		API_END();


		//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

		//API_RESTORE_CONTEXT();
		//API_BLEND_FUNC(SRC_ALPHA, ZERO);
		//API_COLOR_A(128);

		//API_LINE_WIDTH(16);
		API_COLOR(colors[1]);
		if(popup->column[i].title != nullptr) API_CMD_TEXT(border_x+border_w/2, border_y+border_h/2, textFont, OPT_CENTER, popup->column[i].title);
	}

	//ramka
	if(value >= 0 && value < popup->columnsCount)
	{

		int16_t border_x = (x + width*value) + 1;
		int16_t border_y = y+30 + 1;
		int16_t border_w = width - 2;
		if(popup->column[value].options & columnOptionDoubleWide) border_w = width*2 - 2;

		int16_t border_h = height-68 - 3;

		if(style & controlStyleRoundedBorder) API_LINE_WIDTH(32);
		else API_LINE_WIDTH(8);

		API_COLOR(colors[2]);

		API_LINE_WIDTH(16);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(border_x, border_y);
		API_VERTEX2F(border_x+border_w, border_y);
		API_VERTEX2F(border_x+border_w, border_y+border_h);
		API_VERTEX2F(border_x, border_y+border_h);
		API_VERTEX2F(border_x, border_y);
		API_END();
	}



    API_LIB_EndCoProList();
}

void cPatternPopup::refresh2()
{

	if(column[0] != nullptr && popup->column[0].type > 0)
	{
		column[0]->setValue(popup->column[0].value);

		 selfRefresh =  column[0]->update();
	}

}

void cPatternPopup::refresh3()
{
	if(column[1] != nullptr && popup->column[1].type > 0)
	{
		column[1]->setValue(popup->column[1].value);
		column[1]->setText(popup->column[1].text);

		if(selfRefresh == 2)
		{
			column[1]->update();
		}
		else
		{
			 selfRefresh = column[1]->update();
		}
	}
}

void cPatternPopup::refresh4()
{
	if(column[2] != nullptr && popup->column[2].type > 0)
	{
		column[2]->setValue(popup->column[2].value);
		column[2]->setText(popup->column[2].text);

		if(selfRefresh == 2)
		{
			column[2]->update();
		}
		else
		{
			 selfRefresh = column[2]->update();
		}
	}
}

void cPatternPopup::refresh5()
{
	if(column[3] != nullptr && popup->column[3].type > 0)
	{
		column[3]->setValue(popup->column[3].value);

		if(selfRefresh == 2)
		{
			column[3]->update();
		}
		else
		{
			 selfRefresh = column[3]->update();
		}
	}
}

void cPatternPopup::refresh6()
{

}

