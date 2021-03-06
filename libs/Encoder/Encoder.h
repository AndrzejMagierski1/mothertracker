
#ifndef __MEDUSAENCODER_H__
#define __MEDUSAENCODER_H__


#include <stdint.h>
#include "mk20dx128.h"
#include "core_pins.h"



// Keep track of previous Interrupt to determine direction
enum prevInt_t{prevIntTOF, prevIntCompLow, prevIntCompHigh};


#define ENCODER_PPR 96.0

class cEncoder
{
public:


	void switchRead();
	void begin(uint8_t intPin = 255, void (*func)(uint8_t n, uint8_t value) = nullptr);
	void setResolution(uint16_t value);
	void setAcceleration(uint8_t value);
	int32_t read();
	void testMode(uint8_t state);


	void zeroFTM();	//Zero position counter
	int32_t calcPosn(void);	// Calculate current position from 
	//   Base Position and counter value
	void ftm_isr();	// FTM interrupt service routine 
	//  Interrupt on overflow and position compare

private:

	uint8_t resolution_ratio = 1;
	uint8_t resolution = resolution_ratio*8;
	uint8_t test_mode = 0;
	uint8_t speed_up = 0;
	int16_t difference = 0;	

	void (*switchFunc)(uint8_t n, uint8_t value) = nullptr;
	uint8_t switchPin = 0;
	uint8_t powerSwitchLastState = 0;
	int8_t powerSwitchDebounce;

	struct strEncoder
	{
		int16_t position;
		int16_t last_position;
		uint8_t part;
		uint8_t last_part;

		uint8_t last_direction;

		int16_t diffrence_blur;

		uint16_t resolution = 24;
		uint8_t speed = 0;

		int16_t  global_diff = 0;

	} encoder;



	// Compare positions for 16 bit (64K) counter
	static const int NUM_64K=0x10000;	// Value of 16 bit counter
	static const uint16_t COMP1_4=NUM_64K/4;
	static const uint16_t COMP3_4=NUM_64K*3/4;
	static const uint16_t COMP3_8=NUM_64K*3/8;
	static const uint16_t COMP5_8=NUM_64K*5/8;

	static const uint16_t COMP_LOW=COMP3_8;	    // Lower compare point
	static const uint16_t COMP_HIGH=COMP5_8;    // Upper compare point
	// These values are also choosen past the compare points so we
	//   know if we are between the low and high value without a 
	//   compare interrupt we reversed direction
	static const uint16_t LOW_VALUE=COMP1_4;    // Closer to zero
	static const uint16_t HIGH_VALUE=COMP3_4;   // Closer to 64K

	// Bit select values
	static const uint32_t TOF=(1<<7);	// Bit 7 - Timer Overflow
	static const uint32_t TOFDIR=(1<<1);    // Bit 1
	static const uint32_t QUADDIR=(1<2);	// Bit 2
	static const uint32_t TOIE=(1<<6);	// Bit 6 - Int Enable

	// FTM module addresses set up as FTM1 or FTM 2 
	//   Then set up reference for ease of use
	volatile uint32_t * const pSC = 
	    reinterpret_cast<volatile uint32_t*>(0x400B8000);
	volatile uint32_t &FTM_SC=*pSC;	// Status And Control

	volatile uint32_t * const pCNT = 
	    reinterpret_cast<volatile uint32_t*>(0x400B8004);
	volatile uint32_t &FTM_CNT=*pCNT;	// Counter

	volatile uint32_t * const pMOD = 
	    reinterpret_cast<volatile uint32_t*>(0x400B8008);
	volatile uint32_t &FTM_MOD=*pMOD;	// Modulo

	volatile uint32_t * const pC0SC = 
	    reinterpret_cast<volatile uint32_t*>(0x400B800C);
	volatile uint32_t &FTM_C0SC=*pC0SC;	// Channel 0 Status/Control

	volatile uint32_t * const pC0V = 
	    reinterpret_cast<volatile uint32_t*>(0x400B8010);
	volatile uint32_t &FTM_C0V=*pC0V;	// Channel 0 Value

	volatile uint32_t * const pC1SC = 
	    reinterpret_cast<volatile uint32_t*>(0x400B8014);
	volatile uint32_t &FTM_C1SC=*pC1SC;	// Channel 1 Status/Control

	volatile uint32_t * const pCNTIN = 
	    reinterpret_cast<volatile uint32_t*>(0x400B804C);
	volatile uint32_t &FTM_CNTIN=*pCNTIN;   // Counter Initial Value

	volatile uint32_t * const pMODE = 
	    reinterpret_cast<volatile uint32_t*>(0x400B8054);
	volatile uint32_t &FTM_MODE=*pMODE;	// Features Mode Select

	volatile uint32_t * const pCOMBINE = 
	    reinterpret_cast<volatile uint32_t*>(0x400B8064);
	volatile uint32_t &FTM_COMBINE=*pCOMBINE;	// Linked Channels

	volatile uint32_t * const pFMS = 
	    reinterpret_cast<volatile uint32_t*>(0x400B8074);
	volatile uint32_t &FTM_FMS=*pFMS;	// Fault Mode Status

	volatile uint32_t * const pFILTER = 
	    reinterpret_cast<volatile uint32_t*>(0x400B8078);
	volatile uint32_t &FTM_FILTER=*pFILTER; // Input Filter Control

	volatile uint32_t * const pQDCTRL = 
	    reinterpret_cast<volatile uint32_t*>(0x400B8080);
	volatile uint32_t &FTM_QDCTRL=*pQDCTRL; // Quadrature Decode Control 

	// Variables
	volatile uint32_t v_read;   // Some variables have to be read before
				    //   they can be changed
	//volatile int32_t v_basePosn; //Total position from counter overflows
	//volatile int32_t v_prevPosn; //Total position from counter overflows
	volatile bool v_initZeroTOF;	// First time thru after zero, 
					//  TOFDIR not valid
	volatile bool v_prevTOFUp;	// Direction on last overflow
	volatile prevInt_t v_prevInt;	
	volatile uint8_t v_intStatus;	// For interrupt routine debugging


};

const uint16_t potAcc[5][50]=
{
/*1*/	{0,1,2,3,4,5,8,12,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,},
/*2*/	{0,1,2,3,4,5,8,12,17,23,2,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,},
/*3*/	{0,1,2,3,4,6,7,8,9,10,38,47,57,68,80,93,107,122,138,155,173,192,212,233,255,278,302,327,353,380,408,437,467,498,530,563,597,632,668,705,743,782,822,863,905,948},
/*4*/	{0,1,2,3,4,8,12,17,23,30,38,47,57,68,80,93,107,122,138,155,173,192,212,233,255,278,302,327,353,380,408,437,467,498,530,563,597,632,668,705,743,782,822,863,905,948},
/*5*/	{0,1,2,3,4},
};



extern cEncoder Encoder;

#endif
