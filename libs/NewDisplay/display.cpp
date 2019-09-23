

#include "FT812.h"

#include "display.h"


#include "core_pins.h"
#include "elapsedMillis.h"


#include "poly_inv_290.h"


#include "poly_logo_inv.h"

#include "poly_logo_inv_128x128_L8.h"

#include "Roboto_Mono_14_L4.h"
#include "Roboto_Mono_20_L4.h"


// OBRAZY

// CZCIONKI
strFont fonts[displayFontCount] =
{
	{
		Roboto_Mono_14_L4,
		sizeof(Roboto_Mono_14_L4),
		13,
		1000,
		-1732,
		10,
		18,
		L4,
		5,
	},
	{
		Roboto_Mono_20_L4,
		sizeof(Roboto_Mono_20_L4),
		14,
		20000,
		14324,
		12,
		26,
		L4,
		7,
	},
};
// handle nie moze byc wikesze niz 14

strBitmap bitmaps[displayBitmapsCount] =
{
/*
	{
		poly_logo_inv_290x290,
		50000,
		42050,
		290,
		290,
		L4,
		145,
	},
*/
/*
	{
		poly_logo_inv_128x128,
		50000,
		8192,
		128,
		128,
		L4,
		64,
	},
*/
	{
		poly_logo_inv_128x128_l8,
		50000,
		16384,
		128,
		128,
		L8,
		128,
	},
};



strTrackerPattern trackerSeqDisplay;

//void display_table();


cDisplay display;



void cDisplay::begin()
{
	FT812_Init();

//####################################

	for(uint8_t i = 0; i < displayFontCount; i++)
	{
		API_LIB_WriteDataRAMG(fonts[i].data, fonts[i].size, fonts[i].address);
	}

	for(uint8_t i = 0; i < displayBitmapsCount; i++)
	{
		API_LIB_WriteDataRAMG(bitmaps[i].data, bitmaps[i].source, bitmaps[i].address);
	}

//	API_LIB_WriteDataRAMG(Roboto_Mono_14_L4, sizeof(Roboto_Mono_14_L4), 1000);
//	API_LIB_WriteDataRAMG(Roboto_Mono_20_L4, sizeof(Roboto_Mono_20_L4), 20000);
//####################################

	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(DISP_RGB(0,0,0));
	API_CLEAR(1,1,1);

	for(uint8_t i = 0; i < displayFontCount; i++)
	{

		API_BITMAP_HANDLE(fonts[i].handle);
		API_BITMAP_SOURCE(fonts[i].source);
		API_BITMAP_LAYOUT(fonts[i].format, fonts[i].linestride,fonts[i].height);
		API_BITMAP_SIZE(NEAREST, BORDER, BORDER, fonts[i].width, fonts[i].height);
		API_CMD_SETFONT(fonts[i].handle, fonts[i].address);
	}





//	API_BITMAP_HANDLE(14);
//	API_BITMAP_SOURCE(14324);
//	API_BITMAP_LAYOUT(L4, 7, 26);
//	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 12, 26);
//	API_CMD_SETFONT(14, 20000);
//
//	API_BITMAP_HANDLE(13);
//	API_BITMAP_SOURCE(-1732);
//	API_BITMAP_LAYOUT(L4, 5, 18);
//	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 10, 18);
//	API_CMD_SETFONT(13, 1000);


//	API_RESTORE_CONTEXT();
//	API_RESTORE_CONTEXT();

	API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();
    //API_LIB_AwaitCoProEmpty();

    stopAppend = 0;
    synchQueuePosition = 0;
	refreshQueueTop = 0;
	refreshQueueBott = 0;
    updateStep = 0;
}

elapsedMicros testTimer;
static elapsedMillis refreshTimer;
elapsedMillis seqTimer;
hControl hTrackControl;
uint8_t created = 0;
int8_t moveStep = 5;
uint16_t refreshF = 20;



void cDisplay::update()
{
	if(!state) return;

	if(updateDisplaySettings)
	{
		updateDisplaySettings = 0;

		if(backlightBrightness != lastBacklightBrightness)
		{
			lastBacklightBrightness = backlightBrightness;
			EVE_MemWrite8(REG_PWM_DUTY, backlightBrightness);
		}
		if(rotateValue !=  lastRotateValue)
		{
			lastRotateValue = rotateValue;
			EVE_MemWrite8(REG_ROTATE, rotateValue);
		}
	}


//=================================================================================================
//=================================================================================================

	switch(updateStep)
	{
		case 0:	// sprawdz czy ktoras z kontrolek jest w kolejce odswiazania, jesli tak odswiez ja
		{
			if(refreshQueueTop ==  refreshQueueBott) return; // nie jest wymagane odswiezenie

			if(!API_LIB_IsCoProEmpty()) return;
//			testTimer = 0;

			actualUpdating = refreshQueue[refreshQueueBott];
			actualUpdating->update();

//			Serial.print("phase 1 ");
//			Serial.println(testTimer);
			updateStep = 1;
			break;
		}
		case 1: // zapisz DL z porzedniego kroku do ramu ukladu graficznego
		{
			if(!API_LIB_IsCoProEmpty()) return;
//			testTimer = 0;

			uint32_t ramAddress = controlsRamStartAddress+(actualUpdating->ramMapPosition*controlsRamAddressStep);

			uint8_t result = actualUpdating->memCpy(ramAddress);

			if(result == 1)
			{
//				Serial.print("phase 2 ");
//				Serial.println(testTimer);
				updateStep = 0; // jesli obslugiwana kontrolka potrzebuje odswiezenia
				return;			// wiekszej ilosci blokow
			}
			else if(result == 3) // high prioryty kontrolki - odswieza tylko ją
			{
				actualUpdating = nullptr;
				updateStep = 2;
				return;
			}

			// przesun kolejke odswieznania
		    refreshQueueBott++;
			if(refreshQueueBott >= controlsRefreshQueueSize) refreshQueueBott = 0;

			// jesli bottom kolejki dogonil zapisana pozycje synch refreschu
			if(stopAppend && synchQueuePosition == refreshQueueBott)
			{
				stopAppend = 0;
				//synchQueuePosition = -1;
			}

			// jesli kontrolka jest animowana i potzebuje sie automatycznie dalej odswiezac
			// dodawana jest automatycznie na koniec kolejki
/*-*/		if(result == 2) refreshControl(actualUpdating);

			actualUpdating = nullptr;

			updateStep = 2;
			break;
		}
		case 2: // oswiez cały ekran
		{
			if(stopAppend)
			{
				updateStep = 0;
				return;
			}

			if(!API_LIB_IsCoProEmpty()) return;


			API_LIB_BeginCoProListNoCheck();
		    API_CMD_DLSTART();


			API_CLEAR_COLOR(config.bgColor);
			API_CLEAR(1,1,1);


			API_VERTEX_FORMAT(0);

			// wczytaj elementy w odwrotnej kolejnosci niz w jakiej byly tworzone
			for(int8_t  i = controlsCount-1; i >= 0; i--)
			{
				hControl p = controlsTable[i];
				if(p == nullptr) continue;
				if(p->style & controlStyleShow)
				{
					uint32_t ramAddress = controlsRamStartAddress+(p->ramMapPosition*controlsRamAddressStep);
					p->append(ramAddress);
				}
			}


		    API_DISPLAY();
		    API_CMD_SWAP();

			API_LIB_EndCoProList();
			//API_LIB_AwaitCoProEmpty();



			updateStep = 0;

			break;
		}
		default: updateStep = 0; break;
	}

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void cDisplay::setControlPosition(hControl handle, int16_t x, int16_t y)
{
	if(handle == nullptr) return;

	if(x >= 0) handle->posX = x;
	if(y >= 0) handle->posY = y;
}

void cDisplay::setControlSize(hControl handle, int16_t w, int16_t h)
{
	if(handle == nullptr) return;

	if(w >= 0) handle->width = w;
	if(h >= 0) handle->height = h;
}

void cDisplay::setControlStyle(hControl handle, uint32_t style)
{
	if(handle == nullptr) return;
	//sprawdzenie porpawnosci czcionki
	if(((style >> 8) & 15) > displayFontCount)
	{
		style &= ~(15 << 8);
	}

	//handle->cStyle = style;
	handle->setStyle(style);
}

void cDisplay::setControlShow(hControl handle)
{
	if(handle == nullptr) return;

	handle->style |= (controlStyleShow);

	//uint32_t temp_style = handle->style;
	//temp_style &= ~(controlStyleShow);
	//handle->setStyle(style);
}


void cDisplay::setControlHide(hControl handle)
{
	if(handle == nullptr) return;

	handle->style &= ~(controlStyleShow);
}

void cDisplay::setAddControlStyle(hControl handle, uint32_t style)
{
	if(handle == nullptr) return;

	uint32_t temp_style = handle->style;
	temp_style |= (style);
	handle->setStyle(temp_style);
}


void cDisplay::setRemoveControlStyle(hControl handle, uint32_t style)
{
	if(handle == nullptr) return;

	uint32_t temp_style = handle->style;
	temp_style &= ~(style);
	handle->setStyle(temp_style);
}

/*
void cDisplay::setControlText(hControl handle,  char const* text)
{
	setControlText(handle, (char*)text);
}
*/
void cDisplay::setControlText(hControl handle, char* text)
{
	if(handle == nullptr) return;
	handle->setText(text);
}

void cDisplay::setControlValue(hControl handle, int value)
{
	if(handle == nullptr) return;
	handle->setValue(value);
}

void cDisplay::setControlColors(hControl handle, uint32_t colorsTable[])
{
	if(handle == nullptr) return;
	for(uint8_t i = 0; i < handle->colorsCount; i++) if(colorsTable+i == nullptr || colorsTable[i] > 0xFFFFFF) return;

	//handle->colors = colorsTable;
	handle->setColors(colorsTable);
}

void cDisplay::setControlDefaultColors(hControl handle, uint32_t colorsTable[])
{
	if(handle == nullptr) return;
	for(uint8_t i = 0; i < handle->colorsCount; i++) if(colorsTable+i == nullptr || colorsTable[i] > 0xFFFFFF) return;

	handle->setDefaultColors(colorsTable);
}

void cDisplay::setControlData(hControl handle, void* data)
{
	if(handle == nullptr) return;
	handle->setData(data);
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cDisplay::destroyControl(hControl handle)
{
	if(handle == nullptr)	return;

	// przeszukuj tablice kontrolek
	int8_t found = -1;

	for(uint8_t i = 0; i < controlsCount; i++)
	{
		if(controlsTable[i] == handle)
		{
			found = i;
			break;
		}
		else if(controlsTable[i] == nullptr)  // przeszukiwac do pierwszego nullptr?
		{
			break;
		}
	}


	if(found < 0) return; // nie znaleziono - wyjdz

	memoryMap[controlsTable[found]->ramMapPosition] = 0;

	delete handle;  // sproboj zwolnic pamiec mimo wszystko - wrazie wyjatku nie przepelni pamieci

	// przesun wszystkie wskazniki w tablicy powyzej kasowanego
	controlsTable[controlsCount-1] = nullptr; // ostatnia pozycja tablicy moze byc zawsze zerowana
	uint8_t i = 0;
	for(i = found; i < controlsCount-1; i++)
	{
		controlsTable[i] = controlsTable[i+1];
		//if(controlsTable[i+1] == nullptr) break;
	}

}

void cDisplay::refreshControl(hControl handle)
{
	if(handle == nullptr) return;

	// dorzuc kontrolke do kolejki fifo odtwarzania
	// jesli juz jest w kolejce to nic nie rob

	//przeszukaj kolejke
	uint8_t i = refreshQueueBott;
	//jesli aktualnie odswiezana - zresetuj proces odswiezania
	if(refreshQueue[i] == handle)
	{
		if(refreshQueueTop != i)
		{
			i++;
			if(i >= controlsRefreshQueueSize) i = 0;
		}
	}
	// przeszukaj reszte kolejki
	while(i != refreshQueueTop) // ryzykowne ale optymalne
	{
		if(handle == refreshQueue[i])
		{
			return; // znaleziono w kolejce - wyjdz
		}
		i++;
		if(i >= controlsRefreshQueueSize) i = 0;
	}

	// dodaj kontorlke do kolejki
	refreshQueue[refreshQueueTop] = handle;
	refreshQueueTop++;
	if(refreshQueueTop >= controlsRefreshQueueSize) refreshQueueTop = 0;

}

void cDisplay::synchronizeRefresh()
{
	if(refreshQueueTop ==  refreshQueueBott)
	{
		stopAppend = 0;
		//synchQueuePosition = -1;
		return;
	}

	synchQueuePosition = refreshQueueTop;
	stopAppend = 1;
}


void cDisplay::resetControlQueue()
{
	stopAppend = 0;
	refreshQueueTop = 0;
	refreshQueueBott = 0;

	updateStep = 0;
	actualUpdating = nullptr;
}


//=====================================================================================================
// grupowe
//=====================================================================================================
void cDisplay::hideAllControls()
{
	for(uint8_t i = 0; i < controlsCount; i++)
	{
		if(controlsTable[i] != nullptr)
		{
			controlsTable[i]->style &= ~(controlStyleShow);
		}

	}
}

void cDisplay::clear()
{
	API_LIB_BeginCoProList();

    API_CMD_DLSTART();

	API_CLEAR_COLOR(config.bgColor);
	API_CLEAR(1,1,1);

    API_DISPLAY();
    API_CMD_SWAP();

	API_LIB_EndCoProList();
}

//=====================================================================================================
// hardware
//=====================================================================================================
void cDisplay::setBacklightBrightness(uint8_t value)
{
	//
	backlightBrightness = value;
	updateDisplaySettings = 1;
}

void cDisplay::setRotate(uint8_t value)
{
	//
	rotateValue = value;
	updateDisplaySettings = 1;

}



