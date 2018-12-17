#ifndef _MTSEQUENCER_H_
#define _MTSEQUENCER_H_

#include <stdint.h>
#include "scales.h"



/*
	definicje na potrzeby portu programu na mniejsze seczki
*/

#define FV_VER_1 0	// device version
#define FV_VER_2 1	// official update
#define FV_VER_3 0	// fix version


#define MEMORY_STRUCT_VER 2
#define MAXROW 8
#define MAXCOL 32

#define MIN_CHORD 0
#define MAX_CHORD 29
#define NO_CHORD 0


#define I2C_ID_ENC 1
#define I2C_ID_BUTTONS 2
#define I2C_ID_LEDS 3

/*
	definicje globalne
*/
#define MODE_MIDICLOCK_MIN_VALUE		0
#define MODE_MIDICLOCK_INTERNAL			0
#define MODE_MIDICLOCK_INTERNAL_LOCK	1
#define MODE_MIDICLOCK_MIDIDIN			2
#define MODE_MIDICLOCK_USB				3
#define MODE_MIDICLOCK_MAX_VALUE		3	// ograniczenie wyboru



#define MODE_MIDICLOCK_INC_VALUE		8
/*

	konfiguracja IO dla:
	pcb SEQ 1.1

*/


#define MIDI1	Serial1
//#define MIDI2	Serial2


// buttony

#define BUTTON_ROW_PATTERN 1
#define BUTTON_ROW_DUPLICATE 2
#define BUTTON_ROW_QUANTIZE 3
#define BUTTON_ROW_RANDOM 4
#define BUTTON_ROW_ONOFF 5
#define BUTTON_ROW_CLEAR 6
#define BUTTON_ROW_STOP 7
#define BUTTON_ROW_PLAY 8

#define BUTTON_COL_FUNCTION -1


// buttony koniec


// enkoderyyy

#define ENC_TEMPO 0
#define ENC_NOTE 1
#define ENC_VELO 2
#define ENC_MOVE 3
#define ENC_LENGTH 4
#define ENC_ROLL 5


#define CC_ALL_NOTES_OFF 123
#define CC_VAL_NOTES_OFF 0

// enkodery koniec


// inne
#define PLAYMODE_MIN 0
#define PLAYMODE_FORWARD 0
#define PLAYMODE_BACKWARD 1
#define PLAYMODE_PINGPONG 2
#define PLAYMODE_RANDOM 3
#define PLAYMODE_POLY 4
#define PLAYMODE_MAX 3

#define MIDIOUT_DIN1 	0
#define MIDIOUT_DIN2 	1
#define MIDIOUT_USB 	2
#define MIDIOUT_DIN1_C 	3
#define MIDIOUT_DIN2_C 	4
#define MIDIOUT_USB_C	5

#define MIDIOUT_MIN 	0
#define MIDIOUT_MAX 	5




//	AT45
#define HWPIN_AT45_RST	4
#define HWPIN_AT45_CS	16
#define HWPIN_AT45_WP	17


// ENCODERY
#define HWPIN_ENCODER_INT	32
#define HWPIN_ENCODER_RST	25
#define HWPIN_EC6_SW		15
#define HWPIN_EC5_SW		22


// DRIVERY BUTTONOW
#define HWPIN_I2C_SELECT	28

#define HWPIN_GRIDA_INT 	27
#define HWPIN_GRIDB_INT		29
#define HWPIN_GRIDC_INT		30
#define HWPIN_GRIDD_INT		2
#define HWPIN_GRIDF_INT		8


//	LCD
#define HWPIN_LCD_DC		20
#define HWPIN_LCD_CS		21

//	MISC
#define HWPIN_FOOT_SW		5
#define HWPIN_TEST_SW		26

#define HWPIN_LVL_IFT_EN	6
#define HWPIN_BLINK_LED		3
#define HWPIN_PWR_STATUS	33


#define PWR_STATUS_USB 1
#define PWR_STATUS_JACK 0


//(11101)(A2)(A1)(R/W)
// if R/W == 1 then READ
#define ADDR_LED_CONTROLLER_1 0b1110100//0
#define ADDR_LED_CONTROLLER_2 0b1110111//0

//10110(AD1)(AD0)(R/W)
#define ADDR_BUTTON_CONTROLLER_A 	0b1011000	// 176
#define ADDR_BUTTON_CONTROLLER_B 	0b1011001 	// 178
#define ADDR_BUTTON_CONTROLLER_C 	0b1011010   // 180
#define ADDR_BUTTON_CONTROLLER_D 	0b1011011

#define ADDR_BUTTON_CONTROLLER_FUNC 0b1011000

//MCP
#define ADDR_ENCODER_CONTROLLER 0b0100000 //@ 0x40






#define COPY_MODE_BANK 0
#define COPY_MODE_ROW 0
#define COPY_MODE_STEP 0

#define HITMODE_OFFSET 20
#define OFFSET_MIN 1
#define OFFSET_MAX 48

#define DEFAULT_CC 74 // Generally this CC controls a vibrato effect (pitch, loudness, brighness). What is modulated is based on the patch.

#define MAX_TEMPO 400.0
#define MIN_TEMPO 10.0

#define MAX_SWING 75.0
#define MIN_SWING 25.0

#define MAX_NOTE_STEP 127
#define MIN_NOTE_STEP 0

#define MAX_NOTE_TRACK 127
#define MIN_NOTE_TRACK 0






#define MAX_TRACK_LENGTH 32
#define MIN_TRACK_LENGTH 1

#define MAX_STEP_LENGTH 31
#define MIN_STEP_LENGTH 0


#define MIN_STEP_ROLL_VAR 1
#define MAX_STEP_ROLL_VAR 9

#define MIN_STEP_ROLL_NOTE_VAR 1
#define MAX_STEP_ROLL_NOTE_VAR 9


#define MIN_TRACK_ROLL_VAR 1
#define MAX_TRACK_ROLL_VAR 16





// #define MIN_ROLL_CURVE 1
// #define MAX_ROLL_CURVE 5

#define MIN_VELO_STEP 0
#define MAX_VELO_STEP 127

#define MIN_VELO_TRACK 0
#define MAX_VELO_TRACK 100

#define MIN_MOD 0
#define MAX_MOD 127

#define MIN_CHANNEL 1
#define MAX_CHANNEL 16

#define MIN_CHANNEL_IN -1
#define MAX_CHANNEL_IN 16

#define MIN_TRANSPOSE -100
#define MAX_TRANSPOSE 100

#define MIN_MOVE_STEP 0
#define MAX_MOVE_STEP 4000
#define IDLE_MOVE_STEP 2016

#define MIN_MOVE -32
#define MAX_MOVE 32


#define RANDOM_VELO_MIN 	20
#define RANDOM_VELO_MAX 	127
#define RANDOM_MOD_MIN 		20
#define RANDOM_MOD_MAX 		127
#define RANDOM_NOTE_DOWN 	12
#define RANDOM_NOTE_UP 		12


#define MIN_CC 1
#define MAX_CC 127


#define NOTE_JUMPTO 128
#define NOTE_JUMPTO_NEXT 129

#define MIN_JUMPTO 0
#define MAX_JUMPTO 255

#define MIN_MICROMOVE_STEP -1000
#define MAX_MICROMOVE_STEP 1000


#define ROW_COLUMN 0



enum lcdView
{
	LCDVIEW_IDLE,
	LCDVIEW_STEP_PROP,
	LCDVIEW_STEP_NOTE,
	LCDVIEW_STEP_VELO,
	LCDVIEW_STEP_MOVE,
	LCDVIEW_STEP_ROLL,
	LCDVIEW_TRACK_PROP,
	LCDVIEW_TRACK_NOTE,
	LCDVIEW_TRACK_VELO,
	LCDVIEW_TRACK_LENGTH,
	LCDVIEW_TRACK_MOVE,
	LCDVIEW_TRACK_TEMPO,

	LCDVIEW_PATTERN,
	LCDVIEW_DUPLICATE,
	LCDVIEW_QUANTIZE,
	LCDVIEW_RANDOM,
	LCDVIEW_ON_OFF,
	LCDVIEW_CLEAR_TRACK,
	LCDVIEW_CLEAR_PATTERN,
	LCDVIEW_DUPLICATE_PATTERN,
	LCDVIEW_DUPLICATE_TRACK,
	LCDVIEW_DUPLICATE_STEP,

	LCDVIEW_MAX,

	// // komunikaty (bez menu)

	LCDVIEW_FIRMWARE_CHANGE = 200,
	LCDVIEW_POLYEND,
	LCDVIEW_FIRMWARE
};


// #define LCDVIEW_IDLE        		 0
// #define LCDVIEW_STEP_PROP   		 1
// #define LCDVIEW_STEP_NOTE   		 2
// #define LCDVIEW_STEP_VELO   		 3
// #define LCDVIEW_STEP_MOVE   		 4
// #define LCDVIEW_STEP_ROLL   		 5
// #define LCDVIEW_TRACK_PROP  		 6
// #define LCDVIEW_TRACK_NOTE  		 7
// #define LCDVIEW_TRACK_VELO  		 8
// #define LCDVIEW_TRACK_LENGTH		 9
// #define LCDVIEW_TRACK_MOVE			10
// #define LCDVIEW_TRACK_TEMPO			11


// #define LCDVIEW_PATTERN				11
// #define LCDVIEW_DUPLICATE			12
// #define LCDVIEW_QUANTIZE			13
// #define LCDVIEW_RANDOM				14
// #define LCDVIEW_ON_OFF				15
// #define LCDVIEW_CLEAR_TRACK			16
// #define LCDVIEW_CLEAR_PATTERN		17
// #define LCDVIEW_DUPLICATE_PATTERN	18
// #define LCDVIEW_DUPLICATE_TRACK		19
// #define LCDVIEW_DUPLICATE_STEP		20

// #define LCDVIEW_MAX 24 // dotyczy menu


// // komunikaty (bez menu)
// #define LCDVIEW_FIRMWARE_CHANGE		200
// #define LCDVIEW_POLYEND				201
// #define LCDVIEW_FIRMWARE 			202



#define IGNORE_CLOCKS 1	// półśrodek do ignorowania pierwszych clocków



#define LCD_TEXT_COLOUR tft.color565(219,202,145)
#define LCD_GRAY_COLOUR tft.color565(192,192,192)



#define LCD_BACKGROUND ILI9341_BLACK
#define LCD_activeParameter_COLOUR LCD_TEXT_COLOUR
#define LCDPIX_COL_A 25
#define LCDPIX_COL_B 175

#define LCDPIX_ROW_1 32
#define LCDPIX_ROW_2 84
#define LCDPIX_ROW_3 136
#define LCDPIX_ROW_4 188


#define LCDPIX_RECT_H 20
#define LCDPIX_RECT_4H 180
#define LCDPIX_RECT_W 100
#define LCDPIX_RECT_2W 250

#define LCDPIX_activeParameter_H 52
#define LCDPIX_activeParameter_W 300


#define ROLL_CURVE_MIN		1
#define ROLL_CURVE_FLAT 	1
#define ROLL_CURVE_INC		2
#define ROLL_CURVE_DEC		3
#define ROLL_CURVE_INC_DEC 	4
#define ROLL_CURVE_DEC_INC	5
#define ROLL_CURVE_RANDOM	6
#define ROLL_CURVE_MAX		6




#define USTEP_TIMER 0
#define USTEP_EXT_CLOCK 1


struct strDebug
{
	uint8_t midi = 0;

	uint8_t eeprom = 0;

	uint8_t button = 0;
	uint8_t buttonAction = 0;
	uint8_t buttonInt = 0;
	uint8_t buttonCore = 0;
	uint8_t encoder = 0;

	uint8_t player = 0;

	uint8_t flash = 0;

	uint8_t misc = 1;

} ;

// struktury odtwarzacza, zmienne pomocnicze nie zapisywane do banku

struct strPlayerStep
{

	uint8_t wasModification = 0;
	uint8_t justPressed = 0;
	uint8_t learned = 0;
	uint8_t isGhost = 0;	// jeśli > 0 to ma numer stepa którego jest ghostem
	uint8_t isMoving = 0;	// jest przemieszczany
	uint8_t isBlinking = 0;	// jest podświetlany jako ruch

};


struct strPlayerRow
{
	int16_t note_length_timer = 1;	// tu odliczamy ile zostalo stepow do zakonczenia nuty

	uint8_t noteOn_sent = 0;		// znacznik czy została wysłana nuta
	uint8_t note_sent = 0;			// wartość wysłanej nuty
	uint8_t chord_sent = 0;			// wartość wysłanej nuty
	uint8_t midiOut_sent = 0;
	uint8_t channel_sent = 0;
	uint8_t scale_sent = 0;
	uint8_t scaleRoot_sent = 0;




	uint8_t rollLength 	= 0;		// tu wrzucamy długość rolki w stepach
									// zerujemy kiedy wpadnie inny step

	uint8_t rollStep 	= 0;		// step który jest rollowany
	uint16_t rollCounter = 0;		// licznik wykonanych hitów



	uint8_t lastMod = 0;			// ostatnio wyslany parametr

	int8_t actual_pos = 0;

	uint8_t return2start = 0;	// po zakonczonym stepie wraca do pocatku
	uint8_t makeJump = 0;		// flaga przeskoku do odpowiedniego patternu po odegraniu stepu
	uint8_t goToStep = 0;		// odpala odpowiedni step jako kolejny



	uint8_t recNoteOpen = 0;	// czy otwarta nuta? w trakcie nagrywania
	uint8_t recNoteLength = 0;	// aktualna długość otwartej nuty
	uint8_t recNote = 0;
	uint8_t recChannel = 0;
	uint8_t recNoteStep = 0;

	int8_t lastRollNote = 0;

	bool pingPongToogle = 0;

	uint8_t learned = 0;


	bool divChange = 0;
	bool divChangeIncr = 0;

	strPlayerStep step[33];



	// uint8_t microStep = 0;



};



struct strPlayer
{

	uint8_t jumpNOW = 0;

	bool isPlay = 0;
	bool isStop = 1;	// zaczyna od 1 bo jest w STOPie
	uint16_t uStep = 0;
	uint16_t uStepInd[9] = {0};
	bool isREC = 0;

	bool  swingToogle = 0;
	float swing_offset = 50.0;


	bool ramBank = 0;			// jeden z dwóch banków w pamięci
	// uint8_t ramBank2change = 0;
	uint8_t actualBank = 0;
	uint8_t bank2change = 0;
	uint8_t bank2load = 0;
	// bool 	changeBank = 0;
	bool 	loadBank = 0;




	uint16_t metronome_timer = 0;
	uint16_t metronome_timer_max = 48 * 4;

	uint16_t rec_intro_timer = 0;
	uint16_t rec_intro_timer_max = 48 * 4;
	uint16_t rec_intro_step = 0;






	uint8_t copy_mode = COPY_MODE_BANK;
	uint8_t copy_mode_step = 0;

	uint8_t copy_bank_from = 0;
	uint8_t copy_bank_to = 0;

	uint8_t copy_step_from_row = 0;
	uint8_t copy_step_from_col = 0;

	uint8_t copy_step_to_row = 0;
	uint8_t copy_step_to_col = 0;

	uint8_t copy_row_from = 0;
	uint8_t copy_row_to = 0;



	uint8_t pwr_mode = 1;
	uint8_t last_pwr_mode = 0;


	uint8_t ignore_first_clock = IGNORE_CLOCKS;
	float externalTempo = 120.0;
	// float lockedTempo = 120.0;


	bool 	changeBank = 0;



	strPlayerRow row[9];

} ;

struct strGridView
{
	elapsedMillis timer = 0;
	uint32_t timer_max = 2000;
} ;


const int8_t MIN_TEMPO_DIV = -3;
const int8_t MAX_TEMPO_DIV = 3;
const int8_t TEMPODIV_1_1 = 0;
// const uint8_t tempoDivTable[MAX_TEMPO_DIV + 1] =
// {
// 	48, // 1/4	0
// 	36,	// 1/3	1
// 	24,	// 1/2	2
// 	12,	// 1/1	3
// 	6, 	// 2/1	4
// 	4, 	// 3/1	5
// 	3	// 4/1	6
// };







/*
	bank początek ##########################################
*/

#define MASK_ROW_ON 		0b00000001
#define MASK_RANDOM_VELO 	0b00000010
#define MASK_RANDOM_MOD 	0b00000100
#define MASK_RANDOM_NUDGE 	0b00001000

#define MIN_GATEMODE 		0
#define MAX_GATEMODE 		3

#define GATEMODE_NORMAL 	0
#define GATEMODE_MEDIUM 	1
#define GATEMODE_SHORT		2
#define GATEMODE_EXTRASHORT	3


#define NULL_MOD 128

#define DEFAULT_MOD NULL_MOD
/*

1- 1
2- 3
3- 0-7
4- 0-15
5- 0-31
6- 0-63
7- 0-127
8- 0-255

*/


struct strStep
{
	uint8_t velocity 	= 127;
	uint8_t note 		= 50;

	// 2 x byte
	uint16_t chord  	: 11;
	uint16_t length1 	: 5;	//31

	// byte
	uint8_t isOn 	: 1;
	uint8_t rez1    : 1;
	uint8_t offset 	: 6;		// 63 // przesuniecie od 1 do 48


	// byte
	uint8_t hitMode : 5;		// max31
	uint8_t rez2 	: 3; 		// max 7

	// byte
	uint8_t rollCurve : 4;		// max 15
	uint8_t rollNoteCurve : 4;	// max 15


	// byte
	uint8_t modulation 	= NULL_MOD;
};
#define CHANNEL_IN_ALL 0

#define DEFAULT_ROW_LEN 32
#define DEFAULT_ROW_NOTE 36
#define DEFAULT_ROW_CHANNEL 1
#define DEFAULT_TEMPO 120.0
#define DEFAULT_SWING 50.0
struct strRow
{
	uint8_t length 		= DEFAULT_ROW_LEN;
	uint8_t rootNote 	= DEFAULT_ROW_NOTE;
	uint8_t trackVelo 	= MAX_VELO_TRACK;
	uint8_t defaultMod 	= DEFAULT_MOD;	// rezerwa1

	uint8_t channel 	= DEFAULT_ROW_CHANNEL;			// wiersz ma swoj channel
	uint8_t cc 			= DEFAULT_CC;
	uint8_t flags 		= 1;			// bity konfiguracyjne
	uint8_t trackScale 	= 0;			// skala tracka

	uint8_t midiOut 	= MIDIOUT_USB;
	uint8_t playMode 	= PLAYMODE_FORWARD;

	uint8_t gateMode	= GATEMODE_NORMAL;
	int8_t 	tempoDiv 	= TEMPODIV_1_1;
	int8_t 	channelIn	= CHANNEL_IN_ALL;
	uint8_t rezerwa4	= 0;

	strStep step[33];

};

struct strBank
{
	float 	tempo = DEFAULT_TEMPO;
	float 	swing = DEFAULT_SWING;

	uint8_t structVer 			= MEMORY_STRUCT_VER;
	uint8_t structVerControl 	= MEMORY_STRUCT_VER;
	uint8_t rezerwa3 			= 0;
	uint8_t rezerwa4 			= 0;

	strRow row[9];

} ;

// strBank templateBank;

/*
	bank koniec ##########################################
*/

struct strGlobalConfig
{

	uint8_t mode 		= MODE_MIDICLOCK_INTERNAL;
	uint8_t fv_ver_1 	= FV_VER_1;
	uint8_t fv_ver_2 	= FV_VER_2;
	uint8_t fv_ver_3 	= FV_VER_3;
	uint8_t zapas 		= 0;
	uint8_t lastPattern = 0;

	float   tempoLock = 120.0;
	float   swingLock = 50.0;

} ;

const strGlobalConfig config_const;

struct strGhost
{
	uint8_t cnt1 = 1;
	uint8_t cnt1_max = 8;

} ;


struct strChangeBuffer
{
	int8_t  transpose 	= 0;
	int16_t uMoveStep 	= 0;
	int16_t moveStep 	= IDLE_MOVE_STEP;
	int16_t uMoveTrack 	= 1;
	uint8_t trackRoll 	= 1;

} ;

struct strBankCRC
{
	uint32_t part[5];
	uint32_t total;
} ;


struct strHidden
{
	uint8_t firmwareViewCount 			= 0;
	uint8_t firmwareViewCountMax 		= 5;
	elapsedMillis firmwareViewTimeout 	= 0;
} ;

float get_swing(void);
float get_tempo(void);
inline uint8_t get_actual_pos(uint8_t row);
inline uint8_t get_hitMode(uint8_t col, uint8_t row);
inline uint8_t get_isOn(uint8_t col, uint8_t row);
inline uint8_t get_note(uint8_t col, uint8_t row);
inline uint8_t get_row_length(uint8_t row);
inline uint8_t isBlinking(uint8_t col, uint8_t row);
inline uint8_t isGhost(uint8_t col, uint8_t row);
inline uint8_t isPlay(void);
inline uint8_t isREC(void);
inline uint8_t isStop(void);
int8_t getLastRollNoteOffset(uint8_t row);
int8_t getNextRollNoteOffset(uint8_t row);
uint16_t get_size(void);
uint8_t get_copy_bank_from(void);
uint8_t get_copy_bank_to(void);
uint8_t get_copy_mode_step(void);
uint8_t get_copy_row_from(void);
uint8_t get_copy_row_to(void);
uint8_t get_copy_step_from_col(void);
uint8_t get_copy_step_from_row(void);
uint8_t get_copy_step_to_col(void);
uint8_t get_copy_step_to_row(void);
uint8_t get_metronome_intro_step(void);
uint8_t getLongRollVelo(uint8_t rollCurve, float progress);
uint8_t getTempoDiv(int8_t val);
uint8_t isInScale(uint8_t note, uint8_t root, uint8_t scale);
uint8_t isRowOn(uint8_t row);
uint8_t play_uStep(uint8_t row);
uint8_t val2roll(uint8_t val);
void action_buttonClear(void);
void action_buttonDuplicate(void);
void action_buttonOnOff(void);
void action_buttonPattern(void);
void action_buttonPlay(void);
void action_buttonQuantize(void);
void action_buttonRandom(void);
void action_buttonREC(void);
void action_buttonStop(void);
void addNoteOff(uint8_t note, uint8_t velocity, uint8_t channel, uint8_t midiOut);
void addNoteOn(uint8_t note, uint8_t velocity, uint8_t channel, uint8_t midiOut);
void allNoteOffs(void);
void clearRow(uint8_t row);
void clearRow(uint8_t row, uint8_t bank);
void clearStep(uint8_t x, uint8_t row);
void clearStep(uint8_t x, uint8_t row, uint8_t bank);
void copy_bank(uint8_t from, uint8_t to);
void copy_row(uint8_t from, uint8_t to);
void copy_step(uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y);
void divChangeQuantize(uint8_t row);
void flushNotes();
void handle_ghosts(void);
void handle_player(void);
void handle_uStep_timer(void);
void handle_uStep12(uint8_t step);
void hold_function(int16_t x, uint16_t y);
void hold_step(int16_t x, uint16_t y);
void hold_track(int16_t x, uint16_t y);
void incr_uStep(uint8_t row);
void init_defaultPlayerParameters(void);
void init_player(void);
void init_player_lcd_values(void);
void init_player_timer(void);
void initBank(uint8_t bank);
void initRow(uint8_t row, uint8_t bank);
void learnNote(uint8_t note, uint8_t velo, uint8_t channel);
void learnNoteOff(uint8_t note, uint8_t velo, uint8_t channel);
void panic_all_notes_off(void);
void print_uSteps();
void push_step(int16_t x, uint16_t y);
void push_track(uint16_t row);
void randomize_row(uint8_t row);
void rec_metronome(void);
void release_function(uint16_t row);
void release_step(int16_t x, uint16_t y);
void release_track(uint16_t row);
void reset_actual_pos(uint8_t row);
void reset_actual_pos(void);
void resetAllLearned(void);
void resetLastSendMod(void);
void set_power_mode(uint8_t mode);
void trySwitchBank();
void midiSendChordOn(uint8_t note,
                     uint8_t chord,
                     uint8_t velo,
                     uint8_t channel,
                     uint8_t midiOut,
                     uint8_t scale,
                     uint8_t scaleRoot);
void setLoadBank2Ram(uint8_t bank);
void midiSendCC(uint8_t channel,
				uint8_t control,
				uint8_t value,
				uint8_t midiOut);

void midiSendChordOff(uint8_t note,
                      uint8_t chord,
                      uint8_t velo,
                      uint8_t channel,
                      uint8_t midiOut,
                      uint8_t scale,
                      uint8_t scaleRoot);
void switch_bank_with_reset(void);
//uint16_t timerTick = 1;

#endif
