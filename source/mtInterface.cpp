
#include "mtDisplay.h"

#include "mtInterface.h"
#include "mtSampleEditor.h"

extern void sampleEditorEvent(uint8_t event, void* param);



cMtInterface mtInterface;





//=======================================================================
void cMtInterface::begin()
{
	mtSampleEditor.begin();

}



//=======================================================================
void cMtInterface::update()
{


	mtSampleEditor.update();


}
