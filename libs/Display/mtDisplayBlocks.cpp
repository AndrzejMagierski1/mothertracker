



#include <stdint.h>

#include "mtDisplay.h"



//#########################################################################################################
//################################ metody publiczne zarzadzania ekranem ###################################
//#########################################################################################################
void cMtDisplay::setMode(uint8_t mode)
{
	screenMode = mode;
	screenRefresh = 1;
}

void cMtDisplay::setBlockType (uint8_t number, uint32_t type, uint16_t start, char * menu, uint16_t max)
{
	displayBlock[number].type = type;
	displayBlock[number].menu = menu;
	displayBlock[number].max = max;
	displayBlock[number].start = start;
	displayBlock[number].value = start;

	displayRefreshTable.block[number].center = 1;
	displayRefreshTable.block[number].title = 1;
	displayRefreshTable.block[number].label = 1;
	screenRefresh = 1;
}

void cMtDisplay::setBlockTitle(uint8_t number, char text[])
{
	for(uint8_t i = 0; i < 20; i++)	displayBlock[number].title[i] = text[i];

	displayRefreshTable.block[number].title = 1;
	screenRefresh = 1;
}

void cMtDisplay::setBlockLabel(uint8_t number, char text[])
{
	for(uint8_t i = 0; i < 20; i++)	displayBlock[number].label[i] = text[i];

	displayRefreshTable.block[number].label = 1;
	screenRefresh = 1;
}

void cMtDisplay::setBlockCenter(uint8_t number, int32_t value)
{
	displayBlock[number].value = value;

	displayRefreshTable.block[number].center = 1;
	screenRefresh = 1;
}
//#############################################################################

void cMtDisplay::setFmanagerRootTitle(char text[])
{
	for(uint8_t i = 0; i < 20; i++)	displayFmanager.rootTitle[i] = text[i];

	displayRefreshTable.fManager.rootTitle = 1;
	screenRefresh = 1;
}

void cMtDisplay::setFmanagerRootList(uint16_t start, char * list, uint16_t count, uint8_t filename_length)
{
	fManagerList.setList(start, list, count, filename_length);

	displayRefreshTable.fManager.rootList = 1;
	screenRefresh = 1;
}


void cMtDisplay::setFmanagerRootListPos(uint16_t position)
{
	fManagerList.setListPos(position);

	displayRefreshTable.fManager.rootList = 1;
	screenRefresh = 1;
}


void cMtDisplay::setFmanagerLabels(char * labels)
{
	displayFmanager.labels = labels;

	displayRefreshTable.fManager.labels = 1;
	screenRefresh = 1;
}

//#############################################################################

void cMtDisplay::setSampleEditorLabels(char * labels)
{

}

