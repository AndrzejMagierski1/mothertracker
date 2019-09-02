
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

const uint8_t INSTRUMENTS_MAX =        			47;
//const uint8_t SAMPLES_MAX =            			31;

const uint8_t INSTRUMENTS_COUNT =        		48;
//const uint8_t SAMPLES_COUNT =            		32;
const uint8_t PATTERNS_COUNT 	=				32;
const uint8_t PATTERN_LENGTH_MAX 	=			255;

const uint8_t PATTERN_INDEX_MIN 	=			1;
const uint8_t PATTERN_INDEX_MAX 	=			255;





const uint8_t PATTERN_EDIT_STEP_MAX 	=		10;


const uint8_t PROJECT_NAME_SIZE =				32;
const uint8_t INSTRUMENT_NAME_SIZE =			19;
const uint8_t PATTERN_NAME_SIZE	=				15;
const uint8_t SAMPLE_NAME_SIZE =				32;

const uint8_t PATCH_SIZE =						255;
const uint8_t SONG_MAX =						100;

const uint8_t SAMPLE_POINT_POS_MIN =            0;
const uint16_t SAMPLE_POINT_POS_MAX =           MAX_16BIT;

const  int8_t MAX_NOTE =						49;
const  int8_t MIN_NOTE =						0;

const uint8_t INSTRUMEN_ENVELOPES_MAX 		=	3;


const float MIN_PITCH 						=	0.25;
const float MAX_PITCH 						=	4.0;

const int8_t  PANNING_MIN 					=	-100;
const uint8_t  PANNING_MAX 					=   100;
const uint8_t  GLIDE_MIN 					=	0;
const uint16_t  GLIDE_MAX 					=	15000;
const uint8_t  REVERB_SEND_MIN 				=	0;
const uint8_t  REVERB_SEND_MAX				=	100;

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
const uint16_t REVERB_SEND_MASK =				256;

const uint8_t MIN_NOTE_OFFSET =					0;
const uint8_t MAX_NOTE_OFFSET =					13;




const uint8_t MASTER_VOLUME_MIN 			=	0;
const uint8_t MASTER_VOLUME_MAX 			=	100;

const uint8_t INPUT_MIC_GAIN_DEFAULT 		=	35;

const uint8_t LINE_IN_SENS_LEVEL_DEFAULT 	=	15;
const uint8_t LINE_OUT_LEVEL_DEFAULT 		=	29;



const uint8_t REVERB_ROOM_SIZE_MIN 			=	0;
const uint8_t REVERB_ROOM_SIZE_MAX 			=	100;
const uint8_t REVERB_DAMPING_MIN 			=	0;
const uint8_t REVERB_DAMPING_MAX 			=	100;
const int8_t REVERB_PANNING_MIN 			=	-100;
const uint8_t REVERB_PANNING_MAX 			=	100;


const uint8_t LIMITER_ATTACK_MIN 			=	0;
const uint16_t LIMITER_ATTACK_MAX  			=	1000;
const uint8_t LIMITER_RELEASE_MIN  			=	0;
const uint16_t LIMITER_RELEASE_MAX  		=	5000;
const uint8_t LIMITER_TRESHOLD_MIN 			=	0;
const uint16_t LIMITER_TRESHOLD_MAX 		=	32767;



const uint8_t CONFIG_EEPROM_ADDRESS			=	0;

const float ZOOM_MIN 						=	1;
const float ZOOM_MAX 						=	10;
const float ZOOM_FACTOR 					=	0.05;


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

enum instrumentPlayMode
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

//---------------------------------------
enum inputSelect
{
	inputSelectLineIn,
	inputSelectMic,

	inputSelectCount
};

enum outputSelect
{
	outputSelectLineOut,
	outputSelectHeadphones,

	outputSelectCount
};
//---------------------------------------

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

enum recorderMode
{
	recorderModeStop,
	recorderModeRec
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

enum enPlaylist
{
	PLAYLIST_EMPTY_SLOT = 0,
};

//=====================================================================
//-------------------------------------------------
//-------------------------------------------------
struct strInstrument
{
	uint8_t isActive;

	struct strSampleBankSlot
	{
		uint8_t type = 0;
		char file_name[32];

		uint8_t loaded = 0;
		int16_t *address;
		uint32_t length = 0;

		uint16_t wavetable_window_size = 0;

	} sample;


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

    uint8_t reverbSend;
};
//-------------------------------------------------
struct strMtValues
{
	uint8_t lastUsedInstrument = 0;
	uint8_t actualPattern = 1;

	uint8_t padBoardScale = 0;
	uint8_t padBoardNoteOffset = 12;
	uint8_t padBoardRootNote = 0;
	uint8_t padBoardMaxVoices = 8;

	uint8_t volume;

	uint8_t reverbRoomSize;
	uint8_t reverbDamping;
	int8_t reverbPanning;

	uint16_t limiterAttack;
	uint16_t limiterRelease;
	uint16_t limiterTreshold;

	int8_t patternEditStep = 1;

};

//-------------------------------------------------
struct strMtProjectRemote
{
	struct strInstrumentFile
	{
		int8_t index= -1;
		char name[INSTRUMENT_NAME_SIZE];
		struct strSample
		{
			uint8_t type;
			char name[SAMPLE_NAME_SIZE];
		} sample;

	} instrumentFile[INSTRUMENTS_COUNT];

	struct strPaternFile
	{
		int8_t index= -1;
//		char name[PATTERN_NAME_SIZE];
	} patternFile[PATTERNS_COUNT];

	struct strSong
	{
		int16_t playlist[SONG_MAX] { 1, 2, 3, 4, 0 };
//		int8_t mode = SONGMODE_PATTERN;
		int8_t playlistPos = 0;
	} song;


	strMtValues values;

};
//-------------------------------------------------
struct strMtProject
{
	strInstrument instrument[INSTRUMENTS_COUNT];
	uint8_t instruments_count;
	uint8_t patterns_count;

	strMtProjectRemote mtProjectRemote;


	strMtValues values;

	// dynamiczne
	uint32_t max_memory = SAMPLE_MEMORY_MAX;
	uint32_t used_memory;
	uint8_t samples_count;

};

//-------------------------------------------------
struct strMtConfig
{
	struct strGlobalValues
	{
		uint8_t masterVolume = 50;
	} globals;

	struct strAudioCodecConfig
	{
		uint8_t inSelect;
		uint8_t outSelect;

		float headphoneVolume = 0.6;
		uint8_t inputGain; // 0-63
		uint8_t mutedHeadphone;
		uint8_t mutedLineOut;

		uint8_t lineInLeft; // 0-15
		uint8_t lineInRight; // 0-15

		uint8_t lineOutLeft; // 13-31
		uint8_t lineOutRight; //13-31

		uint8_t changeFlag;

	} audioCodecConfig;
};

//-------------------------------------------------

extern strMtProject mtProject;
extern strMtConfig 	mtConfig;



struct strMtDispSpectrum
{
	uint8_t spectrumType = 0;
	int8_t upperData[480];
	int8_t lowerData[480];

	uint8_t pointsType = 0;
	int16_t startPoint;
	int16_t endPoint;
	int16_t loopPoint1;
	int16_t loopPoint2;
};

struct strMtDispValues
{
	uint8_t type[5];
	int16_t value1[5];
//	int16_t value2[MT_DISP_VALUES_MAX];
//	int16_t value3[MT_DISP_VALUES_MAX];
//	int16_t value4[MT_DISP_VALUES_MAX];
//	int16_t value5[MT_DISP_VALUES_MAX];
};

struct strMtDispMultiRowValues
{
	uint8_t type[5];
	char** labels[5];
	int16_t values[5][5];
};


struct strMtDispEnvelope
{
	uint8_t type;
	char * names[INSTRUMEN_ENVELOPES_MAX];

	uint8_t delay;
	uint8_t attack;
	uint8_t hold;
	uint8_t decay;
	uint8_t	sustain;
	uint8_t release;

	uint8_t amount;
};

struct strMtDispTrackTable
{
	struct strStepParams
	{
		uint8_t mode;
		uint8_t iVal1;
		uint8_t iVal2;
		uint8_t iVal3;
		int8_t  iVal4;  // velocity
	}params[5];

	struct strStepFx
	{
		uint8_t mode;
		char* name;
		char* cVal1;
		char* cVal2;
		uint8_t iVal1;
		uint8_t iVal2;

	}fx1[5],fx2[5],fx3[5],fx4[5];

	uint8_t active[5]; // aktualne edytowana wartosc w kazdej kolumnie (params/fxs)
	uint8_t state[5];  // czy step lezy w sekwencji
};


#endif
