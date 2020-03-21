
#ifndef MTHARDWARE_H
#define MTHARDWARE_H


///#include "core_pins.h"
//#include "Arduino.h"
//#include "Wire.h"

#include <stdint.h>


// PAD,POT,BUTTONS MUXS
// pad xy_z control
//#define CTRL_XY_Z				25 			//PTA5			pin-55


// control lines (KINETIS VER)
#define MUX_CTRL_A				8 		//PTA8			pin-60
#define MUX_CTRL_A_PCR			PORTA_PCR8
#define MUX_CTRL_A_GPIO_SET		GPIOA_PSOR
#define MUX_CTRL_A_GPIO_CLR		GPIOA_PCOR
#define MUX_CTRL_A_GPIO_DDR		GPIOA_PDDR

#define MUX_CTRL_B				9 		//PTA9			pin-61
#define MUX_CTRL_B_PCR			PORTA_PCR9
#define MUX_CTRL_B_GPIO_SET		GPIOA_PSOR
#define MUX_CTRL_B_GPIO_CLR		GPIOA_PCOR
#define MUX_CTRL_B_GPIO_DDR		GPIOA_PDDR

#define MUX_CTRL_C				27 		//PTA27			pin-78
#define MUX_CTRL_C_PCR			PORTA_PCR27
#define MUX_CTRL_C_GPIO_SET		GPIOA_PSOR
#define MUX_CTRL_C_GPIO_CLR		GPIOA_PCOR
#define MUX_CTRL_C_GPIO_DDR		GPIOA_PDDR

#define MUX_CTRL_D				28 		//PTA28			pin-79
#define MUX_CTRL_D_PCR			PORTA_PCR28
#define MUX_CTRL_D_GPIO_SET		GPIOA_PSOR
#define MUX_CTRL_D_GPIO_CLR		GPIOA_PCOR
#define MUX_CTRL_D_GPIO_DDR		GPIOA_PDDR

// signal lines
/*
#define MUX_PAD_OUT_0	50 			//PTB5			pin-86
#define MUX_PAD_OUT_1	49	 		//PTB4			pin-85
#define MUX_PAD_OUT_2	A11 		//ADC1_DM0		pin-30
#define MUX_PAD_OUT_3	72 			//ADC1_SE16		pin-35	ADC1_SE16 / ADC0_SE22
#define MUX_PAD_OUT_4	73 			//ADC0_DM0		pin-28	ADC0_DM0 / ADC1_DM3
#define MUX_PAD_OUT_5	74 			//ADC1_DMA0		pin-29	ADC1_DP0 / ADC0_DP3
#define MUX_PAD_OUT_6	A19 		//PTC11			pin-116 0_se7b
#define MUX_PAD_OUT_7	A0 			//PTD1			pin-128 0_se5b
*/

#define MUX_OUT_0		24			//PTE26			pin-47
#define MUX_OUT_1		77 			//PTA7			pin-59	ADC0_SE10
#define BUTTON1			14			//PTD1			pin-128

// LCD DISLPAY
#define LCD_MOSI   		52 			//PTD13			pin-142
#define LCD_MISO   		51	 		//PTD14			pin-143
#define LCD_SCK    		53	 		//PTD12			pin-141
#define LCD_CS     		54 			//PTD15			pin-144
#define LCD_PD     		55 			//PTD11			pin-140
#define LCD_RESET  		82 			//PTD10			pin-139

// FLASH
#define FLASH_MOSI 	 	28 			//PTA16			pin-68
#define FLASH_MISO  	39 			//PTA17			pin-69
#define FLASH_SCK   	27 			//PTA15			pin-67
#define FLASH_CS    	26 			//PTA14			pin-66

// PAD / BUTTONS INT
#define GRID_PADS_INT			38 	//PTC11			pin-116
#define CONTROL_BUTTONS_INT	    14


/*
#define GRID_B			64			//PTE8			pin-11
#define GRID_C			65	 		//PTE9			pin-12
#define GRID_I2C_SELECT	xxx 		//PTA4			pin-54
*/


// I2C
#define GRID_I2C_SDA		4 			//PTA13			pin-65
#define GRID_I2C_SCL		3 			//PTA12			pin-64

#define AUDIO_I2C_SDA		48 			//PTA13			pin-65
#define AUDIO_I2C_SCL		47 			//PTA12			pin-64


// AUDIO CODEC
#define AUDIO_OUT_MUX	68 			//PTE28			pin-49
#define AUDIO_IN_MUX	67 			//PTE27			pin-48

// MIDI DIN/JACK
#define MIDI_OUT		xxx 		//PTE24			pin-45
#define MIDI_IN			xxx 		//PTE25			pin-46

// HAPTIC - DAC OPTION
#define HAPTIC_OUT		xxx 		//DAC0_OUT		pin-38

// BLINK LED
#define BLINK_LED		78 			//PTC19

// TACT SWITCH
#define TACT_SWITCH		9 			//PTC3			pin-106

// SD DETECT
#define SD_DETECT		2 			//PTD0			pin-127

// BOOTLOADER CTRL
#define BL_CTRL			1 			//PTA1			pin-51
#define BL_CTRL_PCR			PORTA_PCR1
#define BL_CTRL_GPIO_SET		GPIOA_PSOR
#define BL_CTRL_GPIO_CLR		GPIOA_PCOR
#define BL_CTRL_GPIO_DDR		GPIOA_PDDR

// RAM ON/OFF
#define RAM_CTRL		24 			//PTA24			pin-75
#define RAM_CTRL_PCR			PORTA_PCR24
#define RAM_CTRL_GPIO_SET		GPIOA_PSOR
#define RAM_CTRL_GPIO_CLR		GPIOA_PCOR
#define RAM_CTRL_GPIO_DDR		GPIOA_PDDR

// ENCODER
#define ENC_A				10 		//PTA10			pin-62
#define ENC_A_PCR			PORTA_PCR10
#define ENC_A_GPIO_SET		GPIOA_PSOR
#define ENC_A_GPIO_CLR		GPIOA_PCOR
#define ENC_A_GPIO_DDR		GPIOA_PDDR

#define ENC_B				11 		//PTA11			pin-63
#define ENC_B_PCR			PORTA_PCR11
#define ENC_B_GPIO_SET		GPIOA_PSOR
#define ENC_B_GPIO_CLR		GPIOA_PCOR
#define ENC_B_GPIO_DDR		GPIOA_PDDR

// ENCODER SWITCH
#define ENC_SWITCH		41 			//PTD29			pin-80


#define SI4703_KLUCZ		24
#define SI4703_RST			67
#define SI4703_GPIO_2		68

#define EXTERNAL_RAM_KEY	80

#define BOOTLOADER_PIN		81





extern uint8_t hardwareTest;


#endif
