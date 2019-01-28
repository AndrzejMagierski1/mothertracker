

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
	if(displayRefreshTable.envelope)
	{
		displayRefreshTable.envelope = 0;
		ramg_envelope();
		if(updateStep > 0) return;
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

	// envelope
	if(elementsState.envelope) API_CMD_APPEND(ramAddress.envelope, ramSize.envelope);





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

void cMtDisplay::setEnvelopes(uint8_t state)
{

	elementsState.envelope = state;
	screenRefresh = 1;
}


//=============================================================



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

void cMtDisplay::changeEnvelopes(strMtDispEnvelope * envelope)
{
	ptrEnvelope = envelope;

	displayRefreshTable.envelope = 1;
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

	if(ptrSpectrum->startPoint >= 0)
	{
		// start point
		API_BEGIN(LINES);
		API_VERTEX2II(ptrSpectrum->startPoint, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
		API_VERTEX2II(ptrSpectrum->startPoint, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
		API_END();
		//znaczek
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(ptrSpectrum->startPoint+1, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
		API_VERTEX2II(ptrSpectrum->startPoint+5, MT_DISP_IEDITOR_SPECTRUM_Y-37,0,0);
		API_VERTEX2II(ptrSpectrum->startPoint+1, MT_DISP_IEDITOR_SPECTRUM_Y-34,0,0);
		API_END();
	}

	if(ptrSpectrum->endPoint >= 0)
	{
		// end point
		API_BEGIN(LINES);
		API_VERTEX2II(ptrSpectrum->endPoint, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
		API_VERTEX2II(ptrSpectrum->endPoint, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
		API_END();
		//znaczek
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(ptrSpectrum->endPoint-1, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
		API_VERTEX2II(ptrSpectrum->endPoint-5, MT_DISP_IEDITOR_SPECTRUM_Y-37,0,0);
		API_VERTEX2II(ptrSpectrum->endPoint-1, MT_DISP_IEDITOR_SPECTRUM_Y-34,0,0);
		API_END();
	}

	if(ptrSpectrum->pointsType > 0 && (ptrSpectrum->loopPoint1 >= 0 || ptrSpectrum->loopPoint2 >= 0))
	{
		// tlo loop
		API_COLOR(displayColors.spectrumPointsBG);
		API_BLEND_FUNC(SRC_ALPHA, ONE);
		API_LINE_WIDTH(8);
		API_BEGIN(RECTS);
		API_VERTEX2II((ptrSpectrum->loopPoint1 >= 0 ? ptrSpectrum->loopPoint1:0), MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
		API_VERTEX2II((ptrSpectrum->loopPoint2 >= 0 ? ptrSpectrum->loopPoint2:479), MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
		API_END();
		API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);


		// linie loop
		API_COLOR(displayColors.spectrumPoint);


		if(ptrSpectrum->loopPoint1 >= 0)
		{
			// loop point 1
			API_BEGIN(LINES);
			API_VERTEX2II(ptrSpectrum->loopPoint1, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
			API_VERTEX2II(ptrSpectrum->loopPoint1, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
			API_END();
			// znaczek loop
			API_BEGIN(LINE_STRIP);
			API_VERTEX2II(ptrSpectrum->loopPoint1+1, MT_DISP_IEDITOR_SPECTRUM_Y+34,0,0);
			API_VERTEX2II(ptrSpectrum->loopPoint1+5, MT_DISP_IEDITOR_SPECTRUM_Y+37,0,0);
			API_VERTEX2II(ptrSpectrum->loopPoint1+1, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
			API_END();
		}
		if(ptrSpectrum->loopPoint2 >= 0)
		{
			// loop point 2
			API_BEGIN(LINES);
			API_VERTEX2II(ptrSpectrum->loopPoint2, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
			API_VERTEX2II(ptrSpectrum->loopPoint2, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
			API_END();
			// znaczek loop
			API_BEGIN(LINE_STRIP);
			API_VERTEX2II(ptrSpectrum->loopPoint2-1, MT_DISP_IEDITOR_SPECTRUM_Y+34,0,0);
			API_VERTEX2II(ptrSpectrum->loopPoint2-5, MT_DISP_IEDITOR_SPECTRUM_Y+37,0,0);
			API_VERTEX2II(ptrSpectrum->loopPoint2-1, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
			API_END();
		}
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
			uint16_t y = (MT_DISP_BLOCK_VALUE_CENTER_Y + 42) - ((80*(ptrValues->value[index]))/100);

	    	//bg
			API_COLOR(displayColors.valueBar);
			API_LINE_WIDTH(8);
			API_BEGIN(RECTS);
			API_VERTEX2II((MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_LABEL_OFFSET) , y ,0,0);
			API_VERTEX2II(MT_DISP_BLOCK_W * (index+1) - 3 , (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
			API_END();

			//number
			API_COLOR(displayColors.fontValue);
			API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrValues->value[index]);


		}
		else if(valuesDisplayMode == 1)
		{


			uint16_t x = MT_DISP_BLOCK_W * index + 3 +(((MT_DISP_BLOCK_W-6)*(ptrValues->value[index]))/100);

	    	//bg
			API_COLOR(displayColors.valueBar);
			API_LINE_WIDTH(8);
			API_BEGIN(RECTS);
			API_VERTEX2II((MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_LABEL_OFFSET) , (MT_DISP_BLOCK_VALUE_CENTER_Y+3) ,0,0);
			API_VERTEX2II(x, (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
			API_END();

			//number
			API_COLOR(displayColors.fontValue);
			API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y-20, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrValues->value[index]);

		}



	}
	else if(ptrValues->type[index] == mtDispValueValueLeftRight_0_100)
	{
		if(valuesDisplayMode == 0)
		{
			uint16_t x1 = (ptrValues->value[index] > 50)
					? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
					: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value[index]-50)*((MT_DISP_BLOCK_W-6)/2) )/50);

			uint16_t x2 = (ptrValues->value[index] < 50)
					? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
					: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value[index]-50)*((MT_DISP_BLOCK_W-6)/2) )/50);

	    	//bg
			API_COLOR(displayColors.valueBar);
			API_LINE_WIDTH(8);
			API_BEGIN(RECTS);
			API_VERTEX2II(x1, (MT_DISP_BLOCK_VALUE_CENTER_Y -40 ) ,0,0);
			API_VERTEX2II(x2, (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
			API_END();

			//number
			API_COLOR(displayColors.fontValue);
			API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY | OPT_SIGNED), ptrValues->value[index]-50);
		}
		else if(valuesDisplayMode == 1)
		{

			uint16_t x1 = (ptrValues->value[index] > 50)
					? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
					: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value[index]-50)*((MT_DISP_BLOCK_W-6)/2) )/50);

			uint16_t x2 = (ptrValues->value[index] < 50)
					? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
					: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value[index]-50)*((MT_DISP_BLOCK_W-6)/2) )/50);

	    	//bg
			API_COLOR(displayColors.valueBar);
			API_LINE_WIDTH(8);
			API_BEGIN(RECTS);
			API_VERTEX2II(x1, (MT_DISP_BLOCK_VALUE_CENTER_Y + 3) ,0,0);
			API_VERTEX2II(x2, (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
			API_END();

			//number
			API_COLOR(displayColors.fontValue);
			API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y-20, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY | OPT_SIGNED), ptrValues->value[index]-50);

		/*
			uint16_t x1 = (ptrValues->value[index] < 50)
					? MT_DISP_BLOCK_W * index + 3
					: MT_DISP_BLOCK_W * index + 3  + (( (ptrValues->value[index]-50)*((MT_DISP_BLOCK_W-6)/2) )/50);

			uint16_t x2 = (ptrValues->value[index] > 50)
					? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W-3
					: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W-3 + (( (ptrValues->value[index]-50)*((MT_DISP_BLOCK_W-6)/2) )/50);

	    	//bg
			API_COLOR(displayColors.valueBar);
			API_LINE_WIDTH(8);
			API_BEGIN(RECTS);
			API_VERTEX2II(x1, (MT_DISP_BLOCK_VALUE_CENTER_Y + 3) ,0,0);
			API_VERTEX2II(x2, (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
			API_END();

			//number
			API_COLOR(displayColors.fontValue);
			API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y-20, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrValues->value[index]-50);

		*/

		}

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

//-----------------------------------------------------------------------------------------------------------
void cMtDisplay::ramg_envelope()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	#define ADSR_START_X MT_DISP_BLOCK_W+(MT_DISP_BLOCK_W/2)
	#define ADSR_START_Y (MT_DISP_BLOCK_VALUE_CENTER_Y + 38)
	#define ADSR_H 70


    uint16_t top_y = ADSR_START_Y - ADSR_H;
    uint16_t bott_y = ADSR_START_Y;
    uint16_t cent_y = top_y + ADSR_H/2;
    uint16_t x_max = ADSR_START_X+70+70+60+70;
    uint16_t step_y = ADSR_H/4;
    uint16_t high_y = ADSR_H;


    uint8_t envAttack = adsr_log[ptrEnvelope->attack];
    uint8_t envDecay = adsr_log[ptrEnvelope->decay];
    uint8_t envSustain = adsr_log[ptrEnvelope->sustain];
    uint8_t envRelease = adsr_log[ptrEnvelope->release];


    uint16_t a_x = ADSR_START_X;
    uint16_t a_y = ADSR_START_Y;
    uint16_t d_x = ADSR_START_X + (envAttack*70)/100;
    uint16_t d_y = ADSR_START_Y - (ADSR_H*ptrEnvelope->amount)/100;
    uint16_t s_x = d_x + (envDecay*70)/100;
    uint16_t s_y = ADSR_START_Y - ((envSustain*70)/100* (ptrEnvelope->amount))/100;
    uint16_t r_x = s_x + 60;
    uint16_t r_y = s_y;
    uint16_t end_x = r_x +	(envRelease*70)/100;
    uint16_t end_y = ADSR_START_Y;


    uint16_t p100_y = top_y;
    int16_t p50_y = cent_y;
    int16_t p25_y = cent_y + step_y;
    int16_t p75_y = top_y + step_y;


/*
	p75_y = bott_y - ((   ptrEnvelope->amount < 75 ? 100 : (100 - ptrEnvelope->amount) + 75  )*high_y)/100;

    if(ptrEnvelope->amount > 75) // 75 - 100
    {
    	// 67 - 50
    	p50_y = bott_y - ((  ((100-ptrEnvelope->amount) *17)/25 + 50   )*high_y)/100;
    	// 33 - 25
    	p25_y = bott_y - ((  ((100-ptrEnvelope->amount)*8)/25 + 25   )*high_y)/100;
    }
    else if(ptrEnvelope->amount > 50) // 50 - 75
    {
    	// 100 - 67
    	p50_y = bott_y - ((  ((75-ptrEnvelope->amount) *33)/25 + 67   )*high_y)/100;
    	// 50 - 33
    	p25_y = bott_y - ((  ((75-ptrEnvelope->amount)*17)/25 + 33   )*high_y)/100;
    }
    else if(ptrEnvelope->amount > 25) // 25 - 50
    {
    	// 100
    	p50_y = bott_y - ((  100   )*high_y)/100;
    	// 100 - 50
    	p25_y = bott_y - ((  ((50-ptrEnvelope->amount)*50)/25 + 50   )*high_y)/100;
    }
    else
    {
    	p50_y = top_y;
    	p25_y = top_y;
    }
*/

	API_SAVE_CONTEXT();


	API_SCISSOR_XY(a_x, top_y-1);
	API_SCISSOR_SIZE(end_x-a_x, bott_y-top_y+1);


	API_CMD_GRADIENT(a_x, top_y, displayColors.envelopeGradTop , a_x, bott_y, displayColors.envelopeGradBott);


	API_COLOR(displayColors.bgColor);

	API_LINE_WIDTH(16);
	API_BEGIN(EDGE_STRIP_A);
	API_VERTEX2II(a_x, a_y, 0,0);
	API_VERTEX2II(d_x, d_y,0,0);
	API_VERTEX2II(s_x, s_y, 0,0);
	API_VERTEX2II(r_x, r_y,0,0);
	API_VERTEX2II(end_x, end_y, 0,0);
	API_END();




	API_COLOR(displayColors.envelopeLines);
	API_LINE_WIDTH(16);
	API_BEGIN(LINE_STRIP);

	API_VERTEX2II(a_x, a_y, 0,0);
	API_VERTEX2II(d_x, d_y,0,0);
	API_VERTEX2II(s_x, s_y, 0,0);
	API_VERTEX2II(r_x, r_y,0,0);
	API_VERTEX2II(end_x, end_y, 0,0);



	API_END();

	API_RESTORE_CONTEXT();

	API_BLEND_FUNC(SRC_ALPHA, ONE);



	//API_BLEND_FUNC(DST_ALPHA, ONE_MINUS_SRC_ALPHA);

	API_COLOR(displayColors.envelopeBGGrid);
	API_LINE_WIDTH(8);
	API_BEGIN(LINES);

	//poziome-----------------------------------------

	API_VERTEX2II(a_x-20, p100_y, 0, 0);
	API_VERTEX2II(x_max, p100_y, 0, 0);

	API_VERTEX2II(a_x-20, p75_y, 0, 0);
	API_VERTEX2II(x_max, p75_y, 0, 0);

	API_VERTEX2II(a_x-20, p50_y, 0, 0);
	API_VERTEX2II(x_max, p50_y, 0, 0);

	API_VERTEX2II(a_x-20, p25_y, 0, 0);
	API_VERTEX2II(x_max, p25_y, 0, 0);

	API_VERTEX2II(a_x-20, bott_y, 0, 0);
	API_VERTEX2II(x_max, bott_y, 0, 0);

	//pionowe----------------------------------------

	API_VERTEX2II(a_x, top_y, 0, 0);
	API_VERTEX2II(a_x, bott_y, 0, 0);

	API_VERTEX2II(d_x, top_y, 0, 0);
	API_VERTEX2II(d_x, bott_y, 0, 0);

	API_VERTEX2II(s_x, top_y, 0, 0);
	API_VERTEX2II(s_x, bott_y, 0, 0);

	API_VERTEX2II(r_x, top_y, 0, 0);
	API_VERTEX2II(r_x, bott_y, 0, 0);

	API_VERTEX2II(end_x, top_y, 0, 0);
	API_VERTEX2II(end_x, bott_y, 0, 0);



	API_END();

	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

	//wartosci----------------------------------------

	API_COLOR(displayColors.fontEnvelope);
	if(envAttack > 0)
	{
		API_CMD_NUMBER((a_x+((d_x-a_x)/2))-5, bott_y-(a_y-d_y)/2, MT_GPU_RAM_FONT1_HANDLE, (OPT_RIGHTX | OPT_CENTERY), ptrEnvelope->attack);
	}


	if(envDecay > 0)
	{
		if(envSustain <= 50)
		{
			API_CMD_NUMBER((s_x+((d_x-s_x)/2))+5, top_y+(s_y-top_y)/2, MT_GPU_RAM_FONT1_HANDLE, (OPT_RIGHTX | OPT_CENTERY), ptrEnvelope->decay);
		}
		else
		{
			if(envDecay < 50)
				API_CMD_NUMBER(d_x+5, s_y+10, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), ptrEnvelope->decay);
			else
				API_CMD_NUMBER((s_x+((d_x-s_x)/2))+5, bott_y-((s_y-a_y)/2)-10, MT_GPU_RAM_FONT1_HANDLE, (OPT_RIGHTX | OPT_CENTERY), ptrEnvelope->decay);
		}
	}


	if(envSustain > 50)
	{
		API_CMD_NUMBER((s_x+((r_x-s_x)/2)), s_y+10, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrEnvelope->sustain);
	}
	else
	{
		API_CMD_NUMBER((s_x+((r_x-s_x)/2)), s_y-10, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrEnvelope->sustain);
	}


	if(envRelease > 0)
	{
		API_CMD_NUMBER((r_x+((end_x-r_x)/2))+5, s_y+(end_y-s_y)/2, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), ptrEnvelope->release);
	}

//	API_CMD_NUMBER(a_x-15, d_y, MT_GPU_RAM_FONT1_HANDLE, (OPT_RIGHTX | OPT_CENTERY), ptrEnvelope->amount);


	API_COLOR(displayColors.envelopeValueBar);
	API_LINE_WIDTH(8);
	API_BEGIN(RECTS);
	API_VERTEX2II((MT_DISP_BLOCK_W * 0 + MT_DISP_BLOCK_LABEL_OFFSET) , d_y ,0,0);
	API_VERTEX2II((MT_DISP_BLOCK_W * 0 + (MT_DISP_BLOCK_W - MT_DISP_BLOCK_LABEL_OFFSET)) , end_y,0,0);
	API_END();

	API_COLOR(displayColors.fontEnvelope);
	API_CMD_NUMBER(MT_DISP_BLOCK_W * 0 + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrEnvelope->amount);


//----------------------------------------------------------------------------------------------------
// lista envelopow
/*
	int16_t x_pos = MT_DISP_BLOCK_W * (0) + ( MT_DISP_BLOCK_MENU_OFFSET);
	int16_t y_pos = (cent_y - (MT_DISP_BLOCK_MENU_Y_SPACE/2));

	API_COLOR(displayColors.envelopelistFrame);
	API_LINE_WIDTH(8);
	API_BEGIN(LINE_STRIP);
	API_VERTEX2II(x_pos, y_pos, 0, 0);
	API_VERTEX2II(x_pos + (MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos, 0, 0);
	API_VERTEX2II(x_pos + (MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
	API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
	API_VERTEX2II(x_pos, y_pos, 0, 0);
	API_END();

	int8_t type = ptrEnvelope->type;

	API_COLOR(displayColors.fontEnvelope);
	API_CMD_TEXT(MT_DISP_BLOCK_W * 0 + (MT_DISP_BLOCK_W/2), cent_y - MT_DISP_BLOCK_MENU_Y_SPACE, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrEnvelope->names[(type-1<0)?INSTRUMEN_ENVELOPES_MAX-1:type-1]);
	API_CMD_TEXT(MT_DISP_BLOCK_W * 0 + (MT_DISP_BLOCK_W/2), cent_y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrEnvelope->names[ptrEnvelope->type]);
	API_CMD_TEXT(MT_DISP_BLOCK_W * 0 + (MT_DISP_BLOCK_W/2), cent_y + MT_DISP_BLOCK_MENU_Y_SPACE, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrEnvelope->names[(type+1>=INSTRUMEN_ENVELOPES_MAX)?0:type+1]);
*/

//----------------------------------------------------------------------------------------------------

	API_LIB_EndCoProList();

	updateAdress = ramAddress.envelope;
	updateSize = &ramSize.envelope;

	updateStep = 1;

}
