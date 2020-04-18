
#ifndef MT_STRUCTS_H
#define MT_STRUCTS_H

#include <stdint.h>
#include "mtEnvelopeGenerator.h"

//=====================================================================
//=====================================================================
//=====================================================================



const uint8_t FV_VER_1	=					0;		// device version
const uint8_t FV_VER_2 =					9;		// official update
const uint8_t FV_VER_3 =					68;		// fix version  100 = brak 3 litery
const uint8_t FV_BETA 	=					1;		// 0/1 - dopisek beta

const uint8_t PROJECT_FILE_VERSION 	=		2;		// wersja struktury pliku projektu
const uint8_t INSTRUMENT_FILE_VERSION 	=	3;		// wersja struktury pliku instrumentu
const uint8_t PATTERN_FILE_VERSION =		1;
const uint8_t EEPROM_STRUCT_VER =			1;


const float DEFAULT_TEMPO = 130;

const uint32_t CONFIG_EEPROM_ADRESS =        	0;

const uint16_t MAX_16BIT =              		65535;
const int16_t MAX_SIGNED_16BIT =				32767;
const int16_t MIN_SIGNED_16BIT =				-32768;
const uint8_t MAX_8BIT = 						255;

const float MAX_WAVE_FLOAT =					1.0;
const float MIN_WAVE_FLOAT =					-1.0;

const uint32_t SOUND_MEMORY_TOTAL =      		(15*1024*1024); // 15 MB
const uint32_t SAMPLE_MEMORY_SIZE = 			(45*256*1024);  // 11,25 MB
const uint32_t REC_EDIT_MEM_OFFEST =      		(15*512*1024);  // 7,5 MB

const uint8_t INSTRUMENTS_MAX =        			47;
//const uint8_t SAMPLES_MAX =            			31;

const uint8_t INSTRUMENTS_COUNT =        		48;
//const uint8_t SAMPLES_COUNT =            		32;
//const uint8_t PATTERN_LENGTH_MAX 	=			255;
const float   GLOBAL_TEMPO_MAX 		=			1000;

const uint8_t PATTERN_INDEX_MIN 	=			1;
const uint8_t PATTERN_INDEX_MAX 	=			255;

const uint8_t FX_MAX 			=        		47;
const uint8_t FX_MAX_FOR_RANDOM	=        		40;
const uint8_t FX_COUNT		 	=        		33;
const uint8_t FX_COUNT_HIDDEN_FXes		 	=	1; // ile fxów na końcu listy jest ukrytych

const uint8_t FX_VALUE_MAX	=					127;

const uint8_t PATTERN_EDIT_STEP_MAX 	=		32;

const uint8_t FX_SLOTS_MAX		 	=       	1;
const uint8_t FX_SLOTS_COUNT	 	=       	2;


const uint8_t PROJECT_NAME_SIZE =				32;
const uint8_t INSTRUMENT_NAME_SIZE =			19;
const uint8_t PATTERN_NAME_SIZE	=				15;
const uint8_t SAMPLE_NAME_SIZE =				31;
const uint8_t FX_NAME_SIZE =					32;

const uint8_t PATCH_SIZE =						255;
const uint8_t SONG_MAX =						255;

const uint8_t SAMPLE_POINT_POS_MIN =            0;
const uint16_t SAMPLE_POINT_POS_MAX =           MAX_16BIT;

const  int8_t MAX_NOTE =						120;
const  int8_t MIN_NOTE =						0;

const  uint8_t DEFAULT_MIDI_VELOCITY 		=	127;

const uint8_t INSTRUMEN_ENVELOPES_MAX 		=	3;


const float MIN_PITCH 						=	0.25;
const float MAX_PITCH 						=	4.0;

const int8_t  PANNING_MIN 					=	0;
const uint8_t  PANNING_MAX 					=   100;
const uint8_t  GLIDE_MIN 					=	0;
const uint16_t  GLIDE_MAX 					=	15000;
const uint8_t  REVERB_SEND_MIN 				=	0;
const uint8_t  REVERB_SEND_MAX				=	100;

const uint16_t  ATTACK_MAX 					=	10000; // zastosowane jest dzielenie przez 100 bez floatow wiec wazne zeby wartosci czasowe max env byly podzielne przez 100
const uint16_t  DECAY_MAX					=	10000;
const float  	SUSTAIN_MAX					=	1.0;
const uint16_t  RELEASE_MAX					=	10000;
const float  	AMOUNT_MAX					=	1.0;
const uint16_t ENVELOPE_MICRO_VAL 			= 	100;	// na ile wartosci rozklada sie zakres micro
const uint16_t ENVELOPE_MICRO_RANGE			= 	1000;

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

const int8_t  MIN_INSTRUMENT_TUNE  			=	-24;
const uint8_t MAX_INSTRUMENT_TUNE  			=	24;
const int8_t  MIN_INSTRUMENT_FINETUNE  		=	-100;
const uint8_t MAX_INSTRUMENT_FINETUNE  		=	100;
const uint8_t MAX_INSTRUMENT_VOLUME 		=	100;
const uint8_t MIN_INSTRUMENT_VOLUME			=	 0;

const uint8_t MAX_WAVETABLE_WINDOW =			255;
const uint16_t STANDARD_WAVETABLE_WINDOW_LEN = 	1024;
const uint16_t SERUM_WAVETABLE_WINDOW_LEN = 	2048;

const uint16_t STANDARD_WAVETABLE_WINDOWS_NUMBER = 	256;

constexpr uint32_t LP1_MASK =						0b00000000000000000000000000000001;
constexpr uint32_t LP2_MASK =						0b00000000000000000000000000000010;
constexpr uint32_t FINETUNE_MASK =					0b00000000000000000000000000000100;
constexpr uint32_t TUNE_MASK =						0b00000000000000000000000000001000;
constexpr uint32_t VOLUME_MASK =					0b00000000000000000000000000010000;
constexpr uint32_t PANNING_MASK =					0b00000000000000000000000000100000;
constexpr uint32_t CUTOFF_MASK =					0b00000000000000000000000001000000;
constexpr uint32_t RESONANCE_MASK =					0b00000000000000000000000010000000;
constexpr uint32_t DELAY_SEND_MASK =				0b00000000000000000000000100000000;
constexpr uint32_t WT_POS_SEND_MASK =				0b00000000000000000000001000000000;
constexpr uint32_t GRANULAR_POS_SEND_MASK =			0b00000000000000000000010000000000;
constexpr uint32_t GRANULAR_LEN_SEND_MASK =			0b00000000000000000000100000000000;
constexpr uint32_t GRANULAR_WAVE_SEND_MASK =		0b00000000000000000001000000000000;
constexpr uint32_t GRANULAR_LOOP_SEND_MASK =		0b00000000000000000010000000000000;
constexpr uint32_t LFO_AMP_SEND_MASK =				0b00000000000000000100000000000000;
constexpr uint32_t LFO_FILTER_SEND_MASK =			0b00000000000000001000000000000000;
constexpr uint32_t LFO_WT_POS_SEND_MASK =			0b00000000000000010000000000000000;
constexpr uint32_t LFO_GRAN_POS_SEND_MASK =			0b00000000000000100000000000000000;
constexpr uint32_t LFO_PANNING_SEND_MASK =			0b00000000000001000000000000000000;
constexpr uint32_t LFO_FINETUNE_SEND_MASK =			0b00000000000010000000000000000000;

const uint8_t MIN_NOTE_OFFSET =					0;
const uint8_t MAX_NOTE_OFFSET =					13;




const uint8_t MASTER_VOLUME_MIN 			=	0;
const uint8_t MASTER_VOLUME_MAX 			=	100;
const uint8_t MASTER_VOLUME_DEFAULT			=	50;

const uint8_t BIT_DEPTH_MIN 				=	4;
const uint8_t BIT_DEPTH_MAX 				=	16;

const uint8_t INPUT_MIC_GAIN_DEFAULT 		=	35;

const uint8_t LINE_IN_SENS_LEVEL_DEFAULT 	=	15;
const uint8_t LINE_OUT_LEVEL_DEFAULT 		=	29;



const uint8_t REVERB_ROOM_SIZE_MIN 			=	0;
const uint8_t REVERB_ROOM_SIZE_MAX 			=	100;
const uint8_t REVERB_DAMPING_MIN 			=	0;
const uint8_t REVERB_DAMPING_MAX 			=	100;
const int8_t REVERB_PANNING_MIN 			=	-100;
const uint8_t REVERB_PANNING_MAX 			=	100;

const uint8_t DELAY_FEEDBACK_MIN			= 	0;
const uint8_t DELAY_FEEDBACK_MAX			=	70;
const uint16_t DELAY_TIME_MIN				= 	1;
const uint16_t DELAY_TIME_MAX				=	3500;

const uint8_t LIMITER_ATTACK_MIN 			=	1;
const uint16_t LIMITER_ATTACK_MAX  			=	1000;
const float LIMITER_RELEASE_MIN  			=	0.02;
const float LIMITER_RELEASE_MAX  			=	10;
const uint8_t LIMITER_TRESHOLD_MIN 			=	0;
const uint16_t LIMITER_TRESHOLD_MAX 		=	32767;

const uint8_t MAX_SLICE_NUMBER				=	48;

const uint8_t CONFIG_EEPROM_ADDRESS			=	0;

const float ZOOM_MIN 						=	1;
const float ZOOM_MAX 						=	10;
const float ZOOM_FACTOR 					=	0.05;

const int8_t PERFORMANCE_VALUE_MAX			= 	100;
const int8_t PERFORMANCE_VALUE_MIN			= 	- 100;



// CONFIG DEFINES

const uint8_t PADS_LIGHT_BACK_DEFAULT 		=	15;
const uint8_t PADS_LIGHT_FRONT_DEFAULT 		=	31;

const uint16_t MIN_GRANULAR_LENGTH			= 44;
const uint16_t MAX_GRANULAR_LENGTH			= 44100;

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
	playModeSingleShot = 0,
	playModeLoopForward,
	playModeLoopBackward,
	playModePingpong,
	playModeSlice,
	playModeBeatSlice,
	playModeWavetable, //tylko na uzytek interface - sample type obsługuje to w silniku
	playModeGranular,
	playModeCount
};

enum mtSampleType
{
	mtSampleTypeWaveFile,
	mtSampleTypeWavetable,

	mtSampleTypeCount
};

//enum filterEnable
//{
//	filterOff=0,
//	filterOn
//};

enum filterType
{
	lowPass,
	highPass,
	bandPass,

	filterTypeMax
};


enum envelopesType
{
    envAmp,
	envPan,
    envCutoff,
	envWtPos,
	envGranPos,
    envFinetune,


	envMax
};

enum lfoShapeType
{
	lfoShapeReverseSaw,
	lfoShapeSaw,
	lfoShapeTriangle,
	lfoShapeSquare,


	lfoShapeMax
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
	fileTypePattern,
	fileTypeSample,
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
enum enPlaylist
{
	PLAYLIST_EMPTY_SLOT = 0,
};


enum interfaceCommands
{
	interfaceDoNothing,
	interfaceOpenLastProject,

	interfaceCommandsCount
};

enum loaderStateType
{
	loaderStateTypeEnded,
	loaderStateTypeInProgress
};

enum granularShapeType
{
	granularShapeSquare,
	granularShapeTriangle,
	granularShapeGauss,

	granularTypeCount
};

enum granularLoopType
{
	granularLoopForward,
	granularLoopBackward,
	granularLoopPingPong,

	granularLoopCount
};


enum powerStateType
{
	powerStateSleep,
	powerStateRun
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

		int16_t *address;
		uint32_t length = 0;

		uint16_t wavetable_window_size = 2048;
		uint32_t wavetableWindowNumber = 0;

	} sample;


    char name[4];

    uint8_t  playMode;
    uint16_t startPoint;
    uint16_t loopPoint1;
    uint16_t loopPoint2;
    uint16_t endPoint;

    uint32_t wavetableCurrentWindow;
//    uint16_t wavetableSync;
//    uint16_t wavetablePWM;
//    uint16_t wavetableFlip;
//    uint16_t wavetableQuantize;

	envelopeGenerator::strEnv envelope[envMax];
	struct strEnvBasedLfo
	{
		uint8_t shape = 0;
		uint8_t speed = 0;
		float 	amount = 1.0;
	} lfo[envMax];
//	LFO::strLfo lfo[lfoMax];

	float cutOff;
	float resonance;
	uint8_t filterType;
	uint8_t filterEnable;

    int8_t tune;
    int8_t fineTune;

    uint8_t volume;

    uint16_t glide;
    int16_t panning;

    uint8_t delaySend;

    uint16_t slices[MAX_SLICE_NUMBER];
    uint8_t sliceNumber;
    uint8_t selectedSlice;

    struct strGranular
    {
    	uint16_t 	grainLength = 44;
    	uint16_t 	currentPosition;
    	uint8_t	 	shape;
    	uint8_t		type;
    } granular;
};
//-------------------------------------------------
struct strMtValues
{
	uint8_t lastUsedNote = 0;
	uint8_t lastUsedInstrument = 0;
	uint8_t lastUsedVolume = 0;
	uint8_t lastUsedFx = 0;

	uint8_t actualPattern = 1;

	uint8_t padBoardScale = 0;
	uint8_t padBoardNoteOffset = 12;
	uint8_t padBoardRootNote = 36;
	uint8_t padBoardMaxVoices = 8;

	uint8_t volume = 50;

//	uint8_t reverbRoomSize = 80;
//	uint8_t reverbDamping = 25;
//	int8_t reverbPanning;

	uint8_t delayFeedback;
	uint16_t delayTime;
	uint8_t delayParams; // 7 - PINGPONG ENABLE , 6 - SYNC ENABLE, 5-0 SYNC RATE

	uint16_t limiterAttack = 100;
	float limiterRelease = 0.512;
	uint16_t limiterTreshold = 16384;

	uint8_t bitDepth = 16;

	int8_t patternEditStep = 1;


	uint8_t trackMute[8] = {0,0,0,0,0,0,0,0};


//********************************* radio
	uint8_t source;
	uint8_t gainLineIn;
	uint8_t gainMicLow;
	uint8_t gainMicHigh;
	uint8_t gainRadio;
	uint8_t monitor;
	float radioFreq = 87.5;

	uint8_t projectNotSavedFlag = 0; // 2 = nie ma folderu w Projects, 1 = nie aktualny wzgledem Projects

// performance 1/2
	uint16_t perfTracksPatterns[8]  = {1,1,1,1,1,1,1,1};
	uint8_t perfFxPlaces[12] 		= {1,2,3,4,5,6,7,8,9,10,11,12}; // jakie efekty w 12 slotach
	int16_t perfFxValues[12][4] 	= {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}};

// song
	float globalTempo = DEFAULT_TEMPO;
	uint16_t patternLength = 32;

	uint8_t instrumentsToSave[INSTRUMENTS_COUNT];
	uint8_t patternsToSave[PATTERN_INDEX_MAX];

// performance 2/2
	uint8_t perfSelectedValues[12] 	= {0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t perfTracksState[8] = {0,0,0,0,0,0,0,0}; // narazie nie uzywana


	struct strMidiInstr
	{
		uint8_t velocity = DEFAULT_MIDI_VELOCITY;

	} midiInstrument[16];

	uint8_t allPatternsBitmask[255];

};

struct strSong
{
	uint8_t playlist[SONG_MAX] { 1 };
	int8_t playlistPos = 0;
};

//-------------------------------------------------
struct strMtProjectRemote
{
	strSong song;
	strMtValues values;
	char projectName[PROJECT_NAME_SIZE]; // uzywane tlyko zeby rozpoznac projekt w workspace

};
//-------------------------------------------------
struct strMtProject
{
	strInstrument instrument[INSTRUMENTS_COUNT];
	uint8_t instruments_count; // uzywane tylko do oblizania postepu w czasie ladowania
	uint8_t patterns_count;    // nie uzywane

	strSong song;
	strMtValues values;

	// dynamiczne
	uint32_t max_memory = SAMPLE_MEMORY_SIZE;
	uint32_t used_memory;

};

//-------------------------------------------------
struct strMtConfig
{
	struct strStartup
	{
		uint8_t startMode = interfaceOpenLastProject;
		char lastProjectName[PROJECT_NAME_SIZE];
		uint8_t powerState = powerStateSleep;

	} startup;

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

		uint8_t lineOutLeft; // 13-31
		uint8_t lineOutRight; //13-31

		//uint8_t changeFlag;

	} audioCodecConfig;

	struct strVersion
	{
		uint8_t ver_1;
		uint8_t ver_2;
		uint8_t ver_3;
		uint8_t beta;
		//uint8_t memoryStructVer;
		uint8_t eepromStructVer;

	} firmware;

	struct strMIDIValues
	{
		uint8_t clkIn;
		uint8_t clkOut;
		uint8_t transportIn;
		uint8_t transportOut;
		uint8_t notesInMode;
		uint8_t notesInChannel; // od 0 = all channels
		uint8_t notesOutMode;
		uint8_t notesOutChannel; // od 0 = channel 1
		uint8_t ccOut[10];
	} midi;

	struct strGlobalValues
	{
		uint8_t padsLightBack = PADS_LIGHT_BACK_DEFAULT;
		uint8_t padsLightFront = PADS_LIGHT_FRONT_DEFAULT;
		uint8_t padsLightBackWeek = PADS_LIGHT_BACK_DEFAULT/2;

		uint8_t padBoardScale;
		uint8_t padBoardNoteOffset;
		uint8_t padBoardRootNote;
		uint8_t padBoardMaxVoices;

		uint16_t perfTracksPatterns[8]  = {1,1,1,1,1,1,1,1};
		uint8_t perfFxPlaces[12] 		= {1,2,3,4,5,6,7,8,9,10,11,12}; // jakie efekty w 12 slotach
		int16_t perfFxValues[12][4] 	= {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}};
		uint8_t perfSelectedValues[12] 	= {0,0,0,0,0,0,0,0,0,0,0,0};
		uint8_t perfTracksState[8] 		= {0,0,0,0,0,0,0,0}; // narazie nie uzywana

	} values;


	struct strGeneralValues
	{
		uint8_t patternDiv;
		uint8_t radioRegion;
		uint8_t dispBrightness;
		uint8_t padsBrightness;
		uint8_t mtpState;

		uint8_t recQuantization;
		uint8_t performanceSource;

		uint8_t padBoardScale;
		uint8_t padBoardNoteOffset;
		uint8_t padBoardRootNote;


	} general;

	struct strInterfaceState
	{
		uint8_t fxPopupDescription;
	} interface;

	struct strMetronome
	{
		uint8_t state; // 0-1
		uint8_t timeSignature; // 0-11 - do zmiany pewnie
		uint8_t volume; // 0-100
	} metronome;



	struct strDebugState
	{
		uint8_t debugLogState;
	} debug;

	uint32_t arcanoidHighestScore;

	uint8_t equalSliceNumber = 1;
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

const char mtNotes[128][5] =
{
	"C0",	"C#0",	"D0",	"D#0",	"E0",	"F0",	"F#0",	"G0",	"G#0",	"A0",	"A#0",	"B0",
	"C1",	"C#1",	"D1",	"D#1",	"E1",	"F1",	"F#1",	"G1",	"G#1",	"A1",	"A#1",	"B1",
	"C2",	"C#2",	"D2",	"D#2",	"E2",	"F2",	"F#2",	"G2",	"G#2",	"A2",	"A#2",	"B2",
	"C3",	"C#3",	"D3",	"D#3",	"E3",	"F3",	"F#3",	"G3",	"G#3",	"A3",	"A#3",	"B3",
	"C4",	"C#4",	"D4",	"D#4",	"E4",	"F4",	"F#4",	"G4",	"G#4",	"A4",	"A#4",	"B4",
	"C5",	"C#5",	"D5",	"D#5",	"E5",	"F5",	"F#5",	"G5",	"G#5",	"A5",	"A#5",	"B5",
	"C6",	"C#6",	"D6",	"D#6",	"E6",	"F6",	"F#6",	"G6",	"G#6",	"A6",	"A#6",	"B6",
	"C7",	"C#7",	"D7",	"D#7",	"E7",	"F7",	"F#7",	"G7",	"G#7",	"A7",	"A#7",	"B7",
	"C8",	"C#8",	"D8",	"D#8",	"E8",	"F8",	"F#8",	"G8",	"G#8",	"A8",	"A#8",	"B8",
	"C9",	"C#9",	"D9",	"D#9",	"E9",	"F9",	"F#9",	"G9",	"G#9",	"A9",	"A#9",	"B9",
	"C10",	"C#10",	"D10",	"D#10",	"E10",	"F10",	"F#10",	"G10"
};

const char mtVolumes[48][4] =
{
		"0",
		"3",
		"5",
		"8",
		"11",
		"14",
		"16",
		"19",
		"22",
		"24",
		"27",
		"30",
		"32",
		"35",
		"38",
		"41",
		"43",
		"46",
		"49",
		"51",
		"54",
		"57",
		"59",
		"62",
		"65",
		"68",
		"70",
		"73",
		"76",
		"78",
		"81",
		"84",
		"86",
		"89",
		"92",
		"95",
		"97",
		"100",
		"103",
		"105",
		"108",
		"111",
		"113",
		"116",
		"119",
		"122",
		"124",
		"127",

};


#endif
