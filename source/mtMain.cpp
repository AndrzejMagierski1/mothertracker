
#include "mtInterface.h"
#include "mtHardware.h"
//#include "load_sd_wav_to_memory.h"
//#include "sdram.h"
#include "mtAudioEngine.h"


//AudioLoadSdWav load1;
//extern AudioPlayMemory playMem1;

//__NOINIT(EXTERNAL_RAM) int16_t wavBuf1[500800];

//uint32_t wavLen1=0;




extern void initHardware();
extern void updateHardware();

//=======================================================================
void setup()
{





	// inicjalizacja hardware jako pierwsza
	initHardware();



	// inincjalizacja interfejsu na koncu


	mtInterface.begin();

//	wavLen1=load1.loadSdWavToMemory("pppfpp.WAV",wavBuf1);


}

//=======================================================================
void loop()
{



	updateHardware();

	mtInterface.update();

}
