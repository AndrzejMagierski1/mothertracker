#ifndef SparkFunSi4703_h
#define SparkFunSi4703_h

#include "Arduino.h"
#include "RDSParser.h"

#define SI4703_I2C_ADDR		0x10

#define SEEK_UP				1
#define SEEK_DOWN			0

#define SPACING_200kHz		0
#define SPACING_100kHz		1
#define SPACING_50kHz		2




class Si4703
{
public:
	Si4703(int resetPin, int sdioPin, int sclkPin,int senPin, int interruptPin);
	void powerOn();					// call in setup
	void powerOff();
	float seekUp(); 					// returns the tuned channel or 0
	float seekDown();
	void setVolume(int volume); 	// 0 to 15

	void setFrequency(float freq);
	void update_RDS();
	float getFrequency();
	int getRSSI();
	uint8_t isSTCset();

	uint8_t rdsReadyFlag;

	RDSParser rds;

private:
	int  _resetPin;
	int  _sdioPin;
	int  _sclkPin;
	int _senPin;
	int _interruptPin;

	void si4703_init();
	void readRegisters();
	byte updateRegisters();
	float seek(byte seekDirection);


	uint16_t si4703_registers[16]; //There are 16 registers, each 16 bits large

};

extern Si4703 radio;

#endif
