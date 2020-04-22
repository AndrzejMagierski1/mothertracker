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
		STEP_NOTE_FADE = -2,
		STEP_NOTE_CUT = -3,
		STEP_NOTE_OFF = -4,
		STEP_NOTE_DEFAULT = 60,
		STEP_NOTE_MAX = 127,

		STEP_VELO_MIN = 0,
		STEP_VELO_MAX = 127,
		STEP_VELO_DEFAULT = -1,
	};
	enum enFillStep
	{

		fillStepNote = -4,
		fillStepNoNote = -3,
		fillStepFx = -2,
		fillStepNoFx = -1,
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

	static constexpr float MAX_TEMPO = 800.0,
			MIN_TEMPO = 40.0,
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
			FX_TYPE_MICROMOVE,
			FX_TYPE_ROLL,
			FX_TYPE_CHANCE,
			FX_TYPE_RANDOM_NOTE,
			FX_TYPE_RANDOM_INSTRUMENT,
			FX_TYPE_RANDOM_VELOCITY,
			FX_TYPE_SEND_CC_A,
			FX_TYPE_SEND_CC_B,
			FX_TYPE_SEND_CC_C,
			FX_TYPE_SEND_CC_D,
			FX_TYPE_SEND_CC_E,
			FX_TYPE_BREAK_PATTERN, // dziala jak koniec patternu
			FX_TYPE_R22,
			FX_TYPE_TEMPO,
			FX_TYPE_RANDOM_VALUE,

			FX_TYPE_R11,
			FX_TYPE_VELOCITY,
			FX_TYPE_GLIDE,
			FX_TYPE_FADE,
			FX_TYPE_CUT,
			FX_TYPE_POSITION,
			FX_TYPE_VOLUME_LFO,
			FX_TYPE_PANNING_LFO,
			FX_TYPE_SAMPLE_SLICE,
			FX_TYPE_REVERSE_PLAYBACK,
			FX_TYPE_FILTER_LOWPASS,
			FX_TYPE_FILTER_HIGHPASS,
			FX_TYPE_FILTER_BANDPASS,
			FX_TYPE_DELAY_SEND,
			FX_TYPE_PANNING,
			FX_TYPE_R4,
			FX_TYPE_FINETUNE_LFO,
			FX_TYPE_MICROTUNING,
			FX_TYPE_FILTER_LFO,
			FX_TYPE_POSITION_LFO,
			FX_TYPE_PROGRAM_CHANGE,
			FX_TYPE_R10,
			FX_TYPE_R30,
			FX_TYPE_R31,

			// FX_TYPE_Rx to pozostałość po starych efektach i zostają tu żeby nic się nie rozpadło
			FX_TYPE_R1,
			FX_TYPE_R2,
			FX_TYPE_R3,
			FX_TYPE_R7,
			FX_TYPE_R8,
			FX_TYPE_R9,

		};

		enum enRollPeriod
		{
			ROLL_PERIOD_NONE = 0,
			ROLL_PERIOD_MIN = 0,
			//			ROLL_PERIOD_16_1=-7,
//			ROLL_PERIOD_12_1,
//			ROLL_PERIOD_8_1,
//			ROLL_PERIOD_6_1,
//			ROLL_PERIOD_4_1,
//			ROLL_PERIOD_3_1,
//			ROLL_PERIOD_2_1,

			ROLL_PERIOD_1_1,
			ROLL_PERIOD_1_2,
			ROLL_PERIOD_1_3,
			ROLL_PERIOD_1_4,
			ROLL_PERIOD_1_6,
			ROLL_PERIOD_1_8,
			ROLL_PERIOD_1_12,
			ROLL_PERIOD_1_16,
			ROLL_PERIOD_MAX = ROLL_PERIOD_1_16
		};
		enum enStutterPeriod
		{
			STUTTER_PERIOD_NONE = 0,
			STUTTER_PERIOD_MIN = 0,
			STUTTER_PERIOD_16_1,
			STUTTER_PERIOD_12_1,
			STUTTER_PERIOD_8_1,
			STUTTER_PERIOD_6_1,
			STUTTER_PERIOD_4_1,
			STUTTER_PERIOD_3_1,
			STUTTER_PERIOD_2_1,
			STUTTER_PERIOD_1_1,
			STUTTER_PERIOD_1_2,
			STUTTER_PERIOD_1_3,
			STUTTER_PERIOD_1_4,
			STUTTER_PERIOD_1_6,
			STUTTER_PERIOD_1_8,
			STUTTER_PERIOD_1_12,
			STUTTER_PERIOD_1_16,
			STUTTER_PERIOD_MAX = STUTTER_PERIOD_1_16
		};

		enum enRollDir
		{
			rollType_const = 0,
			rollType_volDown,
			rollType_volUp,
			rollType_noteDown,
			rollType_noteUp,
			rollType_noteRandom,
			rollNoteDir_max = rollType_noteRandom,
		};

	} fx;

	enum enRollVolumeOption
	{
		rollVolumeOption_const = 0,
		rollVolumeOption_up,
		rollVolumeOption_down,
		rollVolumeOption_random,
	};

	struct strPattern
	{

		struct strFileHeader
		{
			char id_file[2] { 0 }; // typ pliku, np MT = plik projektu, TI = instrument
			uint16_t type;					// typ w liczbie
			char fwVersion[4] { 0 }; 		// wersja fw w ktorej zapisano plik
			char fileStructureVersion[4] { 0 };	// wersja struktury pliku
			uint16_t size = 0;					// rozmiar
		} header;

		float tempoRezerwa = 100.0;
		float swing = DEFAULT_SWING;

		uint8_t rezerwa1 = 0;
		uint8_t rezerwa2 = 0;
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
	enum enAltMidiChannel
	{
		GRID_OUTSIDE_PATTERN = 20,
		GRID_INSIDE_PATTERN = 21
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
	Sequencer::strPattern* getPattern();

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

//	void initPattern(uint8_t pattern);

	void switchStep(uint8_t row);

	uint8_t getLongRollVelo(uint8_t rollCurve, float progress);
	uint8_t getTempoDiv(int8_t val);

	void play_microStep(uint8_t row);

	void incr_uStep(uint8_t row);
	void init_player_timer(void);
	void cancelFxes(int8_t track);
	void cancelFxes();
	void killFxOnSlot(uint8_t row, uint8_t slot);
	uint8_t getRollType(uint8_t value);
	uint8_t rollValToVolumeOption(uint8_t);
	uint8_t getRollVelo(uint8_t);

	inline uint16_t rollValToPeriod(int8_t rollType);
	inline uint16_t stutterValToPeriod(int8_t rollType);

	void divChangeQuantize(uint8_t row);

	void loadDefaultPattern(uint8_t bank);
	//	void clearPattern(strPattern*);
	void loadDefaultTrack(uint8_t track, uint8_t bank);

	void reset_actual_pos(uint8_t row);

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
	uint16_t nanoStepMultiplier = 0;

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

//	enum strRollCurve
//	{
//		ROLL_CURVE_MIN = 1,
//		ROLL_CURVE_FLAT = 1,
//		ROLL_CURVE_INCREMENTAL = 2,
//		ROLL_CURVE_DECREMENTAL = 3,
//		ROLL_CURVE_INC_DEC = 4,
//		ROLL_CURVE_DEC_INC = 5,
//		ROLL_CURVE_RANDOM = 6,
//		ROLL_CURVE_MAX = 6,
//	};

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

		bool breakPattern = 0;

		struct strPerformance
		{
			int16_t patternLength = -1;
			int16_t initialPatternLength = 8;
			float tempo = 0.0;
			int8_t tempoSource = -1;
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
//			uint8_t jumpNOW = 0;
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
			int16_t noteSource = -1;		// źródo

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
			uint8_t rollPeriod = 0; // 1/1, 1/2, 1/4 ...
			uint8_t rollVal = 0;
			uint8_t rollDir = 0;
			uint8_t rollFxId = 0;
			uint8_t rollVolumeOption = rollVolumeOption_const;
			int8_t rollVolumeStart = 127;
			uint16_t rollLength = 1; // obliczona długość na potrzeby vol up i down

			bool pingPongToogle = 0;

			uint8_t return2start = 0; // po zakonczonym stepie wraca do pocatku
			uint8_t makeJump = 0; // flaga przeskoku do odpowiedniego patternu po odegraniu stepu

			bool divChange = 0;

			int8_t performanceStutter = 0;
			int8_t performancePlayMode = 0;
			int8_t performanceSourcePattern = -1;

		} track[MAXTRACK + 1];

		void (*onPatternEnd)(void) = NULL;
		void (*onSongEnd)(void) = NULL;

		uint8_t extRecMetronomeStep = 0;

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
	uint8_t isPreview(void)
	{
		return player.isPlay && player.selectionMode;
	}

	float getActualTempo();
	uint32_t getSeqTimer();

// sekwencerowe

	void reset_actual_pos(void);
	void feedExternalTempo(float setTempo);
	void handle_nanoStep(uint8_t step);

	strPlayer const *ptrPlayer = &player;

	uint8_t* getPatternToSaveToFile()
	{
		return (uint8_t*) &seq[player.ramBank];
	}

	strPattern* getActualPattern()
	{
		return &seq[player.ramBank];
	}
	strPattern* getBuffPattern()
	{
		return &seq[!player.ramBank];
	}

	boolean isPerformanceTrackChange(uint8_t track)
	{
		return player.track[track].performanceSourcePattern >= 0;
	}

	void saveToFileDone()
	{
	}

	void enterPerformanceMode()
	{
		player.songMode = 0;
		player.performanceMode = 1;
		player.performance.initialPatternLength = getActualPattern()->track[0].length;
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

	uint8_t* getPatternToLoadFromFile()
	{
		return (uint8_t*) &seq[!player.ramBank];
	}
	void loadFromFileOK()
	{

	}
	void loadFromFileERROR()
	{
		clearPattern(&seq[0]);
		clearPattern(&seq[1]);
	}
	uint16_t getPatternSize()
	{
		return sizeof(strPattern);
	}

	strPattern* getPatternToUI()
	{
		return &seq[player.ramBank];
	}

	void switchRamPatternsNow();
	void switchPerformanceTrackNow(uint8_t trackToSwitch);
	void setTrackToLoadOnSwitch(uint8_t track, uint8_t sourcePattern);

// SEQUENCER

	void play(void);
	void playPattern(void);
	void playSelection(void);
	float getPlaySelectionProgress(void);
	void playSong(void);
	void playSong(uint8_t);
	void pause(void);
	void rec(void);
	void stop(void);
	void stopManualNotes(void);
	uint8_t isMetronomeActive();
	uint8_t getMetronomeNumerator();
	uint8_t getMetronomeDenominator();

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
	uint8_t getCopySelectionHeight();

	void clearSingleTrack(strPattern::strTrack *track);
	void clearPattern(strPattern *patt);
	void setPatternHeader(strPattern *patt);
	void clearStep(uint8_t x, uint8_t row);
	void clearStep(uint8_t x, uint8_t row, uint8_t bank);
	void clearStep(strPattern::strTrack::strStep*);
	void clearStep(strPattern::strTrack::strStep *step, uint8_t);
	void clearSelected(strSelection *sel, uint8_t elements);
	void clearSelected();
	void clearSelected(uint8_t);

//	void changeSelectionVolume(int16_t value);
	void changeSelectionFxValue(uint8_t index, int16_t value);
	void changeSelectionFxType(uint8_t index, int16_t value);
	void changeSelectionNote(int16_t value);
	void changeSelectionInstrument(int16_t value);
	uint8_t getInstrumentPosInOrder(int8_t value);
	uint8_t getInstrumentFromOrder(int8_t value);
	uint8_t changeInstrumentInSpecificOrder(int8_t actualValue,
											int16_t delta);
	uint8_t getInstrumentVelo(uint8_t);

	void setSelectionFxType(uint8_t index, int16_t value);
	void setSelectionFxValueByPad(uint8_t index, int16_t value);
	void setSelectionInstrument(int16_t value);
	//	void setSelectionVelocity(int16_t value);
	void setSelectionNote(int16_t value);

	void setPerformanceStutter(uint8_t track, int8_t stutter);
	void setPerformancePlayMode(uint8_t track, int8_t stutter);

	void fillRandomNotes(int16_t, int16_t, int16_t, int16_t);
	void fillLinearNotes(int16_t, int16_t, int16_t, int16_t);
	void fillRandomInstruments(int16_t step, int16_t from, int16_t to);
	void fillLinearInstruments(int16_t step, int16_t from, int16_t to);
	void fillLinearFx(int16_t index, int16_t fillStep, int16_t fxType,
						int16_t fromVal,
						int16_t toVal);
	void fillRandomFx(int16_t index, int16_t fillStep, int16_t fxType,
						int16_t fromVal,
						int16_t toVal);

	uint8_t randomFx();

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
	bool isInternalClock(void);
	void internalFxsOff(uint8_t track);

	void blinkNote(uint8_t instrument, uint8_t note, int8_t velocity,
					uint8_t track);
	void closeBlinkNote(void);
	void blinkSelectedStep();

	void loadNextPattern(uint8_t patternNumber);
	void handleNote(byte channel, byte pitch, byte velocity);
	void handleNote(byte channel, byte pitch, byte velocity, int16_t source);
	void handleNoteOld(byte channel, byte pitch, byte velocity);
	int16_t getFxMax(uint8_t fxID);
	int16_t getFxMin(uint8_t fxID);
	int16_t getFxDefault(uint8_t fxID);
	int16_t getFxValueToView(uint8_t fxID, uint8_t track, uint8_t step);
	int16_t getFxValueCorrection(uint8_t type, uint8_t value);
	char getRollTypeChar(uint8_t val);
	uint8_t calcStepLength(uint8_t track, uint8_t step);
	int16_t rollValueToPeriod(int16_t value);
	void makeFxValLabel(char *ptr, uint8_t fxID, uint8_t track, uint8_t step);
	void makeFxValLabel(char *ptr, uint8_t fxType, uint8_t value);
	enum enFxValLabelAlign
	{
		fxValLabelAlign_right = 0,
		fxValLabelAlign_center,
	};
	void makeFxValLabel(char *ptr, uint8_t fxType, uint8_t value,
						enFxValLabelAlign);

	uint8_t isTrackEngineMuted(uint8_t track);

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

	uint8_t changeInstrumentOrder[48 + 16] = {
			63, 62, 61, 60,
			59, 58, 57, 56, 55, 54, 53, 52,
			51, 50, 49, 48,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
			12, 13, 14, 15, 16, 17, 18, 19,
			20, 21, 22, 23, 24, 25, 26, 27,
			28, 29, 30, 31, 32, 33, 34, 35,
			36, 37, 38, 39, 40, 41, 42, 43,
			44, 45, 46, 47, };
};

extern Sequencer sequencer;

#endif
