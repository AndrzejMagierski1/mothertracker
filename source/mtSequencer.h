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

class Sequencer
{
private:

	static const uint8_t MAXROW = 7,
			MINROW = 0,
			MINSTEP = 0,
			MAXSTEP = 31,

			DEFAULT_ROW_LEN = 32,
			DEFAULT_ROW_NOTE = 36,
			DEFAULT_ROW_CHANNEL = 1,

			MIN_CHORD = 0,
			MAX_CHORD = 29,
			NO_CHORD = 0;

	static const uint8_t MIDIOUT_DIN1 = 0,
			MIDIOUT_DIN2 = 1,
			MIDIOUT_USB = 2,
			MIDIOUT_DIN1_C = 3,
			MIDIOUT_DIN2_C = 4,
			MIDIOUT_USB_C = 5,

			MIDIOUT_MIN = 0,
			MIDIOUT_MAX = 5,

			COPY_MODE_BANK = 0,
			COPY_MODE_ROW = 0,
			COPY_MODE_STEP = 0,

			HITMODE_OFFSET = 20,
			OFFSET_MIN = 1,
			OFFSET_MAX = 48,

			CC_ALL_NOTES_OFF = 123,
			CC_VAL_NOTES_OFF = 0,

			DEFAULT_CC = 74; // Generally this CC controls a vibrato effect (pitch, loudness, brighness). What is modulated is based on the patch.

	static constexpr float MAX_TEMPO = 400.0,
			MIN_TEMPO = 10.0,
			MAX_SWING = 75.0,
			MIN_SWING = 25.0,
			DEFAULT_TEMPO = 120.0,

			DEFAULT_SWING = 50.0;

	static const uint8_t MAX_NOTE_TRACK = 127,
			MIN_NOTE_TRACK = 0,
			MAX_TRACK_LENGTH = 32,
			MIN_TRACK_LENGTH = 1,
			MAX_STEP_LENGTH = 31,
			MIN_STEP_LENGTH = 0,
			MIN_STEP_ROLL_VAR = 1,
			MAX_STEP_ROLL_VAR = 9,
			MIN_STEP_ROLL_NOTE_VAR = 1,
			MAX_STEP_ROLL_NOTE_VAR = 9,
			MIN_TRACK_ROLL_VAR = 1,
			MAX_TRACK_ROLL_VAR = 16,

			MIN_VELO_STEP = 0,
			MAX_VELO_STEP = 127,
			MIN_VELO_TRACK = 0,
			MAX_VELO_TRACK = 100,
			MIN_MOD = 0,
			MAX_MOD = 127,
			MIN_CHANNEL = 1,
			MAX_CHANNEL = 16,
			MIN_CC = 1,
			MAX_CC = 127,

			NOTE_JUMPTO = 128,
			NOTE_JUMPTO_NEXT = 129,

			MIN_JUMPTO = 0,
			MAX_JUMPTO = 255;

	static const int8_t MIN_CHANNEL_IN = -1,
			MAX_CHANNEL_IN = 16,
			MIN_TRANSPOSE = -100,
			MAX_TRANSPOSE = 100,
			MIN_MOVE = -32,
			MAX_MOVE = 32;

	static const uint16_t MIN_MOVE_STEP = 0,
			MAX_MOVE_STEP = 4000,
			IDLE_MOVE_STEP = 2016;

	static const int16_t MIN_MICROMOVE_STEP = -1000,
			MAX_MICROMOVE_STEP = 1000;

	static const uint8_t RANDOM_VELO_MIN = 20,
			RANDOM_VELO_MAX = 127,
			RANDOM_MOD_MIN = 20,
			RANDOM_MOD_MAX = 127,
			RANDOM_NOTE_DOWN = 12,
			RANDOM_NOTE_UP = 12;

	static const uint8_t CHANNEL_IN_ALL = 0;

	static const uint8_t MIN_GATEMODE = 0,
			MAX_GATEMODE = 3,
			NULL_MOD = 128, // TODO: co to kurwa za null jak nie null
			DEFAULT_MOD = 128; //NULL_MOD

	static const uint8_t MAX_NOTE_STEP = 127;
	static const uint8_t MIN_NOTE_STEP = 0;

	static const uint8_t PLAYMODE_MIN = 0,
			PLAYMODE_FORWARD = 0,
			PLAYMODE_BACKWARD = 1,
			PLAYMODE_PINGPONG = 2,
			PLAYMODE_RANDOM = 3,
			PLAYMODE_POLY = 4,
			PLAYMODE_MAX = 3;

	const int8_t MIN_TEMPO_DIV = -3;
	const int8_t MAX_TEMPO_DIV = 3;
	static const int8_t TEMPODIV_1_1 = 0;

	// KONIEC STAŁYCH

	struct strBank
	{
		float tempo = DEFAULT_TEMPO;
		float swing = DEFAULT_SWING;

		uint8_t structVer = MEMORY_STRUCT_VER;
		uint8_t structVerControl = MEMORY_STRUCT_VER;
		uint8_t rezerwa3 = 0;
		uint8_t rezerwa4 = 0;

		struct strTrack
		{
			uint8_t isOn :1;

			uint8_t length = DEFAULT_ROW_LEN;
			uint8_t rootNote = DEFAULT_ROW_NOTE;
			uint8_t trackVelo = MAX_VELO_TRACK;
			uint8_t defaultMod = DEFAULT_MOD;	// rezerwa1

			uint8_t channel = DEFAULT_ROW_CHANNEL;	// wiersz ma swoj channel
			uint8_t cc = DEFAULT_CC;

			uint8_t trackScale = 0;			// skala tracka

			uint8_t midiOut = MIDIOUT_USB;
			uint8_t playMode = PLAYMODE_FORWARD;

			uint8_t gateMode = GATEMODE.NORMAL;
			int8_t tempoDiv = TEMPODIV_1_1;
			int8_t channelIn = CHANNEL_IN_ALL;
			uint8_t rezerwa4 = 0;

			struct strStep
			{
				uint8_t isOn :1;
				uint8_t note :7;

				uint8_t velocity = 127;
				uint8_t instrument = 0;

				// 2 x byte
				uint16_t length1;	//31 długość w stepach

				//FX
				struct strFx
				{
					uint8_t isOn :1;
					uint8_t type :7;

					union
					{
						uint16_t value_u16;		// FX_VAL_U16
						int16_t value_i16;		// FX_VAL_I16

						struct					// FX_VAL_U8_U8
						{
							uint8_t val1_u8;
							uint8_t val2_u8;
						};
						struct					// FX_VAL_I8_I8
						{
							int8_t val1_i8;
							int8_t val2_i8;
						};

					};

				} fx[4];

				// do wyjebania:
//				uint8_t offset;
//				uint8_t hitMode;		// tryb grania, jeśli >1 to rolka
//				uint8_t rollCurve;		// max 15
//				uint8_t rollNoteCurve;	// max 15

			} step[32];

		} row[8];

	} seq[2];

	strBank const * pattern = &seq[0];

	struct strGlobalConfig
	{
		uint8_t mode = MODE_MIDICLOCK.INTERNAL_;
		uint8_t lastPattern = 0;
		float tempoLock = 120.0;
		float swingLock = 50.0;

	} config_const;

	struct strGhost
	{
		uint8_t cnt1 = 1;
		uint8_t cnt1_max = 8;

	};

	struct strChangeBuffer
	{
		int8_t transpose = 0;
		int16_t uMoveStep = 0;
		int16_t moveStep = IDLE_MOVE_STEP;
		int16_t uMoveTrack = 1;
		uint8_t trackRoll = 1;

	};

	struct strNoteHandler
	{
		uint8_t free = 1;
		uint8_t onOff = 0;

		uint8_t note;
		uint8_t velocity;
		uint8_t channel;

		uint8_t midiOut = 0;

	} noteHandler[100];


	void initPattern(uint8_t pattern);

	void switchStep(uint8_t row);


	inline uint8_t get_hitMode(uint8_t col, uint8_t row);
	inline uint8_t get_isOn(uint8_t col, uint8_t row);
	inline uint8_t get_note(uint8_t col, uint8_t row);
	inline uint8_t get_row_length(uint8_t row);

	inline uint8_t isPlay(void);
	inline uint8_t isREC(void);
	inline uint8_t isStop(void);
	int8_t getLastRollNoteOffset(uint8_t row);
	int8_t getNextRollNoteOffset(uint8_t row);
	uint8_t getLongRollVelo(uint8_t rollCurve, float progress);
	uint8_t getTempoDiv(int8_t val);
	uint8_t isInScale(uint8_t note, uint8_t root, uint8_t scale);
	uint8_t isRowOn(uint8_t row);
	void play_microStep(uint8_t row);
	uint8_t val2roll(uint8_t val);

	void divChangeQuantize(uint8_t row);
	void handle_ghosts(void);
	void handle_player(void);

	void handle_nanoStep(uint8_t step);
	void incr_uStep(uint8_t row);

	void init_player_timer(void);


	void loadDefaultBank(uint8_t bank);
	void loadDefaultTrack(uint8_t track, uint8_t bank);

	void send_allNotesOff(void);

	void randomize_row(uint8_t row);
	void rec_metronome(void);

	void reset_actual_pos(uint8_t row);
	void reset_actual_pos(void);
	void resetAllLearned(void);
	void resetLastSendMod(void);
	//	void set_power_mode(uint8_t mode);
	void trySwitchBank();

	void setLoadBank2Ram(uint8_t bank);
	void midiSendCC(uint8_t channel, uint8_t control, uint8_t value,
					uint8_t midiOut);


	void switch_bank_with_reset(void);

	void flushNotes();
	void sendNoteOn(uint8_t track, strBank::strTrack::strStep *step);
	void sendNoteOff(uint8_t track, strBank::strTrack::strStep *step);

	void send_clock(uint8_t);

	IntervalTimer midiReceiveTimer;
	IntervalTimer playTimer;

	strChangeBuffer change_buffer;
	strGhost ghost;
	strGlobalConfig config;

	uint16_t nanoStep = 0;

	elapsedMicros playerTimer;
	elapsedMicros flushTimer = 0;

	elapsedMicros timeOfTick = 0;
	const uint8_t arrVal2roll[10] = { 0, 1, 1, 2, 3, 4, 6, 8, 12, 16 };

	static struct strMidiModes
	{
		static const uint8_t MIN_VALUE = 0;
		static const uint8_t INTERNAL_ = 0;
		static const uint8_t INTERNAL_LOCK = 1;
		static const uint8_t MIDIDIN = 2;
		static const uint8_t USB = 3;
		static const uint8_t MAX_VALUE = 3;	// ograniczenie wyboru
		static const uint8_t INC_VALUE = 8;

	} MODE_MIDICLOCK;

	struct strRollCurve
	{
		const uint8_t MIN = 1;
		const uint8_t FLAT = 1;
		const uint8_t INCREMENTAL = 2;
		const uint8_t DECREMENTAL = 3;
		const uint8_t INC_DEC = 4;
		const uint8_t DEC_INC = 5;
		const uint8_t RANDOM = 6;
		const uint8_t MAX = 6;
	} ROLL_CURVE;

	static struct strGateMode
	{
		static const uint8_t NORMAL = 0;
		static const uint8_t MEDIUM = 1;
		static const uint8_t SHORT = 2;
		static const uint8_t EXTRASHORT = 3;
	} GATEMODE;

	struct strDebug
	{
		uint8_t player = 0;
	} debug;

	uint8_t get_fxValType(uint8_t fxType);

	struct strPlayer
	{
		bool changeBank = 0;
		bool isPlay = 0;
		bool isREC = 0;
		bool isStop = 1;
		bool loadBank = 0;
		bool ramBank = 0;
		bool swingToogle = 0;
		float externalTempo = 120.0;
		float swing_offset = 50.0;
		uint16_t metronome_timer = 0;
		uint16_t metronome_timer_max = 48 * 4;
		uint16_t rec_intro_step = 0;
		uint16_t rec_intro_timer = 0;
		uint16_t rec_intro_timer_max = 48 * 4;
		uint16_t uStep = 0;
		uint8_t actualBank = 0;
		uint8_t bank2change = 0;
		uint8_t bank2load = 0;

		uint8_t jumpNOW = 0;

		struct strPlayerTrack
		{

			strBank::strTrack::strStep stepSent;
			uint16_t uStep = 0;
			uint16_t note_length_timer = 1;	// tu odliczamy ile zostalo microstepów
											// do zakonczenia nuty

			uint8_t noteOn_sent = 0;		// znacznik czy została wysłana nuta

			uint8_t rollLength = 0;		// tu wrzucamy długość rolki w stepach
										// zerujemy kiedy wpadnie inny step

			uint8_t rollStep = 0;		// step który jest rollowany
			uint16_t rollCounter = 0;		// licznik wykonanych hitów

			uint8_t lastMod = 0;			// ostatnio wyslany parametr

			int8_t actual_pos = 0;

			uint8_t return2start = 0;// po zakonczonym stepie wraca do pocatku
			uint8_t makeJump = 0;// flaga przeskoku do odpowiedniego patternu po odegraniu stepu
			uint8_t goToStep = 0;		// odpala odpowiedni step jako kolejny

			uint8_t recNoteOpen = 0;// czy otwarta nuta? w trakcie nagrywania
			uint8_t recNoteLength = 0;	// aktualna długość otwartej nuty
			uint8_t recNote = 0;
			uint8_t recChannel = 0;
			uint8_t recNoteStep = 0;

			int8_t lastRollNote = 0;

			bool pingPongToogle = 0;

			uint8_t learned = 0;

			bool divChange = 0;
			bool divChangeIncr = 0;

			struct strPlayerStep
			{

				uint8_t wasModification = 0;
				uint8_t justPressed = 0;
				uint8_t learned = 0;
				uint8_t isGhost = 0;// jeśli > 0 to ma numer stepa którego jest ghostem
				uint8_t isMoving = 0;	// jest przemieszczany
				uint8_t isBlinking = 0;	// jest podświetlany jako ruch

			} step[33];

		} row[9];

	} player;

	/********************************
	 * ******************************
	 * 			PUBLIC				*
	 ********************************
	 ********************************/

public:

	struct strFxConsts
	{
		enum
		{
			FX_TYPE_NONE,
			FX_TYPE_OFFSET,             // przesuniecie wewnątrz stepa 0-48
			FX_TYPE_GLIDE,	  // czas płynnego przejścia do kolejnej nuty/pitcha
			FX_TYPE_SLIDE,	            // podciągnięcie do nuty w czasie
			FX_TYPE_ARP_UP,	            // arpeggio w górę
			FX_TYPE_ARP_DOWN,
			FX_TYPE_SP,		            //	start point
			FX_TYPE_LP1,		        // 	loop point 1
			FX_TYPE_LP2,		        //	loop point 2
			FX_TYPE_MICROTUNE,
			FX_TYPE_SAMPLE_PLAYMODE,
			FX_TYPE_VOL_ROLL,
			FX_TYPE_JUMP_TO_STEP,
			FX_TYPE_JUMP_TO_PATTERN,
			FX_TYPE_PANNING,
			FX_TYPE_PANNING_ROLL,
			FX_TYPE_SLICE_NUMBER,
			FX_TYPE_PROBABILITY,

		};
		enum
		{
			FX_VAL_TYPE_U16,
			FX_VAL_TYPE_I16,
			FX_VAL_TYPE_U8_U8,
			FX_VAL_TYPE_I8_I8,
		};

	} fx;

	// klasowe
	void handle();
	void init();
	void loadDefaultSequence(void);

	// sekwencerowe
	void play(void);
	void rec(void);
	void stop(void);

	void clearStep(uint8_t x, uint8_t row);
	void clearStep(uint8_t x, uint8_t row, uint8_t bank);
	void clearRow(uint8_t row);
	void clearRow(uint8_t row, uint8_t bank);

	void copy_row(uint8_t from, uint8_t to);

	void toggleStep(uint8_t, uint8_t);

	void addNoteOff(uint8_t note, uint8_t velocity, uint8_t channel,
					uint8_t midiOut);
	void addNoteOn(uint8_t note, uint8_t velocity, uint8_t channel,
					uint8_t midiOut);
	void allNoteOffs(void);

	void copy_step(uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y);

	// inne
	void handle_uStep_timer(void);

};
#endif
