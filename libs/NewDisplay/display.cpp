

#include "FT812.h"

#include "display.h"


#include "core_pins.h"
#include "elapsedMillis.h"


#include "poly_inv_290.h"


#include "poly_logo_inv.h"

#include "poly_logo_inv_128x128_L8.h"

#include "Roboto_Mono_14_L4.h"
#include "Roboto_Mono_20_L4.h"


#include "arrow_d1_50x50_L1.h"
#include "arrow_d2_50x50_L1.h"
#include "arrow_u1_50x50_L1.h"
#include "arrow_u2_50x50_L1.h"

#include "songIcons.h"


// OBRAZY

// CZCIONKI
const strFont fonts[displayFontCount] =
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

const strBitmap bitmaps[displayBitmapsCount] =
{

	{poly_logo_inv_128x128_l8,50000,16384,128,128,L8,128,},

	{ arrow_u2_50x50_L1, 71500, 350, 50, 50, L1, 7 },
	{ arrow_u1_50x50_L1, 71000, 350, 50, 50, L1, 7 },
	{ arrow_d1_50x50_L1, 70000, 350, 50, 50, L1, 7 },
	{ arrow_d2_50x50_L1, 70500, 350, 50, 50, L1, 7 },

	{ playIcon20x20,72000 , 200, 20, 20, L4, 10},
	{ loopIcon20x20,72500 , 200, 20, 20, L4, 10},


};


strGetProps getProps;



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

	API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();

    stopAppend = 0;
    synchQueuePosition = 0;
	refreshQueueTop = 0;
	refreshQueueBott = 0;
    updateStep = 0;
}


//=================================================================================================
//=================================================================================================


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

	if(loadImage) // todo
	{
		if(!API_LIB_IsCoProEmpty()) return;

		if(img.status == 1)
		{
			if(imgFile.available())
			{
				int32_t readBytes = 0;
				uint8_t currentBuffer[imgBufforSize];
				readBytes = imgFile.read(currentBuffer, imgBufforSize);

				if(img.loadProgress == 0)
				{
					API_LIB_BeginCoProListNoCheck();
					API_CMD_LOADIMAGE(img.address, OPT_NODL);
					API_LIB_EndCoProList();
				}

				if(readBytes < imgBufforSize)
				{
					// eof
					API_LIB_WriteDataToCMD(currentBuffer,readBytes);
					//API_LIB_WriteDataRAMG(currentBuffer, readBytes, img.address + img.loadProgress*imgBufforSize);
					imgFile.close();
					img.status = 2;
				}
				else
				{
					API_LIB_WriteDataToCMD(currentBuffer,imgBufforSize);
					//API_LIB_WriteDataRAMG(currentBuffer, imgBufforSize, img.address + img.loadProgress*imgBufforSize);
				}
				//API_LIB_WriteDataToCMD(currentBuffer,imgBufforSize);
				//API_CMD_LOADIMAGE(img.address, OPT_NODL);

				img.loadProgress++;

				return;
			}
			else
			{
				imgFile.close();
				img.status = 0;
				img.loadProgress = 0;
				loadImage = 0;
			}
		}
		else if(img.status == 2)
		{
			API_LIB_BeginCoProList();
			API_CMD_GETPROPS(0, 0, 0); // 3 dummy 32-bit values
			API_LIB_EndCoProList();
			API_LIB_AwaitCoProEmpty();

			uint16_t Reg_Cmd_Write_Offset;
			Reg_Cmd_Write_Offset = EVE_MemRead16(REG_CMD_WRITE);

			uint32_t ParameterAddr = ((Reg_Cmd_Write_Offset - 12) & 4095); // 12 bytes back
			getProps.End_Address = EVE_MemRead32((RAM_CMD+ParameterAddr));

			ParameterAddr = ((Reg_Cmd_Write_Offset - 8) & 4095); // 8 bytes back
			getProps.Width = EVE_MemRead32((RAM_CMD+ParameterAddr));

			ParameterAddr = ((Reg_Cmd_Write_Offset - 4) & 4095); // 4 bytes back
			getProps.Height = EVE_MemRead32((RAM_CMD+ParameterAddr));

			img.status = 3;
			loadImage = 0;
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
	for(uint8_t i = 0; i < handle->colorsCount; i++)
	{
		if(colorsTable+i == nullptr || colorsTable[i] > 0xFFFFFF) return;
	}

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
	//jesli aktualnie odswiezana - zresetuj proces odswiezania

	//przeszukaj kolejke
	uint8_t i = refreshQueueBott;
	//
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
// jpg/png
//=====================================================================================================
uint8_t cDisplay::readImgFromSd(char* path, uint32_t address, int16_t max_w, int16_t max_h)
{
	strcat(path,".jpg");

	if(!SD.exists(path))
	{
		path[strlen(path)-3] = 0;
		strcat(path,"jpeg");

		if(!SD.exists(path))
		{
			return 1;
			//path[strlen(path)-4] = 0;
			//strcat(path,"png");
			//if(!SD.exists(path))
			//{
			//	return;
			//}
		}
	}

	if(img.status == 1 || img.status == 2) return 1;

	imgFile = SD.open(path);
	if(!imgFile) return 1;

	int32_t readBytes = 0;
	uint8_t currentBuffer[5000];
	readBytes = imgFile.read(currentBuffer, 5000);
	//imgFile.close();

	uint16_t width = 0;
	uint16_t height = 0;

	if(get_jpeg_size(currentBuffer, readBytes, &width, &height) == 0)
	{
		imgFile.close();
		return 1;
	}

	if(width == 0 || width > max_w || height == 0 || height > max_h)
	{
		imgFile.close();
		return 1;
	}

	img.size = imgFile.size();
	img.address = address;

	if(img.size  > 1000000-address)
	{
		imgFile.close();
		return 1;
	}

	imgFile.seek(0);

	img.type = 0;
	img.loadProgress = 0;
	img.progressMax = img.size/imgBufforSize;
	img.status = 1;

	loadImage = 1;

	return 0;
}

void cDisplay::readImgFromMemory(uint8_t* data, uint32_t size) //todo
{
	loadImage = 1;

	img.type = 0;
	img.loadProgress = 0;
	img.data = data;
	img.size = size;
	img.progressMax = img.size/imgBufforSize;
	img.status = 1;
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



