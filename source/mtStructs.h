
#ifndef MT_STRUCTS_H
#define MT_STRUCTS_H

#include <stdint.h>
#include "mtEnvelopeGenerator.h"
#include "mtLFO.h"
#include "mtSequencer.h"
//=====================================================================
//=====================================================================
//=====================================================================

const uint16_t MAX_16BIT =              		65535;
const int16_t MAX_SIGNED_16BIT =				32767;
const int16_t MIN_SIGNED_16BIT =				-32768;

const float MAX_WAVE_FLOAT =					1.0;
const float MIN_WAVE_FLOAT =					-1.0;

const uint32_t SAMPLE_MEMORY_MAX =      		(8*1024*1024);

const uint8_t INSTRUMENTS_MAX =        			32;
const uint8_t SAMPLES_MAX =            			32;
const uint8_t SAMPLES_FILENAME_LENGTH_MAX =     32;

const uint8_t INSTRUMENTS_COUNT =        		32;
const uint8_t SAMPLES_COUNT =            		32;
const uint8_t PATTERNS_COUNT 	=				32;

const uint8_t PROJECT_NAME_SIZE =				32;
const uint8_t INSTRUMENT_NAME_SIZE =			19;
const uint8_t PATERN_NAME_SIZE	=				15;
const uint8_t SAMPLE_NAME_SIZE =				32;
const uint8_t PATCH_SIZE =						50;


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
const float  	AMOUNT_MAX					=	1.0;

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

const int8_t  MIN_INSTRUMENT_TUNE  	=			-24;
const uint8_t MAX_INSTRUMENT_TUNE  	=			24;
const int8_t  MIN_INSTRUMENT_FINETUNE  	=		-100;
const uint8_t MAX_INSTRUMENT_FINETUNE  	=		100;
const uint8_t MAX_INSTRUMENT_VOLUME 	=		100;


const uint8_t MAX_WAVETABLE_WINDOW =			255;
const uint16_t STANDARD_WAVETABLE_WINDOW_LEN = 	1024;
const uint16_t SERUM_WAVETABLE_WINDOW_LEN = 	2048;

const uint16_t STANDARD_WAVETABLE_WINDOWS_NUMBER = 	256;

const uint8_t LP1_MASK =						1;
const uint8_t LP2_MASK =						2;
const uint8_t FINETUNE_MASK =					4;
const uint8_t TUNE_MASK =						8;
const uint8_t VOLUME_MASK =						16;
const uint8_t PANNING_MASK =					32;
const uint8_t CUTOFF_MASK =						64;
const uint8_t RESONANCE_MASK =					128;


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

	playModeCount
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

enum inputSelect
{
	inputSelectMic,
	inputSelectLineIn
};

enum outputSelect
{
	outputSelectHeadphones,
	outputSelectLineOut
};

enum fileType
{
	fileTypeProject,
	fileTypeInstrument,
	fileTypePattern
};

enum projectType
{
	projectTypeExample,
	projectTypeUserMade
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

struct strAudioCodecConfig
{
	uint8_t inSelect;
	uint8_t outSelect;

	float headphoneVolume;
	uint8_t inputGain; // 0-63
	uint8_t mutedHeadphone;
	uint8_t mutedLineOut;
	uint8_t lineInLeft; // 0-15
	uint8_t lineInRight; // 0-15
	uint8_t lineOutLeft; // 0-15
	uint8_t lineOutRight; // 0-15

	uint8_t changeFlag;
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

    uint16_t wavetableCurrentWindow;
//    uint16_t wavetableSync;
//    uint16_t wavetablePWM;
//    uint16_t wavetableFlip;
//    uint16_t wavetableQuantize;

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

struct strMtProjectRemote
{
	struct strSampleFile
	{
		int8_t index;
		uint8_t type;
		uint16_t wavetable_window_size;
		char name[SAMPLE_NAME_SIZE];
	} sampleFile[SAMPLES_COUNT];

	struct strInstrumentFile
	{
		int8_t index;
		char name[INSTRUMENT_NAME_SIZE];
	} instrumentFile[INSTRUMENTS_COUNT];

	struct strPaternFile
	{
		int8_t index;
		char name[PATERN_NAME_SIZE];
	} patternFile[PATTERNS_COUNT];

};

struct strMtProject
{
	strSampleBank sampleBank;
	strInstrument instrument[INSTRUMENTS_MAX];

	strAudioCodecConfig audioCodacConfig;
	uint8_t instruments_count;

	strMtProjectRemote mtProjectRemote;

};

struct strProjectFileHeader
{
	char id_file[2];
	uint16_t type;
	char version[4];
	char id_data[4];
	uint16_t size;
};

struct strInstrumentFile
{
	struct strInstrumentDataAndHeader
	{
		strProjectFileHeader instrHeader;
		strInstrument instrument;

	} instrumentDataAndHeader;

	uint32_t crc;
} ;

struct strPatternFile
{
	struct strPatternDataAndHeader
	{
		strProjectFileHeader patternHeader;
		Sequencer::strPattern pattern;

	} patternDataAndHeader;

	uint32_t crc;
} ;

struct strProjectFile
{
	struct strProjectDataAndHeader
	{
		strProjectFileHeader projectHeader;
		strMtProjectRemote project;
	} projectDataAndHeader;

	uint32_t crc;
};

extern strMtProject mtProject;



const float notes[MAX_NOTE] =
{
		0.2500000000000,
		0.2648657735898,
		0.2806155120773,
		0.2973017787507,
		0.3149802624737,
		0.3337099635425,
		0.3535533905933,
		0.3745767692192,
		0.3968502629921,
		0.4204482076269,
		0.4454493590702,
		0.4719371563408,
		0.5000000000000,
		0.5297315471796,
		0.5612310241547,
		0.5946035575014,
		0.6299605249474,
		0.6674199270850,
		0.7071067811865,
		0.7491535384383,
		0.7937005259841,
		0.8408964152537,
		0.8908987181403,
		0.9438743126817,
		1.0000000000000,
		1.0594630943593,
		1.1224620483094,
		1.1892071150027,
		1.2599210498949,
		1.3348398541700,
		1.4142135623731,
		1.4983070768767,
		1.5874010519682,
		1.6817928305074,
		1.7817974362807,
		1.8877486253634,
		2.0000000000000,
		2.1189261887186,
		2.2449240966188,
		2.3784142300055,
		2.5198420997898,
		2.6696797083401,
		2.8284271247462,
		2.9966141537534,
		3.1748021039364,
		3.3635856610149,
		3.5635948725614,
		3.7754972507268
};

#endif
