
#include "mtInterface.h"
#include "mtHardware.h"


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "sdram.h"
#include "load_sd_wav_to_memory.h"



AudioPlayMemory          playMem1;
AudioOutputI2S           i2s1;

AudioConnection          patchCord1(playMem1, 0, i2s1, 0);
AudioConnection          patchCord2(playMem1, 0, i2s1, 1);

AudioControlSGTL5000 audioShield;
AudioLoadSdWav load1;
__NOINIT(EXTERNAL_RAM) int16_t wavBuf1[500800];

uint32_t wavLen1=0;




extern void initHardware();
extern void updateHardware();

//=======================================================================
void setup()
{

	delay(1000);
	Serial.begin(9600);
	Serial.println("dasdsads");


	SD.begin(SdioConfig(DMA_SDIO));



	audioShield.enable();
	audioShield.volume(0.7);
	AudioMemory(200);
	Extern_SDRAM_Init();

	wavLen1=load1.loadSdWavToMemory("pppfpp.WAV",wavBuf1);



	// inicjalizacja hardware jako pierwsza
	initHardware();



	// inincjalizacja interfejsu na koncu


	mtInterface.begin();


}

//=======================================================================
void loop()
{
/*	*a=1;
	*b=2;
	*c=*a+*b;
	Serial.println(*c);*/

	uint8_t bu=Serial.read();
	if ((!(playMem1.isPlaying())) && (bu=='q'))
	{
		playMem1.play(wavBuf1,wavLen1,160,714,253,377);
	}
	if(bu=='a') playMem1.stopLoopMode();


	updateHardware();

	mtInterface.update();

}
