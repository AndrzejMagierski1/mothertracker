
#include "FT812.h"
#include "display.h"

#include "SD.h"

static const uint8_t bytesPerPixel = 4;
static const uint32_t bmpWidth = 800;
static const uint32_t bmpHeight = 480;
static const uint32_t fileHeaderSize = 14;
static const uint32_t infoHeaderSize = 56; //56
static const uint32_t bmpDataSize = bmpWidth*bmpHeight*bytesPerPixel;
static const uint32_t fileSize = fileHeaderSize + infoHeaderSize + bmpDataSize;


static const unsigned char fileHeader[] =
{
	'B','M', /// signature
	(uint8_t)(fileSize),(uint8_t)(fileSize>>8),(uint8_t)(fileSize>>16),(uint8_t)(fileSize>>24), /// image file size in bytes
	0,0,0,0, /// reserved
	(uint8_t)(fileHeaderSize + infoHeaderSize),0,0,0, /// start of pixel array
};

static const unsigned char infoHeader[] =
{
	(uint8_t)(infoHeaderSize),0,0,0, /// header size
	(uint8_t)(bmpWidth),(uint8_t)(bmpWidth>>8),(uint8_t)(bmpWidth>>16),(uint8_t)(bmpWidth>>24), /// image width
	(uint8_t)(bmpHeight),(uint8_t)(bmpHeight>>8),(uint8_t)(bmpHeight>>16),(uint8_t)(bmpHeight>>24), /// image height
	1,0, /// number of color planes
	bytesPerPixel*8,0, /// bits per pixel
	3,0,0,0, /// compression //(3)
	0,0,0,0, /// image size
	0,0,0,0, /// horizontal resolution
	0,0,0,0, /// vertical resolution
	0,0,0,0, /// colors in color table
	0,0,0,0, /// important colors
//	0,0xF8,0,0, /// r
//	0xE0,0x7,0,0, /// g
//	0x1F,0,0,0, /// b
//	0,0,0,0, /// a
	0,0,255,0, /// r
	0,255,0,0, /// g
	255,0,0,0, /// b
	0,0,0,255, /// a
};




extern uint8_t lcdPclk;
extern uint8_t lcdRotate;
void cDisplay::doScreenShot()
{

	if(!SD.exists("Snapshots")) SD.mkdir(0,"Snapshots");

	char bmpName[32];
	uint16_t num = 1;
	sprintf(bmpName, "Snapshots/Snapshot_%03d.bmp", 1);
	SdFile bmp;
	while(SD.exists(bmpName))
	{
		num++;
		sprintf(bmpName, "Snapshots/Snapshot_%03d.bmp", num);
	}

	bmp.open(bmpName, FILE_WRITE);

	bmp.write(fileHeader, fileHeaderSize);
	bmp.write(infoHeader, infoHeaderSize);

	uint8_t buf[3200];
	uint16_t readstep = 0;

	API_LIB_BeginCoProList();
	API_CMD_SNAPSHOT2(0x20, controlsRamStartAddress, 0, 240, 1600, 240);
	API_LIB_EndCoProList();
	API_LIB_AwaitCoProEmpty();

	readstep = 0;

	while(readstep < 240)
	{
		API_LIB_ReadDataRAMG(buf, 3200, (controlsRamStartAddress+(3200*240)-3200)-readstep*3200);
		bmp.write(buf, 3200);
		readstep++;
	}

	API_LIB_BeginCoProList();
	API_CMD_SNAPSHOT2(0x20, controlsRamStartAddress, 0, 0, 1600, 240);
	API_LIB_EndCoProList();
	API_LIB_AwaitCoProEmpty();

	readstep = 0;

	while(readstep < 242)
	{
		API_LIB_ReadDataRAMG(buf, 3200, (controlsRamStartAddress+(3200*240)-3200)-readstep*3200);
		bmp.write(buf, 3200);
		readstep++;
	}

	bmp.close();


	API_LIB_BeginCoProList();
	API_CMD_MEMZERO(controlsRamStartAddress,600000);

	API_LIB_EndCoProList();

	//EVE_MemWrite8(REG_PCLK, lcdPclk);

	resetControlQueue();

	for(uint8_t i = 0; i < controlsCount; i++)
	{
		refreshControl(controlsTable[i]);
	}
}

