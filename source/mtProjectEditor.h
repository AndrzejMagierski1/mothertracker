
#ifndef MT_PROJECT_MANAGER_H
#define MT_PROJECT_MANAGER_H

#include <stdint.h>


//=====================================================================
#define SAMPLE_MEMORY_MAX					8*1024*1024
#define INSTRUMENTS_MAX					32
#define SAMPLES_MAX						32
#define SAMPLES_FILENAME_LENGTH_MAX		32



//=====================================================================

struct strInstrument
{
	uint8_t sampleIndex;

	uint8_t playMode;
	uint32_t start_point;
	uint32_t loop_point1;
	uint32_t loop_point2;
	uint32_t end_point;

	uint16_t amp_delay;
	uint16_t amp_attack;
	uint16_t amp_hold;
	uint16_t amp_decay;
	uint16_t amp_sustain;
	uint16_t amp_release;

	uint16_t panning;
};



struct strSampleBank
{
	struct strSampleBankSlot
	{
		uint8_t loaded;
		uint8_t index;
		uint16_t *address;
		char *file_name;

	} sample[SAMPLES_MAX];

	uint32_t max_memory = SAMPLE_MEMORY_MAX;
	uint32_t used_memory;
	uint8_t samples_count;

};

struct strMtProject
{
	strSampleBank sampleBank;
	strInstrument instrument[INSTRUMENTS_MAX];

	uint8_t instruments_count;

};

class cMtProjectEditor
{
public:
	uint8_t readProjectConfig();
	uint8_t loadInstruments();
	uint8_t loadSamples();
	uint8_t loadProject();
	uint8_t isProjectLoaded();



private:





};


extern cMtProjectEditor mtProjectEditor;
extern strMtProject project;



#endif
