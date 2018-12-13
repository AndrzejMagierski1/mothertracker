/*
 * mtSequencer.h
 *
 *  Created on: 13 gru 2018
 *      Author: krzys
 */
#include <stdint.h>

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

	struct strTrack
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
		float swing = defs.DEFAULT_SWING;
		//
		//	uint8_t structVer = MEMORY_STRUCT_VER;
		//	uint8_t structVerControl = MEMORY_STRUCT_VER;
		//	uint8_t rezerwa3 = 0;
		//	uint8_t rezerwa4 = 0;

		strTrack track[8];
	};
} sequencer;

#endif /* SOURCE_MTSEQUENCER_H_ */
