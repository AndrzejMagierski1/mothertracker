
#ifndef MT_STRUCTS_H
#define MT_STRUCTS_H

#include <stdint.h>
#include "mtEnvelopeGenerator.h"
#include "mtLFO.h"

//=====================================================================
//=====================================================================
//=====================================================================

const uint16_t MAX_16BIT =              		65535;
const uint32_t SAMPLE_MEMORY_MAX =      		(8*1024*1024);
const uint8_t  INSTRUMENTS_MAX =        		32;
const uint8_t  SAMPLES_MAX =            		32;
const uint8_t  SAMPLES_FILENAME_LENGTH_MAX =    32;

const uint8_t SAMPLE_POINT_POS_MIN =            0;
const uint16_t SAMPLE_POINT_POS_MAX =           MAX_16BIT;

const  int8_t MAX_NOTE =						48;
const  int8_t MIN_NOTE =						0;

const uint8_t INSTRUMEN_ENVELOPES_MAX 		=	3;


const float MIN_PITCH 						=	0.25;
const float MAX_PITCH 						=	4.0;

const uint8_t  PANNING_MIN 					=	0;
const uint8_t  PANNING_MAX 					=   100;
const uint8_t  GLIDE_MIN 					=	0;
const uint16_t  GLIDE_MAX 					=	15000;

const uint16_t  ATTACK_MAX 					=	11000;
const uint16_t  DECAY_MAX					=	11000;
const float  	SUSTAIN_MAX					=	1.0;
const uint16_t  RELEASE_MAX					=	11000;
const float  AMOUNT_MAX						=	1.0;

const float  RESONANCE_OFFSET				=	0.7;
const float  RESONANCE_MIN					=	0;
const float  RESONANCE_MAX					=	4.3;



const uint16_t HIGH_PASS_FILTER_FREQ =			(14000/128);
const uint16_t LOW_PASS_FILTER_FREQ =			(14000/128);
const float MAX_OCTAVE_CONTROL = 				7.0;

const uint8_t MAX_TARGET =						20;
const uint8_t MAX_MOD =							5;

const float MAX_CUTOFF =						1.0;
const float MIN_CUTOFF =						0.0;

const uint8_t MAX_FINETUNE =					100;

const uint8_t MAX_WAVETABLE_WINDOW =			255;
const uint16_t STANDARD_WAVETABLE_WINDOW_LEN = 	1024;
const uint16_t SERUM_WAVETABLE_WINDOW_LEN = 	2048;

const uint16_t STANDARD_WAVETABLE_WINDOWS_NUMBER = 	256;

//=====================================================================
//=====================================================================
//=====================================================================


enum memoryPlayStatus
{
	successInit=0,
	badStartPoint,
	badLoopPoint1,
	badLoopPoint2,
	pointsBeyondFile
};

enum playMode
{
	singleShot=0,
	loopForward,
	loopBackward,
	loopPingPong,
	wavetable,

	playModeMax
};

enum mtSampleType
{
	mtSampleTypeWaveFile,
	mtSampleTypeWavetable,

	mtSampleTypeCount
};

enum envelopeEnable
{
	envelopeOff=0,
	envelopeOn
};

enum filterEnable
{
	filterOff=0,
	filterOn
};

enum filterType
{
	lowPass,
	highPass,
	bandPass,

	filterTypeMax
};

enum filterControlType
{
	manualControl,
	envelopeControlHighPass,
	envelopeControlLowPass,
	lfoControl,
};

enum envelopesType
{
    envAmp,
    envFilter,
    envPitch
};



enum envelopeTypes
{
	envelopeTypeAmp,
	envelopeTypeFilter,
	//envelopeTypePitch,

	envelopeTypeMax,
};


//=====================================================================
//=====================================================================

enum lfoType
{
	lfoA,
	lfoF,
	lfoP
};

enum lfoEnable
{
	lfoOff=0,
	lfoOn
};

enum targets
{
	noTarget = 0,
	targetAmp,
	targetPitch,
	targetSlide,
	targetGlide,
	targetCutoff,
	targetResonance,
	targetPanning,
	targetLP1,
	targetLP2,

};
enum modyficators
{
	noMod=0,
	manualMod,
	envelopeMod,
	lfoMod,
	sumOfAll = MAX_MOD
};

//=====================================================================

//parametry przetwarzanego pliku
struct strWavFileHeader
{
	uint32_t chunkId;			//0
	uint32_t chunkSize;
	uint32_t format;

	uint32_t subchunk1Id;		//12
	uint32_t subchunk1Size;
	uint16_t AudioFormat;		//20
	uint16_t numChannels;		//22
	uint32_t sampleRate;		//24
	uint32_t byteRate;			//28
	uint16_t blockAlign;		//32
	uint16_t bitsPerSample;		//34

	uint32_t subchunk2Id;		//36
	uint32_t subchunk2Size;		//40

};

struct strSerumWavetableFileHeader
{
	uint32_t chunkId;			//0
	uint32_t chunkSize;			//4
	uint32_t format;			//8
	uint16_t empty1[18];		//12
	uint32_t subchunk1Id;		//48
	uint32_t subchunk1Size;		//52
	uint16_t AudioFormat;		//56
	uint16_t numChannels;		//58
	uint32_t sampleRate;		//60
	uint32_t byteRate;			//64
	uint16_t blockAlign;		//68
	uint16_t bitsPerSample;		//70
	uint8_t  empty2[11];		//72
	uint32_t wavetableFormat;	//83
	uint8_t	 empty3[41];		//87
	uint32_t subchunk2Id;		//128
	uint32_t subchunk2Size;		//132
};


struct strSampleBank
{
	struct strSampleBankSlot
	{
		uint8_t type = 0;
		char file_name[32];

		uint8_t loaded = 0;
		int16_t *address;
		uint32_t length = 0;

		uint16_t wavetable_window_size = 0;

	} sample[SAMPLES_MAX];

	uint32_t max_memory = SAMPLE_MEMORY_MAX;
	uint32_t used_memory;
	uint8_t samples_count;

};


struct strInstrument
{
    uint8_t sampleIndex;

    char name[4];

    uint8_t  playMode;
    uint16_t startPoint;
    uint16_t loopPoint1;
    uint16_t loopPoint2;
    uint16_t endPoint;

    uint16_t wavetableWindowSize;
    uint16_t wavetableCurrentWindow;

	envelopeGenerator::strEnv envelope[3];
	LFO::strLfo lfo[3];

	float cutOff;
	float resonance;
	uint8_t filterType;
	uint8_t filterEnable;

    int8_t tune;
    int8_t fineTune;

    uint8_t volume;

    uint16_t glide;
    int16_t panning;
};





struct strMtProject
{
	strSampleBank sampleBank;
	strInstrument instrument[INSTRUMENTS_MAX];

	uint8_t instruments_count;

};



struct strMtModAudioEngine
{
	int16_t startPoint;
	int16_t endPoint;
	int16_t loopPoint1;
	int16_t loopPoint2;
	float pitchCtrl;
	uint16_t glide;


	uint8_t startPointMod;
	uint8_t endPointMod;
	uint8_t loopPoint1Mod;
	uint8_t loopPoint2Mod;
	uint8_t pitchCtrlMod;
	uint8_t glideMod;
};


extern strMtProject mtProject;




const float notes[MAX_NOTE] =
{
		0.2500, //c0
		0.2649,
		0.2806,
		0.2973,
		0.3150,
		0.3337,
		0.3536,
		0.3746,
		0.3968,
		0.4205,
		0.4455,
		0.4720,
		0.5000,//c1
		0.5298,
		0.5613,
		0.5946,
		0.6300,
		0.6675,
		0.7071,
		0.7492,
		0.7937,
		0.8409,
		0.8909,
		0.9439,
		1.0000,//c2
		1.0595,
		1.1225,
		1.1892,
		1.2599,
		1.3348,
		1.4143,
		1.4984,
		1.5874,
		1.6818,
		1.7818,
		1.8878,
		2.0000,//c3
		2.1190,
		2.2449,
		2.3785,
		2.5199,
		2.6698,
		2.8285,
		2.9967,
		3.1748,
		3.3637,
		3.5636,
		3.7756,
};
#endif
