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
		STEP_VELO_DEFAULT = -1,
	};
	enum enFillStep
	{
		fillStepOccupied = -1,
		fillStepEmpty = -2,
		fillStepRandom = 0,
		fillStepByStep = 1,
	};

	enum enMisc
	{
		MAXTRACK = 7,
		MINTRACK = 0,
		MINSTEP = 0,
		MAXSTEP = 127,
		DEFAULT_PATTERN_LENGTH = 32,

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
			//			DEFAULT_TEMPO = 130.0,

			DEFAULT_SWING = 50.0;

	struct strFxConsts
	{
		enum enFxType
		{
			// powiązane z listą tekstów w
//			 #include <interfaceDefs.h>
			FX_TYPE_NONE,
			FX_TYPE_OFF,
			FX_TYPE_NUDGE,
			FX_TYPE_ROLL,
			FX_TYPE_STEP_CHANCE,
			FX_TYPE_RANDOM_NOTE,
			FX_TYPE_RANDOM_INSTRUMENT,
			FX_TYPE_RANDOM_VELOCITY,
			FX_TYPE_ROLL_NOTE_UP,
			FX_TYPE_ROLL_NOTE_DOWN,
			FX_TYPE_ROLL_NOTE_RANDOM,
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
			FX_TYPE_ROLL_VOL_UP,
			FX_TYPE_ROLL_VOL_DOWN,
			FX_TYPE_TEMPO,
			FX_TYPE_RANDOM_VALUE,

			FX_TYPE_NOT_SEQ_FX,
			FX_TYPE_VELOCITY,
			FX_TYPE_GLIDE,
			FX_TYPE_FADE,
			FX_TYPE_CUT,
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
			FX_TYPE_SAMPLE_END,
			FX_TYPE_WT_POSITION

		};

		enum enRollType
		{
			ROLL_TYPE_MIN = 0,
			ROLL_TYPE_NONE = 0,
			//			ROLL_TYPE_4_1,
//			ROLL_TYPE_3_1,
//			ROLL_TYPE_2_1,
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
			uint8_t length = DEFAULT_PATTERN_LENGTH - 1; // liczy od 0

			struct strStep
			{
				int8_t note = STEP_NOTE_EMPTY;
				uint8_t instrument = 0;
				//FX
				struct strFx
				{
					uint8_t type;
					uint8_t value;

				} fx[2];

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
		//		ELEMENTS_VELO,
		ELEMENTS_FX1,
		ELEMENTS_FX2,
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

	uint8_t getLongRollVelo(uint8_t rollCurve, float progress);
	uint8_t getTempoDiv(int8_t val);

	void play_microStep(uint8_t row);
	void handle_nanoStep(uint8_t step);
	void incr_uStep(uint8_t row);
	void init_player_timer(void);

	inline uint8_t rollTypeToVal(uint8_t rollType);

	void divChangeQuantize(uint8_t row);

	void loadDefaultPattern(uint8_t bank);
	void loadDefaultTrack(uint8_t track, uint8_t bank);

	void reset_actual_pos(uint8_t row);
	void reset_actual_pos(void);

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

	enum strRollCurve
	{
		ROLL_CURVE_MIN = 1,
		ROLL_CURVE_FLAT = 1,
		ROLL_CURVE_INCREMENTAL = 2,
		ROLL_CURVE_DECREMENTAL = 3,
		ROLL_CURVE_INC_DEC = 4,
		ROLL_CURVE_DEC_INC = 5,
		ROLL_CURVE_RANDOM = 6,
		ROLL_CURVE_MAX = 6,
	};

	struct strPlayer
	{
		bool songMode = 0;
		bool performanceMode = 0;
		bool selectionMode = 0;

		bool isPlay = 0;
		bool isREC = 0;
		bool isStop = 1;

		bool ramBank = 0;
		bool swingToogle = 0;
		float externalTempo = 120.0;
		float swing_offset = 50.0;

		uint16_t uStep = 0;
		uint16_t globalPos = 0;
		uint8_t actualBank = 0;

		struct strPerformance
		{
			int8_t patternLength = -1;
			float tempo = 0.0;
		} performance;

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
			struct strSendStep
			{
				int8_t note = STEP_NOTE_EMPTY;

				uint8_t instrument = 0;

				//FX
				struct strFx
				{
					uint8_t type;
					uint8_t value;

				} fx[2];

				int8_t velocity = -1;	// jeśli <0 to nie wysyłamy
			} stepSent, stepToSend;

			bool isActive = 1;

			bool stepOpen = 0;		// wirtualna nuta (zbiór rolek)
			bool noteOpen = 0;		// znacznik czy została wysłana nuta
			uint8_t recOpen = 0;		// znacznik czy została wysłana nuta

			uint16_t uStep = 0;		// aktualny microstep
			int16_t actual_pos = 0;	// aktualna pozycja w stepach
			uint16_t stepTimer = 0;		// tu liczymy microstepy w górę
			uint16_t stepLength = 0;	// z tym porównujemy timer
			uint16_t noteTimer = 0;
			uint16_t noteLength = 0;

			boolean isOffset = 0;
			uint16_t offsetValue = 0;

			boolean cancelStep = 0;

			boolean rollIsOn = 0;
			uint8_t rollType = 0; // 1/1, 1/2, 1/4 ...
			uint8_t rollVal = 0;
			uint8_t rollDir = 0;
			uint8_t rollFxId = 0;

			bool pingPongToogle = 0;

			uint8_t return2start = 0; // po zakonczonym stepie wraca do pocatku
			uint8_t makeJump = 0; // flaga przeskoku do odpowiedniego patternu po odegraniu stepu

			bool divChange = 0;

			int8_t performanceStutter = 0;
			int8_t performancePlayMode = 0;

		} track[MAXTRACK + 1];

		void (*onPatternEnd)(void) = NULL;
		void (*onSongEnd)(void) = NULL;

	} player;

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
	void sendNoteOn(uint8_t track,
					strPlayer::strPlayerTrack::strSendStep *step);
	void sendNoteOff(uint8_t track,
						strPlayer::strPlayerTrack::strSendStep *step);
	void sendNoteOff(uint8_t track);

	void send_clock(uint8_t);
	void send_allNotesOff(void);

	/********************************
	 * ******************************
	 * 			PUBLIC				*
	 ********************************
	 ********************************/

public:

// klasowe
	void handle();
	void init();
	//	void loadDefaultSequence(void);
	void printNotes(bool val);

	uint8_t isPlay(void);
	uint8_t isRec(void);
	uint8_t isStop(void);

// sekwencerowe

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

	void enterPerformanceMode()
	{
		player.songMode = 0;
		player.performanceMode = 1;
		player.performance.patternLength = getActualPattern()->track[0].length;
	}

	enum enSeqState
	{
		SEQ_STATE_STOP = 0,
		SEQ_STATE_PLAY_PATTERN,
		SEQ_STATE_PLAY_SONG,
		SEQ_STATE_PLAY_PERFORMANCE,
	};
	uint8_t getSeqState()
	{
		if (player.isStop) return SEQ_STATE_STOP;
		else if (player.performanceMode) return SEQ_STATE_PLAY_PERFORMANCE;
		else if (player.isPlay && !player.songMode) return SEQ_STATE_PLAY_PATTERN;
		else if (player.isPlay && player.songMode) return SEQ_STATE_PLAY_SONG;
		else
			return SEQ_STATE_STOP;
	}
	uint8_t isPerformanceMode()
	{
		return player.performanceMode;
	}
	void exitPerformanceMode()
	{
		player.performanceMode = 0;
		player.performance.patternLength = -1;
	}

	uint8_t * getPatternToLoadFromFile()
	{
		return (uint8_t *) &seq[!player.ramBank];
	}
	void loadFromFileOK()
	{

	}
	void loadFromFileERROR()
	{
		loadDefaultPattern(0);
		loadDefaultPattern(1);
	}
	uint16_t getPatternSize()
	{
		return sizeof(strPattern);
	}

	strPattern * getPatternToUI()
	{
		return &seq[player.ramBank];
	}

	void switchRamPatternsNow()
	{
		player.ramBank = !player.ramBank;
	}

// SEQUENCER

	void play(void);
	void playPattern(void);
	void playSelection(void);
	void playSong(void);
	void playSong(uint8_t);
	void pause(void);
	void rec(void);
	void stop(void);

// SELECTION
	void insert(strSelection *selection);
	void insertReversed(strSelection *selection);
	void backspace();

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

	void clearStep(uint8_t x, uint8_t row);
	void clearStep(uint8_t x, uint8_t row, uint8_t bank);
	void clearStep(strPattern::strTrack::strStep * step, uint8_t);
	void clearSelected(strSelection * sel, uint8_t elements);
	void clearSelected();
	void clearSelected(uint8_t);

//	void changeSelectionVolume(int16_t value);
	void changeSelectionFxValue(uint8_t index, int16_t value);
	void changeSelectionFxType(uint8_t index, int16_t value);
	void changeSelectionNote(int16_t value);
	void changeSelectionInstrument(int16_t value);

	void setSelectionFxType(uint8_t index, int16_t value);
	void setSelectionFxValue(uint8_t index, int16_t value);
	void setSelectionInstrument(int16_t value);
	//	void setSelectionVelocity(int16_t value);
	void setSelectionNote(int16_t value);

	void setPerformanceStutter(uint8_t track, int8_t stutter);
	void setPerformancePlayMode(uint8_t track, int8_t stutter);

	void fillRandomNotes(int16_t step, int16_t from, int16_t to);
	void fillLinearNotes(int16_t step, int16_t from, int16_t to);
	void fillRandomInstruments(int16_t step, int16_t from, int16_t to);
	void fillLinearInstruments(int16_t step, int16_t from, int16_t to);
	void fillLinearFx(int16_t index, int16_t fillStep, int16_t fxType,
						int16_t fromVal,
						int16_t toVal);
	void fillRandomFx(int16_t index, int16_t fillStep, int16_t fxType,
						int16_t fromVal,
						int16_t toVal);

	bool isStepToFillNote(strPattern::strTrack::strStep *step,
							uint8_t offset,
							int16_t fillStep);
	bool isStepToFillFx(strPattern::strTrack::strStep *step,
						uint8_t offset,
						uint8_t fxIndex,
						int16_t fillStep);

	void invertSelectedSteps(uint8_t);

	void allNoteOffs(void);

	void setOnPatternEnd(void (*action)(void))
	{
		player.onPatternEnd = action;
	}
	void setOnSongEnd(void (*action)(void))
	{
		player.onSongEnd = action;
	}

	void setPerformancePatternLength(int8_t length);
	void setPerformancePatternLengthFromFxVal(int8_t val);

// inne
	void handle_uStep_timer(void);
	void internalFxsOff(uint8_t track);

	void blinkNote(uint8_t instrument, uint8_t note, int8_t velocity,
					uint8_t track);
	void closeBlinkNote(void);
	void blinkSelectedStep();

	void loadNextPattern(uint8_t patternNumber);
	void handleNote(byte channel, byte pitch, byte velocity);
	void handleNoteOld(byte channel, byte pitch, byte velocity);
	int16_t getFxMax(uint8_t fxID);
	int16_t getFxMin(uint8_t fxID);
	int16_t getFxDefault(uint8_t fxID);
	int16_t getFxValueToView(uint8_t fxID, uint8_t track, uint8_t step);

	uint8_t getActualPos()
	{
		return player.track[0].actual_pos;
	}
	uint8_t getPatternLength()
	{
		return getActualPattern()->track[0].length + 1;
	}

	void alignToGlobalPos();
	void alignToGlobalPos(uint8_t);
};

extern Sequencer sequencer;

#endif
