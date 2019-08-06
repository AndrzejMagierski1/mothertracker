

#include "FT812.h"

#include "display.h"


#include "core_pins.h"
#include "elapsedMillis.h"

#include "poly_logo_inv.h"


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
	{
		poly_logo_inv_128x128,
		50000,
		128,
		128,
		L4,
		64,
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
		API_LIB_WriteDataRAMG(bitmaps[i].data, sizeof(bitmaps[i].data), bitmaps[i].address);
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


	API_RESTORE_CONTEXT();
	API_RESTORE_CONTEXT();

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
	//display_table();
	//return;
//=================================================================================================
//=================================================================================================

/*
	if(seqTimer < 3000) return;

		seqTimer = 0;

	if(!API_LIB_IsCoProEmpty()) return;
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();



    static int a = 0;


    if(a)
    {
    API_CLEAR_COLOR_RGB(63,63,63);
	API_CLEAR(1,1,1);


	//API_COLOR_A(128);


	API_COLOR(0x000000);
	API_CMD_TEXT(100, 40, 29, 0, "TEST");
    }
    else

    {
        API_CLEAR_COLOR_RGB(0,0,255);
    	API_CLEAR(1,1,1);
    }


	uint8_t blend[6] =
	{
			ONE,
			ZERO,
			SRC_ALPHA,
			DST_ALPHA,
			ONE_MINUS_SRC_ALPHA,
			ONE_MINUS_DST_ALPHA,
	};

//	for(uint8_t i = 0; i<6;i++)
//	{
//		for(uint8_t j = 0; j<6;j++)
//		{
//			API_RESTORE_CONTEXT();
//			API_COLOR(0x000000);
//			API_BLEND_FUNC(blend[i], blend[j]);
//			API_CMD_TEXT(70*i, 20*j, 28, 0, "TEST");
//
//		}
//	}


    a = !a;

	API_DISPLAY();
	API_CMD_SWAP();

	API_LIB_EndCoProList();

return;

*/


/*
if(seqTimer > 125)
{
	seqTimer = 0;

	trackerSeqDisplay.position++;
	if(trackerSeqDisplay.position > 256) trackerSeqDisplay.position = 1;

	uint8_t value = trackerSeqDisplay.position;

	for(uint8_t i = 0; i < 8; i++)
	{

		for(uint8_t j = 0; j < 15; j++)
		{
			value+=j;

			trackerSeqDisplay.track[i].row[j].vol[0] =  48 + (value%100)/10;
			trackerSeqDisplay.track[i].row[j].vol[1] =  48 + (value%100)%10;

		}


		value = trackerSeqDisplay.position;

	}




	if(hTrackControl != nullptr) display.refreshControl(hTrackControl);

}



if(refreshTimer > refreshF)
{
	refreshTimer = 0;
	if(!created)
	{
		strControlProperties prop;
		prop.text = (char*)"Test";
		prop.style = 	(controlStyleShow );//| controlStyleFont2 | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop.x = 0;
		prop.y = 0;
		prop.w = 50;
		prop.h = 25;
		prop.data = &trackerSeqDisplay;
		hTrackControl = display.createControl<cTracker>(&prop);
		//hTrackControl = display.createControl<cLabel>(&prop);
		//display.refreshControl(hTrackControl);

		created = 1;
	}

	trackerSeqDisplay.part += moveStep;

	if(trackerSeqDisplay.part >= 744)
	{
		trackerSeqDisplay.part = 744;

		moveStep = -1;
		refreshF = 5000;
	}
	else if (trackerSeqDisplay.part <= 0)
	{
		trackerSeqDisplay.part = 0;
		moveStep = 10;
		refreshF = 5000;
	}
	else
	{
		refreshF = 20;
	}

	display.refreshControl(hTrackControl);
}
*/


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
/*			else if(result == 2)
			{
			    refreshQueueBott++;
				if(refreshQueueBott >= controlsRefreshQueueSize) refreshQueueBott = 0;
				refreshControl(actualUpdating);
				actualUpdating = nullptr;
				updateStep = 2; // jesli kontrolka jest animowana i potzebuje sie automatycznie dalej odswiezac
				return;			// dodawana jest automatycznie na koniec kolejki
			}
*/
			// przesun kolejke odswieznania
		    refreshQueueBott++;
			if(refreshQueueBott >= controlsRefreshQueueSize) refreshQueueBott = 0;

			// jesli bot kolejki dogonil zapisana pozycje synch refreschu
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

			// wczytaj elementy w kolejnosci w jakiej byly tworzone
			for(uint8_t  i = 0; i < controlsCount; i++)
			{
				hControl p = controlsTable[i];
				if(p == nullptr) break;
				uint32_t ramAddress = controlsRamStartAddress+(p->ramMapPosition*controlsRamAddressStep);
				if(p->style & controlStyleShow) p->append(ramAddress);
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

void cDisplay::setControlPosition(hControl handle, uint16_t x, uint16_t y)
{
	if(handle == nullptr) return;

	handle->posX = x;
	handle->posY = y;
}

void cDisplay::setControlSize(hControl handle, uint16_t w, uint16_t h)
{
	if(handle == nullptr) return;

	handle->width = w;
	handle->height = h;
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
	//przeszukaj kolejke
	//jesli aktualnie odswiezana - zresetuj proces odswiezania
	if(refreshQueue[refreshQueueBott] == handle)
	{
		refreshQueue[refreshQueueTop] = handle;
		refreshQueueTop++;
		if(refreshQueueTop >= controlsRefreshQueueSize) refreshQueueTop = 0;
		return;
	}
	// przeszukaj reszte kolejki
	uint8_t i = refreshQueueBott;
	// przeszukaj reszte kolejki
	while(i != refreshQueueTop) // ryzykowne ale optymalne
	{
		if(handle == refreshQueue[i]) return; // znaleziono w kolejce - wyjdz
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





