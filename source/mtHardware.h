
#ifndef MTHARDWARE_H
#define MTHARDWARE_H


// PAD,POT,BUTTONS MUXS
// pad xy_z control
#define CTRL_XY_Z		25 			//PTA5			pin-55
// control lines
#define MUX_CTRL_A		PTA8 		//PTA8			pin-60
#define MUX_CTRL_B		PTA9 		//PTA9			pin-61
#define MUX_CTRL_C		PTA10 		//PTA10			pin-62
#define MUX_CTRL_D		PTA11 		//PTA11			pin-63
// signal lines
#define MUX_PAD_OUT_0	50 			//PTB5			pin-86
#define MUX_PAD_OUT_1	49	 		//PTB4			pin-85
#define MUX_PAD_OUT_2	A11 		//ADC1_DM0		pin-30
#define MUX_PAD_OUT_3	xxx 		//ADC1_SE16		pin-35
#define MUX_PAD_OUT_4	xxx 		//ADC0_DM0		pin-28
#define MUX_PAD_OUT_5	xxx 		//ADC1_DMA0		pin-29
#define MUX_PAD_OUT_6	xxx 		//PTA25			pin-76
#define MUX_PAD_OUT_7	PTA11 		//PTA24			pin-75
#define MUX_POT_OUT_0	PTA7 		//PTA24			pin-59


// LCD DISLPAY
#define LCD_MOSI   		52 			//PTD13			pin-142
#define LCD_MISO   		51	 		//PTD14			pin-143
#define LCD_SCK    		53	 		//PTD12			pin-141
#define LCD_CS     		54 			//PTD15			pin-144
#define LCD_PD     		55 			//PTD11			pin-140
#define LCD_RESET  		xxx 		//PTD10			pin-139

// FLASH
#define FLASH_MOSI 	 	28 			//PTA16			pin-68
#define FLASH_MISO  	39 			//PTA17			pin-69
#define FLASH_SCK   	27 			//PTA15			pin-67
#define FLASH_CS    	26 			//PTA14			pin-66

//SEQ GRID
#define GRID_A			24 			//PTE26			pin-47
#define GRID_B			64			//PTE8			pin-11
#define GRID_C			65	 		//PTE9			pin-12
#define GRID_I2C_SELECT	xxx 		//PTA4			pin-54

// I2C
#define I2C_SDA			4 			//PTA13			pin-65
#define I2C_SCL			3 			//PTA12			pin-64

// AUDIO CODEC
#define AUDIO_OUT_MUX	xxx 		//PTE28			pin-49
#define AUDIO_IN_MUX	xxx 		//PTE27			pin-48

// MIDI DIN/JACK
#define MIDI_OUT		xxx 		//PTE24			pin-45
#define MIDI_IN			xxx 		//PTE25			pin-46

// HAPTIC - DAC OPTION
#define HAPTIC_OUT		xxx 		//DAC0_OUT		pin-38


#endif
