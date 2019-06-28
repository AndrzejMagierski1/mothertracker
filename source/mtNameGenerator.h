#ifndef SOURCE_MTNAMEGENERATOR_H_
#define SOURCE_MTNAMEGENERATOR_H_
#include "mtHardware.h"

constexpr uint8_t MAX_NAME_SIZE = 100;
constexpr uint8_t MAX_ROW_NUMBER = 6;

constexpr char smallTable[] =
{
	'1', '2', '3' ,'4' ,'5' ,'6' ,'7' ,'8' ,'9' ,'0' ,
	'q', 'w', 'e' ,'r' ,'t' ,'y' ,'u' ,'i' ,'o' ,'p' ,'^' ,'+',
	'a', 's', 'd' ,'f' ,'g' ,'h' ,'j' ,'k' ,'l' ,'~' ,'@' ,'!',
	'z', 'x', 'c' ,'v' ,'b' ,'n' ,'m' ,',' ,'.' ,'(' ,')' ,'-'
};

constexpr char bigTable[] =
{
	'1', '2', '3' ,'4' ,'5' ,'6' ,'7' ,'8' ,'9' ,'0' ,
	'Q', 'W', 'E' ,'R' ,'T' ,'Y' ,'U' ,'I' ,'O' ,'P' ,'^' ,'+',
	'A', 'S', 'D' ,'F' ,'G' ,'H' ,'J' ,'K' ,'L' ,'~' ,'@' ,'!',
	'Z', 'X', 'C' ,'V' ,'B' ,'N' ,'M' ,',' ,'.' ,'(' ,')' ,'-'
};
enum  charDirection
{
	charDirectionUp,
	charDirectionDown,
	charDirectionLeft,
	charDirectionRight
};

class mtNameGenerator
{
public:
	void begin(uint8_t nRow, uint8_t * nColumns);
	void switchCurrentChar(uint8_t direction);
	uint8_t confirmChar(); // 0 zwraca blad
	uint8_t doBackspace(); // 0 zwraca blad
	void setShift(uint8_t s);
	void clearName();
	void startNewName(char * defaultName = nullptr, uint8_t defaultNameSize = 0);
	char * getName();
private:

	char name[MAX_NAME_SIZE];
	char currentChar;
	uint8_t rowsSize[MAX_ROW_NUMBER];
	uint8_t currentRow;
	uint8_t currentColumn;
	uint8_t currentCharIndex;
	uint8_t currentIndexInString;
	uint8_t numberOfRow;
	uint8_t shiftState;
};


extern mtNameGenerator nameGenerator;

#endif /* SOURCE_MTNAMEGENERATOR_H_ */
