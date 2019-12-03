#ifndef SOURCE_MTCARDCHECKER_H_
#define SOURCE_MTCARDCHECKER_H_

#include "interface.h"


class mtCardChecker
{
public:
	void noSdCardAction();
	void sdCardInsertedAction();



private:
	hControl textControl;

	const char *mainText = "Please insert SD card to continue";

	void showText();
	void initDisplayControls();
	void deinitDisplayControls();
};

extern mtCardChecker mtCardHandler;

#endif /* SOURCE_MTCARDCHECKER_H_ */
