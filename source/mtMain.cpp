
#include "mtInterface.h"
#include "mtHardware.h"
#include "mtAudioEngine.h"


extern void initHardware();
extern void updateHardware();

extern audioEngine engine;
//=======================================================================
void setup()
{





	// inicjalizacja hardware jako pierwsza
	initHardware();
	engine.init();


	// inincjalizacja interfejsu na koncu


	mtInterface.begin();




}

//=======================================================================
void loop()
{



	updateHardware();

	engine.update();
	mtInterface.update();

}
