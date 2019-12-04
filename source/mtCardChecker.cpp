#include "mtCardChecker.h"


mtCardChecker mtCardHandler;


void mtCardChecker::initDisplayControls()
{
	strControlProperties prop;

	prop.x = 400;
	prop.y = 240;
	prop.w = 240;
	prop.h = 25;
	prop.style = (controlStyleCenterY | controlStyleCenterX);
	if(textControl == nullptr)  textControl = display.createControl<cSimpleText>(&prop);
}

void mtCardChecker::deinitDisplayControls()
{
	display.destroyControl(textControl);
	textControl = nullptr;
}

void mtCardChecker::showText()
{
	display.setControlText(textControl, mainText);
	display.setControlShow(textControl);
	display.refreshControl(textControl);
}

void mtCardChecker::noSdCardAction()
{
	uint8_t toggleState = 0;
	interfaceEnvents(eventToggleActiveModule,0,0,&toggleState);
	initDisplayControls();
	showText();
}

void mtCardChecker::sdCardInsertedAction()
{
	deinitDisplayControls();
	uint8_t toggleState = 1;
	interfaceEnvents(eventToggleActiveModule,0,0,&toggleState);
}




