#ifndef SOURCE_MTRANDOMNAMEGENERATOR_H_
#define SOURCE_MTRANDOMNAMEGENERATOR_H_
#include "mtStructs.h"

constexpr uint32_t RANDOM_NAME_TABS_SIZE = 10;
constexpr uint8_t RANDOM_NAME_WORD_1_MAX_SIZE = 10;
constexpr uint8_t RANDOM_NAME_WORD_2_MAX_SIZE = 10;
constexpr uint8_t RANDOM_NAME_MAX_SIZE = RANDOM_NAME_WORD_1_MAX_SIZE + RANDOM_NAME_WORD_2_MAX_SIZE +1;


extern const char * randomNameWord1Tab[RANDOM_NAME_TABS_SIZE];


extern const char * randomNameWord2Tab[RANDOM_NAME_TABS_SIZE];


class mtRandomNameGenerator
{
public:

	char * getRandomName();
private:
	const char ** const word1 = randomNameWord1Tab;
	const char ** const word2 = randomNameWord2Tab;
	char randomName[RANDOM_NAME_MAX_SIZE];
};

extern mtRandomNameGenerator randomNameGenerator;

#endif /* SOURCE_MTRANDOMNAMEGENERATOR_H_ */
