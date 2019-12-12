#include <mtRandomNameGenerator.h>

const char * randomNameWord1Tab[RANDOM_NAME_TABS_SIZE] =
{
		"aaa","bbb","ccc","ddd","eee","fff","ggg","hhh","jjj","kkk"
};

const char * randomNameWord2Tab[RANDOM_NAME_TABS_SIZE] =
{
		"000","111","222","333","444","555","666","777","888","999"
};

mtRandomNameGenerator randomNameGenerator;

char * mtRandomNameGenerator::getRandomName()
{
	uint16_t rand1 = random(0,RANDOM_NAME_TABS_SIZE-1);
	uint16_t rand2 = random(0,RANDOM_NAME_TABS_SIZE-1);

	sprintf(randomName,"%s_%s",word1[rand1],word2[rand2]);

	return randomName;
}
