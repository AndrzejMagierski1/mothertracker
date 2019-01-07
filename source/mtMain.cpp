
#include "mtInterface.h"
#include "mtHardware.h"



extern void initHardware();
extern void updateHardware();

//=======================================================================
void setup()
{

	// inicjalizacja hardware jako pierwsza
	initHardware();




	// inincjalizacja interfejsu na koncu


	mtInterface.begin();


}

//=======================================================================
void loop()
{

	updateHardware();

	mtInterface.update();

}
