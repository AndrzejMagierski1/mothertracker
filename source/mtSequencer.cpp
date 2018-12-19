#include <Arduino.h>

#include "mtSequencer.h"

Sequencer sequencer;

void Sequencer::copy_step(uint8_t from_x, uint8_t from_y, uint8_t to_x,
							uint8_t to_y)
{
	from_x = constrain(from_x, 1, 32);
	from_y = constrain(from_y, 1, 32);

	to_x = constrain(to_x, 1, 32);
	to_y = constrain(to_y, 1, 32);

	seq[player.ramBank].row[to_y].step[to_x] =
			seq[player.ramBank].row[from_y].step[from_x];
}

void Sequencer::copy_row(uint8_t from, uint8_t to)
{
	from = constrain(from, 1, 8);
	to = constrain(to, 1, 8);
	seq[player.ramBank].row[to] = seq[player.ramBank].row[from];
}

void Sequencer::handle_ghosts(void)
{
	for (int8_t col = 1; col <= seq[player.ramBank].row[ghost.cnt1].length;
			col++)
	{
		int8_t motherGhost = col; // numer stepa który jest matką ghostów
		uint8_t temp_row_lenght = seq[player.ramBank].row[ghost.cnt1].length;
		uint8_t temp_hitMode =
				seq[player.ramBank].row[ghost.cnt1].step[col].hitMode;
		uint8_t temp_isOn = seq[player.ramBank].row[ghost.cnt1].step[col].isOn;

		uint8_t temp_length =
				seq[player.ramBank].row[ghost.cnt1].step[col].length1 + 1;

		if (temp_hitMode > 0 && temp_length > 1 && temp_isOn)
		{
			while (temp_length > 1
					&& !seq[player.ramBank].row[ghost.cnt1].step[col + 1
							+ ((col + 1 > temp_row_lenght) * -temp_row_lenght)].isOn)
			{
				player.row[ghost.cnt1].step[col + 1
						+ ((col + 1 > temp_row_lenght) * (-(temp_row_lenght)))].isGhost =
						motherGhost;
				temp_length--;
				col++;
			}
		}
		else
		{
			player.row[ghost.cnt1].step[col].isGhost = 0;
		}
	}

	ghost.cnt1++;
	{
		if (ghost.cnt1 > ghost.cnt1_max)
		{
			ghost.cnt1 = 1;
			// ghost.cnt2++;
			// if(ghost.cnt2>ghost.cnt2_max) ghost.cnt2=1;
		}
	}
}

/*
 void Sequencer::learnNote(uint8_t note, uint8_t velo, uint8_t channel)
 {
 if (player.isREC)
 {
 for (uint8_t row = 1; row <= 8; row++)
 {
 // if ((seq[player.ramBank].row[row].flags & MASK_ROW_ON) && (seq[player.ramBank].row[row].channelIn == channel))
 if (	(seq[player.ramBank].row[row].flags & MASK_ROW_ON) &&
 (seq[player.ramBank].row[row].channelIn == channel || seq[player.ramBank].row[row].channelIn == 0) &&
 seq[player.ramBank].row[row].channelIn != -1)
 {

 uint8_t tempPos = player.row[row].actual_pos;
 if (!player.row[row].step[tempPos].learned && !player.row[row].recNoteOpen && player.row[row].note_length_timer == 0)
 {
 seq[player.ramBank].row[row].step[tempPos].hitMode     	= 1;
 seq[player.ramBank].row[row].step[tempPos].isOn     	= 1;
 seq[player.ramBank].row[row].step[tempPos].note         = note;
 seq[player.ramBank].row[row].step[tempPos].chord        = NO_CHORD;
 seq[player.ramBank].row[row].step[tempPos].velocity     = velo;
 seq[player.ramBank].row[row].step[tempPos].offset       = player.uStep;
 player.row[row].step[tempPos].learned                   = 1;
 player.row[row].recNoteOpen                             = 1;
 player.row[row].recNoteLength                           = 1;
 player.row[row].recNote                                 = note;
 player.row[row].recChannel                              = channel;
 player.row[row].recNoteStep                             = tempPos;



 midiSendChordOn(note,
 NO_CHORD,
 velo,
 seq[player.ramBank].row[row].channel,
 seq[player.ramBank].row[row].midiOut,
 seq[player.ramBank].row[row].trackScale,
 seq[player.ramBank].row[row].rootNote);

 player.row[row].midiOut_sent    = seq[player.ramBank].row[row].midiOut;
 player.row[row].channel_sent    = seq[player.ramBank].row[row].channel;
 player.row[row].scale_sent 	    = seq[player.ramBank].row[row].trackScale;
 player.row[row].note_sent       = note;
 player.row[row].chord_sent      = NO_CHORD;
 player.row[row].noteOn_sent     = 1;





 return;
 }
 }
 }
 }
 else
 {
 for (uint8_t row = 1; row <= 8; row++)
 {
 // przyciski wierszy
 if (isButtonPressed(0, row))
 {
 // int8_t delta = note - seq[player.ramBank].row[row].rootNote;


 //seq[player.ramBank].row[row].trackVelo = velo;
 //seq[player.ramBank].row[row].channel = channel;
 //player.row[row].learned = 1;
 update_temp_track(row);
 set_LCD_mode(LCDVIEW_TRACK_NOTE, 1);

 if (!player.row[row].noteOn_sent && !isPlay())
 {
 midiSendChordOn(note,
 1,
 velo,
 seq[player.ramBank].row[row].channel,
 seq[player.ramBank].row[row].midiOut,
 seq[player.ramBank].row[row].trackScale,
 seq[player.ramBank].row[row].rootNote);

 player.row[row].midiOut_sent    = seq[player.ramBank].row[row].midiOut;
 player.row[row].channel_sent    = seq[player.ramBank].row[row].channel;
 player.row[row].scale_sent    	= seq[player.ramBank].row[row].trackScale;
 player.row[row].scaleRoot_sent  = seq[player.ramBank].row[row].rootNote;
 player.row[row].note_sent       = note;
 player.row[row].chord_sent      = 0;
 player.row[row].noteOn_sent     = 1;
 }

 // transpozycja o delte wzgledem pierwszej zapalonej nuty

 // int8_t 	first_set_note = 0;										// pierwsza nuta z brzegu
 int8_t 	delta = note - seq[player.ramBank].row[row].rootNote; 		// delta miedzy nuta przychodzącą a pierwszą z brzegu
 // bool 	first_note = 0;											// flaga odnalezienia pierwszej nuty z brzegu

 for (uint8_t col = 1; col <= 32; col++)
 {
 if (seq[player.ramBank].row[row].step[col].note <= MAX_NOTE_STEP)
 {
 // przeliczam każda kolejną nutę
 seq[player.ramBank].row[row].step[col].note =
 constrain(seq[player.ramBank].row[row].step[col].note + delta, MIN_NOTE_TRACK, MAX_NOTE_TRACK);
 }
 }

 seq[player.ramBank].row[row].rootNote = note;
 }


 // przyciski stepów
 for (uint8_t col = 1; col <= 32; col++)
 {
 if (isButtonPressed(col, row) && !player.row[row].step[col].learned)
 {
 seq[player.ramBank].row[row].step[col].note = note;
 seq[player.ramBank].row[row].step[col].chord = 0;
 seq[player.ramBank].row[row].step[col].velocity = velo;
 player.row[row].step[col].learned = 1;
 player.row[row].step[col].wasModification = 1;

 update_temp_step();
 set_LCD_mode(LCDVIEW_STEP_PROP);

 if (stepsPressed() == 1)player.row[row].step[col].learned = 0;

 if (!player.row[row].noteOn_sent )
 {
 midiSendChordOn(note,
 0,
 velo,
 seq[player.ramBank].row[row].channel,
 seq[player.ramBank].row[row].midiOut,
 seq[player.ramBank].row[row].trackScale,
 seq[player.ramBank].row[row].rootNote);

 player.row[row].midiOut_sent    = seq[player.ramBank].row[row].midiOut;
 player.row[row].channel_sent    = seq[player.ramBank].row[row].channel;
 player.row[row].scale_sent    	= seq[player.ramBank].row[row].trackScale;
 player.row[row].scaleRoot_sent	= seq[player.ramBank].row[row].rootNote;
 player.row[row].note_sent       = note;
 player.row[row].chord_sent      = 0;
 player.row[row].noteOn_sent     = 1;
 }
 return;
 }
 }
 }
 }
 }
 */
/*
 void Sequencer::learnNoteOff(uint8_t note, uint8_t velo, uint8_t channel)
 {
 if (player.isREC)
 {
 for (uint8_t row = 1; row <= 8; row++)
 {
 if (player.row[row].recChannel == channel && player.row[row].recNote == note && player.row[row].recNoteOpen)//!player.row[row].step[player.row[row].actual_pos].isGhost)
 {
 player.row[row].recNoteOpen = 0;
 seq[player.ramBank].row[row].step[player.row[row].recNoteStep].length1 = player.row[row].recNoteLength - 1;

 midiSendChordOff(note,
 0,
 0,
 player.row[row].channel_sent,
 player.row[row].midiOut_sent,
 player.row[row].scale_sent,
 player.row[row].scaleRoot_sent);
 player.row[row].noteOn_sent = 0;

 player.row[row].recNoteLength = 0;
 }
 }
 }
 else
 {
 for (uint8_t row = 1; row <= 8; row++)
 {
 if (isButtonPressed(0, row) && player.row[row].midiOut_sent && !isPlay())
 {

 midiSendChordOff(player.row[row].note_sent,
 player.row[row].chord_sent,
 0,
 player.row[row].channel_sent,
 player.row[row].midiOut_sent,
 player.row[row].scale_sent,
 player.row[row].scaleRoot_sent);

 player.row[row].noteOn_sent = 0;
 }

 for (uint8_t col = 1; col <= 32; col++)
 {
 if (isButtonPressed(col, row) && player.row[row].note_sent == note)
 {

 midiSendChordOff(player.row[row].note_sent,
 player.row[row].chord_sent,
 0,
 player.row[row].channel_sent,
 player.row[row].midiOut_sent,
 player.row[row].scale_sent,
 player.row[row].scaleRoot_sent);

 player.row[row].noteOn_sent = 0;

 return;

 }
 }
 }
 }

 }
 */

void Sequencer::rec_metronome(void)
{
	if (player.isREC && player.rec_intro_timer)
	{
		player.rec_intro_timer++;

		if (player.rec_intro_timer >= player.rec_intro_timer_max)
		{

			player.rec_intro_timer = 1;
			player.rec_intro_step++;

			if (player.rec_intro_step == 5)
			{
				if (debug.player)
					Serial.println("zaraz...");

				player.isPlay = 1;
				player.uStep = 1;
				for (uint8_t a = 1; a <= 8; a++)
				{
					player.uStepInd[a] = 1;
				}
				//player.metronome_timer = 1;

				player.rec_intro_step = 0;
				player.rec_intro_timer = 0;

//				change_grid_view_mode(VIEWMODE_SEQUENCE);

				if (debug.player)
					Serial.println("nagrywam");

//				reset_ruler_blink();
			}
		}

	}
	else if (!player.isREC)
	{
		player.rec_intro_timer = 0;
		player.rec_intro_step = 0;
	}
}

void Sequencer::action_buttonPlay(void)
{
	if (debug.player)
		Serial.println("play");

	reset_actual_pos();

	player.isStop = 0;
	player.isPlay = 1;
	timerTick = 1;

	player.uStep = 1;
	for (uint8_t a = 0; a <= 8; a++)
	{
		player.uStepInd[a] = 1;

		player.row[a].rollStep = 0;
	}

	player.metronome_timer = 1;

//	send_play();
//
//	set_playLed(1);
//	set_stopLed(0);
//
//	set_grid_view_mode(VIEWMODE_SEQUENCE_PLAY);
//	reset_ruler_blink();
//	init_player_timer();

}

void Sequencer::panic_all_notes_off(void)
{
//	for (uint8_t row = 1; row <= MAXROW; row++)
//	{
//		midiSendCC( seq[player.ramBank].row[row].channel,
//		            CC_ALL_NOTES_OFF,
//		            CC_VAL_NOTES_OFF,
//		            seq[player.ramBank].row[row].midiOut);
//	}
}

void Sequencer::action_buttonStop(void)
{

	if (player.isStop)
	{
		panic_all_notes_off();
	}

//	send_stop();

	player.isPlay = 0;
	player.isStop = 1;
	player.isREC = 0;
	player.uStep = 0;

	timerTick = 1;

	for (uint8_t a = 0; a <= 8; a++)
	{
		player.uStepInd[a] = 0;
		player.row[a].rollStep = 0;

		player.row[a].makeJump = 0;
	}
	player.changeBank = 0;

	player.swingToogle = 1;

//	set_stopLed(1);
//	set_playLed(0);

//	set_grid_view_mode(VIEWMODE_SEQUENCE);

	reset_actual_pos();
	resetLastSendMod();

	allNoteOffs();
	resetAllLearned();

	// flash_bank(player.actualBank, player.ramBank);
}

void Sequencer::resetLastSendMod(void)
{
	for (uint8_t y = 1; y <= 8; y++)
	{
		player.row[y].lastMod = 128;
	}
}

void Sequencer::resetAllLearned(void)
{
	for (uint8_t x = 1; x <= 32; x++)
	{
		for (uint8_t y = 1; y <= 8; y++)
		{
			player.row[y].step[x].learned = 0;
		}
	}
}

void Sequencer::action_buttonREC(void)
{

	if (debug.player)
		Serial.println("REC");
	player.isREC = 1;
	player.rec_intro_timer = 1;
	player.rec_intro_step = 1;

//	change_grid_view_mode(VIEWMODE_METRONOME);

//	send_play();

	player.swing_offset = 50.0;
	//rekonfig timera
	init_player_timer();

}

//void Sequencer::action_buttonClear(void)
//{
//	//set_LCD_mode(LCDVIEW_CLEAR_TRACK, 0);
//
//	for (uint8_t y = 1; y <= 8; y++)
//	{
//		if (isButtonPressed(0, y))
//		{
//			clearRow(y);
//		}
//	}
//}

void Sequencer::clearRow(uint8_t row)
{
	clearRow(row, player.ramBank);
}

void Sequencer::clearRow(uint8_t row, uint8_t bank)
{
	for (uint8_t x = 1; x <= 32; x++)
	{
		clearStep(x, row, bank);
	}
}

void Sequencer::initRow(uint8_t row, uint8_t bank)
{

	for (uint8_t x = 1; x <= 32; x++)
	{
		seq[bank].row[row].length = DEFAULT_ROW_LEN;
		seq[bank].row[row].rootNote = DEFAULT_ROW_NOTE;
		seq[bank].row[row].trackVelo = MAX_VELO_TRACK;
		seq[bank].row[row].defaultMod = DEFAULT_MOD;
		seq[bank].row[row].channel = DEFAULT_ROW_CHANNEL;
		seq[bank].row[row].cc = DEFAULT_CC;
		seq[bank].row[row].flags = 1;
		seq[bank].row[row].trackScale = 0;
		seq[bank].row[row].midiOut = MIDIOUT_USB;
		seq[bank].row[row].playMode = PLAYMODE_FORWARD;
		seq[bank].row[row].gateMode = GATEMODE.NORMAL;
		seq[bank].row[row].tempoDiv = TEMPODIV_1_1;
		seq[bank].row[row].channelIn = CHANNEL_IN_ALL;
		seq[bank].row[row].rezerwa4 = 0;

		clearStep(x, row, bank);
	}
}

void Sequencer::clearStep(uint8_t x, uint8_t row)
{
	// strRow & tempRow = seq[player.ramBank].row[row];
	// strStep & step  = tempRow.step[x];

	// step.hitMode 		= 1;
	// step.isOn 			= 0;
	// step.chord 			= MIN_CHORD;
	// step.velocity 		= MAX_VELO_STEP;
	// step.modulation 	= DEFAULT_MOD;
	// step.rollCurve 		= MIN_STEP_ROLL_VAR;

	// step.rollNoteCurve 	= MIN_STEP_ROLL_NOTE_VAR;
	// step.length1 		= MIN_STEP_LENGTH;
	// step.offset 		= OFFSET_MIN;
	// step.note 			= tempRow.rootNote;

	clearStep(x, row, player.ramBank);
}

void Sequencer::clearStep(uint8_t x, uint8_t row, uint8_t bank)
{
	strBank::strRow & tempRow = seq[bank].row[row];
	strBank::strRow::strStep & step = tempRow.step[x];

	step.hitMode = 1;
	step.isOn = 0;
	step.chord = MIN_CHORD;
	step.velocity = MAX_VELO_STEP;
	step.modulation = DEFAULT_MOD;
	step.rollCurve = MIN_STEP_ROLL_VAR;

	step.rollNoteCurve = MIN_STEP_ROLL_NOTE_VAR;
	step.length1 = MIN_STEP_LENGTH;
	step.offset = OFFSET_MIN;
	step.note = tempRow.rootNote;
}

// void Sequencer::clearBank(uint8_t pattern)
// {
// 	for (uint8_t row = 1; row <= 8; row++)
// 	{
// 		clearRow(row, 3);

// 		for (uint8_t step = 1; step <= 32; step++)
// 		{
// 			clearStep(step, row, 3);
// 		}
// 	}
// }

void Sequencer::initBank(uint8_t bank)
{
	seq[bank].tempo = DEFAULT_TEMPO;
	seq[bank].swing = DEFAULT_SWING;
	seq[bank].structVer = MEMORY_STRUCT_VER;
	seq[bank].structVerControl = MEMORY_STRUCT_VER;
	seq[bank].rezerwa3 = 0;
	seq[bank].rezerwa4 = 0;

	for (uint8_t row = 1; row <= 8; row++)
	{
		initRow(row, bank);
	}

}

void Sequencer::initPattern(uint8_t pattern) // czyści pattern z flasha
{
	// czyścimy bank 3
	initBank(3);

	if (pattern == player.actualBank)
	{
		initBank(1);
		initBank(0);
	}

	// flashuje pattern ramem z 3 pozycji
	// 	TODO
	//	flash_bank(pattern, 3);
}

//void Sequencer::action_buttonOnOff(void)
//{
//	//set_LCD_mode(LCDVIEW_ON_OFF, 0);
//
//	for (uint8_t x = 1; x <= 8; x++)
//	{
//
//		if (isButtonPressed(0, x))
//		{
//			seq[player.ramBank].row[x].flags ^= MASK_ROW_ON;
//		}
//	}
//
//}

//void Sequencer::action_buttonRandom(void)
//{
//	//set_LCD_mode(LCDVIEW_RANDOM, 0);
//
//	for (uint8_t x = 1; x <= 8; x++)
//	{
//		if (isButtonPressed(0, x))
//		{
//			randomize_row(x);
//		}
//	}
//}

void Sequencer::randomize_row(uint8_t row)
{
	bool oneRoll = 0;
	bool randomInScale = seq[player.ramBank].row[row].trackScale > 0;
	int8_t random_from = constrain(
			seq[player.ramBank].row[row].rootNote - RANDOM_NOTE_DOWN,
			MIN_NOTE_TRACK,
			MAX_NOTE_TRACK);
	int8_t random_to = constrain(
			seq[player.ramBank].row[row].rootNote + RANDOM_NOTE_UP,
			MIN_NOTE_TRACK,
			MAX_NOTE_TRACK);

	// Serial.println(random_from);
	// Serial.println(random_to);
	// Serial.println();

	uint8_t dispersion = random(2, 6);

	for (uint8_t col = 1; col <= 32; col++)
	{
		seq[player.ramBank].row[row].step[col].note =
				seq[player.ramBank].row[row].rootNote;
		seq[player.ramBank].row[row].step[col].chord = MIN_CHORD;

		if (oneRoll)
		{
			seq[player.ramBank].row[row].step[col].hitMode = random(0,
																	dispersion)
					< 1;
			if (seq[player.ramBank].row[row].step[col].hitMode)
				seq[player.ramBank].row[row].step[col].isOn = 1;
			else
				clearStep(col, row);
		}
		else
		{
			if (random(0, 4) > 2)
			{
				seq[player.ramBank].row[row].step[col].hitMode = random(0, 5);
				if (seq[player.ramBank].row[row].step[col].hitMode)
					seq[player.ramBank].row[row].step[col].isOn = 1;
				else
					clearStep(col, row);
			}
			else
			{

				seq[player.ramBank].row[row].step[col].hitMode = random(
						0, dispersion) < 1;
				if (seq[player.ramBank].row[row].step[col].hitMode)
					seq[player.ramBank].row[row].step[col].isOn = 1;
				else
					clearStep(col, row);
			}
			if (seq[player.ramBank].row[row].step[col].hitMode > 1)
				oneRoll = 1;

		}

		if (seq[player.ramBank].row[row].step[col].hitMode > 1
				&& seq[player.ramBank].row[row].step[col].isOn)
		{
			seq[player.ramBank].row[row].step[col].rollCurve = random(
					ROLL_CURVE.MIN, ROLL_CURVE.MAX + 1);
		}
		if (seq[player.ramBank].row[row].step[col].hitMode > 0
				&& seq[player.ramBank].row[row].step[col].isOn)
		{

			// jeżeli nie rolka, spróbuj randomizować nudge
			if ((seq[player.ramBank].row[row].step[col].hitMode == 1)
					&& seq[player.ramBank].row[row].step[col].isOn
					&& (seq[player.ramBank].row[row].flags & MASK_RANDOM_NUDGE))
			{
				if (random(0, 2) < 1)
				{
					seq[player.ramBank].row[row].step[col].hitMode =
					HITMODE_OFFSET;
					seq[player.ramBank].row[row].step[col].offset = random(
							OFFSET_MIN, OFFSET_MAX);
					;
				}
				else
				{
					seq[player.ramBank].row[row].step[col].offset = OFFSET_MIN;
				}
			}

			// RANDOM VELO
			if ((seq[player.ramBank].row[row].flags & MASK_RANDOM_VELO))
			{
				seq[player.ramBank].row[row].step[col].velocity = random(
						RANDOM_VELO_MIN, RANDOM_VELO_MAX);
			}
			else
			{
				seq[player.ramBank].row[row].step[col].velocity = 127;
			}

			// RANDOM MOD
			if ((seq[player.ramBank].row[row].flags & MASK_RANDOM_MOD))
			{
				seq[player.ramBank].row[row].step[col].modulation = random(
						RANDOM_MOD_MIN, RANDOM_MOD_MAX);
			}
			else
			{
				seq[player.ramBank].row[row].step[col].modulation = DEFAULT_MOD;
			}

			// RANDOM NOTE
			if (randomInScale)
			{
				for (uint8_t a = 0; a <= 15; a++)
				{
					seq[player.ramBank].row[row].step[col].note = random(
							random_from, random_to);

					if (isInScale(seq[player.ramBank].row[row].step[col].note,
									seq[player.ramBank].row[row].rootNote,
									seq[player.ramBank].row[row].trackScale))
						break;

					if (a == 15)
						seq[player.ramBank].row[row].step[col].note =
								seq[player.ramBank].row[row].rootNote;
				}
				// Serial.println(seq[player.ramBank].row[row].step[col].note);
			}
			else
			{
				seq[player.ramBank].row[row].step[col].note =
						seq[player.ramBank].row[row].rootNote;
			}

			seq[player.ramBank].row[row].step[col].length1 = 0;
		}

	}
}

uint8_t Sequencer::isInScale(uint8_t note, uint8_t root, uint8_t scale)
{
	uint8_t noteName = note % 12; //24 -> 0
	uint8_t rootName = root % 12; //13 -> 1

	// Serial.print("isInScale: note: ");
	// Serial.print(noteName);
	// Serial.print("root: ");
	// Serial.print(rootName);
	// Serial.print("scale: ");
	// Serial.print(scale);

	if (rootName <= noteName)
	{
		if (scaleDef[scale] & (1 << (noteName - rootName)))
		{
			// Serial.println(" <=yes!");
			return 1;
		}
	}
	else
	{
		if (scaleDef[scale] & (0b000000000001 << (12 - rootName + noteName)))
		{
			// Serial.println(" >yes!");
			return 1;
		}
	}
	// Serial.println(" no...");
	return 0;
}
/*
 void Sequencer::action_buttonQuantize(void)
 {
 //set_LCD_mode(LCDVIEW_QUANTIZE, 0);
 for (uint8_t y = 1; y <= 8; y++)
 {
 // tracki
 if (isButtonPressed(ROW_COLUMN, y))
 {
 for (uint8_t x = 0; x <= seq[player.ramBank].row[y].length; x++)
 {
 if ((seq[player.ramBank].row[y].step[x].offset < 24))
 {
 seq[player.ramBank].row[y].step[x].offset = 1;
 }
 else
 {
 if (x < seq[player.ramBank].row[y].length)
 {
 seq[player.ramBank].row[y].step[x].offset = 1;
 seq[player.ramBank].row[y].step[x + 1] = seq[player.ramBank].row[y].step[x];
 clearStep(x, y);
 // seq[player.ramBank].row[y].step[x] = templateBank.row[0].step[0];
 }
 }
 // // quantize dla nut
 // uint8_t scaleCorrection = 0;
 // while (isInScale(seq[player.ramBank].row[y].step[x].note + scaleCorrection,
 //                  seq[player.ramBank].row[y].rootNote,
 //                  seq[player.ramBank].row[y].trackScale))
 // {
 // 	scaleCorrection++;
 // }





 }
 // set_backToIdle();
 }

 //stepy
 for (uint8_t x = 1; x <= seq[player.ramBank].row[y].length; x++)
 {
 if (isButtonPressed(x, y))
 {
 if (seq[player.ramBank].row[y].step[x].offset > 1)
 {
 seq[player.ramBank].row[y].step[x].offset 	= 1;
 seq[player.ramBank].row[y].step[x].hitMode 	= 1;
 seq[player.ramBank].row[y].step[x].isOn 	= 1;
 }
 update_temp_step();
 set_updateLCD();


 // // quantize dla nut
 // uint8_t scaleCorrection = 0;
 // while (isInScale(seq[player.ramBank].row[y].step[x].note + scaleCorrection,
 //                  seq[player.ramBank].row[y].rootNote,
 //                  seq[player.ramBank].row[y].trackScale))
 // {
 // 	scaleCorrection++;
 // }
 }
 }
 }
 }

 */

uint8_t Sequencer::isRowOn(uint8_t row)
{
	return (seq[player.ramBank].row[row].flags & MASK_ROW_ON) > 0;
}

//uint8_t Sequencer::get_copy_row_from(void)
//{
//	return player.copy_row_from;
//}
//uint8_t Sequencer::get_copy_row_to(void)
//{
//	return player.copy_row_to;
//}

/*
 void Sequencer::push_track(uint16_t row)
 {
 // odwrocona kolejnosc:
 // najpierw funkcja, potem wiersz
 uint8_t functionDetected = 0;

 update_temp_track(row);
 set_updateLCD();

 // if(get_LCD_mode() == LCDVIEW_TRACK_PROP)
 // {
 //  update_temp_track(row);
 //  set_updateLCD();
 // }


 if (tracksPressed() > 1) return;

 //Serial.print("push_track");
 for (uint8_t x = 1; x <= 8; x++)
 {
 if (isButtonPressed(-1, x))
 {
 functionDetected = 1;
 if (x == BUTTON_ROW_ONOFF)
 {
 seq[player.ramBank].row[row].flags ^= MASK_ROW_ON;
 }

 else if (x == BUTTON_ROW_CLEAR)
 {
 clearRow(row);
 }

 else if (x == BUTTON_ROW_RANDOM)
 {
 randomize_row(row);
 }

 else if (x == BUTTON_ROW_QUANTIZE)
 {
 action_buttonQuantize();
 }

 else if (x == BUTTON_ROW_DUPLICATE)
 {
 if (player.copy_mode_step == 0)
 {
 set_LCD_mode(LCDVIEW_DUPLICATE_TRACK , 0);
 player.copy_mode = COPY_MODE_ROW;
 player.copy_row_from = row;
 player.copy_mode_step++;
 }
 else if (player.copy_mode_step > 0)
 {
 set_LCD_mode(LCDVIEW_DUPLICATE_TRACK , 0);
 player.copy_mode = COPY_MODE_ROW;
 player.copy_row_to = row;
 player.copy_mode_step++;
 }

 if (player.copy_mode_step > 1)
 {
 copy_row(player.copy_row_from, player.copy_row_to);
 }
 }
 }
 }
 if (!functionDetected && isEncPressed(ENC_NOTE))
 {

 midiSendChordOn(seq[player.ramBank].row[row].rootNote,
 NO_CHORD,
 seq[player.ramBank].row[row].trackVelo,
 seq[player.ramBank].row[row].channel,
 seq[player.ramBank].row[row].midiOut,
 seq[player.ramBank].row[row].trackScale,
 seq[player.ramBank].row[row].rootNote);

 player.row[row].note_sent       = seq[player.ramBank].row[row].rootNote;
 player.row[row].chord_sent      = NO_CHORD;
 player.row[row].noteOn_sent     = 1;
 player.row[row].midiOut_sent    = seq[player.ramBank].row[row].midiOut;
 player.row[row].channel_sent    = seq[player.ramBank].row[row].channel;
 player.row[row].scale_sent    	= seq[player.ramBank].row[row].trackScale;
 player.row[row].scaleRoot_sent  = seq[player.ramBank].row[row].rootNote;
 }


 }
 */
/*
 void Sequencer::release_track(uint16_t row)
 {
 // odwrocona kolejnosc:
 // najpierw funkcja, potem wiersz
 // uint8_t functionDetected = 0;

 // for (uint8_t x = 1; x <= 8; x++)
 // {
 //  if (isButtonPressed(-1, x))
 //  {
 //      functionDetected = 1;

 //  }
 // }
 // if (!functionDetected)
 // {
 if (player.row[row].noteOn_sent && !isPlay())
 {
 player.row[row].noteOn_sent = 0;
 // todo
 //		midiSendChordOff(player.row[row].note_sent,
 //		                 player.row[row].chord_sent,
 //		                 0,
 //		                 player.row[row].channel_sent,
 //		                 player.row[row].midiOut_sent,
 //		                 player.row[row].scale_sent,
 //		                 player.row[row].scaleRoot_sent);
 }

 // Serial.print("tracksPressed: ");
 // Serial.println(tracksPressed());
 if ((tracksPressed() == 1) && (get_LCD_mode() == LCDVIEW_TRACK_PROP))
 {
 set_backToIdle();
 }
 else
 {
 reset_LcdTimer();
 }


 }*/

void Sequencer::init_player(void)
{
	init_defaultPlayerParameters();
}

void Sequencer::init_defaultPlayerParameters(void)
{
	for (uint8_t x = 1; x <= 8; x++)
	{
		seq[player.ramBank].row[x].rootNote = 35 + x;
	}

	for (uint8_t a = 1; a <= 8; a++)
	{
		seq[player.ramBank].row[a].channel = a;
	}
}

uint8_t Sequencer::get_metronome_intro_step(void)
{
	return player.rec_intro_step;
}

uint16_t Sequencer::get_size(void)
{
	return sizeof(seq[0]);
}

void Sequencer::allNoteOffs(void)
{
	for (uint8_t a = 1; a <= 8; a++)
	{
		if (player.row[a].noteOn_sent)
		{
//			// TODO
//			midiSendChordOff(player.row[a].note_sent,
//			                 player.row[a].chord_sent,
//			                 0,
//			                 player.row[a].channel_sent,
//			                 player.row[a].midiOut_sent,
//			                 player.row[a].scale_sent,
//			                 player.row[a].scaleRoot_sent);
			player.row[a].noteOn_sent = 0;
		}

	}

	// Serial.println("all stop");
}

//uint8_t Sequencer::get_copy_step_from_row(void)
//{
//	return player.copy_step_from_row;
//}
//uint8_t Sequencer::get_copy_step_from_col(void)
//{
//	return player.copy_step_from_col;
//}
//
//uint8_t Sequencer::get_copy_step_to_row(void)
//{
//	return player.copy_step_to_row;
//}
//uint8_t Sequencer::get_copy_step_to_col(void)
//{
//	return player.copy_step_to_col;
//}

//uint8_t get_copy_bank_from(void)
//{
//	return player.copy_bank_from;
//}

//uint8_t get_copy_mode_step(void)
//{
//	return player.copy_mode_step;
//}

//uint8_t get_copy_bank_to(void)
//{
//	return player.copy_bank_to;
//}
/*
 void Sequencer::push_step(int16_t x, uint16_t y)
 {
 //uint8_t extraFunction = 0;

 //set_LCD_mode(LCDVIEW_DUPLICATE_PATTERN , 0);


 if (stepsPressed() == 0) seq[2].row[0].step[0] = seq[player.ramBank].row[y].step[x];

 if (isEncPressed(ENC_LENGTH))
 {
 // LENLEN
 seq[player.ramBank].row[y].length = x;
 change_grid_view_mode(VIEWMODE_ROW_LENGTH);
 //extraFunction = 1;
 }

 // granie stepem
 else if (isEncPressed(ENC_NOTE) )
 {
 if (!player.row[y].noteOn_sent)
 {
 midiSendChordOn(seq[player.ramBank].row[y].step[x].note,
 seq[player.ramBank].row[y].step[x].chord,
 seq[player.ramBank].row[y].step[x].velocity,
 seq[player.ramBank].row[y].channel,
 seq[player.ramBank].row[y].midiOut,
 seq[player.ramBank].row[y].trackScale,
 seq[player.ramBank].row[y].rootNote);

 midiSendCC(seq[player.ramBank].row[y].channel,
 seq[player.ramBank].row[y].cc,
 seq[player.ramBank].row[y].step[x].modulation,
 seq[player.ramBank].row[y].midiOut);


 player.row[y].step[x].wasModification = 1;
 player.row[y].noteOn_sent   = 1;
 player.row[y].note_sent     = seq[player.ramBank].row[y].step[x].note;
 player.row[y].chord_sent    = seq[player.ramBank].row[y].step[x].chord;
 player.row[y].midiOut_sent  = seq[player.ramBank].row[y].midiOut;
 player.row[y].channel_sent  = seq[player.ramBank].row[y].channel;
 player.row[y].scale_sent  	= seq[player.ramBank].row[y].trackScale;
 player.row[y].scaleRoot_sent = seq[player.ramBank].row[y].rootNote;



 }

 }

 else if (isEncPressed(ENC_MOVE))
 {
 // player.row[y].actual_pos = x;
 // player.uStepInd[y] = 1;

 player.row[y].goToStep = x;
 }


 // kopiujemy bank
 else if (isButtonPressed(-1, BUTTON_ROW_DUPLICATE) && isButtonPressed(-1, BUTTON_ROW_PATTERN))
 {

 if ((player.copy_mode == COPY_MODE_BANK) && (player.copy_mode_step == 0))
 {
 player.copy_bank_from = x - 1 + (y - 1) * 32;
 player.copy_bank_to = 0;

 player.copy_mode_step++;
 set_LCD_mode(LCDVIEW_DUPLICATE_PATTERN , 0);
 set_updateLCD();
 }

 else if ((player.copy_mode == COPY_MODE_BANK) && (player.copy_mode_step >= 1))
 {
 player.copy_bank_to =  x - 1 + (y - 1) * 32;;
 player.copy_mode_step++;

 set_LCD_mode(LCDVIEW_DUPLICATE_PATTERN , 0);
 set_updateLCD();
 copy_bank(player.copy_bank_from, player.copy_bank_to);
 }


 }

 // czyścimy bank
 else if (isButtonPressed(-1, BUTTON_ROW_CLEAR) && isButtonPressed(-1, BUTTON_ROW_PATTERN))
 {
 initPattern(x - 1 + (y - 1) * 32);
 }


 // kopiujemy step
 else if (isButtonPressed(-1, BUTTON_ROW_DUPLICATE))
 {
 // if(x<=16)
 // {
 //player.copy_mode =
 if (player.copy_mode_step == 0 ) //&& player.copy_mode != COPY_MODE_BANK
 {
 set_LCD_mode(LCDVIEW_DUPLICATE_STEP , 0);
 player.copy_mode_step = 1;
 player.copy_step_from_col = x;
 player.copy_step_from_row = y;
 player.copy_mode = COPY_MODE_STEP;
 }
 else if (player.copy_mode_step > 0 && player.copy_mode == COPY_MODE_STEP)
 {
 set_LCD_mode(LCDVIEW_DUPLICATE_STEP , 0);
 player.copy_mode_step++;
 player.copy_step_to_col = x;
 player.copy_step_to_row = y;
 }
 if (player.copy_mode_step > 1)
 {
 copy_step(player.copy_step_from_col, player.copy_step_from_row, player.copy_step_to_col, player.copy_step_to_row);
 set_LCD_mode(LCDVIEW_DUPLICATE_STEP , 0);
 }
 // }

 }
 // zmieniamy bank
 else if (isButtonPressed(-1, BUTTON_ROW_PATTERN))
 {
 // Serial.printf("player.changeBank: %d\n", player.changeBank);
 // pobierz
 if (player.changeBank)
 {
 setLoadBank2Ram((y - 1) * 32 + x - 1);
 loadBank2Ram(1); // no RAM switch

 for (uint8_t a = 1; a <= 8; a++)
 {
 player.row[a].makeJump = 0;
 }
 }
 else
 {
 setLoadBank2Ram((y - 1) * 32 + x - 1);
 loadBank2Ram();
 }

 switch_bank();
 clear_isBlinking();

 set_updateLCD();

 player.row[y].step[x].isBlinking = 1;



 }






 else
 {
 if ((player.row[y].step[x].wasModification == 0))
 {
 if (x <= seq[player.ramBank].row[y].length)
 {
 if (!seq[player.ramBank].row[y].step[x].isOn)
 {
 seq[player.ramBank].row[y].step[x].isOn = 1;
 // seq[player.ramBank].row[y].step[x].note = seq[player.ramBank].row[y].rootNote;
 // seq[player.ramBank].row[y].step[x].velocity = 127;//seq[player.ramBank].row[y].trackVelo;
 // seq[player.ramBank].row[y].step[x].modulation = seq[player.ramBank].row[y].defaultMod;
 player.row[y].step[x].justPressed = 1;
 }
 }



 // if(seq[player.ramBank].row[y].step[x].hitMode)
 // {
 //  seqLedPwm(x,y,255);
 // }
 // else
 // {
 //  seqLedPwm(x,y,0);
 // }
 }
 }
 }
 */

/*
 void Sequencer::hold_step(int16_t x, uint16_t y)
 {
 player.row[y].step[x].wasModification = 1;
 if (get_LCD_mode() == 0 || ((get_LCD_mode() == LCDVIEW_STEP_PROP) && isStepPressed()))
 {
 set_LCD_mode(LCDVIEW_STEP_PROP);
 seq[2].row[0].step[0] = seq[player.ramBank].row[y].step[x];

 }

 }
 */
/*
 void Sequencer::hold_track(int16_t x, uint16_t y)
 {
 if (get_LCD_mode() == 0 || ((get_LCD_mode() == LCDVIEW_TRACK_PROP) && isTrackPressed()))
 {
 set_LCD_mode(LCDVIEW_TRACK_PROP);
 update_temp_track(y);
 }
 }
 */
/*
 void Sequencer::hold_function(int16_t x, uint16_t y)
 {

 if (y == BUTTON_ROW_CLEAR)            action_buttonClear();

 if (get_LCD_mode() == LCDVIEW_IDLE)
 {
 if (isButtonPressed(BUTTON_COL_FUNCTION, BUTTON_ROW_PATTERN) && player.copy_mode == 0)
 {
 set_LCD_mode(LCDVIEW_PATTERN, 0);
 player.row[(player.actualBank / 32) + 1].step[(player.actualBank % 32) + 1].isBlinking = 1;
 }


 if (isButtonPressed(BUTTON_COL_FUNCTION, BUTTON_ROW_DUPLICATE) && player.copy_mode == 0)
 {
 set_LCD_mode(LCDVIEW_DUPLICATE, 0);
 }

 if (isButtonPressed(BUTTON_COL_FUNCTION, BUTTON_ROW_QUANTIZE))
 {
 set_LCD_mode(LCDVIEW_QUANTIZE, 0);
 }

 if (isButtonPressed(BUTTON_COL_FUNCTION, BUTTON_ROW_RANDOM))
 {
 set_LCD_mode(LCDVIEW_RANDOM, 0);
 }

 if (isButtonPressed(BUTTON_COL_FUNCTION, BUTTON_ROW_ONOFF))
 {
 set_LCD_mode(LCDVIEW_ON_OFF, 0);
 }

 // clear pattern
 if (isButtonPressed(BUTTON_COL_FUNCTION, BUTTON_ROW_CLEAR) && isButtonPressed(BUTTON_COL_FUNCTION, BUTTON_ROW_PATTERN))
 {
 set_LCD_mode(LCDVIEW_CLEAR_PATTERN, 0);
 }
 // clear track
 else if (isButtonPressed(BUTTON_COL_FUNCTION, BUTTON_ROW_CLEAR))
 {
 set_LCD_mode(LCDVIEW_CLEAR_TRACK, 0);
 }
 }

 else if (get_LCD_mode() == LCDVIEW_CLEAR_TRACK)
 {
 if (isButtonPressed(BUTTON_COL_FUNCTION, BUTTON_ROW_CLEAR) && isButtonPressed(BUTTON_COL_FUNCTION, BUTTON_ROW_PATTERN))
 {
 set_LCD_mode(LCDVIEW_CLEAR_PATTERN, 0);
 }
 }

 }

 */

/*
 void Sequencer::release_step(int16_t x, uint16_t y)
 {
 //uint8_t extraFunction = 0;

 if (isEncPressed(ENC_LENGTH))
 {
 //extraFunction = 1;
 }

 else if (isEncPressed(ENC_MOVE))
 {
 //extraFunction = 1;
 }

 else if (isButtonPressed(-1, BUTTON_ROW_PATTERN))
 {
 //extraFunction = 1;
 }

 else if (isButtonPressed(-1, BUTTON_ROW_DUPLICATE))
 {
 //extraFunction = 1;
 }


 else
 {
 if (player.row[y].step[x].wasModification == 0 && !player.row[y].step[x].justPressed)
 {

 if (seq[player.ramBank].row[y].step[x].isOn)
 {
 seq[player.ramBank].row[y].step[x].isOn = 0;
 }
 }
 else
 {
 player.row[y].step[x].wasModification = 0;
 player.row[y].step[x].justPressed = 0;
 player.row[y].step[x].learned = 0;
 }

 // zagralismy wiec gasimy
 if (player.row[y].noteOn_sent && !isPlay())
 {
 midiSendChordOff(player.row[y].note_sent,
 player.row[y].chord_sent,
 0,
 player.row[y].channel_sent,
 player.row[y].midiOut_sent,
 player.row[y].scale_sent,
 player.row[y].scaleRoot_sent);
 player.row[y].noteOn_sent = 0;
 }

 if ((stepsPressed() == 1) && (get_LCD_mode() == LCDVIEW_STEP_PROP))
 {
 set_backToIdle();
 }
 else
 {
 reset_LcdTimer();
 }


 move_selected_steps_on_release(x, y);
 clear_isBlinking();
 }
 }*/

void Sequencer::switchStep(uint8_t row) //przełączamy stepy w zależności od trybu grania
{
	uint8_t x = constrain(row, 1, 8);

	if (player.isREC && player.row[x].recNoteOpen)
	{
		player.row[x].recNoteLength++;
		seq[player.ramBank].row[x].step[player.row[x].recNoteStep].length1 =
				player.row[x].recNoteLength - 1;

	}

	if (player.row[x].goToStep)
	{
		player.row[x].actual_pos = player.row[x].goToStep;
		player.row[x].goToStep = 0;
	}
	else if (player.row[x].makeJump)
	{
		for (uint8_t a = 1; a <= 8; a++)
		{
			player.row[x].makeJump = 0;
		}
		// switch_bank_with_reset();
		player.jumpNOW = 1;
	}
	else
	{
		if (seq[player.ramBank].row[x].playMode == PLAYMODE_FORWARD)
		{
			player.row[x].actual_pos++;
			if ((player.row[x].actual_pos > seq[player.ramBank].row[x].length)
					|| player.row[x].return2start)
			{
				// player.row[x].return2start = 0;
				reset_actual_pos(x);

				// if (player.changeBank)
				// {
				// 	switch_bank();
				// 	// player.row[x].return2start = 1;
				// }
			}
		}
		else if (seq[player.ramBank].row[x].playMode == PLAYMODE_BACKWARD)
		{
			player.row[x].actual_pos--;
			if ((player.row[x].actual_pos < 1) || player.row[x].return2start)
			{
				// player.row[x].return2start = 0;
				reset_actual_pos(x);

				// if (player.changeBank)
				// {
				// 	switch_bank();
				// 	// player.row[x].return2start = 1;
				// }
			}
		}
		else if (seq[player.ramBank].row[x].playMode == PLAYMODE_PINGPONG)
		{
			if (!player.row[x].pingPongToogle)
			{
				if ((player.row[x].actual_pos
						>= seq[player.ramBank].row[x].length)
						|| player.row[x].return2start)
				{
					// player.row[x].return2start = 0;

					// if (player.changeBank)
					// {
					// 	switch_bank();
					// 	// player.row[x].return2start = 1;
					// }
					player.row[x].pingPongToogle =
							!player.row[x].pingPongToogle;
				}
				else
				{
					player.row[x].actual_pos++;
				}

			}
			else
			{
				if ((player.row[x].actual_pos <= 1)
						|| player.row[x].return2start)
				{
					// player.row[x].return2start = 0;

					// if (player.changeBank)
					// {
					// 	switch_bank();
					// 	// player.row[x].return2start = 1;

					// }
					player.row[x].pingPongToogle =
							!player.row[x].pingPongToogle;
				}
				else
				{
					player.row[x].actual_pos--;
				}
			}
		}

		else if (seq[player.ramBank].row[x].playMode == PLAYMODE_RANDOM)
		{
			player.row[x].actual_pos = random(
					1, seq[player.ramBank].row[x].length + 1);
		}
	}

	/*
	 if (player.row[x].return2start)
	 {
	 Serial.print("ret to start ");
	 Serial.println(x);
	 reset_actual_pos();
	 for (uint8_t a = 1 ; a <= 8; a++)
	 {
	 // player.row[x].actual_pos = 1;
	 player.row[x].return2start = 0;
	 timerTick = 1;

	 player.uStepInd[x] = 1;
	 }
	 player.uStep = 1;

	 // action_buttonPlay();


	 }*/

}

void Sequencer::reset_actual_pos(void)
{
	for (uint8_t row = 1; row <= MAXROW; row++)
	{
		player.row[row].pingPongToogle = 0;
		reset_actual_pos(row);
	}
}

void Sequencer::reset_actual_pos(uint8_t row)
{

	if (seq[player.ramBank].row[row].playMode == PLAYMODE_FORWARD)
	{
		player.row[row].actual_pos = 1;
	}

	else if (seq[player.ramBank].row[row].playMode == PLAYMODE_BACKWARD)
	{
		player.row[row].actual_pos = seq[player.ramBank].row[row].length;
	}

	else if (seq[player.ramBank].row[row].playMode == PLAYMODE_PINGPONG)
	{
		player.row[row].actual_pos = 1;
	}

}

uint8_t Sequencer::play_uStep(uint8_t row)
{

	uint8_t anythingSent = 0;
	uint8_t x = constrain(row, 1, 8);
	strBank::strRow & tempSeqRow = seq[player.ramBank].row[x];
	strPlayer::strPlayerRow & tempPlayerRow = player.row[x];

	// TU LICZYMY CZAS DO KONCA NUTY
	if (player.row[x].note_length_timer > 0)					//ZMIANA
	{
		player.row[x].note_length_timer--;
		if (player.row[x].note_length_timer < 0)
			player.row[x].note_length_timer = 0; //bzdura ale huj, zostawie
	}
	// wyłączamy nutę jeśli się skończyła
	if ((player.row[x].note_length_timer <= 1) && (player.row[x].noteOn_sent)
			&& !player.row[x].recNoteOpen)
	{
		// GASIMY
		if (player.row[x].note_sent <= 127)
		{
			// Serial.println("gasimy");

			// wykoment
//			midiSendChordOff(player.row[x].note_sent,
//			                 player.row[x].chord_sent,
//			                 0,
//			                 player.row[x].channel_sent,
//			                 player.row[x].midiOut_sent,
//			                 player.row[x].scale_sent,
//			                 player.row[x].scaleRoot_sent);

			// usbMIDI.send_now();
		}
		else if (player.row[x].note_sent == NOTE_JUMPTO)
		{

		}
		else if (player.row[x].note_sent == NOTE_JUMPTO_NEXT)
		{

		}
		player.row[x].noteOn_sent = 0;
	}

	// wysyłamy zegar
	if ((player.uStepInd[x] > 0) && player.isPlay)
	{
		if (((player.uStepInd[x] - 1) % 8) == 0)
		{
			// wykoment
//			send_clock(x);
			// Serial.println("clock");
			// Serial.println(player.uStepInd[x]);
		}
	}

	// uint8_t temp_hitMode, temp_isOn, temp_offset;//, temp_isGhost;
	const uint8_t temp_hitMode =
			seq[player.ramBank].row[x].step[player.row[x].actual_pos].hitMode;
	const uint8_t temp_rowIsOn =
			(seq[player.ramBank].row[x].flags & MASK_ROW_ON) > 0;
	const uint8_t temp_stepIsOn =
			seq[player.ramBank].row[x].step[player.row[x].actual_pos].isOn;

	const uint8_t temp_rollMode = val2roll(
			seq[player.ramBank].row[x].step[player.row[x].actual_pos].hitMode);
	const uint8_t temp_rollCurve =
			seq[player.ramBank].row[x].step[player.row[x].actual_pos].rollCurve;
	const uint8_t temp_offset =
			seq[player.ramBank].row[x].step[player.row[x].actual_pos].offset;
	const uint8_t temp_stepVelo =
			seq[player.ramBank].row[x].step[player.row[x].actual_pos].velocity;
	// temp_isGhost 	= player.row[x].step[player.row[x].actual_pos].isGhost;

	if (temp_hitMode >= 1 && temp_rowIsOn && temp_stepIsOn)
	{
		//

		// TU GRAMY
		if (((temp_hitMode != HITMODE_OFFSET)
				&& ((player.uStepInd[x] % (48 / temp_rollMode)) == 1))
				|| ((temp_hitMode == HITMODE_OFFSET)
						&& (player.uStepInd[x] == temp_offset)))
		{

			if ((player.uStepInd[x] % 48) == 1)
			{
				player.row[x].rollLength = 0;
				player.row[x].rollCounter = 0;
				// Serial.print("cycki1 ");
			}
			// tempPlayerRow.rollLength = 0;

			const uint8_t tempNote =
					tempSeqRow.step[player.row[x].actual_pos].note;
			const uint8_t tempChord =
					tempSeqRow.step[player.row[x].actual_pos].chord;
			const uint8_t tempMod =
					tempSeqRow.step[player.row[x].actual_pos].modulation;
			const uint8_t tempScale = tempSeqRow.trackScale;
			const uint8_t tempRoot = tempSeqRow.rootNote;
			const uint8_t tempLength =
					tempSeqRow.step[player.row[x].actual_pos].length1 + 1;

			// sprawdzam czy początek rolki
			if ((temp_hitMode != HITMODE_OFFSET) && (temp_hitMode > 1)
					&& ((player.uStepInd[x] % 48) == 1))
			{
				player.row[x].rollLength = tempLength;
				player.row[x].rollStep = player.row[x].actual_pos;
				// Serial.print("cycki2 ");

				// Serial.print("<");
			}
			else if ((temp_hitMode != HITMODE_OFFSET) && (temp_hitMode == 1))
			{
				player.row[x].rollStep = player.row[x].actual_pos;
			}

			// procent postępu rolki
			const float rollProgress = (float(player.uStepInd[row])
					/ (float(tempLength) * 48)) * 100;
			// Serial.print("progress: ");
			// Serial.println(rollProgress);

			float tempVelo =
					temp_hitMode > 1 ?
							getLongRollVelo(temp_rollCurve, rollProgress)
									* ((float) temp_stepVelo / 127) :
							temp_stepVelo;

			tempVelo = (tempVelo / MAX_VELO_TRACK)
					* seq[player.ramBank].row[x].trackVelo;

			//wyciszamy jezeli bylo cos wczesniej
			if (player.row[x].noteOn_sent)
			{
				// midiSendNoteOff(player.row[x].note_sent,
				//                 0,
				//                 player.row[x].channel_sent,
				//                 player.row[x].midiOut_sent);

				// wykoment
//				midiSendChordOff(player.row[x].note_sent,
//				                 player.row[x].chord_sent,
//				                 0,
//				                 player.row[x].channel_sent,
//				                 player.row[x].midiOut_sent,
//				                 player.row[x].scale_sent,
//				                 player.row[x].scaleRoot_sent);

				// usbMIDI.send_now();

			}
			//delay(2);

			//gramy

			if (player.row[x].rollLength > 0)
				++player.row[x].rollCounter;

			if (tempNote <= MAX_NOTE_TRACK && !player.row[x].divChange)
			{

				player.row[x].note_sent = constrain(
						(int16_t ) tempNote
								+ (int16_t ) getNextRollNoteOffset(x),
						MIN_NOTE_STEP, MAX_NOTE_STEP);
				// Serial.println(player.row[x].note_sent);

				midiSendChordOn(
						constrain(player.row[x].note_sent, MIN_NOTE_STEP,
									MAX_NOTE_STEP),
						tempChord,
						uint8_t(tempVelo),
						seq[player.ramBank].row[x].channel,
						seq[player.ramBank].row[x].midiOut,
						tempScale,
						tempRoot);

				anythingSent = 1;

				// usbMIDI.send_now();

				player.row[x].midiOut_sent = seq[player.ramBank].row[x].midiOut;
				player.row[x].channel_sent = seq[player.ramBank].row[x].channel;

				// jesli wystapila zmiana wysylamy mod
				if (tempMod != NULL_MOD)
				{
					midiSendCC(seq[player.ramBank].row[x].cc,
								tempMod,
								seq[player.ramBank].row[x].channel,
								seq[player.ramBank].row[x].midiOut);
					// player.row[x].lastMod = tempMod;
				}
			}

			else if (tempNote == NOTE_JUMPTO)
			{
				uint8_t can_i_jump = 1;
				for (uint8_t a = 1; a <= 8; a++)
					if (player.row[a].makeJump)
						can_i_jump = 0;

				if (can_i_jump)
				{
					seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1 =
							0; // moment przed koncem, zeby nie zaświecała kolejna dioda

					player.row[x].lastMod = tempMod;  // mod mówi gdzie skaczemy

					setLoadBank2Ram(tempMod);

					player.row[x].makeJump = 1;
				}

				// JUMP TO po odliczeniu długości nuty

			}
			else if (tempNote == NOTE_JUMPTO_NEXT)
			{

				uint8_t can_i_jump = 1;
				for (uint8_t a = 1; a <= 8; a++)
					if (player.row[a].makeJump)
						can_i_jump = 0;

				if (can_i_jump)
				{
					seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1 =
							0; // moment przed koncem, zeby nie zaświecała kolejna dioda

					// player.row[x].lastMod = tempMod;    // mod mówi gdzie skaczemy

					setLoadBank2Ram(
							player.actualBank < 255 ?
														player.actualBank + 1 :
														0);

					player.row[x].makeJump = 1;
				}

			}

			// player.row[x].note_sent = constrain(tempNote + getLastRollNoteOffset(x), MIN_NOTE_STEP, MAX_NOTE_STEP);
			player.row[x].chord_sent = tempChord;
			player.row[x].scale_sent = tempScale;
			player.row[x].scaleRoot_sent = tempRoot;
			player.row[x].noteOn_sent = 1;

			if ((temp_hitMode > 1) && (temp_hitMode != HITMODE_OFFSET))
			{
				switch (seq[player.ramBank].row[x].gateMode)
				{
				case GATEMODE.NORMAL:
					player.row[x].note_length_timer = (48 / temp_rollMode) - 2;
					break;

				case GATEMODE.MEDIUM:
					player.row[x].note_length_timer = (48 / temp_rollMode) / 2;
					break;

				case GATEMODE.SHORT:
					player.row[x].note_length_timer = 1;
					break;

				case GATEMODE.EXTRASHORT:
					player.row[x].note_length_timer = 1;
					break;

				default:
					player.row[x].note_length_timer =
							(seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1
									+ 1) * 48 - 3;
				}
			}

			else
			{
				// player.row[x].note_length_timer = seq[player.ramBank].row[x].step[player.row[x].actual_pos].length * 48 - 3;
				// if (seq[player.ramBank].row[x].playMode == PLAYMODE_FORWARD) player.row[x].note_length_timer = 10;

				switch (seq[player.ramBank].row[x].gateMode)
				{
				case GATEMODE.NORMAL:
					player.row[x].note_length_timer =
							(seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1
									+ 1) * 48 - 3;
					break;

				case GATEMODE.MEDIUM:
					player.row[x].note_length_timer =
							(seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1
									+ 1) * 48 - 24;
					break;

				case GATEMODE.SHORT:
					player.row[x].note_length_timer =
							(seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1
									+ 1) * 48 - 36;
					break;

				case GATEMODE.EXTRASHORT:
					player.row[x].note_length_timer =
							(seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1
									+ 1) * 48 - 43;
					break;

				default:
					player.row[x].note_length_timer =
							(seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1
									+ 1) * 48 - 3;
				}
			}

			if (tempNote == NOTE_JUMPTO)
				player.row[x].note_length_timer -= 1; // zeby diody nastepne po jumpie nie zaswiecily
		}
	}

	else if (tempPlayerRow.rollLength > 0)
	{
		// pobieramy dane
		const uint8_t ghostedStep = tempPlayerRow.rollStep;
		// const uint8_t ghost_hitMode 	= seq[player.ramBank].row[x].step[ghostedStep].hitMode;
		const uint8_t ghost_rollMode = val2roll(
				seq[player.ramBank].row[x].step[ghostedStep].hitMode);

		const uint8_t ghost_stepIsOn = val2roll(
				seq[player.ramBank].row[x].step[ghostedStep].isOn);

		const uint8_t temp_rowIsOn = (seq[player.ramBank].row[x].flags
				& MASK_ROW_ON) > 0;
		const uint8_t ghostedRollCurve =
				seq[player.ramBank].row[x].step[ghostedStep].rollCurve;
		const uint8_t ghostedStepLength =
				seq[player.ramBank].row[x].step[ghostedStep].length1 + 1;

		if ((player.uStepInd[row] % 48) == 1)
		{
			--tempPlayerRow.rollLength;
		}

		if (ghostedStep == 0)
		{
			tempPlayerRow.rollLength = 0;
			tempPlayerRow.rollCounter = 0;
		}
		if (((player.uStepInd[row] % (48 / ghost_rollMode)) == 1)
				&& tempPlayerRow.rollLength)
		{
			if (ghost_stepIsOn >= 1 && temp_rowIsOn)
			{

				if (player.row[x].rollLength > 0)
					++player.row[x].rollCounter;

				// procent postępu rolki
				const float rollProgress = (float(
						(ghostedStepLength - tempPlayerRow.rollLength) * 48
								+ player.uStepInd[row])
						/ (float(ghostedStepLength) * 48)) * 100;

				// TU GRAMY
				if (((ghost_rollMode != HITMODE_OFFSET)
						&& ((player.uStepInd[row] % (48 / ghost_rollMode)) == 1))
						|| ((ghost_rollMode == HITMODE_OFFSET)
								&& (player.uStepInd[row] == temp_offset)))
				{
					const uint8_t tempNote = tempSeqRow.step[ghostedStep].note;
					const uint8_t tempStepVelo =
							tempSeqRow.step[ghostedStep].velocity;
					const uint8_t tempChord = tempSeqRow.step[ghostedStep].chord;
					// const uint8_t tempMod 	=	tempSeqRow.step[ghostedStep].modulation;
					const uint8_t tempScale = tempSeqRow.trackScale;
					const uint8_t tempRoot = tempSeqRow.rootNote;

					float tempVelo = getLongRollVelo(ghostedRollCurve,
														rollProgress)
							* ((float) tempStepVelo / 127);
					tempVelo = (tempVelo / MAX_VELO_TRACK)
							* seq[player.ramBank].row[x].trackVelo;

					//wyciszamy jezeli bylo cos wczesniej
					if (player.row[x].noteOn_sent)
					{
						midiSendChordOff(player.row[x].note_sent,
											player.row[x].chord_sent,
											0,
											player.row[x].channel_sent,
											player.row[x].midiOut_sent,
											player.row[x].scale_sent,
											player.row[x].scaleRoot_sent);
					}
					//gramy
					if (tempNote <= MAX_NOTE_TRACK)
					{
						player.row[x].note_sent = constrain(
								(int16_t ) tempNote
										+ (int16_t ) getNextRollNoteOffset(x),
								MIN_NOTE_STEP, MAX_NOTE_STEP);
						// Serial.println(player.row[x].note_sent);
						midiSendChordOn(
										constrain(player.row[x].note_sent,
													MIN_NOTE_STEP,
													MAX_NOTE_STEP),
										tempChord,
										uint8_t(tempVelo),
										seq[player.ramBank].row[x].channel,
										seq[player.ramBank].row[x].midiOut,
										tempScale,
										tempRoot);

						anythingSent = 1;

						player.row[x].midiOut_sent =
								seq[player.ramBank].row[x].midiOut;
						player.row[x].channel_sent =
								seq[player.ramBank].row[x].channel;
					}

					player.row[x].chord_sent = tempChord;
					player.row[x].scale_sent = tempScale;
					player.row[x].scaleRoot_sent = tempRoot;
					player.row[x].noteOn_sent = 1;

					if ((ghost_rollMode >= 1)
							&& (ghost_rollMode != HITMODE_OFFSET))
					{
						switch (seq[player.ramBank].row[x].gateMode)
						{
						case GATEMODE.NORMAL:
							player.row[x].note_length_timer = (48
									/ ghost_rollMode) - 2;
							break;

						case GATEMODE.MEDIUM:
							player.row[x].note_length_timer = (48
									/ ghost_rollMode) / 2;
							break;

						case GATEMODE.SHORT:
							player.row[x].note_length_timer = 1;
							break;

						case GATEMODE.EXTRASHORT:
							player.row[x].note_length_timer = 1;
							break;

						default:
							player.row[x].note_length_timer =
									(seq[player.ramBank].row[x].step[ghostedStep].length1
											+ 1) * 48 - 3;
						}
					}
				}
			}

			// wyłączamy przedłużenie rolki
			if ((tempPlayerRow.rollLength == 1) && (player.uStepInd[row] == 48))
			{
				tempPlayerRow.rollLength = 0;
				player.row[x].rollCounter = 0;
				// Serial.println(">");
			}
		}
		else
		{

		}
	}
	return anythingSent;
}

int8_t Sequencer::getLastRollNoteOffset(uint8_t row)
{
	return player.row[row].lastRollNote;
}
int8_t Sequencer::getNextRollNoteOffset(uint8_t row)
{

	int8_t & retVal = player.row[row].lastRollNote;

	if (player.row[row].rollStep == 0)
		return 0;

	strBank::strRow::strStep & step =
			seq[player.ramBank].row[row].step[player.row[row].rollStep];

	if (step.rollNoteCurve == ROLL_CURVE.FLAT)
		return 0;
	else if (step.rollNoteCurve == ROLL_CURVE.INCREMENTAL)
		return player.row[row].rollCounter - 1;
	else if (step.rollNoteCurve == ROLL_CURVE.DECREMENTAL)
		return -player.row[row].rollCounter + 1;

	else if (step.rollNoteCurve == ROLL_CURVE.INC_DEC)
	{
		uint8_t total = (step.length1 + 1) * val2roll(step.hitMode);

		if (player.row[row].rollCounter < ((total / 2) + 1))
		{
			retVal = player.row[row].rollCounter - 1;
		}
		else
		{
			retVal = total - player.row[row].rollCounter;
		}
	}

	else if (step.rollNoteCurve == ROLL_CURVE.DEC_INC)
	{
		uint8_t total = (step.length1 + 1) * val2roll(step.hitMode);

		if (player.row[row].rollCounter < ((total / 2) + 1))
		{
			retVal = -player.row[row].rollCounter + 1;
		}
		else
		{
			retVal = -(total - player.row[row].rollCounter + 1) + 1;
		}

	}

	else if (step.rollNoteCurve == ROLL_CURVE.RANDOM)
	{
		retVal = random(-6, 6);
	}

	// Serial.print("retVal");
	// Serial.println(retVal);

	return retVal;
}
uint8_t Sequencer::getLongRollVelo(uint8_t rollCurve, float progress)
{
	progress = constrain(progress, 0, 100);
	uint8_t retVal = 1;

	if (rollCurve == ROLL_CURVE.FLAT)
	{
		retVal = 127;
	}

	else if (rollCurve == ROLL_CURVE.INCREMENTAL)
	{
		retVal = (progress / 100) * 127;
	}

	else if (rollCurve == ROLL_CURVE.DECREMENTAL)
	{
		retVal = ((100 - progress) / 100) * 127;
	}

	else if (rollCurve == ROLL_CURVE.INC_DEC)
	{
		if (progress <= 50)
		{
			retVal = (progress / 50) * 127;
		}
		else
		{
			retVal = ((100 - progress) / 50) * 127;
		}
	}
	else if (rollCurve == ROLL_CURVE.DEC_INC)
	{
		if (progress <= 50)
		{
			retVal = ((50 - progress) / 50) * 127;
		}
		else
		{
			retVal = ((progress - 50) / 50) * 127;
		}
	}

	else if (rollCurve == ROLL_CURVE.RANDOM)
	{
		retVal = random(0, 127);
	}

	return retVal > 0 ? retVal : 1;
}

void Sequencer::init_player_timer(void) // MT::refreshTimer
{

	float timer_var = 0;
	float temp_Tempo;

	if (config.mode == MODE_MIDICLOCK.INTERNAL_)
		temp_Tempo = seq[player.ramBank].tempo;
	else if (config.mode == MODE_MIDICLOCK.INTERNAL_LOCK)
		temp_Tempo = config.tempoLock;
	else
		temp_Tempo = player.externalTempo;

	//3125 - wyliczone niegdyś matematycznie, 12 na potrzeby dividerów
	timer_var = ((3125.0 / temp_Tempo) * (player.swing_offset + 50.0)) / 12.0;

	// TODO: - ogarnąć ten timer
//	playTimer.begin(handle_uStep_timer,  timer_var);

}
void Sequencer::print_uSteps()
{
	for (uint8_t a = 1; a <= 8; a++)
	{
		Serial.println(player.uStepInd[a]);
	}
}

void Sequencer::handle_uStep_timer(void)
{
	// Serial.print(player.swingToogle);
	// Serial.print("\t");
	// Serial.println(timeBetweenTicks);
	// Serial.send_now();

	/*
	 1 step = 48 x uStep = 48 x 12 timerTick(576)
	 6912 timerTick = 12 stepów
	 */

	// noInterrupts();
	if ((config.mode == MODE_MIDICLOCK.INTERNAL_)
			|| (config.mode == MODE_MIDICLOCK.INTERNAL_LOCK))
	{
		// handle_uStep(0);
		// play_uStepEmulate(0);
		if (isPlay() || isREC())
		{
			handle_uStep12(0);
			timerTick++;
			if (timerTick > 6912)
				timerTick = 1;

		}

	}
	else // external clock
	{
		// warunek blokujący przejście do kolejnego stepa
		if ((timerTick % 576 != 0) && (player.isPlay || isREC())) // && !clockustep)
		{
			handle_uStep12(0); // to nie jest bez sensu, zostawione do testów
			// play_uStepEmulate(0);

			timerTick++;
			if (timerTick > 6912)
				timerTick = 1;
		}
		else
		{
			// clockFuck = 0;
		}
	}
}

void Sequencer::handle_player(void)
{
}

void Sequencer::addNoteOn(uint8_t note, uint8_t velocity, uint8_t channel,
							uint8_t midiOut)
{
	// Serial.println("add");

	for (uint8_t a = 0; a < 100; a++)
	{
		if (noteHandler[a].free)
		{
			noteHandler[a].note = note;
			noteHandler[a].velocity = velocity;
			noteHandler[a].channel = channel;
			noteHandler[a].midiOut = midiOut;

			noteHandler[a].free = 0;
			noteHandler[a].onOff = 1;
			// Serial.println("addNoteOn");
			break;
		}
	}
}
void Sequencer::addNoteOff(uint8_t note, uint8_t velocity, uint8_t channel,
							uint8_t midiOut)
{
	for (uint8_t a = 0; a < 100; a++)
	{
		if (noteHandler[a].free)
		{
			noteHandler[a].note = note;
			noteHandler[a].velocity = velocity;
			noteHandler[a].channel = channel;
			noteHandler[a].midiOut = midiOut;

			noteHandler[a].free = 0;
			noteHandler[a].onOff = 0;

			// Serial.println("addNotefOrf");
			break;
		}
	}
}

void Sequencer::flushNotes()
{
	flushTimer = 0;

	for (uint8_t a = 0; a < 100; a++)
	{
		if (!noteHandler[a].free && noteHandler[a].onOff)
		{
//			if 		((noteHandler[a].midiOut == MIDIOUT_USB)  || (noteHandler[a].midiOut == MIDIOUT_USB_C)) 		usbMIDI.sendNoteOn(noteHandler[a].note, noteHandler[a].velocity, noteHandler[a].channel);
//			else if ((noteHandler[a].midiOut == MIDIOUT_DIN1) || (noteHandler[a].midiOut == MIDIOUT_DIN1_C)) 		MIDI2.sendNoteOn(noteHandler[a].note, noteHandler[a].velocity, noteHandler[a].channel);
//			else if ((noteHandler[a].midiOut == MIDIOUT_DIN2) || (noteHandler[a].midiOut == MIDIOUT_DIN2_C))		MIDI.sendNoteOn(noteHandler[a].note, noteHandler[a].velocity, noteHandler[a].channel);
			noteHandler[a].free = 1;
		}
		else if (!noteHandler[a].free && !noteHandler[a].onOff)
		{
//			if 		((noteHandler[a].midiOut == MIDIOUT_USB)  || (noteHandler[a].midiOut == MIDIOUT_USB_C)) 		usbMIDI.sendNoteOff(noteHandler[a].note, noteHandler[a].velocity, noteHandler[a].channel);
//			else if ((noteHandler[a].midiOut == MIDIOUT_DIN1) || (noteHandler[a].midiOut == MIDIOUT_DIN1_C)) 		MIDI2.sendNoteOff(noteHandler[a].note, noteHandler[a].velocity, noteHandler[a].channel);
//			else if ((noteHandler[a].midiOut == MIDIOUT_DIN2) || (noteHandler[a].midiOut == MIDIOUT_DIN2_C))		MIDI.sendNoteOff(noteHandler[a].note, noteHandler[a].velocity, noteHandler[a].channel);
			noteHandler[a].free = 1;
		}
	}
	usbMIDI.send_now();
}

void Sequencer::incr_uStep(uint8_t row)
{
	if (!player.row[row].divChange)
	{
		player.uStepInd[row]++;

		if (player.uStepInd[row] > 48)
		{
			player.uStepInd[row] = 1;
			switchStep(row);
		}
	}
}

void Sequencer::divChangeQuantize(uint8_t row)
{
	int8_t tTempoOption = constrain(seq[player.ramBank].row[row].tempoDiv,
									MIN_TEMPO_DIV,
									MAX_TEMPO_DIV);
	uint8_t tDiv = getTempoDiv(tTempoOption);

	if (player.row[row].divChange && ((timerTick % (tDiv * 48)) == 1))
	{
		player.row[row].divChange = 0;
		player.uStepInd[row] = 1;
		switchStep(row);
	}
}

// void Sequencer::incr_uStep(uint8_t row)
// {

// }

void Sequencer::trySwitchBank()
{
	if (player.jumpNOW)
	{
		switch_bank_with_reset();
		player.jumpNOW = 0;
	}
}

uint8_t Sequencer::getTempoDiv(int8_t val)
{

	// 48, // 1/4	-3
	// 36,	// 1/3	-2
	// 24,	// 1/2	-1
	// 12,	// 1/1	0
	// 6, 	// 2/1	1
	// 4, 	// 3/1	2
	// 3	// 4/1	3

	switch (val)
	{
	case -3:
		return 48;
		break;
	case -2:
		return 36;
		break;
	case -1:
		return 24;
		break;
	case 0:
		return 12;
		break;
	case 1:
		return 6;
		break;
	case 2:
		return 4;
		break;
	case 3:
		return 3;
		break;

	default:
		break;
	}

	return 12;
}

void Sequencer::handle_uStep12(uint8_t step)
{
	if (isPlay())
	{

		if ((step == 1))// to znaczy że wywoładnie funkcji przyszło z midi clocka
		{
			player.uStep = 1;

			// podciągamy tick do wielokrotności całego stepa czyli 576 +1
			uint16_t tickBefore = timerTick;
			uint16_t tickAfter = timerTick;
			if (timerTick % 576 != 1)
			{
				uint16_t tempTick = timerTick;
				while (tempTick % 576 != 1)
					tempTick++;
				tickAfter = tempTick;
				// Serial.print("tickAfter ");
				// Serial.println(tickAfter);
			}

			if (tickAfter >= tickBefore)
			{
				for (uint16_t tempTick = tickBefore; tempTick <= tickAfter;
						tempTick++)
				{
					if (tempTick > 6912)
					{
						tempTick = 1;
					}
					timerTick = tempTick;

					for (uint8_t a = 1; a <= 8; a++)
					{
						int8_t tTempoOption = constrain(
								seq[player.ramBank].row[a].tempoDiv,
								MIN_TEMPO_DIV,
								MAX_TEMPO_DIV);
						uint8_t tDiv = getTempoDiv(tTempoOption);

						if (timerTick % tDiv == 1)
						{
							divChangeQuantize(a);
							play_uStep(a);
							incr_uStep(a);
						}
					}
					trySwitchBank();
					if (tempTick == 1)
					{
						break;
					}
				}
			}

			else
			{
				Serial.println("a jednak");
			}

		}

		else // wywoładnie z timera wewnętrznego
		{
			if (!player.isREC)
			{
				for (uint8_t a = 1; a <= 8; a++)
				{
					int8_t tTempoDiv = constrain(
							seq[player.ramBank].row[a].tempoDiv, MIN_TEMPO_DIV,
							MAX_TEMPO_DIV);
					uint8_t tDiv = getTempoDiv(tTempoDiv);

					if (timerTick % tDiv == 1)
					{
						divChangeQuantize(a);
						play_uStep(a);
						incr_uStep(a);
					}
				}
				trySwitchBank();

			}
			else
			{
				for (uint8_t a = 1; a <= 8; a++)
				{
					if (timerTick % 12 == 1)
					{
						divChangeQuantize(a);
						play_uStep(a);
						incr_uStep(a);
					}
				}
				trySwitchBank();
			}
		}

		// stary mechanizm, na potrzeby startowania timera
		if ((timerTick % 12 == 1) || step)
		{
			if (player.uStep == 1)
			{
				// if (config.mode == MODE_MIDICLOCK_INTERNAL)
				// {
				//konfig timera do zmiany czasu trwania kroku na swing
				if (!player.isREC)
				{
					float tempSwing;
					if (config.mode == MODE_MIDICLOCK.INTERNAL_)
						tempSwing = seq[player.ramBank].swing;
					else if (config.mode == MODE_MIDICLOCK.INTERNAL_LOCK)
						tempSwing = config.swingLock;
					else
						tempSwing = 50.0;

					if ((player.swingToogle))
						player.swing_offset = 0 + tempSwing;
					else
						player.swing_offset = 100 - tempSwing;
					player.swingToogle = !player.swingToogle;
				}
				else
				{
					player.swing_offset = 50.0;
				}

				//rekonfig timera
				init_player_timer();
			}

			if (player.isPlay)
			{
				if (player.uStep > 0)
				{
					player.uStep++;

					if (player.uStep > 48)
					{
						player.uStep = 1;
					}
				}
			}
		}

		if (step == 1)
		{
			timerTick++;
			if (timerTick > 6912)
				timerTick = 1;
		}
	}

	if ((timerTick % 12 == 1) || step)
		rec_metronome();

	flushNotes();

}

float Sequencer::get_tempo(void)
{
	if (config.mode == MODE_MIDICLOCK.INTERNAL_)
		return seq[player.ramBank].tempo;
	else if (config.mode == MODE_MIDICLOCK.INTERNAL_LOCK)
		return config.tempoLock;
	else
		return 0;
}

float Sequencer::get_swing(void)
{

	if (config.mode == MODE_MIDICLOCK.INTERNAL_)
		return seq[player.ramBank].swing;
	else if (config.mode == MODE_MIDICLOCK.INTERNAL_LOCK)
		return config.swingLock;
	else
		return 0;
}

inline uint8_t Sequencer::get_note(uint8_t col, uint8_t row)
{
	return seq[player.ramBank].row[row].step[col].note;
}

inline uint8_t Sequencer::get_hitMode(uint8_t col, uint8_t row)
{
	return seq[player.ramBank].row[row].step[col].hitMode;
}

inline uint8_t Sequencer::get_isOn(uint8_t col, uint8_t row)
{
	return seq[player.ramBank].row[row].step[col].isOn;
}

inline uint8_t Sequencer::isGhost(uint8_t col, uint8_t row)
{
	return player.row[row].step[col].isGhost > 0;
}

inline uint8_t Sequencer::isBlinking(uint8_t col, uint8_t row)
{
	return player.row[row].step[col].isBlinking;
}

inline uint8_t Sequencer::get_actual_pos(uint8_t row)
{
	return player.row[row].actual_pos;
}

inline uint8_t Sequencer::get_row_length(uint8_t row)
{
	return seq[player.ramBank].row[row].length;
}

void Sequencer::init_player_lcd_values(void)
{
	change_buffer.transpose = 0;
	change_buffer.uMoveTrack = 1;
	change_buffer.moveStep = IDLE_MOVE_STEP;

	//player.move2change = 0;
}

uint8_t Sequencer::val2roll(uint8_t val)
{
	if (val <= sizeof(arrVal2roll))
	{
		return arrVal2roll[val];
	}
	else if (val == HITMODE_OFFSET)
		return HITMODE_OFFSET; // wyjatek dla nudge //TODO
	else
	{
		return 1;
	}

}

inline uint8_t Sequencer::isPlay(void)
{
	return player.isPlay;
}

inline uint8_t Sequencer::isREC(void)
{
	return player.isREC;
}

inline uint8_t Sequencer::isStop(void)
{
	return player.isStop;
}

void Sequencer::midiSendChordOn(uint8_t note,
								uint8_t chord,
								uint8_t velo,
								uint8_t channel,
								uint8_t midiOut,
								uint8_t scale,
								uint8_t scaleRoot)
{

}
void Sequencer::midiSendChordOff(uint8_t note,
									uint8_t chord,
									uint8_t velo,
									uint8_t channel,
									uint8_t midiOut,
									uint8_t scale,
									uint8_t scaleRoot)
{

}

void Sequencer::midiSendCC(uint8_t channel, uint8_t control, uint8_t value,
							uint8_t midiOut)
{
//	if 		((midiOut == MIDIOUT_USB)  || (midiOut == MIDIOUT_USB_C)) 		usbMIDI.sendControlChange(channel, control, value);
//	else if ((midiOut == MIDIOUT_DIN1) || (midiOut == MIDIOUT_DIN1_C)) 		MIDI2.sendControlChange(channel, control, value);
//	else if ((midiOut == MIDIOUT_DIN2) || (midiOut == MIDIOUT_DIN2_C))		MIDI.sendControlChange(channel, control, value);

	// usbMIDI.send_now();

}

void Sequencer::setLoadBank2Ram(uint8_t bank)
{
	// Serial.print("change bank to:");
	// Serial.println(bank);
//	bank = constrain(bank, 0, 255);
//	set_updateLCD();
//
//	player.changeBank  = 1;
//	player.loadBank = 1;
//	player.bank2load = bank;

}

void Sequencer::switch_bank_with_reset(void)
{

}

