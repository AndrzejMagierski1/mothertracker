



#include <stdint.h>

#include "mtDisplay.h"

//#############################################################################
//#############################################################################
//#############################################################################

void cMtDisplay::dl_load_sample_editor_main()
{

    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(displayBgColor);
	API_CLEAR(1,1,1);

	for(uint8_t i = 0; i < MT_DISP_BLOCK_COUNT ; i++)
	{

		// title
		API_CMD_APPEND(MT_GPU_RAM_SAMPLE_EDITOR_TITLE_ADRESS, sampleEditor.ramTitleSize);

		// spectrum
		API_CMD_APPEND(MT_GPU_RAM_SAMPLE_EDITOR_SPECTRUM_ADRESS, sampleEditor.ramSpectrumSize);

		// buttons labels
		API_CMD_APPEND(MT_GPU_RAM_SAMPLE_EDITOR_LABELS_ADRESS, sampleEditor.ramLabelsSize);


	}

    API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();

}

//#############################################################################
//#############################################################################
//#############################################################################

void cMtDisplay::setSampleEditorTitle(char text[])
{
	for(uint8_t i = 0; i < 20; i++)	sampleEditor.title[i] = text[i];

	displayRefreshTable.sampleEditor.title = 1;
	screenRefresh = 1;
}

void cMtDisplay::setSampleEditorSpectrum(uint16_t start, char * list, uint16_t count, uint8_t filename_length)
{


	displayRefreshTable.sampleEditor.spectrum = 1;
	screenRefresh = 1;
}


void cMtDisplay::setSampleEditorLabels(char * labels)
{
	sampleEditor.labels = labels;

	displayRefreshTable.sampleEditor.labels = 1;
	screenRefresh = 1;
}

//#############################################################################
//#############################################################################
//#############################################################################

void cMtDisplay::ramg_sample_editor_title()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_COLOR(sampleEditor.fontTitleColor);
	API_CMD_TEXT(MT_DISP_FMANAGER_ROOTTITLE_X, MT_DISP_FMANAGER_TITLE_Y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), sampleEditor.title);

    API_LIB_EndCoProList();

    /**/

	updateAdress = MT_GPU_RAM_SAMPLE_EDITOR_TITLE_ADRESS;
	updateSize = &sampleEditor.ramTitleSize;

	updateStep = 1;
}


void cMtDisplay::ramg_sample_editor_spectrum()
{

}


void cMtDisplay::ramg_sample_editor_labels()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

    for(uint8_t i = 0; i < 5; i++)
    {
		API_COLOR(sampleEditor.labelColor);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2II((MT_DISP_BLOCK_W * i + MT_DISP_BLOCK_LABEL_OFFSET) , (MT_DISP_H - MT_DISP_BLOCK_LABEL_H) ,0,0);
		API_VERTEX2II((MT_DISP_BLOCK_W * i + (MT_DISP_BLOCK_W - MT_DISP_BLOCK_LABEL_OFFSET)) , MT_DISP_H,0,0);
		API_END();
		API_COLOR(sampleEditor.fontLabelColor);
		API_CMD_TEXT(MT_DISP_BLOCK_W * i + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_LABEL_Y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), sampleEditor.labels+(i*20));
    }

    API_LIB_EndCoProList();

    /**/

	updateAdress = MT_GPU_RAM_SAMPLE_EDITOR_LABELS_ADRESS;
	updateSize = &sampleEditor.ramLabelsSize;

	updateStep = 1;
}

