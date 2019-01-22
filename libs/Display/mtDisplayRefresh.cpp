

#include "mtDisplay.h"


//#############################################################################
//#############################################################################
//#############################################################################
void cMtDisplay::normalModeDisplayRefresh()
{
	if(displayRefreshTable.spectrumPoints)
	{
		displayRefreshTable.spectrumPoints = 0;
		ramg_spectrum_points();
		if(updateStep > 0) return;
	}
	//-------------------------------------------------
	if(displayRefreshTable.potsLabels)
	{
		displayRefreshTable.potsLabels = 0;
		ramg_pots_labels();
		if(updateStep > 0) return;
	}
	//-------------------------------------------------
	if(displayRefreshTable.buttonsLabels)
	{
		displayRefreshTable.buttonsLabels = 0;
		ramg_buttons_labels();
		if(updateStep > 0) return;
	}
	//-------------------------------------------------
	if(displayRefreshTable.spectrumView)
	{
		displayRefreshTable.spectrumView = 0;
		ramg_spectrum_view();
		if(updateStep > 0) return;
	}
	//-------------------------------------------------

	for(uint8_t i = 0; i < MT_DISP_VALUES_MAX ; i++)
	{
		if(displayRefreshTable.values[i])
		{
			displayRefreshTable.values[i] = 0;
			ramg_values(i);
			if(updateStep > 0) return;
		}
	}

	//-------------------------------------------------

	for(uint8_t i = 0; i < MT_DISP_LISTS_MAX ; i++)
	{
		if(displayRefreshTable.lists[i])
		{
			displayRefreshTable.lists[i] = 0;
			ramg_lists(i);
			if(updateStep > 0) return;
		}
	}


	//-------------------------------------------------
	dl_load_normal_main();


}
//-------------------------------------------------

//#############################################################################
//#############################################################################
//#############################################################################
void cMtDisplay::dl_load_normal_main()
{

    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(displayColors.bgColor);
	API_CLEAR(1,1,1);



	// buttons labels
	if(elementsState.buttonsLabels) API_CMD_APPEND(ramAddress.buttonsLabels, ramSize.buttonsLabels);

	// pots labels
	if(elementsState.potsLabels) API_CMD_APPEND(ramAddress.potsLabels, ramSize.potsLabels);

	// spectrum view
	if(elementsState.spectrumView) API_CMD_APPEND(ramAddress.spectrumView, ramSize.spectrumView);

	// spectrum points
	if(elementsState.spectrumPoints) API_CMD_APPEND(ramAddress.spectrumPoints, ramSize.spectrumPoints);

	// values
	for(uint8_t i = 0; i < MT_DISP_VALUES_MAX ; i++)
	{
		if(elementsState.values[i]) API_CMD_APPEND(ramAddress.values[i], ramSize.values[i]);
	}

	// lists
	for(uint8_t i = 0; i < MT_DISP_LISTS_MAX ; i++)
	{
		if(elementsState.lists[i]) API_CMD_APPEND(ramAddress.lists[i], ramSize.lists[i]);
	}




    API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();

}

//#############################################################################
//#############################################################################
//#############################################################################


void cMtDisplay::setButtonsLabels(uint8_t state)
{
	elementsState.buttonsLabels = state;
	screenRefresh = 1;
}

void cMtDisplay::setPotsLabels(uint8_t state)
{
	elementsState.potsLabels = state;
	screenRefresh = 1;
}

void cMtDisplay::setSpectrum(uint8_t state)
{
	elementsState.spectrumView = state;
	screenRefresh = 1;
}

void cMtDisplay::setSpectrumPoints(uint8_t state)
{
	elementsState.spectrumPoints = state;
	screenRefresh = 1;
}

void cMtDisplay::setList(uint8_t index, uint8_t block, uint8_t width, uint16_t start, char ** list, uint16_t count)
{
	if(count == 0)
	{
		elementsState.lists[index] = 0;
		return;
	}

	lists[index].setList(block, width, start, list, count, &displayColors);
	elementsState.lists[index] = 1;

	displayRefreshTable.lists[index] = 1;
	screenRefresh = 1;
}

void cMtDisplay::setValue(uint8_t state)
{

	for(uint8_t i = 0; i < MT_DISP_VALUES_MAX; i++)
	{
		elementsState.values[i] = state;
		if(state == 1) displayRefreshTable.values[i] = 1;
	}

	screenRefresh = 1;
}


void cMtDisplay::changeButtonsLabels(char ** labels)
{
	ptrButtonsLabels = labels;

	displayRefreshTable.buttonsLabels = 1;
	screenRefresh = 1;
}

void cMtDisplay::changePotsLabels(char ** labels)
{
	ptrPotsLabels = labels;

	displayRefreshTable.potsLabels = 1;
	screenRefresh = 1;
}

void cMtDisplay::changeSpectrum(strMtDispSpectrum *spectrum)
{
	ptrSpectrum = spectrum;

	displayRefreshTable.spectrumView = 1;
	screenRefresh = 1;
}

void cMtDisplay::changeSpectrumPoints(strMtDispSpectrum *spectrum)
{
	ptrSpectrum = spectrum;

	displayRefreshTable.spectrumPoints = 1;
	screenRefresh = 1;
}

void cMtDisplay::changeList(uint8_t index, uint16_t position)
{
	lists[index].setListPos(position);

	displayRefreshTable.lists[index] = 1;
	screenRefresh = 1;
}

void cMtDisplay::changeValues(strMtDispValues * values)
{
	ptrValues = values;

	for(uint8_t i = 0; i < MT_DISP_VALUES_MAX; i++)
	{
		if(ptrValues->type[i] > 0)
		{
			//elementsState.values[i] = 1;
			if(lastValues.type[i] != ptrValues->type[i] || lastValues.value[i] != ptrValues->value[i])
			{
				displayRefreshTable.values[i] = 1;
			}
			else displayRefreshTable.values[i] = 0;
		}
		else elementsState.values[i] = 0;

	}

	screenRefresh = 1;
}



//#############################################################################
//#############################################################################
//#############################################################################

void cMtDisplay::ramg_spectrum_points()
{
	API_LIB_BeginCoProList();
	API_CMD_DLSTART();

	// linie start end
	API_COLOR(displayColors.spectrumPoint);
	API_LINE_WIDTH(8);
	API_BEGIN(LINES);
	// start point
	API_VERTEX2II(ptrSpectrum->startPoint, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
	API_VERTEX2II(ptrSpectrum->startPoint, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);

	// end point
	API_VERTEX2II(ptrSpectrum->endPoint, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
	API_VERTEX2II(ptrSpectrum->endPoint, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
	API_END();

	//znaczki
	API_BEGIN(LINE_STRIP);
	API_VERTEX2II(ptrSpectrum->startPoint+1, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
	API_VERTEX2II(ptrSpectrum->startPoint+5, MT_DISP_IEDITOR_SPECTRUM_Y-37,0,0);
	API_VERTEX2II(ptrSpectrum->startPoint+1, MT_DISP_IEDITOR_SPECTRUM_Y-34,0,0);
	API_END();

	API_BEGIN(LINE_STRIP);
	API_VERTEX2II(ptrSpectrum->endPoint-1, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
	API_VERTEX2II(ptrSpectrum->endPoint-5, MT_DISP_IEDITOR_SPECTRUM_Y-37,0,0);
	API_VERTEX2II(ptrSpectrum->endPoint-1, MT_DISP_IEDITOR_SPECTRUM_Y-34,0,0);
	API_END();

	if(ptrSpectrum->pointsType > 0)
	{
		// tlo loop
		API_COLOR(displayColors.spectrumPointsBG);
		API_BLEND_FUNC(SRC_ALPHA, ONE);
		API_LINE_WIDTH(8);
		API_BEGIN(RECTS);
		API_VERTEX2II(ptrSpectrum->loopPoint1, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
		API_VERTEX2II(ptrSpectrum->loopPoint2, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
		API_END();
		API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

		// linie loop
		API_COLOR(displayColors.spectrumPoint);
		API_BEGIN(LINES);
		// loop point 1
		API_VERTEX2II(ptrSpectrum->loopPoint1, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
		API_VERTEX2II(ptrSpectrum->loopPoint1, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
		// loop point 2
		API_VERTEX2II(ptrSpectrum->loopPoint2, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
		API_VERTEX2II(ptrSpectrum->loopPoint2, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
		API_END();

		// znaczki loop
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(ptrSpectrum->loopPoint1+1, MT_DISP_IEDITOR_SPECTRUM_Y+34,0,0);
		API_VERTEX2II(ptrSpectrum->loopPoint1+5, MT_DISP_IEDITOR_SPECTRUM_Y+37,0,0);
		API_VERTEX2II(ptrSpectrum->loopPoint1+1, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
		API_END();

		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(ptrSpectrum->loopPoint2-1, MT_DISP_IEDITOR_SPECTRUM_Y+34,0,0);
		API_VERTEX2II(ptrSpectrum->loopPoint2-5, MT_DISP_IEDITOR_SPECTRUM_Y+37,0,0);
		API_VERTEX2II(ptrSpectrum->loopPoint2-1, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
		API_END();
	}


	API_LIB_EndCoProList();


	updateAdress = ramAddress.spectrumPoints;
	updateSize = &ramSize.spectrumPoints;

	updateStep = 1;
}

//-----------------------------------------------------------------------------------------------------------
void cMtDisplay::ramg_spectrum_view()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_COLOR(displayColors.spectrumView);
	API_LINE_WIDTH(8);

	if(ptrSpectrum->spectrumType == 1)
	{
		//jedna lamana linia
		API_BEGIN(LINE_STRIP);

		for(uint16_t i = 0; i < 480; i++)
		{
			if(ptrSpectrum->upperData[i] > 0) API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y-ptrSpectrum->upperData[i],0,0);
			else if(ptrSpectrum->lowerData[i] < 0) API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y-ptrSpectrum->lowerData[i],0,0);
			else API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y,0,0);
		}

	}
	else
	{
		API_BEGIN(LINES);
		// 480 pionowych lini
		for(uint16_t i = 0; i < 480; i++)
		{
			API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y-ptrSpectrum->upperData[i],0,0);
			API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y-ptrSpectrum->lowerData[i],0,0);
		}
	}

	API_END();

    API_LIB_EndCoProList();


	updateAdress = ramAddress.spectrumView;
	updateSize = &ramSize.spectrumView;

	updateStep = 1;
}

//-----------------------------------------------------------------------------------------------------------
void cMtDisplay::ramg_pots_labels()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

    for(uint8_t i = 0; i < 5; i++)
    {
		//pots
		API_COLOR(displayColors.fontPotLabel);
		API_CMD_TEXT(MT_DISP_BLOCK_W * i + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_TITLE_Y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), *(ptrPotsLabels+i));
    }

    API_LIB_EndCoProList();


	updateAdress = ramAddress.potsLabels;
	updateSize = &ramSize.potsLabels;

	updateStep = 1;
}

//-----------------------------------------------------------------------------------------------------------
void cMtDisplay::ramg_buttons_labels()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

    for(uint8_t i = 0; i < 5; i++)
    {

    	//buttons
		API_COLOR(displayColors.labelButton);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2II((MT_DISP_BLOCK_W * i + MT_DISP_BLOCK_LABEL_OFFSET) , (MT_DISP_H - MT_DISP_BLOCK_LABEL_H) ,0,0);
		API_VERTEX2II((MT_DISP_BLOCK_W * i + (MT_DISP_BLOCK_W - MT_DISP_BLOCK_LABEL_OFFSET)) , MT_DISP_H,0,0);
		API_END();
		API_COLOR(displayColors.fontButtonLabel);
		API_CMD_TEXT(MT_DISP_BLOCK_W * i + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_LABEL_Y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), *(ptrButtonsLabels+i));
    }

    API_LIB_EndCoProList();


	updateAdress = ramAddress.buttonsLabels;
	updateSize = &ramSize.buttonsLabels;

	updateStep = 1;
}

//-----------------------------------------------------------------------------------------------------------
void cMtDisplay::ramg_values(uint8_t index)
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();


	if(ptrValues->type[index] == mtDispValueValueNumberOnly)
	{
		API_COLOR(displayColors.fontValue);
		API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrValues->value[index]);
	}
	else if(ptrValues->type[index] == mtDispValueValue_0_100)
	{
		if(valuesDisplayMode == 0)
		{

			uint16_t x = MT_DISP_BLOCK_W * index + 3 +(((MT_DISP_BLOCK_W-6)*(ptrValues->value[index]))/100);

	    	//bg
			API_COLOR(displayColors.valueBar);
			API_LINE_WIDTH(16);
			API_BEGIN(RECTS);
			API_VERTEX2II((MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_LABEL_OFFSET) , (MT_DISP_BLOCK_VALUE_CENTER_Y+MT_DISP_BLOCK_LABEL_OFFSET) ,0,0);
			API_VERTEX2II(x, (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
			API_END();

			//number
			API_COLOR(displayColors.fontValue);
			API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y-20, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrValues->value[index]);

		}
		else if(valuesDisplayMode == 1)
		{
			uint16_t y = (MT_DISP_BLOCK_VALUE_CENTER_Y + 42) - ((80*(ptrValues->value[index]))/100);
		//	uint8_t y =

	    	//bg
			API_COLOR(displayColors.valueBar);
			API_LINE_WIDTH(16);
			API_BEGIN(RECTS);
			API_VERTEX2II((MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_LABEL_OFFSET) , y ,0,0);
			API_VERTEX2II(MT_DISP_BLOCK_W * (index+1) - 3 , (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
			API_END();

			//number
			API_COLOR(displayColors.fontValue);
			API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrValues->value[index]);

		}



	}
	else if(ptrValues->type[index] == mtDispValueValueLeftRight_0_100)
	{
		API_COLOR(displayColors.fontValue);
		API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrValues->value[index]);

	}


    API_LIB_EndCoProList();

    // zapisuje ostatnio wyswietlone wartosci
    // w tym meijscu zeby miec pewnoesc ze wyswietlil zmiane
	lastValues.type[index] = ptrValues->type[index];
	lastValues.value[index] = ptrValues->value[index];

	updateAdress = ramAddress.values[index];
	updateSize = &ramSize.values[index];

	updateStep = 1;
}

//-----------------------------------------------------------------------------------------------------------
void cMtDisplay::ramg_lists(uint8_t index)
{
	lists[index].update();
	updateAdress = ramAddress.lists[index];
	updateSize = &ramSize.lists[index];
	updateStep = 1;
	return;
}


