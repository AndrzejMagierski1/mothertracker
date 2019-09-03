#include <Arduino.h>

#include "mtSequencer.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"
#include "mtFileManager.h"

#include "patternEditor.h"
Sequencer sequencer;

inline void timerExternalVector()
{

	sequencer.handle_uStep_timer();

}

void Sequencer::init()
{
	init_player_timer();

}
void Sequencer::handle()
{
//	handle_ghosts();

	if (player.blink.isOpen && player.blink.timer > 500)
	{
		instrumentPlayer[player.blink.track].noteOff();
		player.blink.isOpen = 0;
	}

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

					for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
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
//					trySwitchBank();
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
				for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
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
//				trySwitchBank();

			}
			else
			{
				for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
				{
					if (nanoStep % 12 == 1)
					{
						divChangeQuantize(a);
						play_microStep(a);
						incr_uStep(a);
					}
				}
//				trySwitchBank();
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
	if (row > MAXTRACK)
		return;
	strPattern::strTrack & patternRow = seq[player.ramBank].track[row];
	strPlayer::strPlayerTrack & playerRow = player.row[row];

	strPattern::strTrack::strStep & patternStep = patternRow.step[playerRow.actual_pos];
//	strPlayer::strPlayerTrack::strPlayerStep & playerStep = playerRow.step[playerRow.actual_pos];

	//
	// odliczamy odpaloną nutę
	//

	if (playerRow.stepOpen)
	{
		playerRow.stepTimer++;

		//
		// wyłączamy nutę jeśli się skończyła
		//

		if ((playerRow.stepTimer >= playerRow.stepLength))
		{

			playerRow.stepOpen = 0;
			playerRow.rollMode = fx.ROLL_TYPE_NONE;

		}
	}
	if (playerRow.noteOpen)
	{
		playerRow.noteTimer++;

		if ((playerRow.noteTimer >= playerRow.noteLength))
		{
			sendNoteOff(row,
						&playerRow.stepSent);
			playerRow.noteOpen = 0;
		}
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
	//		WYSTARTOWAĆ STEPA?
	//

	if ((playerRow.uStep == 1) && player.isPlay && row == 0)
	{
		if (playerRow.actual_pos == patternRow.length && player.songMode)
		{
			loadNextPattern(fileManager.getNextSongPattern());
		}
	}

	if (patternRow.isOn)
	{
		boolean startStep = 0;
		boolean isOffset = 0;
		uint16_t offsetValue = 0;

		boolean isJumpToStep = 0;
		uint16_t jumpToStep = 0;

		boolean isRoll = 0;
		int8_t valRoll = 0;

		// przerabiamy FXy
		for (strPattern::strTrack::strStep::strFx &_fx : patternStep.fx)
		{
			if (_fx.isOn)
			{
				switch (_fx.type)
				{
				case fx.FX_TYPE_OFFSET:
					if (!isOffset)
					{
						isOffset = 1;
						offsetValue = _fx.value_u16;
					}
					break;

				case fx.FX_TYPE_JUMP_TO_STEP:
					if (!isJumpToStep)
					{
						isJumpToStep = 1;
						jumpToStep = _fx.value_u16;
					}

				case fx.FX_TYPE_ROLL:
					if (!isRoll)
					{
						isRoll = 1;
						valRoll = _fx.rollType;
					}

				default:
					break;
				}
			}

			else if (_fx.isOn && !isJumpToStep && _fx.type == fx.FX_TYPE_OFFSET)
			{
				isOffset = 1;
				offsetValue = _fx.value_u16;
			}
		}

		if (patternStep.note != STEP_NOTE_EMPTY)
		{

			// nie-offset
			if (!isOffset &&
					playerRow.uStep == 1)
			{
				// wystartuj stepa
				startStep = 1;
				if (playerRow.noteOpen)
				{
					// zeruj wiszącą nutę
					playerRow.noteOpen = 0;
					sendNoteOff(row, &playerRow.stepSent);
					playerRow.rollMode = fx.ROLL_TYPE_NONE;
				}
				if (playerRow.stepOpen)
				{
					// zeruj wiszący step
					playerRow.stepOpen = 0;
					playerRow.rollMode = fx.ROLL_TYPE_NONE;
				}

			}
			// offset
			else if (isOffset &&
					playerRow.uStep == offsetValue)
			{
				startStep = 1;
				if (playerRow.noteOpen)
				{
					playerRow.noteOpen = 0;
					sendNoteOff(row, &playerRow.stepSent);
					playerRow.rollMode = fx.ROLL_TYPE_NONE;
				}
				if (playerRow.stepOpen)
				{
					playerRow.stepOpen = 0;
					playerRow.rollMode = fx.ROLL_TYPE_NONE;
				}
			}
		}

		// odpalamy stepa
		if (startStep)
		{
			// ustawiamy całego stepa
			playerRow.stepOpen = 1;
			playerRow.stepTimer = 0; // od tej pory timer liczy w górę
//			playerRow.stepLength = patternStep.length1;
			// ustawiamy nute
			playerRow.noteOpen = 1;
			playerRow.noteLength = 9999; // w MT nie ma dugości stepa
			playerRow.noteTimer = 0; // od tej pory timer liczy w górę

			playerRow.stepSent = patternStep; // buforujemy wysłanego stepa

			// jeśli rolka to nuty są krótsze od stepa
			if (isRoll)
			{
				playerRow.rollMode = valRoll;
				playerRow.noteLength = rollTypeToVal(playerRow.rollMode) / 2; // TODO: wyliczyć długość rolki
			}
			if (patternStep.note >= 0)
			{
				sendNoteOn(row, &patternStep);
			}
			else if (patternStep.note == STEP_NOTE_OFF)
			{
				sendNoteOff(row, &patternStep);

			}
		}
		// odpalamy efekty po-nutowe
		if (isJumpToStep)
		{
			playerRow.isGoToStep = 1;
			playerRow.goToStep = jumpToStep;
		}

	}

//
//	kontynuowanie nuty
//

	if (playerRow.stepOpen)
	{
		if (playerRow.rollMode != fx.ROLL_TYPE_NONE)
		{
			// sprawdzamy timer microstepów, czy jest wielokrotrością rolki
			if (((playerRow.stepTimer % rollTypeToVal(playerRow.rollMode)) == 1) && playerRow.stepTimer != 1)
			{
//				Serial.println("rolka!");

				playerRow.noteOpen = 1;
				playerRow.noteTimer = 0; // od tej pory timer liczy w górę
				playerRow.noteLength = rollTypeToVal(playerRow.rollMode) / 2; // TODO: wyliczyć długość rolki

				sendNoteOn(row, &playerRow.stepSent);
//				playerRow.stepSent = patternStep; // buforujemy wysłanego stepa
			}
		}
	}

}

uint8_t Sequencer::rollTypeToVal(uint8_t rollType)
{
	switch (rollType)
	{
	case fx.ROLL_TYPE_NONE:
		return 0;
		break;

	case fx.ROLL_TYPE_4_1:
		return 192;
	case fx.ROLL_TYPE_3_1:
		return 144;
	case fx.ROLL_TYPE_2_1:
		return 96;
	case fx.ROLL_TYPE_1_1:
		return 48;
	case fx.ROLL_TYPE_1_2:
		return 24;
	case fx.ROLL_TYPE_1_3:
		return 16;
		break;
	case fx.ROLL_TYPE_1_4:
		return 12;
		break;
	case fx.ROLL_TYPE_1_6:
		return 8;
		break;
	case fx.ROLL_TYPE_1_8:
		return 6;
		break;
	case fx.ROLL_TYPE_1_12:
		return 4;
		break;
	case fx.ROLL_TYPE_1_16:
		return 3;
		break;
	default:
		return 0;
	}
	return 0;
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
				for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
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
	// ghosty nie potrzebne w MT
	/*
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
	 }*/
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
	for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
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

void Sequencer::playPattern(void)
{
	player.songMode = 0;
	play();
}
void Sequencer::playSong(void)
{

	fileManager.loadPattern(fileManager.resetToFirstSongPattern());
	switchNextPatternNow();

	player.songMode = 1;
	play();
}

void Sequencer::pause(void)
{
	player.isPlay = 0;
	send_allNotesOff();
}

void Sequencer::send_allNotesOff(void)
{

	for (uint8_t row = MINTRACK; row <= MAXTRACK; row++)
	{
//		strPlayer::strPlayerTrack & playerRow = player.row[row];
		sendNoteOff(row);
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

	for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
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
		for (uint8_t y = MINTRACK; y <= MAXTRACK; y++)
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
	seq[player.ramBank].tempo = 120.0;

	for (uint8_t x = MINTRACK; x <= MAXTRACK; x++)
	{
		seq[player.ramBank].track[x].rootNote = 35 + x;
		seq[player.ramBank].track[x].channel = x;
		seq[player.ramBank].track[x].isOn = 1;

		seq[player.ramBank].track[x].length = 127;
		for (uint8_t y = MINSTEP; y <= MAXSTEP; y++)
		{
			seq[player.ramBank].track[x].step[y].note = STEP_NOTE_EMPTY;
		}
	}

}

void Sequencer::allNoteOffs(void)
{
	for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
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
	uint8_t x = constrain(row, MINTRACK, MAXTRACK);

	if (player.isREC && player.row[x].recNoteOpen)
	{
		player.row[x].recNoteLength++;
//		seq[player.ramBank].track[x].step[player.row[x].recNoteStep].length1 = player.row[x].recNoteLength - 1;

	}

	if (player.row[x].isGoToStep)
	{
		player.row[x].actual_pos = player.row[x].goToStep;
		player.row[x].isGoToStep = 0;
	}
	else if (player.row[x].makeJump)
	{
		for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
		{
			player.row[x].makeJump = 0;
		}
		// switch_bank_with_reset();
		player.jump.jumpNOW = 1;
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

				if (row == 0 && player.songMode)
				{
					switchNextPatternNow();
					fileManager.switchNextPatternInSong();
					fileManager.refreshPatternView();
				}

				if ((player.onPatternEnd != NULL) && (x == MINTRACK))
					player.onPatternEnd();

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
				if ((player.onPatternEnd != NULL) && (x == MINTRACK))
					player.onPatternEnd();

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
	for (uint8_t row = MINTRACK; row <= MAXTRACK; row++)
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

//void Sequencer::trySwitchBank()
//{
//	if (player.jumpNOW)
//	{
//		switch_bank_with_reset();
//		player.jumpNOW = 0;
//	}
//}

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
	from_track = constrain(from_track, MINTRACK, MAXTRACK);

	to_step = constrain(to_step, MINSTEP, MAXSTEP);
	to_track = constrain(to_track, MINTRACK, MAXTRACK);

	seq[player.ramBank].track[to_track].step[to_step] = seq[player.ramBank].track[from_track].step[from_step];
}

void Sequencer::copy_row(uint8_t from, uint8_t to)
{
	from = constrain(from, MINTRACK, MAXTRACK);
	to = constrain(to, MINTRACK, MAXTRACK);
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

void Sequencer::send_clock(uint8_t arg)
{
// TODO: wypełnić

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
	case fx.FX_TYPE_ROLL:
		return fx.FX_VAL_TYPE_R8_I8;
		break;

	default:
		return fx.FX_VAL_TYPE_UNKNOWN;
		break;
	}
}

//strMtModAudioEngine  playMod = {0};

void Sequencer::sendNoteOn(uint8_t track, strPattern::strTrack::strStep *step)
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

	instrumentPlayer[track].noteOn(step->instrument, step->note,
									step->velocity);

}

void Sequencer::sendNoteOff(uint8_t track, strPattern::strTrack::strStep *step)
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

	instrumentPlayer[track].noteOff();
}
void Sequencer::sendNoteOff(uint8_t track)
{
//	if (player.printNotes)
//	{
//		Serial.printf(
//				"\ttrack %d\n\tnoteOff:\t%d\n\tvelo:\t%d\n\tinstr:\t%d\n\n",
//				track,
//				step->note,
//				step->velocity,
//				step->instrument);
//	}
//	usbMIDI.sendNoteOff(step->note, 0, 1);

	instrumentPlayer[track].noteOff();
}

void Sequencer::blinkNote(uint8_t instrument, uint8_t note, uint8_t velocity,
							uint8_t track)
{
	player.blink.isOpen = 1;
	player.blink.track = track;
	player.blink.timer = 0;

	instrumentPlayer[track].noteOff();
	instrumentPlayer[track].noteOn(instrument,
									note,
									velocity);
}

void Sequencer::loadNextPattern(uint8_t patternNumber)
{
	player.jump.nextPattern = patternNumber;
	player.jump.jumpNOW = 0;

	Serial.printf("loadNextPattern: %d\n", patternNumber);

	fileManager.loadPattern(patternNumber);

}

void Sequencer::handleNote(byte channel, byte note, byte velocity)
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;

	// NOTE ON
	if (velocity != 0)
	{
		if (!isMultiSelection())
		{
			strPattern::strTrack::strStep *step = &seq[player.ramBank].track[sel->firstTrack].step[sel->firstStep];
			if (step->note == STEP_NOTE_EMPTY)
			{
				step->instrument = mtProject.values.lastUsedInstrument;
			}
			step->note = note;
			step->velocity = velocity;

			blinkNote(step->instrument,
						step->note,
						step->velocity,
						sel->firstTrack);
		}
	}

}
