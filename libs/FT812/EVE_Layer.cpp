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
* 1.1       Feb  2018   G Brown         Moved oscillator config #Pragmas to MCU_Layer.c
*                                       Changed ReadPointer and WritePointer reads from 32-bit to 16-bit 
*                                       to match variable size in EVE_WaitCmdFifoEmpty
*  *****************************************************************************
*/

#include <FT812.h>
#include "FT8xx.h"
#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types

#include "SPI2.h"

#define MEM_WRITE	0x80			// FT800 Host Memory Write
#define MEM_READ	0x00			// FT800 Host Memory Read


//############################################################################## 
//##############################################################################
//                        EVE INTERFACE LAYER 
//##############################################################################  
//##############################################################################


// ############### INDIVIDUAL ADDRESSING AND DATA FUNCTIONS ####################

// This section has separate calls for addressing and data phases of the transaction which allows for burst writes and reads.

// ------------------ Send FT81x register address for writing ------------------
void EVE_AddrForWr(uint32_t ftAddress)
{
    uint8_t dummyRead = 0;
    (void)dummyRead;
    // Send three bytes of a register address which has to be subsequently written. Ignore return values as this is an SPI write only.
    dummyRead = SPI2.transfer( (uint8_t) ((ftAddress >> 16) | MEM_WRITE) );  // Send high byte of address with 'write' bits set

    dummyRead = SPI2.transfer( (uint8_t) (ftAddress >> 8) );                 // Send middle byte of address

    dummyRead = SPI2.transfer( (uint8_t) (ftAddress) );                      // Send low byte of address
}

// ------------------ Send FT81x register address for reading ------------------
void EVE_AddrForRd(uint32_t ftAddress)
{
    uint8_t dummyRead = 0;
    (void)dummyRead;
    // Send three bytes of a register address which has to be subsequently read. Ignore return values as this is an SPI write only.
    dummyRead = SPI2.transfer( (uint8_t) ((ftAddress >> 16) | MEM_READ) );   // Send high byte of address with 'read' bits set

    dummyRead = SPI2.transfer( (uint8_t) (ftAddress >> 8) );                 // Send middle byte of address

    dummyRead = SPI2.transfer( (uint8_t) (ftAddress) );                      // Send low byte of address

    dummyRead = SPI2.transfer( 0x00 );                                       // Send dummy byte before doing the read
}

// ------------------------ Send a 32-bit data value --------------------------
void EVE_Write32(uint32_t ftData32)
{    
    // Send four bytes of data after previously sending address. Ignore return values as this is an SPI write only.
    SPI2.transfer( (uint8_t) (ftData32) );                       // Send low byte of data

    SPI2.transfer( (uint8_t) (ftData32 >> 8) );                  //

    SPI2.transfer( (uint8_t) (ftData32 >> 16) );                 //

    SPI2.transfer( (uint8_t) (ftData32 >> 24) );                 // Send high byte of data
}

// ------------------------ Send a 16-bit data value --------------------------
void EVE_Write16(uint16_t ftData16)
{
    // Send two bytes of data after previously sending address. Ignore return values as this is an SPI write only.
    SPI2.transfer( (uint8_t) (ftData16) );                       // Send low byte of data

    SPI2.transfer( (uint8_t) (ftData16 >> 8) );                  // Send high byte of data
}

// ------------------------ Send an 8-bit data value ---------------------------
void EVE_Write8(uint8_t ftData8)
{
    // Send one byte of data after previously sending address. Ignore return values as this is an SPI write only.
    SPI2.transfer( (uint8_t) (ftData8) );                        // Send byte of data
}

// ------------------------ Read a 32-bit data value --------------------------
uint32_t EVE_Read32(void)
{    
    // Read 4 bytes from a register has been previously addressed. Send dummy 00 bytes as only the incoming value is important.
    uint32_t ftData32 = 0x00000000;
    uint32_t tempData32 = 0x00000000;
       
    tempData32 = SPI2.transfer( 0x00 );                                      // Read low byte of data first
    ftData32 = ftData32 | tempData32;
     
    tempData32 = SPI2.transfer( 0x00 );
    tempData32 = ((tempData32 << 8) & 0x0000FF00);
    ftData32 = ftData32 | tempData32;

    tempData32 = SPI2.transfer( 0x00 );
    tempData32 = ((tempData32 << 16) & 0x00FF0000);
    ftData32 = ftData32 | tempData32;    
    
    tempData32 = SPI2.transfer( 0x00 );                                      // Read high byte of data last
    tempData32 = ((tempData32 << 24) & 0xFF000000);
    ftData32 = ftData32 | tempData32;  

    return ftData32;                                                            // Return combined 32-bit value
}

// ------------------------ Read a 16-bit data value ---------------------------
uint16_t EVE_Read16(void)
{
    // Read 2 bytes from a register has been previously addressed. Send dummy 00 bytes as only the incoming value is important.
    uint16_t ftData16 = 0x0000;
    uint16_t tempData16 = 0x0000;
    
    tempData16 = SPI2.transfer( 0x00 );                                      // Read low byte of data first
    ftData16 = tempData16;

    tempData16 = SPI2.transfer( 0x00 );                                      // Read high byte of data last
    tempData16 = ((tempData16 << 8) & 0xFF00);
    ftData16 = ftData16 | tempData16;

    return ftData16;                                                            // Return combined 16-bit value
}

// ------------------------ Read an 8-bit data value ---------------------------
uint8_t EVE_Read8(void)
{
    // Read 1 byte from a register has been previously addressed. Send dummy 00 byte as only the incoming value is important.
    uint8_t ftData8 = 0x00;
    
    ftData8 = SPI2.transfer( 0x00 );                                         // Read byte of data
      
    return ftData8;                                                             // Return 8-bit value read
}



// ################# COMBINED ADDRESSING AND DATA FUNCTIONS ####################

// This section has combined calls which carry out a full write or read cycle including chip select, address, and data transfer.
// This would often be used for register writes and reads. 

// -------------- Write a 32-bit value to specified address --------------------
void EVE_MemWrite32(uint32_t ftAddress, uint32_t ftData32)
{
    MCU_CSlow();                                                                // CS low begins the SPI transfer    
  
    EVE_AddrForWr(ftAddress);                                                   // Send address to be written
  
    EVE_Write32(ftData32);                                                      // Send the data value
    
    MCU_CShigh();                                                               // CS high terminates the SPI transfer

}

// -------------- Write a 16-bit value to specified address --------------------
void EVE_MemWrite16(uint32_t ftAddress, uint16_t ftData16)
{
    MCU_CSlow();                                                                // CS low begins the SPI transfer 
  
    EVE_AddrForWr(ftAddress);                                                   // Send address to be written
  
    EVE_Write16(ftData16);                                                      // Send the data value        
    
    MCU_CShigh();                                                               // CS high terminates the SPI transfer
}

// -------------- Write an 8-bit value to specified address --------------------
void EVE_MemWrite8(uint32_t ftAddress, uint8_t ftData8)
{
    MCU_CSlow();                                                                // CS low begins the SPI transfer 
  
    EVE_AddrForWr(ftAddress);                                                   // Send address to be written

    EVE_Write8(ftData8);                                                        // Send the data value        
    
    MCU_CShigh();                                                               // CS high terminates the SPI transfer
}



// -------------- Read a 32-bit value from specified address --------------------
uint32_t EVE_MemRead32(uint32_t ftAddress)
{
    uint32_t ftData32 = 0x00000000;
  
    MCU_CSlow();                                                                // CS low begins the SPI transfer 
  
    EVE_AddrForRd(ftAddress);                                                   // Send address to be read
       
    ftData32 = EVE_Read32();                                                    // Read the data value    

    MCU_CShigh();                                                               // CS high terminates the SPI transfer
    
    return ftData32;                                                            // Return 32-bit value read
}

// -------------- Read a 16-bit value from specified address --------------------
uint16_t EVE_MemRead16(uint32_t ftAddress)
{
    uint16_t ftData16 = 0x0000;
  
    MCU_CSlow();                                                                // CS low begins the SPI transfer 
    delayMicroseconds(10);
    
    EVE_AddrForRd(ftAddress);                                                   // Send address to be read
        
    ftData16 = EVE_Read16();                                                    // Read the data value

    delayMicroseconds(10);
    MCU_CShigh();                                                               // CS high terminates the SPI transfer

    return ftData16;                                                            // Return 16-bit value read
}

// -------------- Read an 8-bit value from specified address --------------------
uint8_t EVE_MemRead8(uint32_t ftAddress)
{
    uint8_t ftData8 = 0x00;
  
    MCU_CSlow();                                                                // CS low begins the SPI transfer 
  
    EVE_AddrForRd(ftAddress);                                                   // Send address to be read
       
    ftData8 = EVE_Read8();                                                      // Read the data value   

    MCU_CShigh();                                                               // CS high terminates the SPI transfer
 
    return ftData8;                                                             // Return 8-bit value read
}





// ############################# HOST COMMANDS #################################

// -------------------------- Write a host command -----------------------------
void EVE_CmdWrite(uint8_t EVECmd, uint8_t Param)
{
    uint8_t dummyRead = 0;
    (void)dummyRead;

    MCU_CSlow();                                                                // CS low begins the SPI transfer
    
    dummyRead = MCU_SPIReadWrite( (uint8_t) (EVECmd) );                         // Send command 

    dummyRead = MCU_SPIReadWrite( (uint8_t) (Param) );                          // followed by parameter

    dummyRead = MCU_SPIReadWrite( 0x00 );                                       // and a dummy 00 byte
    
    MCU_CShigh();                                                               // CS high terminates the SPI transfer
}

// ######################## SUPPORTING FUNCTIONS ###############################

// --------- Increment co-processor address offset counter --------------------
uint16_t EVE_IncCMDOffset(uint16_t currentOffset, uint16_t commandSize)
{
    uint16_t newOffset;                                                         // Used to hold new offset
    
    newOffset = currentOffset + commandSize;                                    // Calculate new offset
    
    if(newOffset > 4095)                                                        // If new offset past top of buffer...
    {
        newOffset = (newOffset - 4096);                                         // ... roll over
    }
    
    return newOffset;                                                           // Return new offset
}

// ------ Wait for co-processor read and write pointers to be equal ------------

uint8_t EVE_WaitCmdFifoEmpty(void)
{
    uint32_t ReadPointer, WritePointer;
        
    do
    {
        ReadPointer = EVE_MemRead32(REG_CMD_READ);                              // Read the graphics processor read pointer
        WritePointer = EVE_MemRead32(REG_CMD_WRITE);                            // Read the graphics processor write pointer
    }while ((WritePointer != ReadPointer) && (ReadPointer != 0xFFF));           // Wait until the two registers match

    if(ReadPointer == 0xFFF)
        return 0xFF;                                                            // Return 0xFF if an error occurred
    else
        return 0;                                                               // Return 0 if pointers became equal successfully
}


uint32_t error_counter = 0;

uint8_t EVE_IsCmdFifoEmpty(void)
{
    uint32_t ReadPointer = EVE_MemRead32(REG_CMD_READ);
    uint32_t WritePointer = EVE_MemRead32(REG_CMD_WRITE);

/*
    uint8_t flag = 0;
    if(flag == 1)
    {
    	EVE_MemWrite32(REG_CMD_READ, WritePointer);
    	EVE_MemWrite32(REG_CMD_WRITE, WritePointer);
    }
*/

    if(ReadPointer == 0xFFF || error_counter > 100000)
    {
    	//Set REG_CPURESET to 1, to hold the co-processor engine in the reset condition
    	//Set REG_CMD_READ and REG_CMD_WRITE to zero
    	//Set REG_CPURESET to 0, to restart the co-processor engine
    	error_counter = 0;

    	//Serial.println("FT812 ERROR!");

    	EVE_MemWrite32(REG_CPURESET,1);
    	EVE_MemWrite32(REG_CMD_READ, 0);
    	EVE_MemWrite32(REG_CMD_WRITE, 0);
    	EVE_MemWrite32(REG_CPURESET,0);

    	return 1;
    	//cmdOffset = 0;
    }

    if(WritePointer == ReadPointer)
    {
    	error_counter = 0;
    	return 1;

    }
    else error_counter++;


    return 0;
}



// ------ Get current value of the FIFO write pointer REG_CMD_WRITE ------------

uint32_t EVE_GetCurrentWritePointer(void)
{
    uint32_t WritePointer;

    WritePointer = EVE_MemRead32(REG_CMD_WRITE);                                // Read the graphics processor write pointer
  
    return WritePointer;                                                        // New starting point will be first location after the last command
}

// ------------ Check how much free space is available in CMD FIFO -------------

uint16_t EVE_CheckFreeSpace(uint16_t CmdOffset)
{
    uint32_t ReadPointer = 0;
    uint16_t Fullness, Freespace;
        
    ReadPointer = EVE_MemRead32(REG_CMD_READ);                                  // Check the graphics processor read pointer
    Fullness = ((CmdOffset - (uint16_t)ReadPointer) & 4095);                    // Fullness is difference between MCUs current write pointer value and the FT81x's REG_CMD_READ
    Freespace = (4096 - 4) - Fullness;                                          // Free Space is 4K - 4 - Fullness (-4 avoids buffer wrapping round)
    return Freespace;
}








