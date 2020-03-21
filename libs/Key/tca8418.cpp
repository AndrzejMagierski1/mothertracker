/*
 * Arduino Library for TCA8418 I2C keyboard
 *
 * Copyright (C) 2015 RF Designs. All rights reserved.
 *
 * Author: Bob Frady <rfdesigns@live.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License v2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 021110-1307, USA.
 *
 * If you can't comply with GPLv2, alternative licensing terms may be
 * arranged. Please contact RF Designs <rfdesigns@live.com> for proprietary
 * alternative licensing inquiries.
 */

#include <wire.h>
#include "mtHardware.h"
#include <tca8418.h>
#include "debugLog.h"

#ifdef TCA8418_INTERRUPT_SUPPORT
#endif


extern KEYS Keypad;

KEYS::KEYS()
{
	_address = 0;
	_oldPIN = 0;
	_isrIgnore = 0;
	_pcintPin = 0;

//	 _intMode(),
//	_intCallback()


}


void KEYS::begin(uint8_t rows, uint16_t cols, uint8_t config, i2c_t3 * i2c_wire, uint8_t* table)
{
	convertTable = table;
	localWire = i2c_wire;
	_address = 0x34;
	// begin poza
	//localWire->begin(I2C_MASTER, 0x00, I2C_SCL, I2C_SDA, I2C_PULLUP_EXT, 400000);
	configureKeys(rows, cols, config);
}

uint8_t KEYS::readKeypad(void)
{
	return (getKeyEvent());
}

/*
 * Configure the TCA8418 for keypad operation
 */
void KEYS::configureKeys(uint8_t rows, uint16_t cols, uint8_t config)
{
	//Pins all default to GPIO. pinMode(x, KEYPAD); may be used for individual pins.
	writeByte(rows, REG_KP_GPIO1);

	uint8_t col_tmp;
	col_tmp = (uint8_t) (0xff & cols);
	writeByte(col_tmp, REG_KP_GPIO2);
	col_tmp = (uint8_t) (0x03 & (cols >> 8));
	writeByte(col_tmp, REG_KP_GPIO3);

	config |= CFG_AI;
	currentConfig = config;
	writeByte(config, REG_CFG);

	clearInterruptStatus();


}

void KEYS::writeByte(uint8_t data, uint8_t reg)
{
	I2C_BEGINTRANS(_address);
	I2C_WRITE((uint8_t ) reg);

	I2C_WRITE((uint8_t ) data);
	uint8_t error = I2C_ENDTRANS();

	if(error)
	{
		debugLog.addLine("i2c Write error:  ");
		debugLog.addValue(error);
		refreshConfig();
	}
	return;
}

bool KEYS::readByte(uint8_t *data, uint8_t reg)
{
	I2C_BEGINTRANS(_address);
	I2C_WRITE((uint8_t ) reg);
	uint8_t error = I2C_ENDTRANS();

	if(error)
	{
		debugLog.addLine("i2c write in read error");
		debugLog.addValue(error);
		refreshConfig();
	}

	uint8_t timeout = 0;

	uint8_t error_req = I2C_REQUESTFROM(_address, (uint8_t) 0x01);
	if(!error_req)
	{
		debugLog.addLine("i2c error request");
		refreshConfig();
	}
	while (I2C_AVALIBLE() < 1)
	{
		timeout++;
		if (timeout > I2CTIMEOUT)
		{
			*data = 0;
			return (true);
		}
		delay(1);
	} 			// Experimental

	*data = I2C_READ();

	return (false);
}

void KEYS::write3Bytes(uint32_t data, uint8_t reg)
{

	union
	{
		uint8_t b[4];
		uint32_t w;
	} datau;

	datau.w = data;

	I2C_BEGINTRANS(_address);
	I2C_WRITE((uint8_t ) reg);

	I2C_WRITE((uint8_t ) datau.b[0]);
	I2C_WRITE((uint8_t ) datau.b[1]);
	I2C_WRITE((uint8_t ) datau.b[2]);

	I2C_ENDTRANS();
	return;
}

bool KEYS::read3Bytes(uint32_t *data, uint8_t reg)
{

	union
	{
		uint8_t b[4];
		uint32_t w;
	} datau;

	datau.w = *data;

	I2C_BEGINTRANS(_address);
	I2C_WRITE((uint8_t ) reg);
	I2C_ENDTRANS();
	uint8_t timeout = 0;

	I2C_REQUESTFROM(_address, (uint8_t) 0x03);
	while (I2C_AVALIBLE() < 3)
	{
		timeout++;
		if (timeout > I2CTIMEOUT)
		{
			return (true);
		}
		delay(1);
	} 		//Experimental

	datau.b[0] = I2C_READ();
	datau.b[1] = I2C_READ();
	datau.b[2] = I2C_READ();

	*data = datau.w;

	return (false);
}

void KEYS::pinMode(uint32_t pin, uint8_t mode)
{
	uint32_t dbc;

	readGPIO();
	switch (mode)
	{
	case INPUT:
		bitClear(_PORT, pin);
		bitClear(_DDR, pin);
		bitClear(_PKG, pin);
		bitSet(_PUR, pin);
		break;
	case INPUT_PULLUP:
		bitClear(_PORT, pin);
		bitClear(_DDR, pin);
		bitClear(_PKG, pin);
		bitClear(_PUR, pin);
		break;
	case OUTPUT:
		bitClear(_PORT, pin);
		bitSet(_DDR, pin);
		bitClear(_PKG, pin);
		bitSet(_PUR, pin);
		break;
	case KEYPAD:
		bitClear(_PORT, pin);
		bitClear(_DDR, pin);
		bitSet(_PKG, pin);
		bitClear(_PUR, pin);
		break;
	case DEBOUNCE:
		read3Bytes((uint32_t *) &dbc, REG_DEBOUNCE_DIS1);
		bitClear(dbc, pin);
		write3Bytes((uint32_t) dbc, REG_DEBOUNCE_DIS1);
		break;
	case NODEBOUNCE:
		read3Bytes((uint32_t *) &dbc, REG_DEBOUNCE_DIS1);
		bitSet(dbc, pin);
		write3Bytes((uint32_t) dbc, REG_DEBOUNCE_DIS1);
		break;
	default:
		break;
	}
	updateGPIO();
}

void KEYS::digitalWrite(uint32_t pin, uint8_t value)
{

	if (value)
	bitSet(_PORT, pin);
	else
		bitClear(_PORT, pin);

	updateGPIO();
}

uint8_t KEYS::digitalRead(uint32_t pin)
{

	readGPIO();

	return (_PIN & bit(pin)) ? HIGH : LOW;
}

void KEYS::write(uint32_t value)
{

	_PORT = value;

	updateGPIO();
}

uint32_t KEYS::read(void)
{

	readGPIO();

	return _PORT;
}

void KEYS::toggle(uint32_t pin)
{

	_PORT ^= (bit(pin));

	updateGPIO();
}

void KEYS::blink(uint32_t pin, uint16_t count, uint32_t duration)
{

	duration /= count * 2;

	while (count--)
	{
		toggle(pin);
		delay(duration);
		toggle(pin);
		delay(duration);
	}
}

#ifdef TCA8418_INTERRUPT_SUPPORT
void KEYS::enableInterrupt(uint8_t pin, void (*selfCheckFunction)(void))
{

	_pcintPin = pin;

#if ARDUINO >= 100
	::pinMode(pin, INPUT_PULLUP);
#else
	::pinMode(pin, INPUT);
	::digitalWrite(pin, HIGH);
#endif
	attachInterrupt(pin, selfCheckFunction, FALLING);
}

void KEYS::disableInterrupt()
{
	detachInterrupt(_pcintPin);
}

void KEYS::pinInterruptMode(uint32_t pin, uint8_t mode, uint8_t level,
							uint8_t fifo)
{
	uint32_t intSetting, levelSetting, eventmodeSetting;

	read3Bytes((uint32_t *) &intSetting, REG_GPIO_INT_EN1);
	read3Bytes((uint32_t *) &levelSetting, REG_GPIO_INT_LVL1);
	read3Bytes((uint32_t *) &eventmodeSetting, REG_GPI_EM1);

	switch (mode)
	{
	case INTERRUPT:
		bitSet(intSetting, pin);
		break;
	case NOINTERRUPT:
		bitClear(intSetting, pin);
		break;
	default:
		break;
	}

	switch (level)
	{
	case LOW:
		bitClear(levelSetting, pin);
		break;
	case HIGH:
		bitSet(levelSetting, pin);
		break;
	default:
		break;
	}

	switch (fifo)
	{
	case FIFO:
		bitSet(eventmodeSetting, pin);
		break;
	case NOFIFO:
		bitClear(eventmodeSetting, pin);
		break;
	default:
		break;
	}

	write3Bytes((uint32_t) intSetting, REG_GPIO_INT_EN1);
	write3Bytes((uint32_t) levelSetting, REG_GPIO_INT_LVL1);
	write3Bytes((uint32_t) eventmodeSetting, REG_GPI_EM1);

}

void KEYS::pinInterruptMode(uint32_t pin, uint8_t mode)
{
	pinInterruptMode(pin, mode, 0, 0);
}

#endif

void KEYS::readGPIO()
{

#ifdef TCA8418_INTERRUPT_SUPPORT
	/* Store old _PIN value */
	_oldPIN = _PIN;
#endif

	read3Bytes((uint32_t *) &_PORT, REG_GPIO_DAT_OUT1); //Read Data OUT Registers
	read3Bytes((uint32_t *) &_PIN, REG_GPIO_DAT_STAT1);	//Read Data STATUS Registers
	read3Bytes((uint32_t *) &_DDR, REG_GPIO_DIR1);//Read Data DIRECTION Registers
	read3Bytes((uint32_t *) &_PKG, REG_KP_GPIO1);//Read Keypad/GPIO SELECTION Registers
	read3Bytes((uint32_t *) &_PUR, REG_GPIO_PULL1);	//Read KPull-Up RESISTOR Registers
}

void KEYS::updateGPIO()
{

	write3Bytes((uint32_t) _PORT, REG_GPIO_DAT_OUT1); //Write Data OUT Registers
	write3Bytes((uint32_t) _DDR, REG_GPIO_DIR1);//Write Data DIRECTION Registers
	write3Bytes((uint32_t) _PKG, REG_KP_GPIO1);	//Write Keypad/GPIO SELECTION Registers
	write3Bytes((uint32_t) _PUR, REG_GPIO_PULL1);//Write Pull-Up RESISTOR Registers
}

void KEYS::dumpreg(void)
{
	uint8_t data;
	for (int x = 0x01; x < 0x2F; x++)
	{
		readByte(&data, x);
		Serial.print(x, HEX);
		Serial.print(":");
		Serial.print(data, HEX);
		Serial.print(" ");
	}
	Serial.print("\n");
}

uint8_t KEYS::getInterruptStatus(void)
{
	uint8_t status;

	readByte(&status, REG_INT_STAT);
	return (status & 0x0F);
}

void KEYS::clearInterruptStatus(uint8_t flags)
{

	flags &= 0x0F;
	writeByte(flags, REG_INT_STAT);
}

void KEYS::clearInterruptStatus(void)
{
	clearInterruptStatus(0x0F);
}

uint8_t KEYS::getKeyEvent(uint8_t event)
{
	uint8_t keycode;

	if (event > 9)
		return 0x00;

	readByte(&keycode, (REG_KEY_EVENT_A + event));
	return (keycode);
}

uint8_t KEYS::getKeyEvent(void)
{
	return (getKeyEvent(0));
}

uint8_t KEYS::getKeyEventCount(void)
{
	uint8_t count;

	readByte(&count, REG_KEY_LCK_EC);
	return (count & 0x0F);
}

uint32_t KEYS::getGPIOInterrupt(void)
{
	uint32_t Ints;

	union
	{
		uint32_t val;
		uint8_t arr[4];
	} IntU;

	readByte(&IntU.arr[2], REG_GPIO_INT_STAT3);
	readByte(&IntU.arr[1], REG_GPIO_INT_STAT2);
	readByte(&IntU.arr[0], REG_GPIO_INT_STAT1);

	Ints = IntU.val;
	return (Ints);
}

bool KEYS::isKeyDown(uint8_t key)
{
	if (key & 0x80)
	return true;
	else
		return false;
}

bool KEYS::getKey(uint8_t *key)
{
	uint8_t tmpkey;

	tmpkey = readKeypad();
	*key = *key & 0x7F;

	return (isKeyDown(tmpkey));
}

uint8_t KEYS::getKey(void)
{
	return (readKeypad() & 0x7F);
}

uint8_t KEYS::update()
{
	uint8_t result = 0;
	//Check for Interrupt flag and process
	if (keyInt)
	{
		result = 1;
		uint8_t eventCounter = getKeyEventCount();

		for(uint8_t i = 0 ; i < eventCounter; i++ )
		{
			uint8_t key = readKeypad(); //Get first keycode from FIFO;
			uint8_t keyNumber = key & 0x7F;
			uint8_t keyState = key & 0x80 ;

			uint8_t button = convertTable[keyNumber-1];

			if (keyState)
			{
				onPush(button);
				buttonPush[button] = 1;
				holdTim[button] = 0;
				holdFactor[button] = 0;
			}
			else
			{
				onRelease(button);
				buttonPush[button] = 0;
			}
		}

		keyInt = 0; //Reset Our Interrupt flag
		clearInterruptStatus(); //Reset TCA8418 Interrupt Status Register.
	}

	for(uint8_t i = 0; i < BUTTON_MAX ; i++)
	{
		if(buttonPush[i] && (holdTim[i] > HOLD_TIME))
		{
			if(holdFactor[i] < holdTimeStepsCount-1) holdFactor[i]++;

			holdTim[i] = holdTimeStep[holdFactor[i]];
			onHold(i);
		}
	}


	//Do other processing
	return result;
}

void KEYS::setOnPush(void(*funct)(uint8_t))
{
	onPush = funct;
}
void KEYS::setOnRelease(void(*funct)(uint8_t))
{
	onRelease = funct;
}
void KEYS::setOnHold(void(*funct)(uint8_t))
{
	onHold = funct;
}
uint8_t KEYS::isButtonPressed(uint8_t n)
{
	return buttonPush[n];
}

void KEYS::refreshConfig()
{
	if(this == &Keypad)
	{
		localWire->begin(I2C_MASTER, 0x00, GRID_I2C_SCL, GRID_I2C_SDA, I2C_PULLUP_EXT, 400000);
		writeByte(0, REG_CFG);
		writeByte(currentConfig, REG_CFG);
	}

}


