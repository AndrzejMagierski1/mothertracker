
#ifndef LIBS_NEWDISPLAY_DISPLAYSTRUCTS_H_
#define LIBS_NEWDISPLAY_DISPLAYSTRUCTS_H_

#include <stdint.h>
#include <stddef.h>


class cDisplayControl;
class cDebugLog;
class cDisplay;

extern "C" {
extern void *memcpy (void *dst, const void *src, size_t count);
}

//#########################################################################
//							DEFINICJE
//#########################################################################

const uint8_t controlsCount = 55;
const uint32_t controlsRamStartAddress = 100000;
const uint32_t controlsRamAddressStep = 10000;

const uint32_t imgRamAddress = 670000;
const uint32_t imgSizeMax = 1000000-imgRamAddress;
const int32_t imgBufforSize = 5000;

const uint8_t displayFontCount = 5;


enum enDisplayBitmaps
{
	displayPolyLogoBitmap,
	displayPlayIcon,
	displayLoopIcon,

	displayDoubleArrowL,
	displayDoubleArrowR,
	displayArrowU,
	displayArrowD,

	displayBitmapsCount,
};



enum controlsStyle
{
	controlStyleHide = 0,
	controlStyleShow = 1,

	controlStyleCreateSubcontrols = 2,

	controlStyleCenterX = 4,
	controlStyleCenterY = 8,
	controlStyleRightX 	= 16,

	controlStyleShowBitmap		= 32,
	controlStyleManualText		= 64,
	controlStyleVerticalText 	= 128,

	controlStyleFont1 	= 256,
	controlStyleFont2 	= 512,
	controlStyleFont3 	= 512+256,
	controlStyleFont4 	= 1024,

	controlStyleFont5 	= 256+1024,
	controlStyleFont6 	= 512+1024,
	controlStyleFont7 	= 512+256+1024,
	controlStyleFont8 	= 1024+1024,

	controlStyleBorder 			= (1<<12),
	controlStyleBackground 		= (1<<13),
	controlStyleRoundedBorder 	= (1<<14),
	controlStyleNoTransparency 	= (1<<15),

	controlStyleValueLeftRight_100_100 	= (1<<16),
	controlStyleValueLeftRight_24_24 	= (1<<17),
	controlStyleValue_0_100 			= (1<<18),
	controlStyleValueNumberOnly			= (1<<19),
	controlStyleCompareTwoValues 		= (1<<20),

	controlStyleBottomShadow 			= (1<<21),

	controlStyleTransformImg 			= (1<<22),

};


typedef enum enControlId
{
	controlIdBar,
	controlIdBgLabel,
	controlIdFrame,
	controlIdEdit,
	controlIdImg,
	controlIdLabel,
	controlIdHorizontalBar,
	controlIdLineIndicator,
	controlIdList,
	controlIdKeyboard,
	controlIdNotePopup,
	controlIdMultiLabel,
	controlIdParamValueList,
	controlIdPoints,
	controlIdProcessingPopup,
	controlIdProgressCursor,
	controlIdSelectionWindow,
	controlIdSimpleText,
	controlIdSlicePoints,
	controlIdSongPlayer,
	controlIdSpectrum,
	controlIdStartScreen,
	controlIdTemplate,
	controlIdTextBox,
	controlIdTextPopup,
	controlIdTracker,
	controlIdWtProgress,


/*
 *
: cDisplayControl(controlId)

*/
} control_id_t;

struct strControlProperties
{
	const char* text = nullptr;
	int value = 0;
	void* data = nullptr;

	uint32_t style = 0;
	uint32_t* colors = nullptr;

	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t w = 0;
	uint16_t h = 0;
};

struct strFont
{
	const uint8_t* data;
	uint32_t size;
	int16_t handle;
	uint32_t address;
	int32_t source;
	uint16_t width;
	uint16_t height;
	uint8_t format;
	uint16_t linestride;
};

struct strBitmap
{
	const uint8_t* data;
	uint32_t address;
	uint32_t source;
	uint16_t width;
	uint16_t height;
	uint8_t format;
	uint16_t linestride;
};

struct strGetProps
{
	uint32_t End_Address;
	uint32_t Width;
	uint32_t Height;
};

extern const strFont fonts[];
extern const strBitmap bitmaps[];

extern const strBitmap bitmaps[];
extern strGetProps getProps;


#define FONT_INDEX_FROM_STYLE (((style >> 8) & 15)-1)


const uint32_t one_true_red  = 0xcd1818;
const uint32_t one_true_green  = 0x32d642;

uint16_t getTextWidth(strFont* font, const char* text, uint16_t length);


// NIE-EDITABLE
const uint8_t controlsRefreshQueueSize = controlsCount+1;

#define DISP_RGB(red,green,blue) ((((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))

uint8_t get_jpeg_size(uint8_t* data, int32_t data_size, uint16_t *width, uint16_t *height);




typedef cDisplayControl* hControl;





#endif /* LIBS_NEWDISPLAY_DISPLAYSTRUCTS_H_ */
