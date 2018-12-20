
#include "mtInterface.h"


cMtInterface mtInterface;

keyScanner seqButtonsA,seqButtonsB,seqButtonsC;
mtLEDs leds;
mtLED grid;




void IO7326_INT_FUNCT_A() { seqButtonsA.intAction(); }
void IO7326_INT_FUNCT_B() { seqButtonsB.intAction(); }
void IO7326_INT_FUNCT_C() { seqButtonsC.intAction(); }

//=======================================================================
void cMtInterface::begin()
{

	////////////////// IO7326 A
	seqButtonsA.setButtonPushFunc(onButtonPush);
	seqButtonsA.setButtonReleaseFunc(onButtonRelease);
	seqButtonsA.setButtonHoldFunc(onButtonHold);
	seqButtonsA.setButtonDoubleFunc(onButtonDouble);
	seqButtonsA.setHoldTime(200);
	seqButtonsA.setDoubleTime(300);
	////////////////// IO7326 B
	seqButtonsB.setButtonPushFunc(onButtonPush);
	seqButtonsB.setButtonReleaseFunc(onButtonRelease);
	seqButtonsB.setButtonHoldFunc(onButtonHold);
	seqButtonsB.setButtonDoubleFunc(onButtonDouble);
	seqButtonsB.setHoldTime(200);
	seqButtonsB.setDoubleTime(300);
	////////////////// IO7326 C
	seqButtonsC.setButtonPushFunc(onButtonPush);
	seqButtonsC.setButtonReleaseFunc(onButtonRelease);
	seqButtonsC.setButtonHoldFunc(onButtonHold);
	seqButtonsC.setButtonDoubleFunc(onButtonDouble);
	seqButtonsC.setHoldTime(200);
	seqButtonsC.setDoubleTime(300);


	////////////////// IO7326 A
	seqButtonsA.begin(IO7326_ADDR1,I2C_SDA,I2C_SCL,GRID_A,IO7326_INT_FUNCT_A);
	////////////////// IO7326 B
	seqButtonsB.begin(IO7326_ADDR3,I2C_SDA,I2C_SCL,GRID_B,IO7326_INT_FUNCT_B);
	////////////////// IO7326 C
	seqButtonsC.begin(IO7326_ADDR2,I2C_SDA,I2C_SCL,GRID_C,IO7326_INT_FUNCT_C);

/*	leds.begin();
	leds.setAllLEDPWM(leds.ledPWMseq, 0);*/

	grid.begin(IS31FL3731_ADDR1);
	leds.setAllLEDPWM(grid.ledPWM, 0);

/*	for(int i=1;i<=128;i++)
	{
		for(int j=1;j<=8;j++)
		{
			leds.setLEDseq(j,i,1,5);
			delay(500);
			leds.setLEDseq(j,i,0,5);
		}
	}
	leds.update();*/
	for(int i=0;i<127;i++)
	{
		grid.setLED(i,1,5);
		grid.update_all_leds();
		delay(100);
		grid.setLED(i,0,5);
		grid.update_all_leds();
	}
}


//=======================================================================
void cMtInterface::update()
{
	if(Wire2.done())
		{
			// odczyt przyciskow
			if(seqButtonsA.update())
			{
				return;
			}
			if(seqButtonsB.update())
			{
				return;
			}
			if(seqButtonsC.update())
			{
				return;
			}
//			leds.update();
			grid.update_all_leds();
		}
}
