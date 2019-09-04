#ifndef SparkFunSi4703_h
#define SparkFunSi4703_h

#include "Arduino.h"
#include "RDSParser.h"

#define SI4703_I2C_ADDR			0x10
#define RADIO_I2C_TIMEOUT_MS 	50

#define SEEK_UP					1
#define SEEK_DOWN				0

#define SPACING_200kHz			0
#define SPACING_100kHz			1
#define SPACING_50kHz			2

typedef struct
{
	char nazwaStacji[30];
	char textStacji[64];
	uint8_t godzina;
	uint8_t minuta;

}rds_data_t;

typedef enum
{
	notInitialized,
	seekInit,
	loopUntilSTC,
	stationFound,
	seekDeinit,
	callback,

}seek_state_t;

typedef struct
{
	seek_state_t seek_state;
	uint8_t seek_dir;

}seek_control_t;

typedef void (*user_callback_t)(void);


class Si4703
{
public:
	Si4703(int resetPin, int sdioPin, int sclkPin, int interruptPin);
	void powerOn();					// call in setup
	void powerOff();
	void seekUp(); 					// returns the tuned channel or 0
	void seekDown();
	void setVolume(int volume); 	// 0 to 15

	void setFrequency(float freq);
	bool update_RDS();
	float getFrequency();
	int getRSSI();
	uint8_t isSTCset();

	void stateMachineSeek();


	void setSeekCallback(user_callback_t callback);
	void resetSeekCallback();

	void clearRDS();

	uint8_t getInitializationStatus();

	uint8_t rdsReadyFlag;

	RDSParser rds;
	rds_data_t rds_data;

private:
	int  _resetPin;
	int  _sdioPin;
	int  _sclkPin;
	int _interruptPin;

	void si4703_init();
	void readRegisters();
	byte updateRegisters();
	float seek(byte seekDirection);
	void initSeek(uint8_t seekDirection);

	uint8_t isInitializedProperly=1;
	uint8_t firstReadFlag=0;
	uint16_t si4703_registers[16]; //There are 16 registers, each 16 bits large



};

extern Si4703 radio;


#endif
