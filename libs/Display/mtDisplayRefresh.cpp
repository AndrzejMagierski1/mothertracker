

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
	for(uint8_t i = 0; i < MT_DISP_VALUES_MAX ; i++)
	{
		if(displayRefreshTable.multiRowValues[i])
		{
			displayRefreshTable.multiRowValues[i] = 0;
			ramg_multi_row_values(i);
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
	if(displayRefreshTable.trackTable)
	{
		displayRefreshTable.trackTable = 0;
		ramg_track_table();
		if(updateStep > 0) return;
	}
	//-------------------------------------------------
	if(displayRefreshTable.textEdit)
	{
		displayRefreshTable.textEdit = 0;
		ramg_text_edit();
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

	// multi row values
	for(uint8_t i = 0; i < MT_DISP_VALUES_MAX ; i++)
	{
		if(elementsState.multiRowValues[i]) API_CMD_APPEND(ramAddress.multiRowValues[i], ramSize.multiRowValues[i]);
	}

	// lists
	for(uint8_t i = 0; i < MT_DISP_LISTS_MAX ; i++)
	{
		if(elementsState.lists[i]) API_CMD_APPEND(ramAddress.lists[i], ramSize.lists[i]);
	}

	// envelope
	if(elementsState.envelope) API_CMD_APPEND(ramAddress.envelope, ramSize.envelope);

	// tracktable
	if(elementsState.trackTable) API_CMD_APPEND(ramAddress.trackTable, ramSize.trackTable);

	// edit text
	if(elementsState.textEdit) API_CMD_APPEND(ramAddress.textEdit, ramSize.textEdit);








	if(elementsState.waitSpinner) API_CMD_SPINNER(240, 64, 3, 0);


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
	elementsState.waitSpinner = 0;
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

void cMtDisplay::setMultiRowValue(uint8_t state)
{
	for(uint8_t i = 0; i < MT_DISP_VALUES_MAX; i++)
	{
		elementsState.multiRowValues[i] = state;
		if(state == 1) displayRefreshTable.multiRowValues[i] = 1;
	}

	screenRefresh = 1;
}

void cMtDisplay::setEnvelopes(uint8_t state)
{
	elementsState.envelope = state;
	screenRefresh = 1;
}

void cMtDisplay::setTrackTable(uint8_t state)
{
	elementsState.trackTable = state;
	screenRefresh = 1;
}


void cMtDisplay::setTextEdit(uint8_t x, uint8_t y, uint8_t length, char* text, char* label)
{
	if(length == 0)
	{
		elementsState.textEdit = 0;
		return;
	}

	textEditData.editPos = 0;
	textEditData.label = label;
	textEditData.text = text;
	textEditData.x = x;
	textEditData.y = y;
	textEditData.length = length;

	elementsState.textEdit = 1;
	displayRefreshTable.textEdit = 1;
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
			if(lastValues.type[i] != ptrValues->type[i] || lastValues.value1[i] != ptrValues->value1[i])
			{
				displayRefreshTable.values[i] = 1;
			}
			else displayRefreshTable.values[i] = 0;
		}
		else elementsState.values[i] = 0;
	}
	screenRefresh = 1;
}

void cMtDisplay::changeMultiRowValues(strMtDispMultiRowValues * values)
{
	ptrMultiRowValues = values;

	for(uint8_t i = 0; i < MT_DISP_VALUES_MAX; i++)
	{
		if(ptrMultiRowValues->type[i] > 0)
		{
			if(lastValues.type[i] != ptrMultiRowValues->type[i]
			|| lastMultiRowValues.values[i][0] != ptrMultiRowValues->values[i][0]
			|| lastMultiRowValues.values[i][1] != ptrMultiRowValues->values[i][1]
			|| lastMultiRowValues.values[i][2] != ptrMultiRowValues->values[i][2]
			|| lastMultiRowValues.values[i][3] != ptrMultiRowValues->values[i][3]
			|| lastMultiRowValues.values[i][4] != ptrMultiRowValues->values[i][4])
			{
				displayRefreshTable.multiRowValues[i] = 1;
			}
			else displayRefreshTable.multiRowValues[i] = 0;
		}
		else elementsState.multiRowValues[i] = 0;
	}
	screenRefresh = 1;
}

void cMtDisplay::changeEnvelopes(strMtDispEnvelope * envelope)
{
	ptrEnvelope = envelope;

	displayRefreshTable.envelope = 1;
	screenRefresh = 1;
}


void cMtDisplay::changeTrackTable(strMtDispTrackTable * trackTable)
{
	ptrTrackTable = trackTable;

	displayRefreshTable.trackTable = 1;
	screenRefresh = 1;
}

void cMtDisplay::changeTextEdit(char* text, uint8_t pos)
{
	textEditData.editPos = pos;
	textEditData.text = text;

	displayRefreshTable.textEdit = 1;
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

	elementsState.waitSpinner = 0;
	elementsState.spectrumView = 1;

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
	else if(ptrSpectrum->spectrumType == 0)
	{
		API_BEGIN(LINES);
		// 480 pionowych lini
		for(uint16_t i = 0; i < 480; i++)
		{
			API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y-ptrSpectrum->upperData[i],0,0);
			API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y-ptrSpectrum->lowerData[i],0,0);
		}
	}
	else if(ptrSpectrum->spectrumType == 3)
	{
		elementsState.waitSpinner = 1;
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
		API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrValues->value1[index]);
	}
	else if(ptrValues->type[index] == mtDispValueValue_0_100)
	{
		if(valuesDisplayMode == 0)
		{
			uint16_t y = (MT_DISP_BLOCK_VALUE_CENTER_Y + 42) - ((80*(ptrValues->value1[index]))/100);

	    	//bg
			API_COLOR(displayColors.valueBar);
			API_LINE_WIDTH(8);
			API_BEGIN(RECTS);
			API_VERTEX2II((MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_LABEL_OFFSET) , y ,0,0);
			API_VERTEX2II(MT_DISP_BLOCK_W * (index+1) - 3 , (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
			API_END();

			//number
			API_COLOR(displayColors.fontValue);
			API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrValues->value1[index]);


		}
		else if(valuesDisplayMode == 1)
		{


			uint16_t x = MT_DISP_BLOCK_W * index + 3 +(((MT_DISP_BLOCK_W-6)*(ptrValues->value1[index]))/100);

	    	//bg
			API_COLOR(displayColors.valueBar);
			API_LINE_WIDTH(8);
			API_BEGIN(RECTS);
			API_VERTEX2II((MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_LABEL_OFFSET) , (MT_DISP_BLOCK_VALUE_CENTER_Y+3) ,0,0);
			API_VERTEX2II(x, (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
			API_END();

			//number
			API_COLOR(displayColors.fontValue);
			API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y-20, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrValues->value1[index]);

		}
	}
	else if(ptrValues->type[index] == mtDispValueValueLeftRight_0_100)
	{
		if(valuesDisplayMode == 0)
		{
			uint16_t x1 = (ptrValues->value1[index] > 50)
					? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
					: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value1[index]-50)*((MT_DISP_BLOCK_W-6)/2) )/50);

			uint16_t x2 = (ptrValues->value1[index] < 50)
					? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
					: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value1[index]-50)*((MT_DISP_BLOCK_W-6)/2) )/50);

	    	//bg
			API_COLOR(displayColors.valueBar);
			API_LINE_WIDTH(8);
			API_BEGIN(RECTS);
			API_VERTEX2II(x1, (MT_DISP_BLOCK_VALUE_CENTER_Y -40 ) ,0,0);
			API_VERTEX2II(x2, (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
			API_END();

			//number
			API_COLOR(displayColors.fontValue);
			API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY | OPT_SIGNED), ptrValues->value1[index]-50);
		}
		else if(valuesDisplayMode == 1)
		{

			uint16_t x1 = (ptrValues->value1[index] > 50)
					? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
					: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value1[index]-50)*((MT_DISP_BLOCK_W-6)/2) )/50);

			uint16_t x2 = (ptrValues->value1[index] < 50)
					? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
					: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value1[index]-50)*((MT_DISP_BLOCK_W-6)/2) )/50);

	    	//bg
			API_COLOR(displayColors.valueBar);
			API_LINE_WIDTH(8);
			API_BEGIN(RECTS);
			API_VERTEX2II(x1, (MT_DISP_BLOCK_VALUE_CENTER_Y + 3) ,0,0);
			API_VERTEX2II(x2, (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
			API_END();

			//number
			API_COLOR(displayColors.fontValue);
			API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y-20, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY | OPT_SIGNED), ptrValues->value1[index]-50);

		}

	}
	else if(ptrValues->type[index] == mtDispValueValueLeftRight_24_24)
	{
		uint16_t x1 = (ptrValues->value1[index] > 0)
				? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
				: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value1[index])*((MT_DISP_BLOCK_W-6)/2) )/24);

		uint16_t x2 = (ptrValues->value1[index] < 0)
				? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
				: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value1[index])*((MT_DISP_BLOCK_W-6)/2) )/24);

    	//bg
		API_COLOR(displayColors.valueBar);
		API_LINE_WIDTH(8);
		API_BEGIN(RECTS);
		API_VERTEX2II(x1, (MT_DISP_BLOCK_VALUE_CENTER_Y -40 ) ,0,0);
		API_VERTEX2II(x2, (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
		API_END();

		//number
		API_COLOR(displayColors.fontValue);
		API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY | OPT_SIGNED), ptrValues->value1[index]);

	}
	else if(ptrValues->type[index] == mtDispValueValueLeftRight_100_100)
	{
		uint16_t x1 = (ptrValues->value1[index] > 0)
				? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
				: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value1[index])*((MT_DISP_BLOCK_W-6)/2) )/100);

		uint16_t x2 = (ptrValues->value1[index] < 0)
				? MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2
				: MT_DISP_BLOCK_W * index + MT_DISP_BLOCK_W/2 + (( (ptrValues->value1[index])*((MT_DISP_BLOCK_W-6)/2) )/100);

    	//bg
		API_COLOR(displayColors.valueBar);
		API_LINE_WIDTH(8);
		API_BEGIN(RECTS);
		API_VERTEX2II(x1, (MT_DISP_BLOCK_VALUE_CENTER_Y -40 ) ,0,0);
		API_VERTEX2II(x2, (MT_DISP_BLOCK_VALUE_CENTER_Y + 42),0,0);
		API_END();

		//number
		API_COLOR(displayColors.fontValue);
		API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY | OPT_SIGNED), ptrValues->value1[index]);

	}


    API_LIB_EndCoProList();

    // zapisuje ostatnio wyswietlone wartosci
    // w tym meijscu zeby miec pewnoesc ze wyswietlilo zmiane
	lastValues.type[index] = ptrValues->type[index];
	lastValues.value1[index] = ptrValues->value1[index];
//	lastValues.value2[index] = ptrValues->value2[index];
//	lastValues.value3[index] = ptrValues->value3[index];
//	lastValues.value4[index] = ptrValues->value4[index];
//	lastValues.value5[index] = ptrValues->value5[index];

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
   // uint16_t high_y = ADSR_H;


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




void cMtDisplay::ramg_multi_row_values(uint8_t index)
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();


    if(ptrMultiRowValues->type[index] == mtDispValueMultiValue4Row)
	{
    	int16_t x_pos = MT_DISP_BLOCK_W * (index) + ( MT_DISP_BLOCK_MENU_OFFSET);
    	int16_t y_pos = (ptrMultiRowValues->values[index][0]*MT_DISP_BLOCK_MENU_Y_SPACE + 30);


    	API_COLOR(displayColors.multiRowValueFrame);
    	API_LINE_WIDTH(8);
    	API_BEGIN(LINE_STRIP);
    	API_VERTEX2II(x_pos, y_pos, 0, 0);
    	API_VERTEX2II(x_pos + (MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos, 0, 0);
    	API_VERTEX2II(x_pos + (MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
    	API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
    	API_VERTEX2II(x_pos, y_pos, 0, 0);
    	API_END();

    	for(uint8_t i = 0; i < 4; i++)
    	{

    		y_pos = (MT_DISP_BLOCK_MENU_Y_SPACE/2) + 30 + i*MT_DISP_BLOCK_MENU_Y_SPACE;

    		// param name
    		API_COLOR(displayColors.fontMultiRowValue);
    		API_CMD_TEXT(x_pos+4 , y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), *(ptrMultiRowValues->labels[index]+i));

    		// param value
    		API_COLOR(displayColors.fontMultiRowValue);
    		API_CMD_NUMBER(MT_DISP_BLOCK_W * (index+1) - 7, y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_RIGHTX | OPT_CENTERY), ptrMultiRowValues->values[index][i+1]);


    	}



	}
	else if(ptrMultiRowValues->type[index] == mtDispValueMultiValue3Row)
	{

		//number
		API_COLOR(displayColors.fontValue);
		API_CMD_NUMBER(MT_DISP_BLOCK_W * index + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y-20, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrMultiRowValues->values[index][0]);

	}


    API_LIB_EndCoProList();

    // zapisuje ostatnio wyswietlone wartosci
    // w tym meijscu zeby miec pewnoesc ze wyswietlilo zmiane
    lastMultiRowValues.type[index] = ptrMultiRowValues->type[index];
    lastMultiRowValues.values[index][0] = ptrMultiRowValues->values[index][0];
	lastMultiRowValues.values[index][1] = ptrMultiRowValues->values[index][1];
	lastMultiRowValues.values[index][2] = ptrMultiRowValues->values[index][2];
	lastMultiRowValues.values[index][3] = ptrMultiRowValues->values[index][3];
	lastMultiRowValues.values[index][4] = ptrMultiRowValues->values[index][4];

	updateAdress = ramAddress.multiRowValues[index];
	updateSize = &ramSize.multiRowValues[index];

	updateStep = 1;
}


void cMtDisplay::ramg_track_table()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	int16_t x_pos = 0;
	int16_t y_pos = 20 + (2) * MT_DISP_BLOCK_MENU_Y_SPACE;
	uint8_t x_length = 0;
	// linie


	API_COLOR(displayColors.trackTableFrame);
	API_LINE_WIDTH(8);
	API_BEGIN(LINES);


	//API_VERTEX2II(20, 0, 0, 0);
	//API_VERTEX2II(20, 479, 0, 0);


	for(uint16_t i = 20; i < 800; i+=95)
	{
		API_VERTEX2F(i*16, 0*16);
		API_VERTEX2F(i*16, 479*16);





	}

	API_VERTEX2F(0*16, 230*16);
	API_VERTEX2F(799*16, 230*16);

	API_VERTEX2F(0*16, 250*16);
	API_VERTEX2F(799*16, 250*16);

	API_END();




	uint8_t row = 201;

	for(uint16_t i = 20; i < 480; i+=20)
	{
			API_COLOR(DISP_RGB(255,255,255));
			API_CMD_NUMBER((799-21) , i, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERY), row);

			API_COLOR(DISP_RGB(255,255,255));
			API_CMD_NUMBER(0 , i, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERY), row);

			row++;
	}



	for(uint16_t i = 22; i < 93*8; i+=95)
	{
			API_COLOR(DISP_RGB(255,0,0));
			API_CMD_TEXT  ((i) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "C#1");
			API_CMD_TEXT  ((i) , 240, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "D-2");
			//API_CMD_TEXT  ((i) , 260, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "F#3");

			API_COLOR(DISP_RGB(0,255,0));
			API_CMD_TEXT  ((i+28) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "01");
			//API_CMD_TEXT  ((i+27) , 240, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "03");
			//API_CMD_TEXT  ((i+28) , 260, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "18");

			API_COLOR(DISP_RGB(128,0,128));
			//API_CMD_TEXT  ((i+45) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "00");
			API_CMD_TEXT  ((i+45) , 240, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "60");
			//API_CMD_TEXT  ((i+45) , 260, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "99");

			API_COLOR(DISP_RGB(128,128,20));
			//API_CMD_TEXT  ((i+64) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "H43");
			API_CMD_TEXT  ((i+64) , 240, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "A30");
			//API_CMD_TEXT  ((i+64) , 260, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "B12");
	}




/*
	API_VERTEX2II(MT_DISP_BLOCK_W * (1), 20, 0, 0);
	API_VERTEX2II(MT_DISP_BLOCK_W * (1), 20+5*MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);

	API_VERTEX2II(MT_DISP_BLOCK_W * (2), 20, 0, 0);
	API_VERTEX2II(MT_DISP_BLOCK_W * (2), 20+5*MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);

	API_VERTEX2II(MT_DISP_BLOCK_W * (3), 20, 0, 0);
	API_VERTEX2II(MT_DISP_BLOCK_W * (3), 20+5*MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);

	API_VERTEX2II(MT_DISP_BLOCK_W * (4), 20, 0, 0);
	API_VERTEX2II(MT_DISP_BLOCK_W * (4), 20+5*MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
*/
	API_END();


/*
	// ramka aktulanie edytowanego parametru tylko dla pierszych 4 kolumn
	if(ptrTrackTable->active[0] < 4)
	{
		x_pos = MT_DISP_BLOCK_W * ((ptrTrackTable->params[2].mode > 0) ? ptrTrackTable->active[0] : 0);

		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(x_pos, y_pos+1, 0, 0);
		API_VERTEX2II(x_pos + (MT_DISP_BLOCK_W) -1, y_pos+1, 0, 0);
		API_VERTEX2II(x_pos + (MT_DISP_BLOCK_W) -1, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE -1, 0, 0);
		API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE-1, 0, 0);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_END();
	}
	else
	{
		x_pos = MT_DISP_BLOCK_W * (4) + ((ptrTrackTable->fx1[2].mode > 2) ? MT_DISP_BLOCK_W : (MT_DISP_BLOCK_W/2));
		x_length = (ptrTrackTable->fx1[2].mode > 2) ? (MT_DISP_BLOCK_W/2) : MT_DISP_BLOCK_W;
		// ramka wartosci efektu
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_VERTEX2II(x_pos + x_length -1, y_pos, 0, 0);
		API_VERTEX2II(x_pos + x_length -1, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_END();
	}

	for(uint8_t i = 0; i < 5; i++)
	{
		if(ptrTrackTable->state[i]) // czy step lezy w sekwencji
		{
			y_pos = (MT_DISP_BLOCK_MENU_Y_SPACE/2) + 20 + i*MT_DISP_BLOCK_MENU_Y_SPACE;
			x_pos = 0;

			// param value
			API_COLOR(displayColors.fontTrackTable);
			if(ptrTrackTable->params[i].mode) // czy step aktywny ? isOn == 1
			{
				API_CMD_TEXT  ((0) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/2 , y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), &mtDispNotes[ptrTrackTable->params[i].iVal1][0]);
				API_CMD_NUMBER((1) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/2 , y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrTrackTable->params[i].iVal2);
				//API_CMD_NUMBER((2) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/2 , y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrTrackTable->params[i].iVal3);
				if(ptrTrackTable->params[i].iVal4 >= 0) // velocity
				{
					API_CMD_NUMBER((2) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/2 , y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrTrackTable->params[i].iVal4);
				}
				else
				{
					API_CMD_TEXT((2) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/2 , y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), "---");
				}

			}
			else
			{
				API_CMD_TEXT((0) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/2, y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), "---");
				API_CMD_TEXT((1) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/2, y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), "---");
				API_CMD_TEXT((2) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/2, y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), "---");

			}

			// fxs
			API_CMD_TEXT((3) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/2 , y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrTrackTable->fx1[i].name);

			if(ptrTrackTable->fx1[i].mode > 2)
			{
				API_CMD_NUMBER((4) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/4 , y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrTrackTable->fx1[i].iVal1);
				API_CMD_NUMBER((4) * MT_DISP_BLOCK_W + (MT_DISP_BLOCK_W/4)*3 , y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrTrackTable->fx1[i].iVal2);

			}
			else if(ptrTrackTable->fx1[i].mode > 0)
			{
				API_CMD_NUMBER((4) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/2 , y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), ptrTrackTable->fx1[i].iVal1);
			}
			else
			{
				API_CMD_TEXT((4) * MT_DISP_BLOCK_W + MT_DISP_BLOCK_W/2, y_pos, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), "---");

			}

		}

	}
*/


    API_LIB_EndCoProList();

	updateAdress = ramAddress.trackTable;
	updateSize = &ramSize.trackTable;

	updateStep = 1;
}


void cMtDisplay::ramg_text_edit()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

    //label
	API_CMD_TEXT(textEditData.x, textEditData.y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), textEditData.label);

	int16_t x_pos = textEditData.x + textEditData.length;
	int16_t y_pos = textEditData.y+12;

    //ramka
	API_COLOR(displayColors.trackTableFrame);
	API_LINE_WIDTH(12);
	API_BEGIN(LINE_STRIP);
	API_VERTEX2II(textEditData.x, y_pos, 0, 0);
	API_VERTEX2II(x_pos, y_pos, 0, 0);
	API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
	API_VERTEX2II(textEditData.x, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
	API_VERTEX2II(textEditData.x, y_pos, 0, 0);
	API_END();

	//text
	API_CMD_TEXT(textEditData.x+(textEditData.length/2) , y_pos+8, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), textEditData.text);

	// pozycja edycji
/*
	API_BEGIN(LINES);
	API_VERTEX2II(textEditData.editPos, y_pos, 0, 0);
	API_VERTEX2II(x_pos + (MT_DISP_BLOCK_W/4) -1, y_pos, 0, 0);
	API_END();
*/

    API_LIB_EndCoProList();

	updateAdress = ramAddress.textEdit;
	updateSize = &ramSize.textEdit;

	updateStep = 1;
}






