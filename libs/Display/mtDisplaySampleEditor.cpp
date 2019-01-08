



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

		// buttons labels
		API_CMD_APPEND(MT_GPU_RAM_BLOCK_LABELS_ADRESS + (MT_GPU_RAM_BLOCK_LABELS_SIZE * i), displayBlock[i].ramLabelSize);

	}

    API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();

}

//#############################################################################
//#############################################################################
//#############################################################################


void cMtDisplay::setSampleEditorLabels(char * labels)
{

}

//#############################################################################
//#############################################################################
//#############################################################################



//#############################################################################
//#############################################################################
//#############################################################################


void cMtDisplay::ramg_sample_editor_labels()
{

}
