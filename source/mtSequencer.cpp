#include <Arduino.h>

#include "mtSequencer.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"

Sequencer sequencer;

inline void timerExternalVector()
{

	sequencer.handle_uStep_timer();

}

/*
 * PUBLIC
 */
void Sequencer::init()
{
	init_player_timer();

}
void Sequencer::handle()
{
//	uint32_t size;
//	size = sizeof(strBank);
//	handle_ghosts();
}

void Sequencer::handle_uStep_timer(void)
{
	/*
	 1 step sekwencji = 48 x uStep = 48 x 12 nanoStep(576)
	 6912 nanoStep = 12 stepów
	 */

	// noInterrupts();
	if ((config.mode == MODE_MIDICLOCK.INTERNAL_) || (config.mode == MODE_MIDICLOCK.INTERNAL_LOCK))
	{
		if (isPlay() || isREC())
		{
			handle_nanoStep(0);
			nanoStep++;
			if (nanoStep > 6912)
				nanoStep = 1;
		}
	}
	else // external clock
	{
		// warunek blokujący przejście do kolejnego stepa
		if ((nanoStep % 576 != 0) && (player.isPlay || isREC())) // && !clockustep)
		{
			handle_nanoStep(0); // to nie jest bez sensu, zostawione do testów
			// play_uStepEmulate(0);

			nanoStep++;
			if (nanoStep > 6912)
				nanoStep = 1;
		}
		else
		{
			// clockFuck = 0;
		}
	}
}

void Sequencer::handle_nanoStep(uint8_t step)
{
	if (isPlay())
	{
		if ((step == 1))// to znaczy że wywoładnie funkcji przyszło z midi clocka
		{
			player.uStep = 1;

			// podciągamy tick do wielokrotności całego stepa czyli 576 +1
			uint16_t tickBefore = nanoStep;
			uint16_t tickAfter = nanoStep;
			if (nanoStep % 576 != 1)
			{
				uint16_t tempTick = nanoStep;
				while (tempTick % 576 != 1)
					tempTick++;
				tickAfter = tempTick;
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
					nanoStep = tempTick;

					for (uint8_t a = MINROW; a <= MAXROW; a++)
					{
						int8_t tTempoOption = constrain(
								seq[player.ramBank].track[a].tempoDiv,
								MIN_TEMPO_DIV,
								MAX_TEMPO_DIV);
						uint8_t tDiv = getTempoDiv(tTempoOption);

						if (nanoStep % tDiv == 1)
						{
							divChangeQuantize(a);
							play_microStep(a);
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
				for (uint8_t a = MINROW; a <= MAXROW; a++)
				{
					int8_t tTempoDiv = constrain(
							seq[player.ramBank].track[a].tempoDiv,
							MIN_TEMPO_DIV,
							MAX_TEMPO_DIV);
					uint8_t tDiv = getTempoDiv(tTempoDiv);

					if (nanoStep % tDiv == 1)
					{
						divChangeQuantize(a);
						play_microStep(a);
						incr_uStep(a);
					}
				}
				trySwitchBank();

			}
			else
			{
				for (uint8_t a = MINROW; a <= MAXROW; a++)
				{
					if (nanoStep % 12 == 1)
					{
						divChangeQuantize(a);
						play_microStep(a);
						incr_uStep(a);
					}
				}
				trySwitchBank();
			}
		}

		// stary mechanizm, na potrzeby startowania timera
		if ((nanoStep % 12 == 1) || step)
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
			nanoStep++;
			if (nanoStep > 6912)
				nanoStep = 1;
		}
	}

	if ((nanoStep % 12 == 1) || step)
		rec_metronome();

	flushNotes();

}

void Sequencer::play_microStep(uint8_t row)
{
	if (row > MAXROW)
		return;
	strBank::strTrack & patternRow = seq[player.ramBank].track[row];
	strPlayer::strPlayerTrack & playerRow = player.row[row];

	strBank::strTrack::strStep & patternStep = patternRow.step[playerRow.actual_pos];
//	strPlayer::strPlayerTrack::strPlayerStep & playerStep = playerRow.step[playerRow.actual_pos];

	//
	// odliczamy odpaloną nutę
	//

	if (playerRow.stepTimer > 0)
		playerRow.stepTimer--;

	//
	// wyłączamy nutę jeśli się skończyła
	//

	if ((playerRow.stepTimer <= 1) && (playerRow.stepOpen) && !playerRow.recNoteOpen)
	{
		sendNoteOff(row,
					&playerRow.stepSent);
		playerRow.stepOpen = 0;
	}

	//
	// wysyłamy zegar
	// TODO: ogarnąć warunek na mniej skomplikowany
	//

	if ((playerRow.uStep > 0) && player.isPlay)
	{
		if (((playerRow.uStep - 1) % 8) == 0)
			send_clock(row);
	}

	//
	//		wystartować nutę?
	//
	if (patternRow.isOn && patternStep.isOn)
	{
		boolean sendNote = 0;
		// nie-offset
		if (patternStep.fx[0].type != fx.FX_TYPE_OFFSET &&
				playerRow.uStep == 1)
		{
			sendNote = 1;
			if (playerRow.stepOpen) sendNoteOff(row, &playerRow.stepSent);

			sendNoteOn(row, &patternStep);
		}
		// offset
		else if (patternStep.fx[0].type == fx.FX_TYPE_OFFSET &&
				playerRow.uStep == patternStep.fx[0].value_u16)
		{
			sendNote = 1;
			if (playerRow.stepOpen) sendNoteOff(row, &playerRow.stepSent);

			sendNoteOn(row, &patternStep);
		}

		if (sendNote)
		{

			playerRow.stepOpen = 1;
			playerRow.stepTimer = patternStep.length1;
			playerRow.stepSent = patternStep; // buforujemy wysłanego stepa
		}
	}

	//
	//	kontynuowanie nuty
	//

	if (playerRow.stepOpen)
	{

	}

}

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
				for (uint8_t a = MINROW; a <= MAXROW; a++)
				{
					player.row[a].uStep = 1;
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

void Sequencer::handle_ghosts(void)
{
	for (int8_t col = 1; col <= seq[player.ramBank].track[ghost.cnt1].length;
			col++)
	{
		int8_t motherGhost = col; // numer stepa który jest matką ghostów
		uint8_t temp_row_lenght = seq[player.ramBank].track[ghost.cnt1].length;
//		uint8_t temp_hitMode = seq[player.ramBank].row[ghost.cnt1].step[col].hitMode;
		uint8_t temp_isOn = seq[player.ramBank].track[ghost.cnt1].step[col].isOn;

		uint8_t temp_length = seq[player.ramBank].track[ghost.cnt1].step[col].length1 + 1;

		if (temp_length > 1 && temp_isOn)
		{
			while (temp_length > 1 && !seq[player.ramBank].track[ghost.cnt1].step[col + 1 + ((col + 1 > temp_row_lenght) * -temp_row_lenght)].isOn)
			{
				player.row[ghost.cnt1].step[col + 1 + ((col + 1 > temp_row_lenght) * (-(temp_row_lenght)))].isGhost = motherGhost;
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

void Sequencer::play(void)
{
	if (debug.player)
		Serial.println("play");

	reset_actual_pos();

	player.isStop = 0;
	player.isPlay = 1;
	nanoStep = 1;

	player.uStep = 1;
	for (uint8_t a = MINROW; a <= MAXROW; a++)
	{
		player.row[a].uStep = 1;

//		player.row[a].rollStep = 0;
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

void Sequencer::pause(void)
{
	player.isPlay = 0;
	send_allNotesOff();
}

void Sequencer::send_allNotesOff(void)
{

	for (uint8_t row = MINROW; row <= MAXROW; row++)
	{
		strPlayer::strPlayerTrack & playerRow = player.row[row];
		if (playerRow.stepOpen) sendNoteOff(row, &playerRow.stepSent);
	}
}

void Sequencer::stop(void)
{

	send_allNotesOff();

//	send_stop();

	player.isPlay = 0;
	player.isStop = 1;
	player.isREC = 0;
	player.uStep = 0;

	nanoStep = 1;

	for (uint8_t a = MINROW; a <= MAXROW; a++)
	{
		player.row[a].uStep = 0;
//		player.row[a].rollStep = 0;

		player.row[a].makeJump = 0;
	}
	player.changeBank = 0;

	player.swingToogle = 1;

//	set_stopLed(1);
//	set_playLed(0);

//	set_grid_view_mode(VIEWMODE_SEQUENCE);

	reset_actual_pos();
//	resetLastSendMod();

	allNoteOffs();
	resetAllLearned();

	// flash_bank(player.actualBank, player.ramBank);
}

//void Sequencer::resetLastSendMod(void)
//{
//	for (uint8_t y = MINROW; y <= MAXROW; y++)
//	{
//		player.row[y].lastMod = 128;
//	}
//}

void Sequencer::resetAllLearned(void)
{
	for (uint8_t x = MINSTEP; x <= MAXSTEP; x++)
	{
		for (uint8_t y = MINROW; y <= MAXROW; y++)
		{
			player.row[y].step[x].learned = 0;
		}
	}
}

void Sequencer::rec(void)
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
	// TODO: ograniczyć ba
	for (uint8_t x = MINSTEP; x <= MAXSTEP; x++)
	{
		clearStep(x, row, bank);
	}
}

void Sequencer::loadDefaultTrack(uint8_t row, uint8_t bank)
{

	for (uint8_t x = 1; x <= MAXSTEP; x++)
	{
		seq[bank].track[row].length = DEFAULT_ROW_LEN;
		seq[bank].track[row].rootNote = DEFAULT_ROW_NOTE;
		seq[bank].track[row].trackVelo = MAX_VELO_TRACK;
		seq[bank].track[row].defaultMod = DEFAULT_MOD;
		seq[bank].track[row].channel = DEFAULT_ROW_CHANNEL;
		seq[bank].track[row].cc = DEFAULT_CC;
//		seq[bank].row[row].flags = 1;
		seq[bank].track[row].isOn = 1;
		seq[bank].track[row].trackScale = 0;
		seq[bank].track[row].midiOut = MIDIOUT_USB;
		seq[bank].track[row].playMode = PLAYMODE_FORWARD;
		seq[bank].track[row].gateMode = GATEMODE.NORMAL;
		seq[bank].track[row].tempoDiv = TEMPODIV_1_1;
		seq[bank].track[row].channelIn = CHANNEL_IN_ALL;
		seq[bank].track[row].rezerwa4 = 0;

		clearStep(x, row, bank);
	}
}

void Sequencer::clearStep(uint8_t x, uint8_t row)
{

	clearStep(x, row, player.ramBank);
}

void Sequencer::clearStep(uint8_t x, uint8_t row, uint8_t bank)
{
	strBank::strTrack & tempRow = seq[bank].track[row];
	strBank::strTrack::strStep & step = tempRow.step[x];

//	step.hitMode = 1;
	step.isOn = 0;
//	step.chord = MIN_CHORD;
	step.velocity = MAX_VELO_STEP;
//	step.modulation = DEFAULT_MOD;
//	step.rollCurve = MIN_STEP_ROLL_VAR;

//	step.rollNoteCurve = MIN_STEP_ROLL_NOTE_VAR;
	step.length1 = MIN_STEP_LENGTH;
//	step.offset = OFFSET_MIN;
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

void Sequencer::loadDefaultBank(uint8_t bank)
{
	seq[bank].tempo = DEFAULT_TEMPO;
	seq[bank].swing = DEFAULT_SWING;
	seq[bank].structVer = MEMORY_STRUCT_VER;
	seq[bank].structVerControl = MEMORY_STRUCT_VER;
	seq[bank].rezerwa3 = 0;
	seq[bank].rezerwa4 = 0;

	for (uint8_t row = MINROW; row <= MAXROW; row++)
	{
		loadDefaultTrack(row, bank);
	}

}

void Sequencer::initPattern(uint8_t pattern) // czyści pattern z flasha
{
	// czyścimy bank 3
	loadDefaultBank(3);

	if (pattern == player.actualBank)
	{
		loadDefaultBank(1);
		loadDefaultBank(0);
	}

	// flashuje pattern ramem z 3 pozycji
	// 	TODO
	//	flash_bank(pattern, 3);
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

uint8_t Sequencer::isRowOn(uint8_t row)
{
	return seq[player.ramBank].track[row].isOn;
}

void Sequencer::loadDefaultSequence(void)
{
	for (uint8_t x = MINROW; x <= MAXROW; x++)
	{
		seq[player.ramBank].track[x].rootNote = 35 + x;
		seq[player.ramBank].track[x].channel = x;
		seq[player.ramBank].track[x].isOn = 1;

		seq[player.ramBank].track[x].length = 16;
		for (uint8_t y = MINSTEP; y <= MAXSTEP; y++)
		{
			seq[player.ramBank].track[x].step[y].isOn = 0;
		}
	}
	//seq[player.ramBank].track[0].tempoDiv=1;
	seq[player.ramBank].track[0].step[0].isOn = 1;
	seq[player.ramBank].track[0].step[0].velocity = 100;
	seq[player.ramBank].track[0].step[0].instrument = 0;
	seq[player.ramBank].track[0].step[0].note = 28;
	seq[player.ramBank].track[0].step[0].length1 = 100;

	seq[player.ramBank].track[1].step[1].isOn = 1;
	seq[player.ramBank].track[1].step[1].velocity = 100;
	seq[player.ramBank].track[1].step[1].instrument = 0;
	seq[player.ramBank].track[1].step[1].note = 24;
	seq[player.ramBank].track[1].step[1].length1 = 100;

	seq[player.ramBank].track[2].step[2].isOn = 1;
	seq[player.ramBank].track[2].step[2].velocity = 100;
	seq[player.ramBank].track[2].step[2].instrument = 0;
	seq[player.ramBank].track[2].step[2].note = 29;
	seq[player.ramBank].track[2].step[2].length1 = 100;

	seq[player.ramBank].track[3].step[3].isOn = 1;
	seq[player.ramBank].track[3].step[3].velocity = 100;
	seq[player.ramBank].track[3].step[3].instrument = 5;
	seq[player.ramBank].track[3].step[3].note = 11;
	seq[player.ramBank].track[3].step[3].length1 = 50;

	seq[player.ramBank].track[4].step[4].isOn = 1;
	seq[player.ramBank].track[4].step[4].velocity = 100;
	seq[player.ramBank].track[4].step[4].instrument = 5;
	seq[player.ramBank].track[4].step[4].note = 14;
	seq[player.ramBank].track[4].step[4].length1 = 50;

	seq[player.ramBank].track[5].step[5].isOn = 1;
	seq[player.ramBank].track[5].step[5].velocity = 100;
	seq[player.ramBank].track[5].step[5].instrument = 5;
	seq[player.ramBank].track[5].step[5].note = 13;
	seq[player.ramBank].track[5].step[5].length1 = 50;

	seq[player.ramBank].track[6].step[6].isOn = 1;
	seq[player.ramBank].track[6].step[6].velocity = 100;
	seq[player.ramBank].track[6].step[6].instrument = 5;
	seq[player.ramBank].track[6].step[6].note = 12;
	seq[player.ramBank].track[6].step[6].length1 = 50;


	seq[player.ramBank].track[7].step[7].isOn = 1;
	seq[player.ramBank].track[7].step[7].velocity = 100;
	seq[player.ramBank].track[7].step[7].instrument = 0;
	seq[player.ramBank].track[7].step[7].note = 21;
	seq[player.ramBank].track[7].step[7].length1 = 100;



	seq[player.ramBank].track[0].step[8].isOn = 1;
	seq[player.ramBank].track[0].step[8].velocity = 100;
	seq[player.ramBank].track[0].step[8].instrument = 0;
	seq[player.ramBank].track[0].step[8].note = 28;
	seq[player.ramBank].track[0].step[8].length1 = 100;

	seq[player.ramBank].track[1].step[9].isOn = 1;
	seq[player.ramBank].track[1].step[9].velocity = 100;
	seq[player.ramBank].track[1].step[9].instrument = 0;
	seq[player.ramBank].track[1].step[9].note = 24;
	seq[player.ramBank].track[1].step[9].length1 = 100;

	seq[player.ramBank].track[2].step[10].isOn = 1;
	seq[player.ramBank].track[2].step[10].velocity = 100;
	seq[player.ramBank].track[2].step[10].instrument = 0;
	seq[player.ramBank].track[2].step[10].note = 29;
	seq[player.ramBank].track[2].step[10].length1 = 100;

	seq[player.ramBank].track[3].step[11].isOn = 1;
	seq[player.ramBank].track[3].step[11].velocity = 100;
	seq[player.ramBank].track[3].step[11].instrument = 5;
	seq[player.ramBank].track[3].step[11].note = 11;
	seq[player.ramBank].track[3].step[11].length1 = 50;

	seq[player.ramBank].track[4].step[12].isOn = 1;
	seq[player.ramBank].track[4].step[12].velocity = 100;
	seq[player.ramBank].track[4].step[12].instrument = 5;
	seq[player.ramBank].track[4].step[12].note = 14;
	seq[player.ramBank].track[4].step[12].length1 = 50;

	seq[player.ramBank].track[5].step[13].isOn = 1;
	seq[player.ramBank].track[5].step[13].velocity = 100;
	seq[player.ramBank].track[5].step[13].instrument = 5;
	seq[player.ramBank].track[5].step[13].note = 13;
	seq[player.ramBank].track[5].step[13].length1 = 50;

	seq[player.ramBank].track[6].step[14].isOn = 1;
	seq[player.ramBank].track[6].step[14].velocity = 100;
	seq[player.ramBank].track[6].step[14].instrument = 5;
	seq[player.ramBank].track[6].step[14].note = 12;
	seq[player.ramBank].track[6].step[14].length1 = 50;


	seq[player.ramBank].track[7].step[15].isOn = 1;
	seq[player.ramBank].track[7].step[15].velocity = 100;
	seq[player.ramBank].track[7].step[15].instrument = 0;
	seq[player.ramBank].track[7].step[15].note = 21;
	seq[player.ramBank].track[7].step[15].length1 = 100;



//	seq[player.ramBank].track[0].step[5].isOn = 1;
////	seq[player.ramBank].row[0].step[1].hitMode = 1;
//	seq[player.ramBank].track[0].step[5].note = 36;
//	seq[player.ramBank].track[0].step[5].length1 = 100;
//
//	seq[player.ramBank].track[0].step[10].isOn = 1;
////	seq[player.ramBank].row[0].step[2].hitMode = 1;
//	seq[player.ramBank].track[0].step[10].note = 28;
//	seq[player.ramBank].track[0].step[10].length1 = 150;
//	seq[player.ramBank].track[0].step[10].fx[0].isOn = 1;
//	seq[player.ramBank].track[0].step[10].fx[0].type = fx.FX_TYPE_OFFSET;
//	seq[player.ramBank].track[0].step[10].fx[0].value_u16 = 10;

}

void Sequencer::allNoteOffs(void)
{
	for (uint8_t a = MINROW; a <= MAXROW; a++)
	{
		if (player.row[a].stepOpen)
		{
//			// TODO
//			midiSendChordOff

			player.row[a].stepOpen = 0;
		}

	}

	// Serial.println("all stop");
}

void Sequencer::switchStep(uint8_t row) //przełączamy stepy w zależności od trybu grania
{
	uint8_t x = constrain(row, MINROW, MAXROW);

	if (player.isREC && player.row[x].recNoteOpen)
	{
		player.row[x].recNoteLength++;
		seq[player.ramBank].track[x].step[player.row[x].recNoteStep].length1 = player.row[x].recNoteLength - 1;

	}

	if (player.row[x].goToStep)
	{
		player.row[x].actual_pos = player.row[x].goToStep;
		player.row[x].goToStep = 0;
	}
	else if (player.row[x].makeJump)
	{
		for (uint8_t a = MINROW; a <= MAXROW; a++)
		{
			player.row[x].makeJump = 0;
		}
		// switch_bank_with_reset();
		player.jumpNOW = 1;
	}
	else
	{
		if (seq[player.ramBank].track[x].playMode == PLAYMODE_FORWARD)
		{
			player.row[x].actual_pos++;
			if ((player.row[x].actual_pos > seq[player.ramBank].track[x].length) || player.row[x].return2start)
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
		else if (seq[player.ramBank].track[x].playMode == PLAYMODE_BACKWARD)
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
		else if (seq[player.ramBank].track[x].playMode == PLAYMODE_PINGPONG)
		{
			if (!player.row[x].pingPongToogle)
			{
				if ((player.row[x].actual_pos >= seq[player.ramBank].track[x].length) || player.row[x].return2start)
				{
					// player.row[x].return2start = 0;

					// if (player.changeBank)
					// {
					// 	switch_bank();
					// 	// player.row[x].return2start = 1;
					// }
					player.row[x].pingPongToogle = !player.row[x].pingPongToogle;
				}
				else
				{
					player.row[x].actual_pos++;
				}

			}
			else
			{
				if ((player.row[x].actual_pos <= 1) || player.row[x].return2start)
				{
					// player.row[x].return2start = 0;

					// if (player.changeBank)
					// {
					// 	switch_bank();
					// 	// player.row[x].return2start = 1;

					// }
					player.row[x].pingPongToogle = !player.row[x].pingPongToogle;
				}
				else
				{
					player.row[x].actual_pos--;
				}
			}
		}

		else if (seq[player.ramBank].track[x].playMode == PLAYMODE_RANDOM)
		{
			player.row[x].actual_pos = random(
					1, seq[player.ramBank].track[x].length + 1);
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
	for (uint8_t row = MINROW; row <= MAXROW; row++)
	{
		player.row[row].pingPongToogle = 0;
		reset_actual_pos(row);
	}
}

void Sequencer::reset_actual_pos(uint8_t row)
{

	if (seq[player.ramBank].track[row].playMode == PLAYMODE_FORWARD)
	{
		player.row[row].actual_pos = MINSTEP;
	}

	else if (seq[player.ramBank].track[row].playMode == PLAYMODE_BACKWARD)
	{
		player.row[row].actual_pos = seq[player.ramBank].track[row].length;
	}

	else if (seq[player.ramBank].track[row].playMode == PLAYMODE_PINGPONG)
	{
		player.row[row].actual_pos = MINSTEP;
	}

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

	playTimer.begin(timerExternalVector, timer_var);

}
//void Sequencer::print_uSteps()
//{
//	for (uint8_t a = MINROW; a <= MAXROW; a++)
//	{
//		Serial.println(player.row[a].uStep);
//	}
//}

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
		player.row[row].uStep++;

		if (player.row[row].uStep > 48)
		{
			player.row[row].uStep = 1;
			switchStep(row);
		}
	}
}

void Sequencer::divChangeQuantize(uint8_t row)
{
	int8_t tTempoOption = constrain(seq[player.ramBank].track[row].tempoDiv,
									MIN_TEMPO_DIV,
									MAX_TEMPO_DIV);
	uint8_t tDiv = getTempoDiv(tTempoOption);

	if (player.row[row].divChange && ((nanoStep % (tDiv * 48)) == 1))
	{
		player.row[row].divChange = 0;
		player.row[row].uStep = 1;
		switchStep(row);
	}
}

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

	// 48, 	// 1/4	-3
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

//uint8_t Sequencer::val2roll(uint8_t val)
//{
//	if (val <= sizeof(arrVal2roll))
//	{
//		return arrVal2roll[val];
//	}
//	else if (val == HITMODE_OFFSET)
//	return HITMODE_OFFSET; // wyjatek dla nudge //TODO
//	else
//	{
//		return 1;
//	}
//
//}

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

void Sequencer::copy_step(uint8_t from_step, uint8_t from_track,
							uint8_t to_step,
							uint8_t to_track)
{
	from_step = constrain(from_step, MINSTEP, MAXSTEP);
	from_track = constrain(from_track, MINROW, MAXROW);

	to_step = constrain(to_step, MINSTEP, MAXSTEP);
	to_track = constrain(to_track, MINROW, MAXROW);

	seq[player.ramBank].track[to_track].step[to_step] = seq[player.ramBank].track[from_track].step[from_step];
}

void Sequencer::copy_row(uint8_t from, uint8_t to)
{
	from = constrain(from, MINROW, MAXROW);
	to = constrain(to, MINROW, MAXROW);
	seq[player.ramBank].track[to] = seq[player.ramBank].track[from];
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

void Sequencer::send_clock(uint8_t arg)
{
	// TODO: wypełnić

}

void Sequencer::toggleStep(uint8_t row, uint8_t step)
{
	seq[player.ramBank].track[row].step[step].isOn = !seq[player.ramBank].track[row].step[step].isOn;
	seq[player.ramBank].track[row].step[step].note = DEFAULT_ROW_NOTE;
}

uint8_t Sequencer::get_fxValType(uint8_t fxType)
{
	switch (fxType)
	{
	case fx.FX_TYPE_NONE:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_OFFSET:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_GLIDE:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_SLIDE:
		return fx.FX_VAL_TYPE_U8_U8;
		break;

	case fx.FX_TYPE_ARP_UP:
		return fx.FX_VAL_TYPE_U8_U8;
		break;

	case fx.FX_TYPE_ARP_DOWN:
		return fx.FX_VAL_TYPE_U8_U8;
		break;

	case fx.FX_TYPE_SP:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_LP1:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_LP2:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_MICROTUNE:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_SAMPLE_PLAYMODE:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_JUMP_TO_STEP:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_JUMP_TO_PATTERN:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_PANNING:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_PANNING_ROLL:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_SLICE_NUMBER:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_PROBABILITY:
		return fx.FX_VAL_TYPE_U16;
		break;

	default:
		return fx.FX_VAL_TYPE_UNKNOWN;
		break;
	}
}

strMtModAudioEngine  playMod = {0};

void Sequencer::sendNoteOn(uint8_t track, strBank::strTrack::strStep *step)
{
	if (player.printNotes)
	{
		Serial.printf("track %d\nnoteOn:\t%d\nvelo:\t%d\ninstr:\t%d\n\n",
						track,
						step->note,
						step->velocity,
						step->instrument);
	}

	usbMIDI.sendNoteOn(step->note, step->velocity, 1);

	instrumentPlayer[track].noteOn(step->instrument, step->note, step->velocity);

}

void Sequencer::sendNoteOff(uint8_t track, strBank::strTrack::strStep *step)
{
	if (player.printNotes)
	{
		Serial.printf(
				"\ttrack %d\n\tnoteOff:\t%d\n\tvelo:\t%d\n\tinstr:\t%d\n\n",
				track,
				step->note,
				step->velocity,
				step->instrument);
	}
	usbMIDI.sendNoteOff(step->note, 0, 1);
}



