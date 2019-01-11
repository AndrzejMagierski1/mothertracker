
#include "mtInterface.h"
#include "mtHardware.h"
#include "load_sd_wav_to_memory.h"
#include "sdram.h"
#include "mtAudioEngine.h"


AudioLoadSdWav load1;
extern AudioPlayMemory playMem1;

__NOINIT(EXTERNAL_RAM) int16_t wavBuf1[500800];

uint32_t wavLen1=0;




extern void initHardware();
extern void updateHardware();

//=======================================================================
void setup()
{





	// inicjalizacja hardware jako pierwsza
	initHardware();



	// inincjalizacja interfejsu na koncu


	mtInterface.begin();

	wavLen1=load1.loadSdWavToMemory("pppfpp.WAV",wavBuf1);


}

//=======================================================================
void loop()
{


	uint8_t bu=Serial.read();
	if ((!(playMem1.isPlaying())) && (bu=='q'))
	{
		playMem1.play(wavBuf1,wavLen1,160,714,253,377);
	}
	if(bu=='a') playMem1.stopLoopMode();


	updateHardware();

	mtInterface.update();

}
