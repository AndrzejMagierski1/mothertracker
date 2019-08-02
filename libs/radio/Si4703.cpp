#include <Si4703.h>
#include "Arduino.h"
#include "i2c_t3.h"
#include "SI4703_regdefs.h"
#include "RDSParser.h"
#include "mtHardware.h"


#ifdef HW_WITH_RADIO
Si4703 radio(SI4703_RST, 48, 47, SI4703_SEN,SI4703_GPIO_2);
#endif

Si4703::Si4703(int resetPin, int sdioPin, int sclkPin, int senPin, int interruptPin)
{
	_resetPin = resetPin;
	_sdioPin = sdioPin;
	_sclkPin = sclkPin;
	_senPin = senPin;
	_interruptPin = interruptPin;
}

void SI4703_interrupt()
{
	radio.rdsReadyFlag=1;
}

void Si4703::powerOn()
{
	si4703_init();
}

void Si4703::powerOff()
{
	/*Procesura zgodnie z AN230*/
	readRegisters();
	si4703_registers[rSYSCONFIG1] &= ~RDS;
	updateRegisters();

	readRegisters();
	si4703_registers[rPOWERCFG] |= DISABLE;
	updateRegisters();
}

void Si4703::setFrequency(float freq)
{
	int newChannel;

	newChannel= (float)((freq-87.5)/0.05) + 0.5; // 0.5 for ceil rounding

	readRegisters();
	si4703_registers[rCHANNEL] &= ~CHANN_MASK; //Clear out the channel bits
	si4703_registers[rCHANNEL] |= newChannel; //Mask in the new channel
	si4703_registers[rCHANNEL] |= TUNE; //Set the TUNE bit to start
	updateRegisters();

	while(1)
	{
		readRegisters();
		if((si4703_registers[rSTATUSRSSI] & STC) != 0)
		{
			break; //Tuning complete!
		}
	}

	readRegisters();
	si4703_registers[rCHANNEL] &= ~TUNE; //Clear the tune after a tune has completed
	updateRegisters();

	//Wait for the si4703 to clear the STC as well
	while(1)
	{
		readRegisters();
		if((si4703_registers[rSTATUSRSSI] & STC) == 0)
		{
			break; //Tuning complete!
		}
	}

}

float Si4703::seekUp()
{
	return seek(SEEK_UP);
}

float Si4703::seekDown()
{
	return seek(SEEK_DOWN);
}

void Si4703::setVolume(int volume)
{
	readRegisters();
	if(volume < 0) volume = 0;
	if (volume > 15) volume = 15;
	si4703_registers[rSYSCONFIG2] &= ~VOLUME_MASK; //Clear volume bits
	si4703_registers[rSYSCONFIG2] |= volume; //Set new volume
	updateRegisters();
}

void Si4703::update_RDS()
{
	if(radio.rdsReadyFlag)
	{
		readRegisters();
		if((si4703_registers[rSTATUSRSSI] & 0x0600)  == 0)
		{

			rds.processData(si4703_registers[rRDSA],si4703_registers[rRDSB],si4703_registers[rRDSC], si4703_registers[rRDSD]);
		}

		radio.rdsReadyFlag=0;
	}
}

//To get the Si4703 inito 2-Wire mode, SEN needs to be high and SDIO needs to be low after a reset
//The breakout board has SEN pulled high, but also has SDIO pulled high. Therefore, after a normal power up
//The Si4703 will be in an unknown state. RST must be controlled
void Si4703::si4703_init()
{
	pinMode(_resetPin, OUTPUT);
	pinMode(_sdioPin, OUTPUT); //SDIO is connected to A4 for I2C
	pinMode(_senPin,OUTPUT);
	digitalWrite(_sdioPin, LOW); //A low SDIO indicates a 2-Wire interface
	digitalWrite(_senPin,HIGH);
	digitalWrite(_resetPin, LOW); //Put Si4703 into reset
	delay(1); //Some delays while we allow pins to settle
	digitalWrite(_resetPin, HIGH); //Bring Si4703 out of reset with SDIO set to low and SEN pulled high with on-board resistor
	delay(1); //Allow Si4703 to come out of reset

	Wire.begin(I2C_MASTER, 0x00, I2C_PINS_47_48, I2C_PULLUP_EXT, 400000,I2C_OP_MODE_IMM);

	readRegisters(); //Read the current register set
	si4703_registers[rTEST1] = 0x8100; //Enable the oscillator, from AN230 page 9, rev 0.61 (works)
	updateRegisters(); //Update

	delay(500); //Wait for clock to settle - from AN230 page 9

	readRegisters(); //Read the current register set
	si4703_registers[rPOWERCFG] = (ENABLE | DMUTE | DSMUTE | RDSM);

	si4703_registers[rSYSCONFIG1] |= DE; //50kHz Europe setup
	si4703_registers[rSYSCONFIG2] |= SPACE(SPACING_50kHz); //50kHz channel spacing for Europe

	si4703_registers[rSYSCONFIG2] &= ~VOLUME_MASK; //Clear volume bits
	si4703_registers[rSYSCONFIG2] |= VOLUME(0x01); //Set volume to lowest
	si4703_registers[rSYSCONFIG2] |= SEEKTH(0x19);// Minimalne RSSI prawidlowej stacji dla seek
	si4703_registers[rSYSCONFIG3] |= (SKCNT(0x08) | SKSNR(0x04));// autoseek snr i impulse detection
	updateRegisters(); //Update

	delay(110); //Max powerup time, from datasheet page 13

	readRegisters();
	si4703_registers[rSYSCONFIG1] |= (RDS | STCIEN | GPIO2(0x01)); //Enable RDS
	updateRegisters();

	rds.init();

	pinMode(_interruptPin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(_interruptPin), SI4703_interrupt, FALLING);
}

//Read the entire register control set from 0x00 to 0x0F
void Si4703::readRegisters()
{
	//Si4703 begins reading from register upper register of 0x0A and reads to 0x0F, then loops to 0x00.
	Wire.requestFrom(SI4703_I2C_ADDR, 32); //We want to read the entire register set from 0x0A to 0x09 = 32 bytes.

	while(Wire.available() < 32) ;

	//Remember, register 0x0A comes in first so we have to shuffle the array around a bit
	for(int x = 0x0A ; ; x++)
	{
		if(x == 0x10) x = 0; //Loop back to zero
		si4703_registers[x] = Wire.read() << 8;
		si4703_registers[x] |= Wire.read();
		if(x == 0x09) break; //We're done!
	}
}

//Write the current 9 control registers (0x02 to 0x07) to the Si4703
//It's a little weird, you don't write an I2C addres
//The Si4703 assumes you are writing to 0x02 first, then increments
uint8_t Si4703::updateRegisters()
{
	//Tylko rejestry od 0x02  do 0x07 sa aktualizowane, reszta to readonly
	Wire.beginTransmission(SI4703_I2C_ADDR);

	for(int regSpot = 0x02 ; regSpot < 0x08 ; regSpot++)
	{
		uint8_t high_byte = si4703_registers[regSpot] >> 8;
		uint8_t low_byte = si4703_registers[regSpot] & 0x00FF;

		Wire.write(high_byte); //Upper 8 bits
		Wire.write(low_byte); //Lower 8 bits
	}

	if(Wire.endTransmission())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

//Seeks out the next available station
//Returns the freq if it made it
//Returns zero if failed
float Si4703::seek(byte seekDirection)
{
	int valueSFBL=0;

	readRegisters();
	//Set seek mode wrap bit
	//si4703_registers[rPOWERCFG] |= SKMODE; //Allow wrap
	si4703_registers[rPOWERCFG] &= ~SKMODE; //Disallow wrap - if you disallow wrap, you may want to tune to 87.5 first
	if(seekDirection == SEEK_DOWN)
	{
		si4703_registers[rPOWERCFG] &= ~SEEKUP; //Seek down is the default upon reset
	}
	else
	{
		si4703_registers[rPOWERCFG] |= SEEKUP; //Set the bit to seek up
	}

	si4703_registers[rPOWERCFG] |= SEEK; //Start seek
	updateRegisters(); //Seeking will now start

	//Poll to see if STC is set
	while(1)
	{
		readRegisters();
		if((si4703_registers[rSTATUSRSSI] & STC) != 0)
		{
			break; //Tuning complete!
		}
	}

	readRegisters();
	valueSFBL=si4703_registers[rSTATUSRSSI] & SF_BL; //Store the value of SFBL
	si4703_registers[rPOWERCFG] &= ~SEEK; //Clear the seek bit after seek has completed
	updateRegisters();

	//Wait for the si4703 to clear the STC as well
	while(1)
	{
		readRegisters();
		if((si4703_registers[rSTATUSRSSI] & STC) == 0)
		{
			break; //Tuning complete!
		}
	}

	if(valueSFBL)
	{ //The bit was set indicating we hit a band limit or failed to find a station
		return 0;
	}

	return getFrequency();
}

//Reads the current channel from READCHAN
//Returns a number like 973 for 97.3MHz
float Si4703::getFrequency()
{
	int channel;
	float freq;

	readRegisters();

	channel = READCHANN(si4703_registers[rREADCHAN]);

	freq= (channel * 0.05 + 87.5);

	return freq;
}

int Si4703::getRSSI()
{
	readRegisters();
	return RSSI(si4703_registers[rSTATUSRSSI]);
}
