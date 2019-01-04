/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <SPI2.h>
#include "pins_arduino.h"



/**********************************************************/
/*     32 bit Teensy 3.0 and 3.1			  */
/**********************************************************/

#if defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISK)

SPI2Class SPI2;

uint8_t SPI2Class::interruptMasksUsed = 0;
uint32_t SPI2Class::interruptMask[(NVIC_NUM_INTERRUPTS+31)/32];
uint32_t SPI2Class::interruptSave[(NVIC_NUM_INTERRUPTS+31)/32];
#ifdef SPI_TRANSACTION_MISMATCH_LED
uint8_t SPI2Class::inTransactionFlag = 0;
#endif

void SPI2Class::begin()
{
	SIM_SCGC3 |= SIM_SCGC3_SPI2;
	SPI2_MCR = SPI_MCR_MDIS | SPI_MCR_HALT | SPI_MCR_PCSIS(0x1F);
	SPI2_CTAR0 = SPI_CTAR_FMSZ(7) | SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_CSSCK(1);
	SPI2_CTAR1 = SPI_CTAR_FMSZ(15) | SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_CSSCK(1);
	SPI2_MCR = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x1F);
	SPCR2.enable_pins(); // pins managed by SPCRemulation in avr_emulation.h
}

void SPI2Class::end() {
	SPCR2.disable_pins();
	SPI2_MCR = SPI_MCR_MDIS | SPI_MCR_HALT | SPI_MCR_PCSIS(0x1F);
}

void SPI2Class::usingInterrupt(IRQ_NUMBER_t interruptName)
{
	uint32_t n = (uint32_t)interruptName;

	if (n >= NVIC_NUM_INTERRUPTS) return;

	//Serial.print("usingInterrupt ");
	//Serial.println(n);
	interruptMasksUsed |= (1 << (n >> 5));
	interruptMask[n >> 5] |= (1 << (n & 0x1F));
	//Serial.printf("interruptMasksUsed = %d\n", interruptMasksUsed);
	//Serial.printf("interruptMask[0] = %08X\n", interruptMask[0]);
	//Serial.printf("interruptMask[1] = %08X\n", interruptMask[1]);
	//Serial.printf("interruptMask[2] = %08X\n", interruptMask[2]);
}

void SPI2Class::notUsingInterrupt(IRQ_NUMBER_t interruptName)
{
	uint32_t n = (uint32_t)interruptName;
	if (n >= NVIC_NUM_INTERRUPTS) return;
	interruptMask[n >> 5] &= ~(1 << (n & 0x1F));
	if (interruptMask[n >> 5] == 0) {
		interruptMasksUsed &= ~(1 << (n >> 5));
	}
}

const uint16_t SPI2Settings::ctar_div_table[23] = {
	2, 3, 4, 5, 6, 8, 10, 12, 16, 20, 24, 32, 40,
	56, 64, 96, 128, 192, 256, 384, 512, 640, 768
};
const uint32_t SPI2Settings::ctar_clock_table[23] = {
	SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR | SPI_CTAR_CSSCK(0),
	SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_DBR | SPI_CTAR_CSSCK(0),
	SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0),
	SPI_CTAR_PBR(2) | SPI_CTAR_BR(0) | SPI_CTAR_DBR | SPI_CTAR_CSSCK(0),
	SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0),
	SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_CSSCK(1),
	SPI_CTAR_PBR(2) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0),
	SPI_CTAR_PBR(1) | SPI_CTAR_BR(1) | SPI_CTAR_CSSCK(1),
	SPI_CTAR_PBR(0) | SPI_CTAR_BR(3) | SPI_CTAR_CSSCK(2),
	SPI_CTAR_PBR(2) | SPI_CTAR_BR(1) | SPI_CTAR_CSSCK(0),
	SPI_CTAR_PBR(1) | SPI_CTAR_BR(3) | SPI_CTAR_CSSCK(2),
	SPI_CTAR_PBR(0) | SPI_CTAR_BR(4) | SPI_CTAR_CSSCK(3),
	SPI_CTAR_PBR(2) | SPI_CTAR_BR(3) | SPI_CTAR_CSSCK(2),
	SPI_CTAR_PBR(3) | SPI_CTAR_BR(3) | SPI_CTAR_CSSCK(2),
	SPI_CTAR_PBR(0) | SPI_CTAR_BR(5) | SPI_CTAR_CSSCK(4),
	SPI_CTAR_PBR(1) | SPI_CTAR_BR(5) | SPI_CTAR_CSSCK(4),
	SPI_CTAR_PBR(0) | SPI_CTAR_BR(6) | SPI_CTAR_CSSCK(5),
	SPI_CTAR_PBR(1) | SPI_CTAR_BR(6) | SPI_CTAR_CSSCK(5),
	SPI_CTAR_PBR(0) | SPI_CTAR_BR(7) | SPI_CTAR_CSSCK(6),
	SPI_CTAR_PBR(1) | SPI_CTAR_BR(7) | SPI_CTAR_CSSCK(6),
	SPI_CTAR_PBR(0) | SPI_CTAR_BR(8) | SPI_CTAR_CSSCK(7),
	SPI_CTAR_PBR(2) | SPI_CTAR_BR(7) | SPI_CTAR_CSSCK(6),
	SPI_CTAR_PBR(1) | SPI_CTAR_BR(8) | SPI_CTAR_CSSCK(7)
};

static void updateCTAR(uint32_t ctar)
{
	if (SPI2_CTAR0 != ctar) {
		uint32_t mcr = SPI2_MCR;
		if (mcr & SPI_MCR_MDIS) {
			SPI2_CTAR0 = ctar;
			SPI2_CTAR1 = ctar | SPI_CTAR_FMSZ(8);
		} else {
			SPI2_MCR = SPI_MCR_MDIS | SPI_MCR_HALT | SPI_MCR_PCSIS(0x1F);
			SPI2_CTAR0 = ctar;
			SPI2_CTAR1 = ctar | SPI_CTAR_FMSZ(8);
			SPI2_MCR = mcr;
		}
	}
}

void SPI2Class::setBitOrder(uint8_t bitOrder)
{
	SIM_SCGC3 |= SIM_SCGC3_SPI2;
	uint32_t ctar = SPI2_CTAR0;
	if (bitOrder == LSBFIRST) {
		ctar |= SPI_CTAR_LSBFE;
	} else {
		ctar &= ~SPI_CTAR_LSBFE;
	}
	updateCTAR(ctar);
}

void SPI2Class::setDataMode(uint8_t dataMode)
{
	SIM_SCGC3 |= SIM_SCGC3_SPI2;

	// TODO: implement with native code

	SPCR = (SPCR & ~SPI_MODE_MASK) | dataMode;
}

void SPI2Class::setClockDivider_noInline(uint32_t clk)
{
	SIM_SCGC3 |= SIM_SCGC3_SPI2;
	uint32_t ctar = SPI2_CTAR0;
	ctar &= (SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_LSBFE);
	if (ctar & SPI_CTAR_CPHA) {
		clk = (clk & 0xFFFF0FFF) | ((clk & 0xF000) >> 4);
	}
	ctar |= clk;
	updateCTAR(ctar);
}

bool SPI2Class::pinIsChipSelect(uint8_t pin)
{
	if (pin == 10 || pin == 9 || pin == 6 || pin == 2 || pin == 15) return true;
	if (pin >= 20 && pin <= 23) return true;
	return false;
}

bool SPI2Class::pinIsChipSelect(uint8_t pin1, uint8_t pin2)
{
	if (!pinIsChipSelect(pin1) || !pinIsChipSelect(pin2)) return false;
	if ((pin1 ==  2 && pin2 == 10) || (pin1 == 10 && pin2 ==  2)) return false;
	if ((pin1 ==  6 && pin2 ==  9) || (pin1 ==  9 && pin2 ==  6)) return false;
	if ((pin1 == 20 && pin2 == 23) || (pin1 == 23 && pin2 == 20)) return false;
	if ((pin1 == 21 && pin2 == 22) || (pin1 == 22 && pin2 == 21)) return false;
	return true;
}

uint8_t SPI2Class::setCS(uint8_t pin)
{
	switch (pin) {
	  case 10: CORE_PIN10_CONFIG = PORT_PCR_MUX(2); return 0x01; // PTC4
	  case 2:  CORE_PIN2_CONFIG  = PORT_PCR_MUX(2); return 0x01; // PTD0
	  case 9:  CORE_PIN9_CONFIG  = PORT_PCR_MUX(2); return 0x02; // PTC3
	  case 6:  CORE_PIN6_CONFIG  = PORT_PCR_MUX(2); return 0x02; // PTD4
	  case 20: CORE_PIN20_CONFIG = PORT_PCR_MUX(2); return 0x04; // PTD5
	  case 23: CORE_PIN23_CONFIG = PORT_PCR_MUX(2); return 0x04; // PTC2
	  case 21: CORE_PIN21_CONFIG = PORT_PCR_MUX(2); return 0x08; // PTD6
	  case 22: CORE_PIN22_CONFIG = PORT_PCR_MUX(2); return 0x08; // PTC1
	  case 15: CORE_PIN15_CONFIG = PORT_PCR_MUX(2); return 0x10; // PTC0
	}
	return 0;
}


/**********************************************************/
/*     32 bit Teensy-LC                                   */
/**********************************************************/

#elif defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISL)

SPI2Class SPI;
SPI1Class SPI1;

uint32_t SPI2Class::interruptMask = 0;
uint32_t SPI2Class::interruptSave = 0;
uint32_t SPI1Class::interruptMask = 0;
uint32_t SPI1Class::interruptSave = 0;
#ifdef SPI_TRANSACTION_MISMATCH_LED
uint8_t SPI2Class::inTransactionFlag = 0;
uint8_t SPI1Class::inTransactionFlag = 0;
#endif

void SPI2Class::begin()
{
	SIM_SCGC4 |= SIM_SCGC4_SPI2;
	SPI2_C1 = SPI_C1_SPE | SPI_C1_MSTR;
	SPI2_C2 = 0;
	uint8_t tmp __attribute__((unused)) = SPI2_S;
	SPCR2.enable_pins(); // pins managed by SPCRemulation in avr_emulation.h
}

void SPI2Class::end() {
	SPCR2.disable_pins();
	SPI2_C1 = 0;
}

const uint16_t SPI2Settings::br_div_table[30] = {
	2, 4, 6, 8, 10, 12, 14, 16, 20, 24,
	28, 32, 40, 48, 56, 64, 80, 96, 112, 128,
	160, 192, 224, 256, 320, 384, 448, 512, 640, 768,
};

const uint8_t SPI2Settings::br_clock_table[30] = {
	SPI_BR_SPPR(0) | SPI_BR_SPR(0),
	SPI_BR_SPPR(1) | SPI_BR_SPR(0),
	SPI_BR_SPPR(2) | SPI_BR_SPR(0),
	SPI_BR_SPPR(3) | SPI_BR_SPR(0),
	SPI_BR_SPPR(4) | SPI_BR_SPR(0),
	SPI_BR_SPPR(5) | SPI_BR_SPR(0),
	SPI_BR_SPPR(6) | SPI_BR_SPR(0),
	SPI_BR_SPPR(7) | SPI_BR_SPR(0),
	SPI_BR_SPPR(4) | SPI_BR_SPR(1),
	SPI_BR_SPPR(5) | SPI_BR_SPR(1),
	SPI_BR_SPPR(6) | SPI_BR_SPR(1),
	SPI_BR_SPPR(7) | SPI_BR_SPR(1),
	SPI_BR_SPPR(4) | SPI_BR_SPR(2),
	SPI_BR_SPPR(5) | SPI_BR_SPR(2),
	SPI_BR_SPPR(6) | SPI_BR_SPR(2),
	SPI_BR_SPPR(7) | SPI_BR_SPR(2),
	SPI_BR_SPPR(4) | SPI_BR_SPR(3),
	SPI_BR_SPPR(5) | SPI_BR_SPR(3),
	SPI_BR_SPPR(6) | SPI_BR_SPR(3),
	SPI_BR_SPPR(7) | SPI_BR_SPR(3),
	SPI_BR_SPPR(4) | SPI_BR_SPR(4),
	SPI_BR_SPPR(5) | SPI_BR_SPR(4),
	SPI_BR_SPPR(6) | SPI_BR_SPR(4),
	SPI_BR_SPPR(7) | SPI_BR_SPR(4),
	SPI_BR_SPPR(4) | SPI_BR_SPR(5),
	SPI_BR_SPPR(5) | SPI_BR_SPR(5),
	SPI_BR_SPPR(6) | SPI_BR_SPR(5),
	SPI_BR_SPPR(7) | SPI_BR_SPR(5),
	SPI_BR_SPPR(4) | SPI_BR_SPR(6),
	SPI_BR_SPPR(5) | SPI_BR_SPR(6)
};

uint8_t SPI2Class::setCS(uint8_t pin)
{
	switch (pin) {
	  case 10: CORE_PIN10_CONFIG = PORT_PCR_MUX(2); return 0x01; // PTC4
	  case 2:  CORE_PIN2_CONFIG  = PORT_PCR_MUX(2); return 0x01; // PTD0
	}
	return 0;
}

void SPI1Class::begin()
{
	SIM_SCGC4 |= SIM_SCGC4_SPI1;
	SPI1_C1 = SPI_C1_SPE | SPI_C1_MSTR;
	SPI1_C2 = 0;
	uint8_t tmp __attribute__((unused)) = SPI1_S;
	SPCR1.enable_pins(); // pins managed by SPCRemulation in avr_emulation.h
}

void SPI1Class::end() {
	SPCR1.disable_pins();
	SPI1_C1 = 0;
}

uint8_t SPI1Class::setCS(uint8_t pin)
{
	switch (pin) {
	  case 6:  CORE_PIN6_CONFIG = PORT_PCR_MUX(2); return 0x01; // PTD4
	}
	return 0;
}




/**********************************************************/
/*     32 bit Arduino Due				  */
/**********************************************************/

#elif defined(__arm__) && defined(__SAM3X8E__)

#include <SPI2.h>


SPI2Class::SPI2Class(Spi *_spi, uint32_t _id, void(*_initCb)(void)) :
	spi(_spi), id(_id), initCb(_initCb), initialized(false)
{
	// Empty
}

void SPI2Class::begin() {
	init();
	// NPCS control is left to the user

	// Default speed set to 4Mhz
	setClockDivider(BOARD_SPI_DEFAULT_SS, 21);
	setDataMode(BOARD_SPI_DEFAULT_SS, SPI_MODE0);
	setBitOrder(BOARD_SPI_DEFAULT_SS, MSBFIRST);
}

void SPI2Class::begin(uint8_t _pin) {
	init();

	uint32_t spiPin = BOARD_PIN_TO_SPI_PIN(_pin);
	PIO_Configure(
		g_APinDescription[spiPin].pPort,
		g_APinDescription[spiPin].ulPinType,
		g_APinDescription[spiPin].ulPin,
		g_APinDescription[spiPin].ulPinConfiguration);

	// Default speed set to 4Mhz
	setClockDivider(_pin, 21);
	setDataMode(_pin, SPI_MODE0);
	setBitOrder(_pin, MSBFIRST);
}

void SPI2Class::init() {
	if (initialized)
		return;
	interruptMode = 0;
	interruptMask = 0;
	interruptSave = 0;
	initCb();
	SPI_Configure(spi, id, SPI_MR_MSTR | SPI_MR_PS | SPI_MR_MODFDIS);
	SPI_Enable(spi);
	initialized = true;
}

#ifndef interruptsStatus
#define interruptsStatus() __interruptsStatus()
static inline unsigned char __interruptsStatus(void) __attribute__((always_inline, unused));
static inline unsigned char __interruptsStatus(void) {
  unsigned int primask;
  asm volatile ("mrs %0, primask" : "=r" (primask));
  if (primask) return 0;
  return 1;
}
#endif

void SPI2Class::usingInterrupt(uint8_t interruptNumber)
{
	uint8_t irestore;

	irestore = interruptsStatus();
	noInterrupts();
	if (interruptMode < 2) {
		if (interruptNumber > NUM_DIGITAL_PINS) {
			interruptMode = 2;
		} else {
			uint8_t imask = interruptMask;
			Pio *pio = g_APinDescription[interruptNumber].pPort;
			if (pio == PIOA) {
				imask |= 1;
			} else if (pio == PIOB) {
				imask |= 2;
			} else if (pio == PIOC) {
				imask |= 4;
			} else if (pio == PIOD) {
				imask |= 8;
			}
			interruptMask = imask;
			interruptMode = 1;
		}
	}
	if (irestore) interrupts();
}

void SPI2Class::beginTransaction(uint8_t pin, SPI2Settings settings)
{
	if (interruptMode > 0) {
		if (interruptMode == 1) {
			uint8_t imask = interruptMask;
			if (imask & 1) NVIC_DisableIRQ(PIOA_IRQn);
			if (imask & 2) NVIC_DisableIRQ(PIOB_IRQn);
			if (imask & 4) NVIC_DisableIRQ(PIOC_IRQn);
			if (imask & 8) NVIC_DisableIRQ(PIOD_IRQn);
		} else {
			interruptSave = interruptsStatus();
			noInterrupts();
		}
	}
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(pin);
	bitOrder[ch] = settings.border;
	SPI_ConfigureNPCS(spi, ch, settings.config);
}

void SPI2Class::endTransaction(void)
{
	if (interruptMode > 0) {
		if (interruptMode == 1) {
			uint8_t imask = interruptMask;
			if (imask & 1) NVIC_EnableIRQ(PIOA_IRQn);
			if (imask & 2) NVIC_EnableIRQ(PIOB_IRQn);
			if (imask & 4) NVIC_EnableIRQ(PIOC_IRQn);
			if (imask & 8) NVIC_EnableIRQ(PIOD_IRQn);
		} else {
			if (interruptSave) interrupts();
		}
	}
}

void SPI2Class::end(uint8_t _pin) {
	uint32_t spiPin = BOARD_PIN_TO_SPI_PIN(_pin);
	// Setting the pin as INPUT will disconnect it from SPI peripheral
	pinMode(spiPin, INPUT);
}

void SPI2Class::end() {
	SPI_Disable(spi);
	initialized = false;
}

void SPI2Class::setBitOrder(uint8_t _pin, BitOrder _bitOrder) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	bitOrder[ch] = _bitOrder;
}

void SPI2Class::setDataMode(uint8_t _pin, uint8_t _mode) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	mode[ch] = _mode | SPI_CSR_CSAAT;
	// SPI_CSR_DLYBCT(1) keeps CS enabled for 32 MCLK after a completed
	// transfer. Some device needs that for working properly.
	SPI_ConfigureNPCS(spi, ch, mode[ch] | SPI_CSR_SCBR(divider[ch]) | SPI_CSR_DLYBCT(1));
}

void SPI2Class::setClockDivider(uint8_t _pin, uint8_t _divider) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	divider[ch] = _divider;
	// SPI_CSR_DLYBCT(1) keeps CS enabled for 32 MCLK after a completed
	// transfer. Some device needs that for working properly.
	SPI_ConfigureNPCS(spi, ch, mode[ch] | SPI_CSR_SCBR(divider[ch]) | SPI_CSR_DLYBCT(1));
}

byte SPI2Class::transfer(byte _pin, uint8_t _data, SPITransferMode _mode) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	// Reverse bit order
	if (bitOrder[ch] == LSBFIRST)
		_data = __REV(__RBIT(_data));
	uint32_t d = _data | SPI_PCS(ch);
	if (_mode == SPI_LAST)
		d |= SPI_TDR_LASTXFER;

	// SPI_Write(spi, _channel, _data);
    while ((spi->SPI_SR & SPI_SR_TDRE) == 0)
	;
    spi->SPI_TDR = d;

    // return SPI_Read(spi);
    while ((spi->SPI_SR & SPI_SR_RDRF) == 0)
	;
    d = spi->SPI_RDR;
	// Reverse bit order
	if (bitOrder[ch] == LSBFIRST)
		d = __REV(__RBIT(d));
    return d & 0xFF;
}

void SPI2Class::attachInterrupt(void) {
	// Should be enableInterrupt()
}

void SPI2Class::detachInterrupt(void) {
	// Should be disableInterrupt()
}

#if SPI_INTERFACES_COUNT > 0
static void SPI_0_Init(void) {
	PIO_Configure(
			g_APinDescription[PIN_SPI_MOSI].pPort,
			g_APinDescription[PIN_SPI_MOSI].ulPinType,
			g_APinDescription[PIN_SPI_MOSI].ulPin,
			g_APinDescription[PIN_SPI_MOSI].ulPinConfiguration);
	PIO_Configure(
			g_APinDescription[PIN_SPI_MISO].pPort,
			g_APinDescription[PIN_SPI_MISO].ulPinType,
			g_APinDescription[PIN_SPI_MISO].ulPin,
			g_APinDescription[PIN_SPI_MISO].ulPinConfiguration);
	PIO_Configure(
			g_APinDescription[PIN_SPI_SCK].pPort,
			g_APinDescription[PIN_SPI_SCK].ulPinType,
			g_APinDescription[PIN_SPI_SCK].ulPin,
			g_APinDescription[PIN_SPI_SCK].ulPinConfiguration);
}

SPI2Class SPI(SPI_INTERFACE, SPI_INTERFACE_ID, SPI_0_Init);
#endif





#endif


