#ifndef _MTSEQUENCER_H_
#define _MTSEQUENCER_H_

#include <stdint.h>
#include "scales.h"
#include "mtStructs.h"

class Sequencer
{

public:

	enum enStep
	{
		STEP_NOTE_EMPTY = -1,
		STEP_NOTE_OFF = -2,
		STEP_NOTE_CUT = -3,
		STEP_NOTE_FADE = -4,
		STEP_NOTE_DEFAULT = 60,
		STEP_NOTE_MAX = 127,

		STEP_VELO_MIN = 0,
		STEP_VELO_MAX = 127,
	};

	enum
	{
		MAXTRACK = 7,
		MINTRACK = 0,
		MINSTEP = 0,
		MAXSTEP = 127,

	};
	enum enPlaymode
	{
		PLAYMODE_MIN = 0,
		PLAYMODE_FORWARD = 0,
		PLAYMODE_BACKWARD,
		PLAYMODE_RANDOM,
		PLAYMODE_PINGPONG,
		PLAYMODE_POLY,
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
			// powiązane z listą tekstów w interfaceDefs.h
			FX_TYPE_NONE,
			FX_TYPE_NUDGE,
			FX_TYPE_zapas1,
			FX_TYPE_ROLL,
			FX_TYPE_STEP_CHANCE,
			FX_TYPE_RANDOM_NOTE,
			FX_TYPE_RANDOM_INSTRUMENT,
			FX_TYPE_RANDOM_VELOCITY,
			FX_TYPE_ROLL_UP,
			FX_TYPE_ROLL_DOWN,
			FX_TYPE_ROLL_RANDOM,
			FX_TYPE_SEND_CC_1,
			FX_TYPE_SEND_CC_2,
			FX_TYPE_SEND_CC_3,
			FX_TYPE_SEND_CC_4,
			FX_TYPE_SEND_CC_5,
			FX_TYPE_SEND_CC_6,
			FX_TYPE_SEND_CC_7,
			FX_TYPE_SEND_CC_8,
			FX_TYPE_SEND_CC_9,
			FX_TYPE_SEND_CC_10,

			FX_TYPE_NOT_SEQ_FX,
			FX_TYPE_GLIDE,
			FX_TYPE_SAMPLE_START,
			FX_TYPE_SAMPLE_LOOP_START,
			FX_TYPE_SAMPLE_LOOP_END,
			FX_TYPE_SAMPLE_SLICE,
			FX_TYPE_REVERSE_PLAYBACK,
			FX_TYPE_FILTER_LOWPASS,
			FX_TYPE_FILTER_HIGHPASS,
			FX_TYPE_FILTER_BANDPASS,
			FX_TYPE_REVERB_SEND,
			FX_TYPE_PANNING,
			FX_TYPE_AMP_ATTACK,
			FX_TYPE_AMP_RELEASE,
			FX_TYPE_MICROTUNING,
			FX_TYPE_TREMOLO_FAST,
			FX_TYPE_TREMOLO_SLOW,
			FX_TYPE_VIBRATO_FAST,
			FX_TYPE_VIBRATO_SLOW,

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
			uint8_t zapas10 :1;

			uint8_t length = MAXSTEP;

			uint8_t zapas5 = 0;
			uint8_t zapas6 = 0;
			uint8_t zapas7 = 0;
			uint8_t zapas9 = 0;
			uint8_t zapas11 = 0;
			uint8_t zapas8 = 0;
			uint8_t zapas1 = 0;
			uint8_t zapas2 = 0;
			uint8_t zapas3 = 0;

			int8_t tempoDiv = TEMPODIV_1_1;

			int8_t zapas4 = 0;
			uint8_t rezerwa4 = 0;

			struct strStep
			{
				int8_t note = STEP_NOTE_EMPTY;

				int8_t velocity = -1;	// jeśli <0 to nie wysyłamy
				uint8_t instrument = 0;

				//FX
				struct strFx
				{
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
	Sequencer::strPattern *getPattern();

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

	inline uint8_t isPlay(void);
	inline uint8_t isREC(void);
	inline uint8_t isStop(void);

	uint8_t getLongRollVelo(uint8_t rollCurve, float progress);
	uint8_t getTempoDiv(int8_t val);

	void play_microStep(uint8_t row);
	void handle_nanoStep(uint8_t step);
	void incr_uStep(uint8_t row);
	void init_player_timer(void);

	inline uint8_t rollTypeToVal(uint8_t rollType);

	void divChangeQuantize(uint8_t row);

	void loadDefaultBank(uint8_t bank);
	void loadDefaultTrack(uint8_t track, uint8_t bank);

	void reset_actual_pos(uint8_t row);
	void reset_actual_pos(void);

	//__________________________________________
	//
	//				MIDI
	//__________________________________________

	void midiSendCC(uint8_t channel, uint8_t control, uint8_t value,
					uint8_t midiOut);

	void flushNotes();
	void sendNoteOn(uint8_t track, uint8_t note, uint8_t velocity,
					uint8_t instrument);
	void sendNoteOff(uint8_t track, uint8_t note, uint8_t velocity,
						uint8_t instrument);
	void sendNoteOn(uint8_t track, strPattern::strTrack::strStep *step);
	void sendNoteOff(uint8_t track, strPattern::strTrack::strStep *step);
	void sendNoteOff(uint8_t track);

	void send_clock(uint8_t);
	void send_allNotesOff(void);
	//__________________________________________
	//
	//
	//__________________________________________

	IntervalTimer midiReceiveTimer;
	IntervalTimer playTimer;

//	strChangeBuffer change_buffer;
	strGhost ghost;
	strGlobalConfig config;

	uint16_t nanoStep = 0;

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

			uint8_t rollType = 0; // 1/1, 1/2, 1/4 ...

			boolean rollIsOn = 0;
			uint8_t rollVal = 0;
			uint8_t rollDir = 0;

			bool pingPongToogle = 0;

			uint8_t return2start = 0; // po zakonczonym stepie wraca do pocatku
			uint8_t makeJump = 0; // flaga przeskoku do odpowiedniego patternu po odegraniu stepu

			bool divChange = 0;

			int8_t performanceStutter = 0;
			int8_t performancePlayMode = 0;

		} track[MAXTRACK + 1];

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

	strPattern * getActualPattern()
	{
		return &seq[player.ramBank];
	}

	void saveToFileDone()
	{
	}

	void forcePatternMode()
	{
		player.songMode = 0;
	}

	enum
	{
		SEQ_STATE_STOP = 0,
		SEQ_STATE_PLAY_PATTERN,
		SEQ_STATE_PLAY_SONG,
	};
	uint8_t getSeqState()
	{
		if (player.isStop) return SEQ_STATE_STOP;
		else if (player.isPlay && !player.songMode) return SEQ_STATE_PLAY_PATTERN;
		else if (player.isPlay && player.songMode) return SEQ_STATE_PLAY_SONG;
		else
			return SEQ_STATE_STOP;
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
	void backspace();

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
	void setSelectionNote(int16_t value);

	void setPerformanceStutter(uint8_t track, int8_t stutter);
	void setPerformancePlayMode(uint8_t track, int8_t stutter);

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
