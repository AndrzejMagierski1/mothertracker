/*
 * mtSequencer.h
 *
 *  Created on: 13 gru 2018
 *      Author: krzys
 */
#include <stdint.h>
#include "Arduino.h"

#ifndef SOURCE_MTSEQUENCER_H_
#define SOURCE_MTSEQUENCER_H_

class Sequencer
{
public:

	static class Defs
	{
	public:
		static const uint8_t ROW_STEPS_MAX = 32;
		static const uint8_t PATTERN_TRACKS_MAX = 8;
		static const uint8_t TRACK_LENGTH_MAX = 32;
		static const uint8_t TRACK_VOLUME_MAX = 100;
		static const uint8_t TRACK_DEFAULT_ROOT_NOTE = 100;
		static const uint8_t TRACK_DEFAULT_SCALE = 1;

		static const int8_t TEMPODIV_MIN = -3;
		static const int8_t TEMPODIV_1_2 = -3;
		static const int8_t TEMPODIV_1_3 = -2;
		static const int8_t TEMPODIV_1_4 = -1;
		static const int8_t TEMPODIV_1_1 = 0;
		static const int8_t TEMPODIV_2_1 = 1;
		static const int8_t TEMPODIV_3_1 = 2;
		static const int8_t TEMPODIV_4_1 = 3;
		static const int8_t TEMPODIV_MAX = 3;

		static const uint8_t NEXTSTEP_FORWARD = 1;
		static constexpr float TEMPO_DEFAULT = 120.0;

		static constexpr float SWING_DEFAULT = 50.0;
		static constexpr float SWING_MIN = 25.0;
		static constexpr float SWING_MAX = 75.0;

		static const uint8_t CLOCKMODE_INTERNAL = 0;
		static const uint8_t CLOCKMODE_EXTERNAL = 1;
		static const uint8_t CLOCKMODE_INTERNAL_LOCK = 2;

	} defs;

	struct strStep
	{
		uint8_t velocity = 127;
		uint8_t note = 50;

		// 2 x byte
		uint16_t chord :11;
		uint16_t length1 :5;	//31

		// byte
		uint8_t isOn :1;
		uint8_t rez1 :1;
		uint8_t offset :6;		// 63 // przesuniecie od 1 do 48

		// byte
		uint8_t hitMode :5;		// max31
		uint8_t rez2 :3; 		// max 7

		// byte
		uint8_t rollCurve :4;		// max 15
		uint8_t rollNoteCurve :4;	// max 15

		// byte
		uint8_t modulation :7;
		uint8_t modulationActive :1;
	};
	struct strRow
	{
		uint8_t length = defs.TRACK_LENGTH_MAX;
		uint8_t volume = defs.TRACK_VOLUME_MAX;
		int8_t panning = 0;

		uint8_t nextStepMode = defs.NEXTSTEP_FORWARD;
		uint8_t rootNote = defs.TRACK_DEFAULT_ROOT_NOTE;
		uint8_t scale = defs.TRACK_DEFAULT_SCALE;
		int8_t tempoDiv = defs.TEMPODIV_1_1;

		//	uint8_t gateMode = GATEMODE_NORMAL;
		//	int8_t tempoDiv = TEMPODIV_1_1;
		//	int8_t channelIn = CHANNEL_IN_ALL;
		//	uint8_t rezerwa4 = 0;

		strStep step[defs.ROW_STEPS_MAX];
	};
	struct strPattern
	{
		float tempo = defs.TEMPO_DEFAULT;
		float swing = defs.SWING_DEFAULT;
		//
		//	uint8_t structVer = MEMORY_STRUCT_VER;
		//	uint8_t structVerControl = MEMORY_STRUCT_VER;
		//	uint8_t rezerwa3 = 0;
		//	uint8_t rezerwa4 = 0;

		strRow row[8];
	} seq;

public:
	Sequencer();
	void init_player_timer();
	void timerVector();
//	uint8_t isPlay();
//	uint8_t isRec();

private:

	struct strPlayerStep
	{
		uint8_t wasModification = 0;
		uint8_t justPressed = 0;
		uint8_t learned = 0;
		uint8_t isGhost = 0; // jeśli > 0 to ma numer stepa którego jest ghostem
		uint8_t isMoving = 0;	// jest przemieszczany
		uint8_t isBlinking = 0;	// jest podświetlany jako ruch
	};
	struct strPlayerRow
	{
		int16_t note_length_timer = 1;// tu odliczamy ile zostalo stepow do zakonczenia nuty

		uint8_t noteOn_sent = 0;		// znacznik czy została wysłana nuta
		uint8_t note_sent = 0;			// wartość wysłanej nuty
		uint8_t chord_sent = 0;			// wartość wysłanej nuty
		uint8_t midiOut_sent = 0;
		uint8_t channel_sent = 0;
		uint8_t scale_sent = 0;
		uint8_t scaleRoot_sent = 0;

		uint8_t rollLength = 0;		// tu wrzucamy długość rolki w stepach
		// zerujemy kiedy wpadnie inny step

		uint8_t rollStep = 0;		// step który jest rollowany
		uint16_t rollCounter = 0;		// licznik wykonanych hitów

		uint8_t lastMod = 0;			// ostatnio wyslany parametr

		int8_t actual_pos = 0;

		uint8_t return2start = 0;	// po zakonczonym stepie wraca do pocatku
		uint8_t makeJump = 0;// flaga przeskoku do odpowiedniego patternu po odegraniu stepu
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
		bool isRec = 0;

		uint16_t uStep = 0;
		uint16_t uStepInd[9] =
		{ 0 };

		bool swingToogle = 0;
		float swingOffset = 50.0;

		bool ramBank = 0;			// jeden z dwóch banków w pamięci
		// uint8_t ramBank2change = 0;
		uint8_t actualBank = 0;
		uint8_t bank2change = 0;
		uint8_t bank2load = 0;
		// bool 	changeBank = 0;
		bool loadBank = 0;

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

		bool changeBank = 0;

		strPlayerRow row[9];

	} player;

	void handle_uStep12(uint8_t step);
	void flushNotes();
	void rec_metronome(void);
	void divChangeQuantize(uint8_t row);
	void trySwitchBank();
	uint8_t play_uStep(uint8_t row);
	void incr_uStep(uint8_t row);



	uint8_t getTempoDiv(int8_t val);

	float tempo = defs.TEMPO_DEFAULT;
	float tempoLock = defs.TEMPO_DEFAULT;
	float tempoExternal = defs.TEMPO_DEFAULT;

	uint8_t clockMode = 0; // 0 = internal, 1 = external
	IntervalTimer playTimer;
	uint16_t timerTick = 0;

} sequencer;

#endif /* SOURCE_MTSEQUENCER_H_ */
