

#include "mk20dx128.h"

#include <inttypes.h>

#include <elapsedMillis.h>
#include <Encoder.h>



elapsedMillis resetTimer;
uint8_t stopped = 0;
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


	cEncoder Encoder;


void cEncoder::begin(uint8_t intPin, void (*func)(uint8_t n, uint8_t value))
{

//===================  SETUP ==============================
    // Set up input pins
// FTM2
	// K20 pin 41,42
	// Bit 8-10 is Alt Assignment
	PORTA_PCR10 = 0x00000612;   //Alt6-QD_FTM2,FilterEnable,Pulldown
	PORTA_PCR11 = 0x00000612;   //Alt6-QD_FTM2,FilterEnable,Pulldown
	//Default is input, don't need to write PDDR (Direction)
	//PORT_PCR_MUX(6);

    //Set FTMEN to be able to write registers
    FTM_MODE=0x04;	    // Write protect disable - reset value
    FTM_MODE=0x05;	    // Set FTM Enable

    // Set registers written in pins_teensy.c back to default
    FTM_CNT = 0;
    FTM_MOD = 0;
    FTM_C0SC =0;
    FTM_C1SC =0;
    FTM_SC = 0;

    // Set registers to count quadrature
    FTM_FILTER=0x22;	// 2x4 clock filters on both channels
    FTM_CNTIN=0;
    FTM_MOD=0xFFFF;	// Maximum value of counter
    FTM_CNT=0;		// Updates counter with CNTIN

    // Set Registers for output compare mode
    FTM_COMBINE=0;	    // Reset value, make sure
    FTM_C0SC=0x10;	    // Bit 4 Channel Mode
    FTM_C0V= COMP_LOW;	    // Initial Compare Interrupt Value

    FTM_QDCTRL=0b11000001;	    // Quadrature control
    //        Filter enabled, QUADEN set
	    
    // Write Protect Enable
    FTM_FMS=0x40;		// Write Protect, WPDIS=1

//===================  START ==============================
	//Enable interrupts and start counting

    zeroFTM();	// Zero FTM counter

    // Enable global FTMx interrupt
    // Assume global interrupts already enabled 
    //   No individual FTM sources enabled yet

//	NVIC_ENABLE_IRQ(IRQ_FTM2);

//===================  ENCODER SWITCH =====================
	 
//	attachInterrupt(intPin, func, FALLING);

    pinMode(intPin, INPUT);
    switchFunc = func;
    switchPin = intPin;

//===================  RESET  =============================
	//zeroFTM();

};


void cEncoder::setRes(uint8_t step_res)
{
	resolution = step_res; 
	//zeroFTM();
}

void cEncoder::setSpeedUp(uint8_t speed)
{
	speed_up = speed; 
	//zeroFTM();
}

int32_t cEncoder::read()
{

//trzeba odejmowac wielkeorotnosc aktualnej 
//resolution od licznika enkoderra zamiast zerowac
	int16_t raw_delta = calcPosn();
	float temp_resolution = 1;
	int16_t positiveValue = 0;


	// przy dluzszym brraku krecenia zeruje licznik encodera
	if(raw_delta == 0)
	{
		if(!stopped)
		{
			if(difference != 0)
			{
				resetTimer = 0;
				stopped = 1;
			}
		}
		else if(resetTimer > 500)  /// tu czas
		{
			difference = 0;
			stopped = 0;
		}
	}
	else
	{
		stopped = 0;
	}

	//if(temp_resolution == 0) temp_resolution = resolution;
	raw_delta = raw_delta + difference;
	difference = raw_delta % resolution;

/*	if(positiveValue > 0)
	{
		Serial.print(positiveValue);
		Serial.print("  ");
		Serial.print(temp_resolution);
		Serial.print("  ");
		Serial.println(raw_delta / temp_resolution);
	}
*/


	if(speed_up)
	{
		positiveValue = (raw_delta >= 0 ? raw_delta : raw_delta*(-1));

		if(positiveValue > 25*5)
		{
			temp_resolution = resolution / 6.0;
		}
		else if(positiveValue > 20*5)
		{
			temp_resolution = resolution / 5.0;
		}
		else if(positiveValue > 16*5)
		{
			temp_resolution = resolution / 4.0;
		}
		else if(positiveValue > 12*5)
		{
			temp_resolution = resolution / 3.0;
		}
		else if(positiveValue > 8*5)
		{
			temp_resolution = resolution / 2.0;
		}
		else if(positiveValue > 6*5)
		{
			temp_resolution = resolution / 1.5;
		}
		else
		{
			temp_resolution = resolution;
		}
	}





	return raw_delta / temp_resolution;
}


void cEncoder::testMode(uint8_t state)
{
	test_mode = state;
}

//--------------------------------------------------------------------------

// Initializies FTM for initialization or zero
//   Turn off counting, then no interrupts should be generated
//     and no Overflow bits set
void cEncoder::zeroFTM(void)
{ 

    // Turn off counter to disable interrupts and clear any 
    //   overflow and compare bits

    v_read=FTM_FMS;	// Requires reading before WPDIS can be set

//	noInterrupts(); 
    FTM_MODE=0x05;	// Write Protect Disable with FTMEN set
    FTM_QDCTRL=0xC0;	// Disable QUADEN, filter still enabled
                        //   Turns off counter so no interrupts
// 	interrupts();

    FTM_CNT=0;
    v_read=FTM_SC;	// Need to read Overflow before it can be set
    FTM_SC=0;		// Disable TOF interrupt, clear Overflow bit
    v_read=FTM_C0SC;	// Need to read Overflow before it can be set
    FTM_C0SC=0x10;	// Disable Channel compare int, clear compare bit
			//   Leaves Mode bit for compare set

    
    //v_basePosn=0;
    v_initZeroTOF=true;	// Special case to determine direction 

    //v_prevInt=prevIntTOF;   // Value not used, remove code
    if (FTM_QDCTRL & QUADDIR)
    {	//QuadDir says counting up
		FTM_C0V=COMP_LOW;	// Compare Interrupt Value
    } 
    else 
    {			//   according to last counting value
		FTM_C0V=COMP_HIGH;
    }

    // Enable counter again
    FTM_QDCTRL=0xC1;	// QUADEN
    FTM_FMS=0x40;  	// Write Protect, WPDIS=1
   
    //  Set channel interrupt
    FTM_C0SC=0x50;     // Enable Channel interrupt and Mode 

}
//--------------------------------------------------------------------------

// Calculates current position from Base Position and counter
//  Need to make sure get valid reading without counter values changing
//  Also need to know whether to add or subtract value (depends on dir)
//
//  Cases to look at
//   TOIE (TOF Interrupt Enable) disabled
//     We have not passed a compare interrupt since we hit TOF
//     Add or subtract counter value depending on whether closer to 0 or 64K
//     If far enough away from TOF, use TOFDIR to determine direction
//       and whether to add or subtract. 
//       If no TOF since zero and TOFDIR not valid,
//         need to determine direction from TOF
//   TOIE enabled
//     Direction (prevTOFUp) should have been set in Compare Interrupt
//     Use to determine add or subtract
//     Re-check TOIE
//       If it is set, interrupt just occured, just crossed zero 
//       Re-read basePosition, total position is basePosition
//	


int32_t cEncoder::calcPosn(void)
{
    uint16_t count16;	// 16 bit position counter
   // uint32_t count32;
    //bool cpTOFDirUp;	// TOFDIR in QDCTRL (bit 1)
    //int32_t cpBasePosn;
    //bool dataConsistent;

   // noInterrupts();	    // Variables can change if FTM interrrupt occurs

    if (!(FTM_SC & TOIE))
    {  // Check TOIE - TOF Interrupt disabled
		count16 = FTM_CNT;    // Read current counter 
		FTM_CNT = 0;
		//count32 = count16;
		//Serial.print(v_basePosn);Serial.print("   -  "); Serial.println(count32);
		// TOF Interrupt disabled so no change to basePosn

		if (count16 < LOW_VALUE)
		{
			//w lewo
		    return /*currentPosn = v_basePosn + */count16;

		}
		if (count16 > HIGH_VALUE)
		{
			//w prawo
			return /*currentPosn = v_basePosn + */count16 - NUM_64K;
		}





        // Away from TOF, add or subtract depending on DIR last crossed TOF 
/*		if (v_initZeroTOF)
		{	// No TOF since zero
	    	if (FTM_SC & TOF)
	    	{  //TOF Set
				cpTOFDirUp = FTM_QDCTRL & TOFDIR;	// Away from TOF so stable
	    	} 
	    	else 
	    	{
				cpTOFDirUp = true;	// Reversal will cause TOF
						//   so counting up
	    	}
		}
		else
		{
		    cpTOFDirUp = FTM_QDCTRL & TOFDIR; // Away from TOF so stable
		}

		// Use last TOFDIR to determine direction 
		if (cpTOFDirUp)
		{
		    currentPosn = v_basePosn + count32;
		    interrupts();   // Turn interrupts back on before return
		     Serial.println("enc3");
		    return currentPosn;
		}; 
		if (!cpTOFDirUp)
		{
		    currentPosn = v_basePosn + count32 - NUM_64K;
		    interrupts();   // Turn interrupts back on before return
		     Serial.println("enc4");
		    return currentPosn;
		};

*/

    }
/*
    // TOF Interrupt enabled
    interrupts();   // Allow TOF interrupt if pending
    count16 = FTM_CNT;    // Read current counter 
    count32 = count16;
    cpBasePosn = v_basePosn;  // Unchanging copy
    dataConsistent = (FTM_SC & TOIE);  // Still TOIE, no TOF
    // prevTOFUp set in Compare int that turns on TOIE
    //   Compare Int could occur but prevTOFUp should remain the same
    if (dataConsistent)
    {
		// v_prevTOFUp changes in compare int, should be stable
		// Need to go thru compare int to set TOIE after zero
		if (v_prevTOFUp)
		{	// Counting up
		    currentPosn=cpBasePosn + count32;
		} 
		else 
		{
		    currentPosn=cpBasePosn + count32 - NUM_64K;
		}
		return currentPosn;
    } 
    else 
    {	// Just crossed over TOF
		return v_basePosn;  // Position is new basePosition, no addtl counts
    }
*/
    return 0;
}

//--------------------------------------------------------------------------


/*  Interrupt from timer overflow and position compare
 *  These interrupts are used to determine basePosn, the total accumulated
 *    position
 *  The interrupts occur at over/underflow (TOF) or compare interrupts
 *    at two points through 64K count.  This determines direction and also
 *    certain tasks are not done until we know we are far enough away from
 *    TOF position so we won't interrupt during the task.
 *  To generate base position
 *     Interrupt on Timer Overflow (TOV)
 *       Look at current and previous overflow direction
 *       If overflow in same direction, add or subtract counter value (64K)
 *       If overflow in opposite direction, reversed direction
 *         BasePosition count remains the same
 *       Set Interrupt to Compare at low or high value, 
 *         depending on direction
 *       Disable Overflow interrupt.  Don't look at Overflow interrupt
 *         until pass Compare values.  Just oscillating and doesn't affect
 *         Base Position
 *       Leave Overflow bit set 
 *         Will be cleared during Compare Interrupt
 *     Interrupt on Compare
 *       Save last overflow direction to know which way were going
 *       Clear overflow bit 
 *         Far enough away so no oscillation
 *         Need to interrupt again when reach overflow point
 *       Set interrupt to overflow 
 *  The previous overflow direction is set by reading TOFDIR during
 *    the Compare interrupts. This is a bit set indicating the direction
 *    we crossed TOF last.  After zeroing the position, this bit is not
 *    set until we overflow/underflow.   
 */

/*
void MedusaEncoder::ftm_isr(void)
{
    Serial.println("ftm interrupt");


    //First determine which interrupt enabled
	if(FTM_SC == (TOF|TOIE)) // TOIE and TOF set, TOF interrupt
	{	
		v_intStatus=1;

		v_initZeroTOF=false;	// TOF occured, TOFDIR valid

		// If previous direction and current same add or subtract 64K
		//   from base position
		// Otherwise reversed in mid-count, base position stays same
		FTM_SC = 0;	    // Clear TOIE, TOF don't care 
		FTM_C0SC= 0x50;	    // Set CHIE Channel Interrupt Enable
		
		if (v_prevInt==prevIntCompHigh)
		{	// Counting up
		    v_intStatus=110;
		    FTM_C0V=COMP_LOW;	    // Set compare to closest value
			// SYNCEN in FTM1_COMBINE 0, update on next counter edge 
		    
		    if (v_prevTOFUp)
		    {
				v_intStatus=111;
				v_basePosn+=NUM_64K;    // Up for one counter cycle
			}
		} 
		else 
		{    // prevIntCompLow, counting down
		    v_intStatus=100;
		    FTM_C0V=COMP_HIGH;	// First expected compare counting down
		    
		    if (!v_prevTOFUp)
		    {
				v_intStatus=101;
				v_basePosn-=NUM_64K;
		    }
		}   // Previous and current direction not the same
		    //   indicate reversal, basePosn remains same
	}
	else 
	{ // Channel Compare Int
		v_intStatus=2;
		
		if (v_initZeroTOF)
		{	// No TOF since zero
		    if (FTM_SC & TOF)
		    {  //TOF Set
				v_initZeroTOF=false;	// TOF occured, TOFDIR valid
		    } 
		    else 
		    {
				v_prevTOFUp = true;	// Reversal will cause TOF
						//   so counting up
		    }
		}
		
		if (!v_initZeroTOF)
		{	// TOFDIR valid
		    v_prevTOFUp=FTM_QDCTRL & TOFDIR;	// Dir last crossed TOF
		}

		FTM_SC = TOIE;	// Should have been read in if statement
				// Can clear TOF, set TOIE 
		v_read=FTM_C0SC;   // Read to clear CHF Channel Flag
		FTM_C0SC= 0x50;    // Clear Channel Flag, leave CHIE set
		
		if (FTM_C0V==COMP_LOW)
		{
		    v_intStatus=30;
		    v_prevInt=prevIntCompLow;
		    FTM_C0V=COMP_HIGH;	// Set to other compare value
		} 
		else 
		{    // Compare at high value
		    v_intStatus=40;
		    v_prevInt=prevIntCompHigh;
		    FTM_C0V=COMP_LOW;
		}	

	}
}


*/
//--------------------------------------------------------------------------


void cEncoder::switchRead()
{
	if(digitalRead(switchPin) == LOW)
	{
		if(powerSwitchDebounce <= 0 && powerSwitchDebounce > (0-10) )
		{
			powerSwitchDebounce--;
		}
		else if(powerSwitchDebounce <= 0 && powerSwitchLastState != 1)
		{
			powerSwitchDebounce = 1;
			powerSwitchLastState = 1;
			//powerSwitchHoldTimer = 0;
			// press
			switchFunc(33,1);
		}
		/*
		else if(powerSwitchHoldTimer > 800 &&  powerSwitchLastState != 2)
		{
			powerSwitchLastState = 2;
			// hold
			switchFunc(33,2);
		}
		*/
	}
	else
	{
		if(powerSwitchLastState > 0 && powerSwitchDebounce < 10)
		{
			powerSwitchDebounce++;
		}
		else if(powerSwitchDebounce > 0 && powerSwitchLastState != 0)
		{
			powerSwitchDebounce = 0;
			powerSwitchLastState = 0;
			//buttons[i].hold_time = 0;
			// release
			switchFunc(33,0);
		}


	}
}
