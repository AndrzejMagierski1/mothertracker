

#include <stdint.h>
#include <i2c_t3.h>
//#include <Arduino.h>
//#include <Adafruit_GFX.h>  
//#include <i2c_t3.h>
 
#include "mtLED.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }            
#endif

uint8_t sweep[256] = {1, 5, 10, 15, 25, 35, 45, 70, 95, 145, 195, 255, 255, 195, 145, 95, 70, 45, 35, 25, 15, 10, 5, 1};

const uint8_t gamma255[256] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255

};

//mtLEDs leds;
mtLED leds;
/* Constructor */
mtLED::mtLED()
{
	memset(ledState, 0, 144);
	memset(ledBits, 0, 18);
	memset(ledPWM, 0, 144);

	for(uint8_t i = 0; i < 127; i++)
	{
		//sweep[i] = 	(uint8_t)(-0.05*((i-50)*(i-50))+125);

		sweep[i] = gamma255[i*2]*0.5;
		sweep[i+127] = gamma255[255-i*2]*0.5;
	}



} 

void mtLED::begin(uint8_t addr)
{

	//init i2c
#ifdef mtLED_I2C_DMA
	Wire2.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000, I2C_OP_MODE_DMA);
#else
	Wire2.begin(I2C_MASTER, 0x00, I2C_PINS_3_4, I2C_PULLUP_EXT, 400000);
#endif
	Wire2.setDefaultTimeout(1000); // 250ms default timeout



	// Wire.setOpMode(I2C_OP_MODE_DMA);
	//       if(Wire.i2c->opMode == I2C_OP_MODE_DMA)
	//           Serial.printf("OK (Channel %d)\n",Wire.i2c->DMA->channel);
	//       else
	//           Serial.print("Failed, using ISR\n");
	//koniec initu i2c


	_i2caddr = addr; 
	_frame = 0;

	// shutdown
	writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);

	delay(10);

	// out of shutdown
	writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01);  

	// picture mode
	writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE);

	displayFrame(_frame); 

	// all LEDs on & 0 PWM
	setLEDOnOffBytes(ledBits,_frame);

	clear(); // set each led to 0 PWM

/*
	//wszystkie ledy na mode on
	Wire.beginTransmission(_i2caddr);
	Wire.write((byte)0x00);
	for (uint8_t i = 0; i <= 0x11; i++)
	{
		Wire.write(0xff);
	}
	Wire.endTransmission();
*/

	// for (uint8_t f=0; f<8; f++) {
	//   for (uint8_t i=0; i<=0x11; i++)
	//     writeRegister8(f, i, 0xff);     // each 8 LEDs on
	// }







	audioSync(false);
}



void mtLED::update_all_leds()
{
	if(leds_need_update)
	{
		//Wire.setOpMode(I2C_OP_MODE_ISR);
		checkAndRepair();

		Wire2.beginTransmission(_i2caddr);
		Wire2.write((byte)ISSI_COMMANDREGISTER);
		Wire2.write(_frame);
		Wire2.endTransmission();

		Wire2.beginTransmission(_i2caddr);
		Wire2.write((byte)0x00);

		for (uint8_t i = 0; i < 18; i++)  //0x11
		{
			Wire2.write(ledBits[i]);
		}

		for (uint8_t i = 0; i < 18; i++)  //0x11
		{
			Wire2.write(0);
		}

		for(uint8_t i=0; i < 144; i++)
		{	
				Wire2.write((byte)ledPWM[i]);
		}

		#ifdef mtLED_I2C_DMA
		//Wire.sendTransmission();
		Wire2.sendTransmission();
		#else
		Wire2.sendTransmission();
		//Wire.endTransmission();
		#endif

		//Wire.setOpMode(I2C_OP_MODE_DMA);

		leds_need_update = 0;
	}
} 


void mtLED::update_pads()
{

}
 

void mtLED::setLED(uint8_t num, uint8_t state, uint8_t gamma_pwm)
{
	//ledState[num] = 1;
	num+= 4*(num/12); // special for tracker 12 x 4 grid

	ledPWM[num] = gamma32[gamma_pwm];

	if(state) ledBits[num/8] |= (1 << num % 8);
	else ledBits[num/8] &= ~(1 << num % 8);
	leds_need_update = 1;
}


void mtLED::setLEDpwm(uint8_t num, uint8_t state, uint8_t pwm)
{
	//ledState[num] = 1;
	ledPWM[num] = pwm;
	if(state) ledBits[num/8] |= (1 << num % 8);
	else ledBits[num/8] &= ~(1 << num % 8);
	leds_need_update = 1;
}


void mtLED::clearAllPadsLED()
{
	for(uint8_t led = 0; led < 64; led++) 
	{
		ledPWM[medusaPadsLeds[led]] = 0;
		ledBits[medusaPadsLeds[led]/8] &= ~(1 << medusaPadsLeds[led] % 8);
	}
	leds_need_update = 1;
}



void mtLED::clear(void)
{
	// all LEDs on & 0 PWM

	selectBank(_frame);



	// print_i2c_status();
	// _count = 0;
	// while(!Wire.done()) _count++; // Since write is non-blocking, do some counting while waiting

	//wait4wire();

	for (uint8_t i = 0; i < 6; i++)
	{
		Wire2.beginTransmission(_i2caddr);
		Wire2.write((byte) 0x24 + i * 24);
		// write 24 bytes at once
		for (uint8_t j = 0; j < 18; j++) {
			Wire2.write((byte) 0);
		}
		#ifdef mtLED_I2C_DMA
		//Wire.sendTransmission();
		Wire2.endTransmission();
		#else
		Wire2.endTransmission();
		#endif

		// _count = 1;
		//      while(!Wire.done()) _count++; // Since write is non-blocking, do some counting while waiting
		//     Serial.printf("clear : %d\n", _count++);

	}
}


// void SEQ_IS31FL3731::fillAll(void) {
//   // all LEDs on & 0 PWM
 
//   selectBank(_frame);

//   // print_i2c_status();       
//   // _count = 0;
//   // while(!Wire.done()) _count++; // Since write is non-blocking, do some counting while waiting


//   for (uint8_t i=0; i<6; i++) {
//     Wire.beginTransmission(_i2caddr);
//     Wire.write((byte) 0x24+i*24);
//     // write 24 bytes at once
//     for (uint8_t j=0; j<24; j++) {
//       Wire.write((byte) 255);
//     }
//     Wire.sendTransmission();

//     _count = 1;
//          while(!Wire.done()) _count++; // Since write is non-blocking, do some counting while waiting
//         Serial.printf("write register : %d\n", _count++);
//   }
// }

void mtLED::setLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank) {
	if (lednum >= 144) return;
	writeRegister8(bank, 0x24 + lednum, pwm);
}


void mtLED::fastSetLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank)
{
	if (lednum >= 144) return;
	//writeRegister8(bank, 0x24+lednum, pwm);

	Wire2.beginTransmission(_i2caddr);
	Wire2.write((byte)ISSI_COMMANDREGISTER);
	Wire2.write(bank);
	Wire2.endTransmission();

	Wire2.beginTransmission(_i2caddr);
	Wire2.write((byte)0x24 + lednum);
	Wire2.write((byte)pwm);
	Wire2.endTransmission();

	//Serial.println("fast led");

}

void mtLED::setAllLEDPWM(uint8_t *arr, uint8_t bank)
{

	//Wire.finish();
	Wire2.beginTransmission(_i2caddr);
	Wire2.write((byte)ISSI_COMMANDREGISTER);
	Wire2.write((byte)bank);
	Wire2.endTransmission();

	//while(!Wire.done()){}

	//Wire.finish();


	Wire2.beginTransmission(_i2caddr);
	Wire2.write((byte)0x24);
	for (uint8_t a = 0; a <= 143; a++)
	{
		Wire.write((byte)arr[a]);
	}
	Wire2.endTransmission();

}

void mtLED::setLEDOnOff2Bytes(uint16_t *arr, uint8_t bank)
{
	Wire2.beginTransmission(_i2caddr);
	Wire2.write((uint8_t)ISSI_COMMANDREGISTER);
	Wire2.write((uint8_t)bank);
	Wire2.endTransmission();


	Wire2.beginTransmission(_i2caddr);
	Wire2.write((byte)0x00);
	for (uint8_t a = 0; a <= 9; a++)
	{
		Wire2.write((byte)arr[a]);
		Wire2.write((byte)(arr[a] >> 8));

	}

	Wire2.endTransmission();



	// Wire.beginTransmission(_i2caddr);
	// Wire.write((byte)0x00);
	// for (uint8_t i = 0; i <= 0x11; i++)
	// {
	//   Wire.write(0xff);
	// }
	// Wire.endTransmission();
}


void mtLED::setLEDOnOffBytes(uint8_t *arr, uint8_t bank)
{
	Wire2.beginTransmission(_i2caddr);
	Wire2.write((uint8_t)ISSI_COMMANDREGISTER);
	Wire2.write((uint8_t)bank);
	Wire2.endTransmission();
/*

	Wire.beginTransmission(_i2caddr);
	Wire.write((byte)0x00);
	for (uint8_t a = 0; a <= 9; a++)
	{
		Wire.write((byte)arr[a]);
		Wire.write((byte)(arr[a] >> 8));

	}

	Wire.endTransmission();

*/

	Wire2.beginTransmission(_i2caddr);
	Wire2.write((byte)0x00);
	for (uint8_t i = 0; i < 18; i++)  //0x11
	{
	  Wire2.write(arr[i]);
	}
	Wire2.endTransmission();
}


// void SEQ_IS31FL3731::fastSetLEDPWM(void)
// {

// }



void mtLED::setFrame(uint8_t f) {
	_frame = f;
}

void mtLED::displayFrame(uint8_t f) {
	if (f > 7) f = 0;
	writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, f);
}


void mtLED::selectBank(uint8_t b)
{

	Wire2.beginTransmission(_i2caddr);
	Wire2.write((uint8_t)ISSI_COMMANDREGISTER);
	Wire2.write((uint8_t)b);
	Wire2.endTransmission();

	// _count = 1;
	//        while(!Wire.done()) _count++; // Since write is non-blocking, do some counting while waiting
	//       Serial.printf("select bank : %d\n", _count++);

	// _count = 1;
	//while(!Wire.done()) _count++; // Since write is non-blocking, do some counting while waiting
	//       Serial.printf("select bank : %d\n", _count++);
	//       print_i2c_status();
}

void mtLED::audioSync(bool sync) {
	if (sync) {
		writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x1);
	} else {
		writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x0);
	}
}

/*************/
void mtLED::writeRegister8(uint8_t b, uint8_t reg, uint8_t data) {

	Wire2.beginTransmission(_i2caddr);
	Wire2.write((byte)ISSI_COMMANDREGISTER);
	Wire2.write((byte)b);
	Wire2.endTransmission();



	Wire2.beginTransmission(_i2caddr);
	Wire2.write((byte)reg);
	Wire2.write((byte)data);
	Wire2.endTransmission();

	if (b == ISSI_BANK_FUNCTIONREG)
	{
		functionRegBuff[reg] = data;
	}

	// _count = 1;
	//        while(!Wire.done()) _count++; // Since write is non-blocking, do some counting while waiting
	//       Serial.printf("write reg 8 : %d\n", _count++);

	// _count = 1;
	//while(!Wire.done()) _count++; // Since write is non-blocking, do some counting while waiting
	// Serial.printf("write register : %d\n", _count++);

	// print_i2c_status();
	//Serial.print("$"); Serial.print(reg, HEX);
	//Serial.print(" = 0x"); Serial.println(data, HEX);
}

uint8_t  mtLED::readRegister8(uint8_t bank, uint8_t reg)
{
	uint8_t x;
	x = 0;

	while (!Wire2.done()) _count++; // Since write is non-blocking, do some counting while waiting


	Wire2.beginTransmission(_i2caddr);
	Wire2.write((byte)ISSI_COMMANDREGISTER);
	Wire2.write((byte)bank);
	Wire2.endTransmission();


	Wire2.beginTransmission(_i2caddr);
	Wire2.write((byte)reg);
	Wire2.endTransmission();

	Wire2.requestFrom(_i2caddr, uint8_t(1) );

	while (Wire2.available())
	{
		// Serial.println(x);
		x = Wire2.read();
		// Serial.println(x);
	}
	Wire2.endTransmission();

// Serial.print("$"); Serial.print(reg, HEX);
//  Serial.print(": 0x"); Serial.println(x, HEX);

	return x;
}

void mtLED::print_i2c_status(void)
{
	// switch(Wire.status())
	//   {
	//   case I2C_WAITING:  Serial.print("I2C waiting, no errors\n"); break;
	//   case I2C_ADDR_NAK: Serial.print("Slave addr not acknowledged\n"); break;
	//   case I2C_DATA_NAK: Serial.print("Slave data not acknowledged\n"); break;
	//   case I2C_ARB_LOST: Serial.print("Bus Error: Arbitration Lost\n"); break;
	//   case I2C_TIMEOUT:  Serial.print("I2C timeout\n"); break;
	//   case I2C_BUF_OVF:  Serial.print("I2C buffer overflow\n"); break;
	//   default:           Serial.print("I2C busy\n"); break;
	//   }
}

void mtLED::debug(void)
{
	_debug = !_debug;
}




uint8_t mtLED::get_g32(uint8_t g)
{
	if (g <= 31)
	{
		return gamma32[g];
	}
	if (g > 31)
	{
		return gamma32[31];
	}
	else
	{
		return 0;
	}
}

bool mtLED::isDeadResetCheck(void)
{
	bool x;
	if (readRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN) == 1)
	{
		x = 0;
	}
	else
	{
		x = 1;
		begin(_i2caddr);
	}









	return x;

}


void mtLED::dropRegInfo(void)
{
	for (uint8_t reg = 0x00; reg <= 0x0C; reg++)
	{
		uint8_t tempData = readRegister8(ISSI_BANK_FUNCTIONREG, reg);

		Serial.print("reg: ");
		Serial.println(reg, HEX);
		Serial.println(tempData);

		if (tempData == functionRegBuff[reg]) Serial.println("match ok");
		else
		{
			Serial.println("missmatch, trying repair...");
			checkAndRepairReg(reg);
		}
		Serial.println(); 
	}
}

bool mtLED::checkAndRepairReg(uint8_t reg)
{
	if (readRegister8(ISSI_BANK_FUNCTIONREG, reg) != functionRegBuff[reg])
	{
		writeRegister8(ISSI_BANK_FUNCTIONREG, reg, functionRegBuff[reg]);
		//Serial.println("------ reg error -------");
		return 1;
	}

	return 0;
}

void mtLED::checkAndRepair(void)
{

	for (uint8_t reg = 0; reg <= 0x0D; reg++)
	{
		// omijamy wyjątki:
		if (reg == 0x07) continue;

		checkAndRepairReg(reg);
	}

}


uint16_t incr = 0;



void mtLED::animatePadsLeds()
{

	if(incr >= 255) incr = 0;

	for (uint8_t x = 0; x < 8; x++)
	{
		for (uint8_t y = 0; y < 8; y++)
		{
			setLEDpwm(medusaPadsLeds[x*8+y], 1, sweep[(x+y+incr)%255]);
			//ledmatrix.drawPixel(x, y, sweep[(x+y+incr)%24]);
		}
	}

	incr++;

	update_all_leds();
}

//////////////////////////////// mtLEDs


mtLEDs::mtLEDs()
{
	memset(ledPWMseq, 0, 288);
	memset(ledPWMgrid, 0, 144);
}


void mtLEDs::begin()
{
//	ledsSeqA.begin(IS31FL3731_ADDR4);
//	ledsSeqB.begin(IS31FL3731_ADDR3);
	ledsGrid.begin(IS31FL3731_ADDR1);

}
void mtLEDs::updateSeq()
{
	ledsSeqA.update_all_leds();
	ledsSeqB.update_all_leds();

}

void mtLEDs::updateGrid()
{

	ledsGrid.update_all_leds();
}

void mtLEDs::setLEDseq(uint8_t x,uint8_t y, uint8_t state, uint8_t gamma_pwm)
{

	uint8_t num=0;
	if(y<=16)
	{
		if(y<=8) num = 16*(y-1) + x-1;
		else num= 16*(y-8)-8+ x-1;

		ledsSeqA.setLED(num, state, gamma_pwm);
	}
	else if(y>=17 && y<=20)
	{
		y-=16;

		num=16*(y-1)+x-1;
		ledsSeqB.setLED(num, state, gamma_pwm);
	}

}
void mtLEDs::setLEDgrid(uint8_t num, uint8_t state, uint8_t gamma_pwm)
{
	uint8_t j=0,i=0;

	//num--; // zeby mogl byc zakres 1-64
	j=num/8;
	j=7-j;
	i=num%8; // przeliczenia związane z "obrotem" macierzy

	num=i*8+j;

	j=num/8;
	num+=j*8; // obliczenia związane z podłączeniem fizycznym ledow do jednej z dwoch macierzy sterownika (co drugi adres)


	ledsGrid.setLED(num, state, gamma_pwm);
}

void mtLEDs::setLEDseqPWM(uint8_t x,uint8_t y, uint8_t state, uint8_t pwm)
{
	uint8_t num=0;
	if(y<=16)
	{
		if(y<=8) num = 16*(y-1) + x-1;
		else num= 16*(y-8)-8+x-1;

		ledsSeqA.setLEDpwm(num, state,pwm);
	}
	else
	{
		y-=16;
		num=16*(y-1)+x-1;
		ledsSeqB.setLEDpwm(num, state, pwm);
	}
}

void mtLEDs::setLEDgridPWM(uint8_t x,uint8_t y, uint8_t state, uint8_t pwm)
{

	uint8_t num=0;
	y=9-y;
	num=16*(x-1)+y-1;
	ledsGrid.setLED(num, state, pwm);
}

void mtLEDs::fastSetLEDseqPWM(uint8_t x,uint8_t y, uint8_t pwm )
{
	uint8_t num=0;
	uint8_t bank = 0;
	if(y<=16)
	{
		if(y<=8) num = 16*(y-1) + x-1;
		else num= 16*(y-8)-8+ x-1;

		ledsSeqA.fastSetLEDPWM(num,pwm,bank);
	}
	else
	{
		y-=16;
		num=16*(y-1)+x-1;
		ledsSeqB.fastSetLEDPWM(num,pwm,bank);
	}

}

void mtLEDs::fastSetLEDgridPWM(uint8_t x,uint8_t y, uint8_t pwm)
{
	uint8_t num=0;
	y=9-y;
	num=16*(x-1)+y-1;
	ledsGrid.fastSetLEDPWM(num, pwm);
}

void mtLEDs::setAllLEDPWM(uint8_t *arrSeq,uint8_t *arrGrid, uint8_t bank)
{
//	ledsSeqA.setAllLEDPWM(arrSeq,bank);
//	ledsSeqB.setAllLEDPWM((arrSeq+144),bank);
	ledsGrid.setAllLEDPWM(arrGrid,0);
}


void mtLEDs::setSleep()
{
//	ledsSeqA.writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);
//	ledsSeqB.writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);
	ledsGrid.writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);
}


