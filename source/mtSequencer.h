#ifndef _MTSEQUENCER_H_
#define _MTSEQUENCER_H_

#include <stdint.h>
#include "scales.h"
#include "mtStructs.h"

class Sequencer
{

public:
	enum
	{
		MAXTRACK = 7,
		MINTRACK = 0,
		MINSTEP = 0,
		MAXSTEP = 127,

		DEFAULT_ROW_LEN = 32,
		DEFAULT_ROW_NOTE = 36,
		DEFAULT_ROW_CHANNEL = 1,

		MIN_CHORD = 0,
		MAX_CHORD = 29,
		NO_CHORD = 0,

		STEP_NOTE_EMPTY = -1,
		STEP_NOTE_OFF = -2,
		STEP_NOTE_DEFAULT = 60,

//	static const uint8_t
		MIDIOUT_DIN1 = 0,
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

		DEFAULT_CC = 74, // Generally this CC controls a vibrato effect (pitch, loudness, brighness). What is modulated is based on the patch.

		MAX_NOTE_TRACK = 127,
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
		MAX_JUMPTO = 255,

		MIN_CHANNEL_IN = -1,
		MAX_CHANNEL_IN = 16,
		MIN_TRANSPOSE = -100,
		MAX_TRANSPOSE = 100,
		MIN_MOVE = -32,
		MAX_MOVE = 32,

		MIN_MOVE_STEP = 0,
		MAX_MOVE_STEP = 4000,
		IDLE_MOVE_STEP = 2016,

		SEQ_STATE_STOP = 0,
		SEQ_STATE_PLAY = 1,
		SEQ_STATE_PAUSE = 2,
		MIN_MICROMOVE_STEP = -1000,
		MAX_MICROMOVE_STEP = 1000,
		CHANNEL_IN_ALL = 0,

		MIN_GATEMODE = 0,
		MAX_GATEMODE = 3,
		NULL_MOD = 128, // TODO: co to kurwa za null jak nie null
		DEFAULT_MOD = 128, //NULL_MOD

		MAX_NOTE_STEP = 127,
		MIN_NOTE_STEP = -2,
	};
	enum enPlaymode
	{
		PLAYMODE_MIN = 0,
		PLAYMODE_FORWARD = 0,
		PLAYMODE_BACKWARD = 1,
		PLAYMODE_PINGPONG = 2,
		PLAYMODE_RANDOM = 3,
		PLAYMODE_POLY = 4,
		PLAYMODE_MAX = 3
	};
	enum enTempoDiv
	{
		MIN_TEMPO_DIV = -3,
		MAX_TEMPO_DIV = 3,
		TEMPODIV_1_1 = 0,
	};

	static constexpr float MAX_TEMPO = 400.0,
			MIN_TEMPO = 10.0,
			MAX_SWING = 75.0,
			MIN_SWING = 25.0,
			DEFAULT_TEMPO = 130.0,

			DEFAULT_SWING = 50.0;

	struct strFxConsts
	{
		enum enMisc
		{
			FX_INDEX_MAX = 3
		};
		enum enFxType
		{
			FX_TYPE_NONE,
			FX_TYPE_NUDGE,
			FX_TYPE_CUTOFF,
			FX_TYPE_ROLL,
			FX_TYPE_STEP_CHANCE,
			FX_TYPE_RANDOM_NOTE,
			FX_TYPE_RANDOM_INSTRUMENT,
			FX_TYPE_RANDOM_VELOCITY,

		};
		enum enFxVal
		{
			FX_VAL_TYPE_UNKNOWN,		        // unsigned 16
			FX_VAL_TYPE_U16,	// unsigned 16
			FX_VAL_TYPE_I16,	// signed 16
			FX_VAL_TYPE_U8_U8,	// uint8 i uint8
			FX_VAL_TYPE_I8_I8,	// int8 i int8
			FX_VAL_TYPE_R8_I8,	// roll(int8) i int8
		};

		enum enRollType
		{
			ROLL_TYPE_MIN = 0,
			ROLL_TYPE_NONE = 0,
			ROLL_TYPE_4_1,
			ROLL_TYPE_3_1,
			ROLL_TYPE_2_1,
			ROLL_TYPE_1_1,
			ROLL_TYPE_1_2,
			ROLL_TYPE_1_3,
			ROLL_TYPE_1_4,
			ROLL_TYPE_1_6,
			ROLL_TYPE_1_8,
			ROLL_TYPE_1_12,
			ROLL_TYPE_1_16,
			ROLL_TYPE_MAX = ROLL_TYPE_1_16
		};

	} fx;

// KONIEC STAŁYCH

	struct strPattern
	{

		struct strFileHeader
		{
			char id_file[2];
			uint16_t type;
			char version[4];
			char id_data[4];
			uint16_t size;
		} header;

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

			uint8_t trackScale = 0;	// skala tracka

			uint8_t midiOut = MIDIOUT_USB;
			uint8_t playMode = PLAYMODE_FORWARD;

			uint8_t gateMode = GATEMODE.NORMAL;
			int8_t tempoDiv = TEMPODIV_1_1;
			int8_t channelIn = CHANNEL_IN_ALL;
			uint8_t rezerwa4 = 0;

			struct strStep
			{
				int8_t note = STEP_NOTE_EMPTY;

				int8_t velocity = -1;	// jeśli <0 to nie wysyłamy
				uint8_t instrument = 0;

				//FX
				struct strFx
				{
//					uint8_t isOn;
					uint8_t type;

					struct						// FX_VAL_U8_U8
					{
						uint8_t value;
						uint8_t value2;
					};

				} fx[1];

			} step[MAXSTEP + 1];

		} track[8];

		// poniżej nie brać pod uwagę licząc CRC
		uint32_t crc;

	};

	enum enSelectionElements
	{
		ELEMENTS_ALL_NO_PREFERENCES,
		ELEMENTS_ALL_WITH_PREFERENCES,
		ELEMENTS_NOTES,
		ELEMENTS_INSTRUMENTS,
		ELEMENTS_VELO,
		ELEMENTS_FXes,
	};
	enum midiChannel
	{
		MIDI_CHANNEL_GRID = 20
	};
	struct strSelection

	{
		uint8_t firstStep = 0;
		uint8_t firstTrack = 0;
		uint8_t lastStep = 0;
		uint8_t lastTrack = 0;

	} selection, copySelection, pasteSelection;

	uint8_t get_fxValType(uint8_t fxType);
	private:

	strPattern seq[3];

//	strPattern::strTrack copyTrackBuffer[8];

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

//	struct strChangeBuffer
//	{
//		int8_t transpose = 0;
//		int16_t uMoveStep = 0;
//		int16_t moveStep = IDLE_MOVE_STEP;
//		int16_t uMoveTrack = 1;
//		uint8_t trackRoll = 1;
//
//	};

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
//	void trySwitchBank();

	void midiSendCC(uint8_t channel, uint8_t control, uint8_t value,
					uint8_t midiOut);

	void flushNotes();
	void sendNoteOn(uint8_t track, uint8_t note, uint8_t velocity,
					uint8_t instrument);
	void sendNoteOff(uint8_t track, uint8_t note, uint8_t velocity,
						uint8_t instrument);
	void sendNoteOff(uint8_t track);

	void send_clock(uint8_t);

	inline uint8_t rollTypeToVal(uint8_t rollType);

	IntervalTimer midiReceiveTimer;
	IntervalTimer playTimer;

//	strChangeBuffer change_buffer;
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

	struct strPlayer
	{
		bool songMode = 0;
		bool printNotes = 0;
		bool changeBank = 0;
		bool isPlay = 0;
		bool isREC = 0;
		bool isStop = 1;
		//		bool loadBank = 0;
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
		//		uint8_t bank2change = 0;
//		uint8_t bank2load = 0;

		struct strBlink
		{
			elapsedMillis timer = 0;
			int8_t note = 0;
			int8_t track = 0;
			int8_t instrument = 0;
			bool isOpen = 0;
		} blink;

		struct strJump
		{
			uint8_t jumpNOW = 0;
			uint8_t nextPattern = 0;

		} jump;

		struct strPlayerTrack
		{
			strPattern::strTrack::strStep stepSent, stepToSend;
			bool stepOpen = 0;		// wirtualna nuta (zbiór rolek)
			bool noteOpen = 0;		// znacznik czy została wysłana nuta

			uint16_t uStep = 0;		// aktualny microstep
			int8_t actual_pos = 0;	// aktualna pozycja w stepach
			uint16_t stepTimer = 0;		// tu liczymy microstepy w górę
			uint16_t stepLength = 0;	// z tym porównujemy timer
			uint16_t noteTimer = 0;
			uint16_t noteLength = 0;

			boolean isOffset = 0;
			uint16_t offsetValue = 0;

			uint8_t rollMode = 0;

			boolean isRoll = 0;
			int8_t valRoll = 0;

			bool pingPongToogle = 0;

			uint8_t return2start = 0;// po zakonczonym stepie wraca do pocatku
			uint8_t makeJump = 0;// flaga przeskoku do odpowiedniego patternu po odegraniu stepu

			bool divChange = 0;

		} row[MAXTRACK + 1];

		void (*onPatternEnd)(void) = NULL;

	} player;

	/********************************
	 * ******************************
	 * 			PUBLIC				*
	 ********************************
	 ********************************/

public:

// klasowe
	void handle();
	void init();
	void loadDefaultSequence(void);
	void printNotes(bool val);
	void printNotes(void)
	{
		player.printNotes = !player.printNotes;
	}

// sekwencerowe

//	strPattern const * pattern = &seq[0];

	strPlayer const * ptrPlayer = &player;

	uint8_t * getPatternToSaveToFile()
	{
		return (uint8_t *) &seq[player.ramBank];
	}
	void saveToFileDone()
	{
	}
	uint8_t getSeqState()
	{
		if (player.isStop) return 0;
		else if (player.isPlay) return 1;
		else if (!player.isStop && !player.isPlay) return 2;
		else
			return 0;
	}

	uint8_t * getPatternToLoadFromFile()
	{
		return (uint8_t *) &seq[!player.ramBank];
	}
	void loadFromFileOK()
	{
//		Serial.println("ok");
	}
	void loadFromFileERROR()
	{
		loadDefaultBank(0);
		loadDefaultBank(1);
	}
	uint16_t getPatternSize()
	{
		return sizeof(strPattern);
	}

	strPattern * getPatternToUI()
	{
		return &seq[player.ramBank];
	}

	void switchNextPatternNow()
	{
		player.ramBank = !player.ramBank;
	}

	void play(void);
	void playPattern(void);
	void playSong(void);
	void playSong(uint8_t);
	void pause(void);
	void rec(void);
	void stop(void);

	void insert(strSelection *selection);
	void insertReversed(strSelection *selection);

// SELECTION
	void copySelectionToBuffer(strSelection *from, strSelection *to);
	void pasteSelectionFromBuffer(strSelection *from, strSelection *to,
									uint8_t elements);
	void copyToBuffer();
	void pasteFromBuffer(uint8_t);
	bool isSelectionCorrect(strSelection *selection);
	bool isSingleSelection(strSelection *selection);

	void setSelection(uint8_t stepFrom,
						uint8_t trackFrom,
						uint8_t stepTo,
						uint8_t trackTo);
	void setCopySelection(uint8_t stepFrom,
							uint8_t trackFrom,
							uint8_t stepTo,
							uint8_t trackTo);
	void setPasteSelection(uint8_t stepFrom,
							uint8_t trackFrom,
							uint8_t stepTo,
							uint8_t trackTo);

	void setTempo(float val);

	void clearStep(uint8_t x, uint8_t row);
	void clearStep(uint8_t x, uint8_t row, uint8_t bank);
	void clearStep(strPattern::strTrack::strStep * step, uint8_t);
	void clearSelected(strSelection * sel, uint8_t elements);
	void clearSelected();
	void clearSelected(uint8_t);

	void changeSelectionVolume(int16_t value);
	void changeSelectionFxValue(int16_t value);
	void setSelectionFxValue(int16_t value);
	void changeSelectionFxType(int16_t value);
	void setSelectionFxType(int16_t value);
	void changeSelectionInstrument(int16_t value);
	void setSelectionInstrument(int16_t value);
	void setSelectionVelocity(int16_t value);

	void clearRow(uint8_t row);
	void clearRow(uint8_t row, uint8_t bank);

	void copy_row(uint8_t from, uint8_t to);

//	void toggleStep(uint8_t, uint8_t);

	void addNoteOff(uint8_t note, uint8_t velocity, uint8_t channel,
					uint8_t midiOut);
	void addNoteOn(uint8_t note, uint8_t velocity, uint8_t channel,
					uint8_t midiOut);
	void allNoteOffs(void);

	void copy_step(uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y);

	void setOnPatternEnd(void (*action)(void))
	{
		player.onPatternEnd = action;
	}

// inne
	void handle_uStep_timer(void);

	void fillRandomNotes(uint8_t step, uint8_t from, uint8_t to);
	void fillLinearNotes(uint8_t step, uint8_t from, uint8_t to);
	void fillRandomInstruments(uint8_t step, uint8_t from, uint8_t to);
	void fillLinearInstruments(uint8_t step, uint8_t from, uint8_t to);
	void fillRandomVelocity(uint8_t step, uint8_t from, uint8_t to);
	void fillLinearVelocity(uint8_t step, uint8_t from, uint8_t to);
	void fillLinearFx(uint8_t fillStep, uint8_t fxType, uint8_t fromVal,
						uint8_t toVal);
	void fillRandomFx(uint8_t fillStep, uint8_t fxType, uint8_t fromVal,
						uint8_t toVal);

	void changeSelectionNote(int16_t value);
	void blinkNote(uint8_t instrument, uint8_t note, uint8_t velocity,
					uint8_t track);
	void closeBlinkNote(void);
	void blinkSelectedStep();
	void randomSelectedNotes(uint8_t from, uint8_t to, uint8_t scale);
	void randomSelectedInstruments(uint8_t from, uint8_t to);
	void randomSelectedVelo(uint8_t from, uint8_t to);
	void invertSelectedSteps();

	void loadNextPattern(uint8_t patternNumber);
	void handleNote(byte channel, byte pitch, byte velocity);

};

extern Sequencer sequencer;

#endif
