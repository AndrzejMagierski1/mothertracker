
#include "configEditor/configEditor.h"
#include "configEditor/configMenu.h"


#include "configEditor/configMenuActions.h"



void setDisplayBrightness()
{
	if(mtConfig.general.dispBrightness > 2) mtConfig.general.dispBrightness = 2;

	switch(mtConfig.general.dispBrightness)
	{
	case 0:	display.setBacklightBrightness(20); break;
	case 1:	display.setBacklightBrightness(50); break;
	case 2:	display.setBacklightBrightness(128); break;
	default: break;
	}
}


void setPadsBrightness()
{
	if(mtConfig.general.padsBrightness > 2) mtConfig.general.padsBrightness = 2;

	mtConfig.values.padsLightBack = (PADS_LIGHT_BACK_DEFAULT*(mtConfig.general.padsBrightness+1))/3;
	mtConfig.values.padsLightFront = (PADS_LIGHT_FRONT_DEFAULT*(mtConfig.general.padsBrightness+1))/3;
	mtConfig.values.padsLightBackWeek = ((PADS_LIGHT_BACK_DEFAULT/2)*(mtConfig.general.padsBrightness+1))/3;
}
