
#include <stdint.h>
#include "mtStructs.h"



#include "mtDisplay.h"

//#############################################################################
//#############################################################################
//#############################################################################

void cMtDisplay::dl_load_instrument_editor_main()
{

    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(displayBgColor);
	API_CLEAR(1,1,1);


	// spectrum
	API_CMD_APPEND(MT_GPU_RAM_INSTRUMENT_EDITOR_SPECTRUM_ADRESS, instrumentEditor.ramSpectrumSize);

	// points
	API_CMD_APPEND(MT_GPU_RAM_INSTRUMENT_EDITOR_POINTS_ADRESS, instrumentEditor.ramPointsSize);


	// buttons labels
	API_CMD_APPEND(MT_GPU_RAM_INSTRUMENT_EDITOR_LABELS_ADRESS, instrumentEditor.ramLabelsSize);

	// sample list
	if(instrumentEditorSampleList.enabled())
	{
		API_CMD_APPEND(MT_GPU_RAM_INSTRUMENT_EDITOR_SAMPLE_ADRESS, instrumentEditor.ramSampleListSize);
	}



    API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();

}

//#############################################################################
//#############################################################################
//#############################################################################

void cMtDisplay::setInstrumentEditorPoints(strSpectrum *spectrum)
{
	instrumentEditor.spectrum = spectrum;

	displayRefreshTable.instrumentEditor.background = 1;
	displayRefreshTable.instrumentEditor.points = 1;
	screenRefresh = 1;
}

void cMtDisplay::setInstrumentEditorSpectrum(strSpectrum *spectrum)
{
	instrumentEditor.spectrum = spectrum;

	displayRefreshTable.instrumentEditor.spectrum = 1;
	screenRefresh = 1;
}


void  cMtDisplay::setInstrumentEditorButtonsLabels(char ** labels)
{
	instrumentEditor.buttonsLabels = labels;

	displayRefreshTable.instrumentEditor.labels = 1;
	screenRefresh = 1;
}

void  cMtDisplay::setInstrumentEditorPotsLabels(char ** labels)
{
	instrumentEditor.potsLabels = labels;

	displayRefreshTable.instrumentEditor.labels = 1;
	screenRefresh = 1;
}

void  cMtDisplay::setInstrumentEditorSampleList(uint16_t start, char ** list, uint16_t count)
{

	instrumentEditorSampleList.setList(3, 1, start, list, count);

	displayRefreshTable.instrumentEditor.sampleList = 1;
	screenRefresh = 1;

}

void cMtDisplay::setInstrumentEditorSampleListPos(uint16_t position)
{
	instrumentEditorSampleList.setListPos(position);

	displayRefreshTable.instrumentEditor.sampleList = 1;
	screenRefresh = 1;
}
//#############################################################################
//#############################################################################
//#############################################################################


void cMtDisplay::ramg_instrument_editor_points()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();


	API_COLOR(instrumentEditor.loopColor);

	API_BLEND_FUNC(SRC_ALPHA, ONE);

	API_LINE_WIDTH(8);
	API_BEGIN(RECTS);
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint1, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint2, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
	API_END();

	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

	API_COLOR(instrumentEditor.pointsColor);
	API_LINE_WIDTH(8);

	API_BEGIN(LINES);

	// start point
	API_VERTEX2II(instrumentEditor.spectrum->startPoint, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->startPoint, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);

	// end point
	API_VERTEX2II(instrumentEditor.spectrum->endPoint, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->endPoint, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);

	// loop point 1
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint1, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint1, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);

	// loop point 2
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint2, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint2, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);

	API_END();


	//znaczki
	API_BEGIN(LINE_STRIP);
	API_VERTEX2II(instrumentEditor.spectrum->startPoint+1, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->startPoint+5, MT_DISP_IEDITOR_SPECTRUM_Y-37,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->startPoint+1, MT_DISP_IEDITOR_SPECTRUM_Y-34,0,0);
	API_END();

	API_BEGIN(LINE_STRIP);
	API_VERTEX2II(instrumentEditor.spectrum->endPoint-1, MT_DISP_IEDITOR_SPECTRUM_Y-40,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->endPoint-5, MT_DISP_IEDITOR_SPECTRUM_Y-37,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->endPoint-1, MT_DISP_IEDITOR_SPECTRUM_Y-34,0,0);
	API_END();


	API_BEGIN(LINE_STRIP);
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint1+1, MT_DISP_IEDITOR_SPECTRUM_Y+34,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint1+5, MT_DISP_IEDITOR_SPECTRUM_Y+37,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint1+1, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
	API_END();

	API_BEGIN(LINE_STRIP);
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint2-1, MT_DISP_IEDITOR_SPECTRUM_Y+34,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint2-5, MT_DISP_IEDITOR_SPECTRUM_Y+37,0,0);
	API_VERTEX2II(instrumentEditor.spectrum->loopPoint2-1, MT_DISP_IEDITOR_SPECTRUM_Y+40,0,0);
	API_END();



    API_LIB_EndCoProList();

    /**/

	updateAdress = MT_GPU_RAM_INSTRUMENT_EDITOR_POINTS_ADRESS;
	updateSize = &instrumentEditor.ramPointsSize;

	updateStep = 1;
}


void cMtDisplay::ramg_instrument_editor_spectrum()
{

    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_COLOR(instrumentEditor.spectrumColor);
	API_LINE_WIDTH(8);


	if(instrumentEditor.spectrum->spectrumType == 1)
	{
		//jedna lamana linia
		API_BEGIN(LINE_STRIP);

		for(uint16_t i = 0; i < 480; i++)
		{
			if(instrumentEditor.spectrum->upperData[i] > 0) API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y-instrumentEditor.spectrum->upperData[i],0,0);
			else if(instrumentEditor.spectrum->lowerData[i] < 0) API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y-instrumentEditor.spectrum->lowerData[i],0,0);
			else API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y,0,0);
		}

	}
	else
	{
		API_BEGIN(LINES);
		// 480 pionowych lini
		for(uint16_t i = 0; i < 480; i++)
		{
			API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y-instrumentEditor.spectrum->upperData[i],0,0);
			API_VERTEX2II( i, MT_DISP_IEDITOR_SPECTRUM_Y-instrumentEditor.spectrum->lowerData[i],0,0);
		}
	}


	API_END();

    API_LIB_EndCoProList();

    /**/

	updateAdress = MT_GPU_RAM_INSTRUMENT_EDITOR_SPECTRUM_ADRESS;
	updateSize = &instrumentEditor.ramSpectrumSize;

	updateStep = 1;
}


void cMtDisplay::ramg_instrument_editor_labels()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

    for(uint8_t i = 0; i < 5; i++)
    {

    	//buttons
		API_COLOR(instrumentEditor.labelColor);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2II((MT_DISP_BLOCK_W * i + MT_DISP_BLOCK_LABEL_OFFSET) , (MT_DISP_H - MT_DISP_BLOCK_LABEL_H) ,0,0);
		API_VERTEX2II((MT_DISP_BLOCK_W * i + (MT_DISP_BLOCK_W - MT_DISP_BLOCK_LABEL_OFFSET)) , MT_DISP_H,0,0);
		API_END();
		API_COLOR(instrumentEditor.fontLabelColor);
		API_CMD_TEXT(MT_DISP_BLOCK_W * i + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_LABEL_Y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), *(instrumentEditor.buttonsLabels+i));


		//pots
		API_COLOR(instrumentEditor.fontTitleColor);
		API_CMD_TEXT(MT_DISP_BLOCK_W * i + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_TITLE_Y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), *(instrumentEditor.potsLabels+i));


    }

    API_LIB_EndCoProList();

    /**/

	updateAdress = MT_GPU_RAM_INSTRUMENT_EDITOR_LABELS_ADRESS;
	updateSize = &instrumentEditor.ramLabelsSize;

	updateStep = 1;
}

