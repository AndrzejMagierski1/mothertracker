#include "ArcanoidLevels.h"
#include "ArcanoidRoundCreator.h"

uint8_t level_1(void)
{
	placeBlockRow(0,11,0,50,47,1);
	placeBlockRow(11,11,0,74,47,1);
	placeBlockRow(22,11,0,98,47,1);
	placeBlockRow(33,11,0,122,47,1);

	return 44;
}

uint8_t level_2(void)
{
	placeBlockRow(0,1,0,50,47,1);
	placeBlockRow(1,2,0,74,47,1);
	placeBlockRow(3,3,0,98,47,2);
	placeBlockRow(6,4,0,122,47,2);
	placeBlockRow(10,5,0,146,47,3);
	placeBlockRow(15,6,0,170,47,4);
	placeBlockRow(21,7,0,194,47,5);
	placeBlockRow(28,8,0,218,47,6);

	return 36;
}

uint8_t level_3(void)
{
	placeBlockRow(0,11,0,50,47,1);
	placeBlockRow(11,11,0,98,47,1);
	placeBlockRow(22,11,0,146,47,1);
	placeBlockRow(33,11,0,194,47,1);

	return 44;
}

uint8_t level_4(void)
{
	placeBlockRow(0,4,0,50,47,1);
	placeBlockRow(4,4,390,50,47,1);

	placeBlockRow(8,3,225,74,47,1);

	placeBlockRow(11,4,0,98,47,1);
	placeBlockRow(15,4,390,98,47,1);

	placeBlockRow(19,3,225,122,47,1);

	placeBlockRow(22,4,0,146,47,1);
	placeBlockRow(26,4,390,146,47,1);

	placeBlockRow(30,3,225,170,47,1);

	placeBlockRow(33,4,0,194,47,1);
	placeBlockRow(37,4,390,194,47,1);

	return 41;
}

level_pntrs run_level={level_1,level_2,level_3,level_4};

uint8_t load_level(uint8_t levelnum)
{
	return run_level[levelnum-1]();
}






