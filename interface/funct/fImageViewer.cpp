


#include "imageViewer.h"

#include "keyScanner.h"
#include "mtPadBoard.h"

#include "mtPadsBacklight.h"






cImageViewer imageViewer;
static cImageViewer* IV = &imageViewer;




static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functCopy();
static uint8_t functShift(uint8_t value);


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);




static  uint8_t functInstrument(uint8_t state);
static uint8_t functStepNote(uint8_t value);



static uint8_t functActionButton(uint8_t button, uint8_t state);


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);



void cImageViewer::update()
{

	if(refreshImageState == 1)
	{
		if(display.getImgLoadState() == 0 || display.getImgLoadState() == 3)
		{
			char path[40];
			sprintf(path,"Manual/manual%d", imageNumber);

			if(display.readImgFromSd(path, 300000, 800, 390))
			{
				refreshImageState = 0;
				display.setControlHide(image);
			}
			else refreshImageState = 2;
		}
	}
	else if(refreshImageState == 2)
	{
		if(display.isImgLoaded())
		{
			display.setControlShow(image);
			display.refreshControl(image);
			refreshImageState = 0;
		}
	}


}

void cImageViewer::start(uint32_t options)
{
	moduleRefresh = 1;


	// ustawienie funkcji

	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();


	countImages();
	refreshImage();


}

void cImageViewer::stop()
{
	moduleRefresh = 0;
	mtPadBoard.releaseAllInstrument();
	padsBacklight.clearAllPads(1, 1, 1);
}

void cImageViewer::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);


	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);

	//FM->setButtonObj(interfaceButtonShift, functShift);

	FM->setButtonObj(interfaceButton0, functActionButton);
	FM->setButtonObj(interfaceButton1, functActionButton);
	FM->setButtonObj(interfaceButton2, functActionButton);
	FM->setButtonObj(interfaceButton3, functActionButton);
	FM->setButtonObj(interfaceButton4, functActionButton);
	FM->setButtonObj(interfaceButton5, functActionButton);
	FM->setButtonObj(interfaceButton6, functActionButton);
	FM->setButtonObj(interfaceButton7, functActionButton);


	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setPadsGlobal(functPads);

}


void cImageViewer::refreshImage()
{
	if(imagesCount == 0)
	{
		sprintf(titleText, "Manual - no data on SD card", imageNumber, imagesCount);
		display.setControlText(titleLabel, titleText);
		display.refreshControl(titleLabel);

		display.setControlHide(image);
		refreshImageState = 0;
		return;
	}

	display.setControlHide(image);
	refreshImageState = 1;

	sprintf(titleText, "Manual %d/%d", imageNumber, imagesCount);
	display.setControlText(titleLabel,titleText);
	display.refreshControl(titleLabel);
}

void cImageViewer::nextImage()
{
	if(imagesCount <= 1) return;

	char path[40];
	uint8_t loop_start = imageNumber;

	imageNumber++;
	sprintf(path, "Manual/manual%d.jpg", imageNumber);

	while(!SD.exists(path))
	{
		imageNumber++;
		if(imageNumber > 48) imageNumber = 0;
		if(imageNumber == loop_start) break;
		sprintf(path, "Manual/manual%d.jpg", imageNumber);
	}
}


void cImageViewer::previouseImage()
{
	if(imagesCount <= 1) return;

	char path[40];
	uint8_t loop_start = imageNumber;

	imageNumber--;
	sprintf(path, "Manual/manual%d.jpg", imageNumber);

	while(!SD.exists(path))
	{
		imageNumber--;
		if(imageNumber < 1) imageNumber = 48;
		if(imageNumber == loop_start) break;
		sprintf(path,"Manual/manual%d.jpg", imageNumber);
	}
}

void cImageViewer::countImages()
{
	imagesCount = 0;
	char path[40];

	for(uint8_t i = 0; i<48; i++)
	{
		sprintf(path,"Manual/manual%d.jpg", i);

		if(SD.exists(path))
		{
			imagesCount++;
		}

	}

}

void cImageViewer::setImage(uint8_t number)
{
	if(imagesCount == 0) return;
	char path[40];

	sprintf(path,"Manual/manual%d.jpg", number);

	if(SD.exists(path))
	{
		imageNumber = number;
	}

}

//==============================================================================================================
static  uint8_t functEncoder(int16_t value)
{
	if(value > 0)
	{
		IV->nextImage();
		IV->refreshImage();
	}
	else
	{
		IV->previouseImage();
		IV->refreshImage();
	}

	return 1;
}



//=========================================================================================================
static  uint8_t functLeft()
{
	IV->previouseImage();
	IV->refreshImage();

	return 1;
}


static  uint8_t functRight()
{
	IV->nextImage();
	IV->refreshImage();

	return 1;
}


static  uint8_t functUp()
{


	return 1;
}


static  uint8_t functDown()
{



	return 1;
}



//=========================================================================================================
static  uint8_t functPlayAction()
{


	return 1;
}




static  uint8_t functRecAction()
{


	return 1;
}

static uint8_t functShift(uint8_t value)
{




	return 1;
}


static uint8_t functSwitchModule(uint8_t button)
{
	IV->eventFunct(eventSwitchModule,IV,&button,0);

	return 1;
}







//##############################################################################################
//###############################        ACTION BUTTONS        #################################
//##############################################################################################


static uint8_t functActionButton(uint8_t button, uint8_t state)
{

	if(state == buttonPress)
	{
		if(button == 0)
		{
			IV->previouseImage();
			IV->refreshImage();
		}
		else if(button == 7)
		{

			IV->nextImage();
			IV->refreshImage();
		}
	}
	else if(state == buttonRelease)
	{

	}





	return 1;
}


//##############################################################################################
//###############################          PAD BOARD           #################################
//##############################################################################################




static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{

	if(state == 1)
	{
		IV->setImage(pad+1);
		IV->refreshImage();
	}
	else if(state == 0)
	{



	}

	return 1;
}


