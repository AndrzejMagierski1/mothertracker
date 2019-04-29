

#include "FT812.h"

#include "display.h"




#include "poly_logo_inv.h"


#include "Roboto_Mono_10_L4.h"
#include "Roboto_Mono_14_L4.h"




// OBRAZY
#define MT_GPU_RAM_POLY_LOGO_ADRESS	50000
#define MT_GPU_RAM_POLY_LOGO_HANDLE	12

#define DISP_RGB(red,green,blue) ((((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))



cDisplay display;



void cDisplay::begin()
{
	FT812_Init();

//####################################

	API_LIB_WriteDataRAMG(Roboto_Mono_14_L4, sizeof(Roboto_Mono_14_L4), MT_GPU_RAM_FONT1_ADRESS);
	API_LIB_WriteDataRAMG(Roboto_Mono_10_L4, sizeof(Roboto_Mono_10_L4), MT_GPU_RAM_FONT2_ADRESS);

	API_LIB_WriteDataRAMG(poly_logo_inv_128x128, sizeof(poly_logo_inv_128x128), MT_GPU_RAM_POLY_LOGO_ADRESS);

//####################################

    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(DISP_RGB(0,0,0));
	API_CLEAR(1,1,1);

	API_BITMAP_HANDLE(MT_GPU_RAM_FONT1_HANDLE);
	API_BITMAP_SOURCE(-1732);
	API_BITMAP_LAYOUT(L4,5,18);
	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 10,18);
	API_CMD_SETFONT(MT_GPU_RAM_FONT1_HANDLE, MT_GPU_RAM_FONT1_ADRESS);

	API_BITMAP_HANDLE(MT_GPU_RAM_FONT2_HANDLE);
	API_BITMAP_SOURCE(9484);
	API_BITMAP_LAYOUT(L4,4,13);
	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 7,13);
	API_CMD_SETFONT(MT_GPU_RAM_FONT2_HANDLE, MT_GPU_RAM_FONT2_ADRESS);



	API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();
    API_LIB_AwaitCoProEmpty();

	refreshQueueTop = 0;
	refreshQueueBott = 0;
    updateStep = 0;
}

void cDisplay::update()
{
	switch(updateStep)
	{
		case 0:	// sprawdz czy ktoras z kontrolek jest w kolejce odswiazania, jesli tak odswiez ja
		{
			if(refreshQueueTop ==  refreshQueueBott) return; // nie jest wymagane odswiezenie

			if(!API_LIB_IsCoProEmpty()) return;

			actualUpdating = refreshQueue[refreshQueueBott];
			actualUpdating->update();
			updateStep = 1;
			break;
		}
		case 1: // zapisz DL z porzedniego kroku do ramu ukladu graficznego
		{
			if(!API_LIB_IsCoProEmpty()) return;

			uint32_t ramAddress = controlsRamStartAddress+(actualUpdating->ramMapPosition*10000);
			if(actualUpdating->memCpy(ramAddress))
			{
				updateStep = 0; // jesli obslugiwana kontrolka potrzebuje odswiezenia
				return;			// wiekszej ilosci blokow
			}


		    refreshQueueBott++;
			if(refreshQueueBott >= controlsRefreshQueueSize) refreshQueueBott = 0;
			actualUpdating = nullptr;

			updateStep = 2;
			break;
		}
		case 2: // oswiez cały ekran
		{
			if(!API_LIB_IsCoProEmpty()) return;

			API_LIB_BeginCoProListNoCheck();
		    API_CMD_DLSTART();

			API_CLEAR_COLOR(config.bgColor);
			API_CLEAR(1,1,1);

			// wczytaj elementy w kolejnosci w jakiej byly tworzone
			for(uint8_t  i = 0; i < controlsCount; i++)
			{
				hControl p = controlsTable[i];
				if(p == nullptr) break;
				uint32_t ramAddress = controlsRamStartAddress+(p->ramMapPosition*10000);
				if(p->cState) p->append(ramAddress);
			}

		    API_DISPLAY();
		    API_CMD_SWAP();
		    API_LIB_EndCoProList();
		    API_LIB_AwaitCoProEmpty();
			uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);


			updateStep = 0;
			dlOffset = 0;
			break;
		}
		default: updateStep = 0; break;
	}

}
/*
template <typename controlClass> hControl cDisplay::createControl(char text[], uint8_t state, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	hControl newControlSlot = controlsTable[0];

	uint8_t i = 0;

	while(nullptr != newControlSlot++)
	{
		if(++i >= controlsCount) return nullptr; //zabezpieczenie przed przekroczeniem max ilosci mozliwych kontrolek
	}

	newControlSlot = new controlClass(text, state, x, y, w, h);

	return newControlSlot;
}
*/
void cDisplay::setControlState(hControl handle, uint8_t state)
{
	handle->cState = state;
}

void cDisplay::destroyControl(hControl handle)
{
	if(handle == nullptr)	return;

	// przeszukuj tablice kontrolek
	int8_t found = -1;

	for(uint8_t  i = 0; i < controlsCount; i++)
	{
		if(controlsTable[i] == handle)
		{
			found = i;
			break;
		}
		else if(controlsTable[i] == nullptr) break; // przeszukiwac do pierwszego nullptr?
	}

	delete handle; // sproboj zwolnic pamiec mimo wszystko - wrazie wyjatku nie przepelni pamieci

	if(found < 0) return; // nie znaleziono - wyjdz


	memoryMap[controlsTable[found]->ramMapPosition] = 0;

	// przesun wszystkie wskazniki w tablicy powyzej kasowanego
	uint8_t i = 0;
	for(i = found; i < controlsCount-1; i++)
	{
		controlsTable[i] = controlsTable[i+1];
		//if(controlsTable[i+1] == nullptr) break;
	}
	controlsTable[controlsCount-1] = nullptr; // ostatnia pozycja tablicy moze byc zawsze zerowana
}

void cDisplay::refreshControl(hControl handle)
{
	// dorzuc kontrolke do kolejki fifo odtwarzania
	// jesli juz jest w kolejce to nic nie rob

	//przeszukaj kolejke
	uint8_t i = refreshQueueBott;
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


