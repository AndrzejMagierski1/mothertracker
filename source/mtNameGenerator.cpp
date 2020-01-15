#include "mtNameGenerator.h"
#include "mtHardware.h"

#include <string.h>

mtNameGenerator nameGenerator;

void mtNameGenerator::begin(uint8_t nRow, uint8_t * nColumns)
{
	if(nRow > MAX_ROW_NUMBER) return;
	numberOfRow =  nRow;
	memcpy(rowsSize,nColumns,nRow);
}

void mtNameGenerator::switchCurrentChar(uint8_t direction)
{
	uint8_t localRowLastIndex = numberOfRow-1;
	switch(direction)
	{
		case charDirectionUp:

			if(currentRow > 0) currentRow--;
			else currentRow = localRowLastIndex;

			break;
		case charDirectionDown:

			if(currentRow < localRowLastIndex) currentRow++;
			else currentRow = 0;

			break;
		case charDirectionLeft:

			if(currentColumn > 0 ) currentColumn--;
			else currentColumn = rowsSize[currentRow] - 1;

			break;
		case charDirectionRight:

			if(currentColumn < (rowsSize[currentRow] - 1) ) currentColumn++;
			else currentColumn = 0;

			break;
		default:

			break;
	}
	currentCharIndex = 0;
	for(uint8_t i = 0; i < currentRow; i++)
	{
		currentCharIndex+= rowsSize[i];
	}
	currentCharIndex+=currentColumn;

	if(shiftState) currentChar = bigTable[currentCharIndex];
	else currentChar = smallTable[currentCharIndex];

}

uint8_t mtNameGenerator::confirmChar()
{
	if(currentIndexInString > MAX_NAME_SIZE) return 0;
	name[currentIndexInString] =  currentChar;
	currentIndexInString++;
	return 1;
}

uint8_t mtNameGenerator::doBackspace()
{
	if(currentIndexInString == 0) return 0;
	currentIndexInString--;
	name[currentIndexInString] = 0;
	return 1;
}

void mtNameGenerator::setShift(uint8_t s)
{
	shiftState = s;
}

void mtNameGenerator::clearName()
{
	memset(name,0,MAX_NAME_SIZE);
	currentIndexInString = 0;
}

void mtNameGenerator::startNewName(char * defaultName, uint8_t defaultNameSize)
{
	if(defaultName != nullptr) strcpy(name,defaultName);
	currentIndexInString = defaultNameSize;
	currentRow = 0;
	currentColumn = 0;
	currentCharIndex = 0;
	shiftState = 0;
	currentChar = smallTable[currentCharIndex];
}

char * mtNameGenerator::getName()
{
	return name;
}
