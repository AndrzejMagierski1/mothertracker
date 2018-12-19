
#include "mtInterface.h"


cMtInterface mtInterface;

keyScanner seqButtonsA,seqButtonsB,seqButtonsC;





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
		}
}
