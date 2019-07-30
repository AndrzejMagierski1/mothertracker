/*
********************************************************************************
*             FT81x simple library for PIC MCU                                 *
********************************************************************************
Copyright (c) Future Technology Devices International / BridgeTek

THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

FTDI DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON FTDI PARTS.

FTDI DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

********************************************************************************
* This code is provided as an example only and is not guaranteed by FTDI.
* FTDI accept no responsibility for any issues resulting from its use.
* The developer of the final application incorporating any parts of this
* sample project is responsible for ensuring its safe and correct operation
* and for any consequences resulting from its use.
*
* Rev       Date        Author          Comment
********************************************************************************
* 1.0       June 2017	G Brown         Initial Release
* 1.1       Feb  2018   G Brown         Moved oscillator config #Pragmas to here from EVE_Layer
*  *****************************************************************************
*/


#include "FT812.h"
#include "FT8xx.h"

#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types

#include "mtHardware.h"
#include "SPI2.h"

#define CS LCD_CS
#define PD LCD_PD

//#define lcd_480x128


SPI2Settings settingsSLOW(1000000,  MSBFIRST, SPI_MODE0);
SPI2Settings settingsFAST(20000000, MSBFIRST, SPI_MODE0);

//############################################################################## 
//##############################################################################
//                                MCU LAYER 
//##############################################################################  
//##############################################################################

#define LCD_800_480
#define FT81x_ACTIVE	0x00
#define FT81x_CLKEXT	0x44

// LCD display parameters
uint16_t lcdWidth;                                                              // Active width of LCD display
uint16_t lcdHeight;                                                             // Active height of LCD display
uint16_t lcdHcycle;                                                             // Total number of clocks per line
uint16_t lcdHoffset;                                                            // Start of active line
uint16_t lcdHsync0;                                                             // Start of horizontal sync pulse
uint16_t lcdHsync1;                                                             // End of horizontal sync pulse
uint16_t lcdVcycle;                                                             // Total number of lines per screen
uint16_t lcdVoffset;                                                            // Start of active screen
uint16_t lcdVsync0;                                                             // Start of vertical sync pulse
uint16_t lcdVsync1;                                                             // End of vertical sync pulse
uint8_t lcdPclk;                                                                // Pixel Clock
uint8_t lcdSwizzle;                                                             // Define RGB output pins
uint8_t lcdPclkpol;                                                             // Define active edge of PCLK
uint8_t lcdCsSpread;
uint8_t lcdRotate;
uint8_t lcdDither;

uint32_t ramDisplayList = RAM_DL;                                               // Set beginning of display list memory
uint32_t ramCommandBuffer = RAM_CMD;                                            // Set beginning of graphics command memory

uint16_t FT81x_GPIO_DAT;                                                        // Used for GPIO registers
uint16_t FT81x_GPIO_DIR;
uint8_t dly = 0;
uint8_t PWM = 0;

// This is the MCU specific section and contains the functions which talk to the
// PIC registers. If porting the code to a different PIC or to another MCU, these
// should be modified to suit the registers of the selected MCU.

// ------------------- MCU specific initialisation  ----------------------------
void FT812_Init(void)
{
	uint32_t timeout=0;

    pinMode(CS, OUTPUT);
    pinMode(PD, OUTPUT);
    MCU_CShigh();
    MCU_PDhigh();

    SPI2.setMOSI(LCD_MOSI);
    SPI2.setMISO(LCD_MISO);
    SPI2.setSCK(LCD_SCK);

	SPI2.begin();

   // SPI2.setMOSI(LCD_MOSI);
   // SPI2.setMISO(LCD_MISO);
   // SPI2.setSCK(LCD_SCK);

	SPI2.beginTransaction(settingsSLOW);

    // ----------------------- Cycle PD pin to reset device --------------------

	MCU_Delay_20ms();
    MCU_PDlow();                                                                // PD low to reset device
    MCU_Delay_20ms();
    MCU_PDhigh();                                                               // PD high again
    MCU_Delay_20ms();

    // ---------------------- Delay to allow FT81x start-up --------------------
/*

    EVE_CmdWrite(0x42,0);
    delay(1);
    EVE_CmdWrite(0x61,0);
    delay(1);
    EVE_CmdWrite(0x00,0);
    delay(1);
  //  EVE_CmdWrite(0x44,0);
    delay(1);
    EVE_CmdWrite(0x49,0);
    delay(1);
    EVE_CmdWrite(0x68,0);
    delay(1);
*/

   // EVE_CmdWrite(0x61,2);


    MCU_Delay_500ms();

//  EVE_CmdWrite(FT81x_CLKEXT, 0x00);                                                        // PD high again
//  MCU_Delay_20ms();

    EVE_CmdWrite(FT81x_ACTIVE, 0x00);                                           // Sends 00 00 00 to wake FT8xx

    MCU_Delay_500ms();                                                          // 500ms delay (EVE requires at least 300ms here))




    // --------------- Check that FT8xx ready and SPI comms OK -----------------

    uint8_t ftID = EVE_MemRead8(REG_ID);

    while (ftID != 0x7C)                                        // Read REG_ID register until reads 0x7C
    {
    	ftID = EVE_MemRead8(REG_ID);
    	timeout++;
    	if(timeout>=0xFFFF)
    	{
    		if(hardwareTest)
    		{
    			 Serial.println("Display init error");
    		}
    		break;
    	}
    }

   // if(timeout >= 0xFFFF) return;
    timeout=0;

    while (EVE_MemRead8(REG_CPURESET) != 0x00)                                  // Ensure CPUreset register reads 0 and so FT8xx is ready
    {
    	timeout++;
    	if(timeout>=0xFFFF)
    	{
    		if(hardwareTest)
    		{
    			 Serial.println("Display init error");
    		}
    		break;
    	}
    }
    //if(timeout >= 0xFFFF) return;
    // ---------------- Configure the GPIO and PWM  --------------------

    EVE_MemWrite8(REG_PWM_DUTY, 0);                                             // Backlight off

    FT81x_GPIO_DAT = EVE_MemRead16(REG_GPIOX);                                  // Read the FT81x GPIO register for a read/modify/write operation
    FT81x_GPIO_DAT = FT81x_GPIO_DAT | 0x8000;                                   // set bit 15 of GPIO register (DISP). If using audio, also enable the amplifier shutdown GPIO line here
    EVE_MemWrite16(REG_GPIOX, FT81x_GPIO_DAT);                                  // Enable the DISP signal to the LCD panel

    FT81x_GPIO_DIR = EVE_MemRead16(REG_GPIOX_DIR);                              // Read the FT81x GPIO DIR register for a read/modify/write operation
    FT81x_GPIO_DIR = FT81x_GPIO_DIR | 0x8000;                                   // DISP is output, if using audio, also set the associated shutdown line as output (check your Eval module schematic))
    EVE_MemWrite16(REG_GPIOX_DIR, FT81x_GPIO_DIR);                              // Enable the DISP signal to the LCD panel

    // ------------------------- Display settings ------------------------------
/*
	#define FT_DispSwizzle 0
	#define FT_DispPCLKPol 1
	#define FT_DispCSpread 0
	#define FT_DispDither 0
*/

#ifdef lcd_480x128
    // WF52ATLASDNN0 display parameters
	lcdWidth   = 480;                                                           // Active width of LCD display
    lcdHeight  = 128;                                                           // Active height of LCD display

    lcdHcycle  = 531;                                                           // Total number of clocks per line
    lcdHoffset = 51;                                                            // Start of active line
    lcdHsync0  = 8;                                                             // Start of horizontal sync pulse
    lcdHsync1  = 12;                                                            // End of horizontal sync pulse

    lcdVcycle  = 292;                                                           // Total number of lines per screen
    lcdVoffset = 91;                                                            // Start of active screen
    lcdVsync0  = 8;                                                             // Start of vertical sync pulse
    lcdVsync1  = 12;                                                             // End of vertical sync pulse

    lcdPclk    = 6;                                                             // Pixel Clock
    lcdSwizzle = 0;                                                             // Define RGB output pins
    lcdPclkpol = 1;                                                             // Define active edge of PCLK
    lcdCsSpread = 1;
    lcdRotate = 0;

#else

    // WF70A2TIAGDNN0 display parameters
    lcdWidth   = 800;                                                           // Active width of LCD display
    lcdHeight  = 480;                                                           // Active height of LCD display

    lcdHcycle  = 1056;                                                           // Total number of clocks per line
    lcdHoffset = 46;                                                            // Start of active line
    lcdHsync0  = 0;                                                             // Start of horizontal sync pulse
    lcdHsync1  = 10;                                                            // End of horizontal sync pulse

    lcdVcycle  = 525;                                                           // Total number of lines per screen
    lcdVoffset = 23;                                                            // Start of active screen
    lcdVsync0  = 0;                                                             // Start of vertical sync pulse
    lcdVsync1  = 10;                                                             // End of vertical sync pulse

    lcdPclk    = 2;                                                             // Pixel Clock
    lcdSwizzle = 0;                                                             // Define RGB output pins
    lcdPclkpol = 1;                                                             // Define active edge of PCLK

    lcdCsSpread = 0;
    lcdRotate   = 4;
    //lcdRotate   = 4;
    lcdDither = 0;


#endif

/*
	lcdWidth   = 480;                                                           // Active width of LCD display
    lcdHeight  = 128;                                                           // Active height of LCD display

    lcdHcycle  = 531;                                                           // Total number of clocks per line
    lcdHoffset = 51;                                                            // Start of active line
    lcdHsync0  = 8;                                                             // Start of horizontal sync pulse
    lcdHsync1  = 12;                                                            // End of horizontal sync pulse

    lcdVcycle  = 292;                                                           // Total number of lines per screen
    lcdVoffset = 91;                                                            // Start of active screen
    lcdVsync0  = 8;                                                             // Start of vertical sync pulse
    lcdVsync1  = 12;                                                             // End of vertical sync pulse

    lcdPclk    = 6;                                                             // Pixel Clock
    lcdSwizzle = 0;                                                             // Define RGB output pins
    lcdPclkpol = 1;

    lcdCsSpread = 1;
    lcdRotate = 0;
*/
/*

#define EVE_HSIZE	(800L)
#define EVE_VSIZE	(480L)

#define EVE_VSYNC0	(9L)
#define EVE_VSYNC1	(12L)
#define EVE_VOFFSET	(32L)
#define EVE_VCYCLE	(525L)

#define EVE_HSYNC0	(42L)
#define EVE_HSYNC1	(90L)
#define EVE_HOFFSET	(88L)
#define EVE_HCYCLE 	(953)

#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(1L)
#define EVE_CSPREAD	(0L)

    lcdWidth   = EVE_HSIZE;                                                           // Active width of LCD display
    lcdHeight  = EVE_VSIZE;                                                           // Active height of LCD display

    lcdHcycle  = EVE_HCYCLE;                                                           // Total number of clocks per line
    lcdHoffset = EVE_HOFFSET;                                                            // Start of active line
    lcdHsync0  = EVE_HSYNC0;                                                             // Start of horizontal sync pulse
    lcdHsync1  = EVE_HSYNC1;                                                            // End of horizontal sync pulse

    lcdVcycle  = EVE_VCYCLE;                                                           // Total number of lines per screen
    lcdVoffset = EVE_VOFFSET;                                                            // Start of active screen
    lcdVsync0  = EVE_VSYNC0;                                                             // Start of vertical sync pulse
    lcdVsync1  = EVE_VSYNC1;                                                             // End of vertical sync pulse

    lcdPclk    = EVE_PCLK;                                                             // Pixel Clock
    lcdSwizzle = EVE_SWIZZLE;                                                             // Define RGB output pins
    lcdPclkpol = EVE_PCLKPOL;                                                             // Define active edge of PCLK

    lcdCsSpread = EVE_CSPREAD;

*/

    EVE_MemWrite16(REG_HSIZE,   lcdWidth);
    EVE_MemWrite16(REG_HCYCLE,  lcdHcycle);
    EVE_MemWrite16(REG_HOFFSET, lcdHoffset);
    EVE_MemWrite16(REG_HSYNC0,  lcdHsync0);
    EVE_MemWrite16(REG_HSYNC1,  lcdHsync1);
    EVE_MemWrite16(REG_VSIZE,   lcdHeight);
    EVE_MemWrite16(REG_VCYCLE,  lcdVcycle);
    EVE_MemWrite16(REG_VOFFSET, lcdVoffset);
    EVE_MemWrite16(REG_VSYNC0,  lcdVsync0);
    EVE_MemWrite16(REG_VSYNC1,  lcdVsync1);
    EVE_MemWrite8(REG_SWIZZLE,  lcdSwizzle);
    EVE_MemWrite8(REG_PCLK_POL, lcdPclkpol);
    EVE_MemWrite8(REG_CSPREAD,  lcdCsSpread);


    EVE_MemWrite8(REG_ROTATE,  lcdRotate);
    EVE_MemWrite8(REG_DITHER,  lcdDither);


    // ---------------------- Touch and Audio settings -------------------------

    EVE_MemWrite16(REG_TOUCH_RZTHRESH, 1200);                                   // Eliminate any false touches

    EVE_MemWrite8(REG_VOL_PB, ZERO);                                            // turn recorded audio volume down
    EVE_MemWrite8(REG_VOL_SOUND, ZERO);                                         // turn volume down
    EVE_MemWrite16(REG_SOUND, 0x6000);                                          // set volume mute

   // ---------------------- Create an initial screen before we enable the display -------------------------

	ramDisplayList = RAM_DL;                                                    // start of Display List
    EVE_MemWrite32(ramDisplayList, 0x02000000);                                 // Clear Color RGB sets the colour to clear screen to

    ramDisplayList += 4;                                                        // point to next location
    EVE_MemWrite32(ramDisplayList, (0x26000000 | 0x00000007));                  // Clear 00100110 -------- -------- -----CST  (C/S/T define which parameters to clear)

    ramDisplayList += 4;                                                        // point to next location
    EVE_MemWrite32(ramDisplayList, 0x00000000);                                 // DISPLAY command 00000000 00000000 00000000 00000000 (end of display list)

    EVE_MemWrite32(REG_DLSWAP, DLSWAP_FRAME);                                   // Swap display list to make the edited one active




    // -------------------- Now turn on PCLK and ramp the PWM up  -------------------------------------
    // ---------------- This provides a clean start-up for the application ----------------------------

    EVE_MemWrite8(REG_PCLK, lcdPclk);                                           // Now start clocking data to the LCD panel



//    for(PWM = 0; PWM <= 128; PWM ++)
//    {
//        EVE_MemWrite8(REG_PWM_DUTY, PWM);
//        delay(1);
//    }
//
    EVE_MemWrite8(REG_PWM_DUTY, 128);


// TOUCH
//    EVE_MemWrite16( REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);

	if(hardwareTest)
	{
		 Serial.println("Display init succesfull");
	}



	SPI2.beginTransaction(settingsFAST);

	//delay(10000);
}

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
void MCU_CSlow(void)
{
	digitalWrite(CS, LOW);                                                          // CS# line low
    asm("nop");
}  

// --------------------- Chip Select line high ---------------------------------
void MCU_CShigh(void)
{
    asm("nop");
    digitalWrite(CS, HIGH);                                                      // CS# line high
}

// -------------------------- PD line low --------------------------------------
void MCU_PDlow(void)
{
	digitalWrite(PD, LOW);                                                     // PD# line low
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
	digitalWrite(PD, HIGH);                                                        // PD# line high
}

// ################################# SPI #######################################

// --------------------- SPI Send and Receive ----------------------------------
uint8_t MCU_SPIReadWrite(uint8_t DataToWrite)
{
    return SPI2.transfer(DataToWrite);
}

void MCU_Delay_20ms(void)
{
	delay(20);
}

void MCU_Delay_3us(void)
{
	delayMicroseconds(3);
}


void MCU_Delay_500ms(void)
{
	delay(500);
}

void MCU_set_sleepMode(void)
{
	MCU_CSlow();
	EVE_AddrForWr(REG_PWM_DUTY);
	EVE_Write8(0);
	MCU_CShigh();
	EVE_CmdWrite(0x43,0x00);
    EVE_CmdWrite(0x50,0x00);
}

void MCU_set_runMode(void)
{
	EVE_CmdWrite(FT81x_ACTIVE, 0x00);
}
