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
	if (player.blink.isOpen && player.blink.timer > 500)
	{
		closeBlinkNote();
	}
}
void Sequencer::closeBlinkNote(void)
{
	if (player.blink.instrument < INSTRUMENTS_COUNT)
	{
		instrumentPlayer[player.blink.track].noteOff();
		player.blink.isOpen = 0;
	}
	else
	{
		player.blink.isOpen = 0;
		usbMIDI.sendNoteOff(player.blink.note, 0,
							player.blink.instrument - INSTRUMENTS_COUNT);
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
				{/*
				 int8_t tTempoOption = constrain(
				 seq[player.ramBank].track[a].tempoDiv,
				 MIN_TEMPO_DIV,
				 MAX_TEMPO_DIV);
				 uint8_t tDiv = getTempoDiv(tTempoOption);*/
					uint8_t tDiv = getTempoDiv(0);

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
//				int8_t tTempoDiv = constrain(
//						seq[player.ramBank].track[a].tempoDiv,
//						MIN_TEMPO_DIV,
//						MAX_TEMPO_DIV);

				uint8_t tDiv = getTempoDiv(TEMPODIV_1_1);

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

void Sequencer::play_microStep(uint8_t row)
{
	if (row > MAXTRACK)
		return;
	strPattern::strTrack & patternRow = seq[player.ramBank].track[row];
	strPlayer::strPlayerTrack & playerRow = player.track[row];

	strPattern::strTrack::strStep & patternStep = patternRow.step[playerRow.actual_pos];
	strPattern::strTrack::strStep & stepToSend = player.track[row].stepToSend;
	stepToSend = patternStep;

//	strPlayer::strPlayerTrack::strPlayerStep & playerStep = playerRow.step[playerRow.actual_pos];

	//	******************************************************************************
	// 	odliczamy odpaloną nutę i stepa, step może zawierać kilka nut
	//	******************************************************************************

	if (playerRow.stepOpen)
	{
		playerRow.stepTimer++;

		//
		// wyłączamy nutę jeśli się skończyła
		//

		if ((playerRow.stepTimer >= playerRow.stepLength))
		{

			playerRow.stepOpen = 0;
			playerRow.rollIsOn = 0;
			playerRow.rollType = fx.ROLL_TYPE_NONE;

		}
	}
	if (playerRow.noteOpen)
	{
		playerRow.noteTimer++;

		if ((playerRow.noteTimer >= playerRow.noteLength))
		{
			sendNoteOff(row,
						playerRow.stepSent.note,
						playerRow.stepSent.velocity,
						playerRow.stepSent.instrument);

			playerRow.noteOpen = 0;
		}
	}

	//	**************************
	// 	wysyłamy zegar
	// 	TODO: ogarnąć warunek na mniej skomplikowany
	//	**************************

	if ((playerRow.uStep > 0) && player.isPlay)
	{
		if (((playerRow.uStep - 1) % 8) == 0)
			send_clock(row);
	}

	// jeśli ostatni step, zażądaj ładowania kolejnego patternu
	if ((playerRow.uStep == 1) && player.isPlay && row == 0)
	{
		if (playerRow.actual_pos == patternRow.length && player.songMode)
		{
			loadNextPattern(fileManager.getNextSongPattern());
		}
	}

	// ************************************
	// 		sprawdzamy PRE EFEKTY
	// ************************************

	boolean startStep = 0;
	boolean cancelStep = 0;

	if (playerRow.uStep == 1)
	{

		strPattern::strTrack::strStep::strFx &_fx = patternStep.fx[0];
		switch (_fx.type)
		{

		case fx.FX_TYPE_NUDGE:

			playerRow.isOffset = 1;
			playerRow.offsetValue = _fx.value + 1;

			break;
		case fx.FX_TYPE_STEP_CHANCE:
			if (random(0, 128) > _fx.value)
				cancelStep = 1;

			break;

		case fx.FX_TYPE_SEND_CC_1:
			usbMIDI.sendControlChange(1, _fx.value, 1);
			break;

		case fx.FX_TYPE_SEND_CC_2:
			usbMIDI.sendControlChange(2, _fx.value, 1);
			break;

		default:
			break;
		}

		if (patternStep.note == STEP_NOTE_EMPTY)
		{
			// wysyłam tylko fxa jeśli nie ma nuty
			if (_fx.type > fx.FX_TYPE_NOT_SEQ_FX)
			{
				instrumentPlayer[row].seqFx(_fx.type, _fx.value);
			}
			strPattern::strTrack::strStep::strFx &_fx = patternStep.fx[0];
			switch (_fx.type)
			{
			case fx.FX_TYPE_ROLL:
				case fx.FX_TYPE_ROLL_UP:
				case fx.FX_TYPE_ROLL_DOWN:
				case fx.FX_TYPE_ROLL_RANDOM:

				playerRow.rollIsOn = 1;
				playerRow.rollVal = _fx.value;
				playerRow.rollDir = _fx.type;
				playerRow.rollType = _fx.value;

				break;
			}
		}
	}

	// **************************
	// 		odpalamy step?
	// **************************

	if (patternStep.note != STEP_NOTE_EMPTY)
	{

		// nie-offset
		if (!playerRow.isOffset &&
				playerRow.uStep == 1)
		{
			// wystartuj stepa
			startStep = 1;
		}
		// offset
		else if (playerRow.isOffset &&
				playerRow.uStep == playerRow.offsetValue)
		{
			startStep = 1;
		}
	}

	// **************************
	// 		odpalamy stepa
	// **************************
	if (startStep && !cancelStep)
	{
		if (playerRow.stepOpen || playerRow.stepOpen)
		{
			sendNoteOff(row,
						playerRow.stepSent.note,
						playerRow.stepSent.velocity,
						playerRow.stepSent.instrument);
			playerRow.stepOpen = 0;
			playerRow.noteOpen = 0;
			playerRow.rollIsOn = 0;
			playerRow.rollType = fx.ROLL_TYPE_NONE;
		}

		// EFEKTY WŁAŚCIWE
		strPattern::strTrack::strStep::strFx &_fx = patternStep.fx[0];
		switch (_fx.type)
		{
		case fx.FX_TYPE_ROLL:
			case fx.FX_TYPE_ROLL_UP:
			case fx.FX_TYPE_ROLL_DOWN:
			case fx.FX_TYPE_ROLL_RANDOM:

			playerRow.rollIsOn = 1;
			playerRow.rollVal = _fx.value;
			playerRow.rollType = _fx.value;
			playerRow.rollDir = _fx.type;

			break;

		case fx.FX_TYPE_RANDOM_NOTE:
			stepToSend.note = constrain(
					random(patternStep.note - _fx.value,
							patternStep.note + _fx.value + 1),
					0,
					127);
			break;
		case fx.FX_TYPE_RANDOM_INSTRUMENT:
			if (stepToSend.instrument < INSTRUMENTS_COUNT)
			{
				stepToSend.instrument = constrain(
						random(patternStep.instrument - _fx.value,
								patternStep.instrument + _fx.value + 1),
						0,
						INSTRUMENTS_COUNT - 1);
			}
			else
			{
				stepToSend.instrument = constrain(
						random(patternStep.instrument - _fx.value,
								patternStep.instrument + _fx.value + 1),
						INSTRUMENTS_COUNT,
						INSTRUMENTS_COUNT + 16 + 1);
			}
			break;

		case fx.FX_TYPE_RANDOM_VELOCITY:
			if (stepToSend.velocity >= 0)
			{
				stepToSend.velocity = constrain(
						random(patternStep.velocity - _fx.value,
								patternStep.velocity + _fx.value + 1),
						0,
						127);
			}
			else
			{
				stepToSend.velocity = random(0, _fx.value + 1);

			}
			break;

		default:
			break;
		}

		// ustawiamy całego stepa
		playerRow.stepOpen = 1;
		playerRow.stepTimer = 0; // od tej pory timer liczy w górę

		// ustawiamy nute
		playerRow.noteOpen = 1;
		playerRow.noteLength = 9999; // w MT nie ma dugości stepa
		playerRow.noteTimer = 0; // od tej pory timer liczy w górę

		playerRow.stepSent = stepToSend; // buforujemy wysłanego stepa

		// jeśli rolka to nuty są krótsze od stepa
		if (playerRow.rollIsOn)
		{
			playerRow.rollType = playerRow.rollVal;
			playerRow.noteLength = rollTypeToVal(playerRow.rollType) / 2; // TODO: wyliczyć długość rolki
			playerRow.stepOpen = 1;
		}
		if (patternStep.note >= 0)
		{
			if (playerRow.rollDir == fx.FX_TYPE_ROLL_RANDOM && playerRow.rollIsOn)
			{
				playerRow.stepToSend.note = constrain(
						random(
								playerRow.stepToSend.note - 12,
								playerRow.stepToSend.note + 12 + 1),
						0,
						127);
			}

			sendNoteOn(row, &stepToSend);

			playerRow.stepOpen = 1;
			playerRow.noteOpen = 1;
			playerRow.stepLength = 9999;

			playerRow.stepSent = playerRow.stepToSend;
		}
		else if (patternStep.note == STEP_NOTE_OFF)
		{
			sendNoteOff(row, &playerRow.stepSent);
			playerRow.stepOpen = 0;
			playerRow.rollIsOn = 0;
			playerRow.rollType = fx.ROLL_TYPE_NONE;
		}
	}

	// **************************
	// 		kontynuowanie nuty
	// **************************
	if (playerRow.performanceStutter != 0)
	{
		uint8_t tempRollType = 0;

		switch (playerRow.performanceStutter)
		{
		case -4:
			tempRollType = fx.ROLL_TYPE_4_1;
			break;
		case -3:
			tempRollType = fx.ROLL_TYPE_3_1;
			break;
		case -2:
			tempRollType = fx.ROLL_TYPE_2_1;
			break;
		case -1:
			tempRollType = fx.ROLL_TYPE_1_1;
			break;
		default:
			if (playerRow.performanceStutter > 0)
			{
				tempRollType = fx.ROLL_TYPE_1_1 + playerRow.performanceStutter - 1;
			}
		}
		if (((playerRow.stepTimer % rollTypeToVal(tempRollType)) == 1))
		{
			playerRow.stepToSend = playerRow.stepSent;
			playerRow.noteOpen = 1;
			playerRow.noteTimer = 0; // od tej pory timer liczy w górę
			sendNoteOn(row,
						playerRow.stepToSend.note,
						playerRow.stepToSend.velocity,
						playerRow.stepToSend.instrument);

			playerRow.stepSent = playerRow.stepToSend;
		}
	}
	else if (playerRow.stepOpen)
	{
		if (playerRow.rollType != fx.ROLL_TYPE_NONE)
		{
			// sprawdzamy timer microstepów, czy jest wielokrotrością rolki
			if (((playerRow.stepTimer % rollTypeToVal(playerRow.rollType)) == 1) && playerRow.stepTimer != 1)
			{
				playerRow.stepToSend = playerRow.stepSent;

				playerRow.noteOpen = 1;
				playerRow.noteTimer = 0; // od tej pory timer liczy w górę
				playerRow.noteLength = rollTypeToVal(playerRow.rollType) / 2; // TODO: wyliczyć długość rolki

				if (playerRow.rollDir == fx.FX_TYPE_ROLL_UP)
				{
					playerRow.stepToSend.note = constrain(
							++playerRow.stepToSend.note,
							0,
							127);
				}
				else if (playerRow.rollDir == fx.FX_TYPE_ROLL_DOWN)
				{
					playerRow.stepToSend.note = constrain(
							--playerRow.stepToSend.note,
							0,
							127);
				}
				else if (playerRow.rollDir == fx.FX_TYPE_ROLL_RANDOM)
				{
					playerRow.stepToSend.note = constrain(
							random(
									playerRow.stepToSend.note - 12,
									playerRow.stepToSend.note + 12 + 1),
							0,
							127);
				}
				sendNoteOn(row,
							playerRow.stepToSend.note,
							playerRow.stepToSend.velocity,
							playerRow.stepToSend.instrument);

				playerRow.stepSent = playerRow.stepToSend;
			}
		}
	}
}

void Sequencer::setPerformanceStutter(uint8_t track, int8_t stutter)
{
	player.track[track].performanceStutter = stutter;
}
void Sequencer::setPerformancePlayMode(uint8_t track, int8_t playmode)
{
	player.track[track].performancePlayMode = playmode;
}

Sequencer::strPattern *Sequencer::getPattern()
{
	return &seq[player.ramBank];
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

void Sequencer::play(void)
{

	reset_actual_pos();

	player.isStop = 0;
	player.isPlay = 1;
	nanoStep = 1;

	player.uStep = 1;
	for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
	{
		player.track[a].uStep = 1;
	}

	player.metronome_timer = 1;

}

void Sequencer::playPattern(void)
{
	player.songMode = 0;
	play();
}
void Sequencer::playSong(void)
{
	fileManager.savePattern(mtProject.values.actualPattern);
	fileManager.loadPattern(fileManager.resetToFirstSongPattern());
	switchRamPatternsNow();

	player.songMode = 1;
	play();
}
void Sequencer::playSong(uint8_t fromPos)
{

	fileManager.loadPattern(fileManager.getSongPattern(fromPos));
	fileManager.setSongPos(fromPos);

	switchRamPatternsNow();

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
		player.track[a].uStep = 0;
		player.track[a].makeJump = 0;
	}
//	player.changeBank = 0;

	player.swingToogle = 1;

	reset_actual_pos();

	allNoteOffs();
}

void Sequencer::rec(void)
{

	player.isREC = 1;
	player.rec_intro_timer = 1;
	player.rec_intro_step = 1;

	player.swing_offset = 50.0;

	init_player_timer();

}

void Sequencer::loadDefaultSequence(void)
{
	seq[player.ramBank].tempo = 120.0;

	for (uint8_t x = MINTRACK; x <= MAXTRACK; x++)
	{
//		seq[player.ramBank].track[x].rootNote = 35 + x;
//		seq[player.ramBank].track[x].channel = x;
//		seq[player.ramBank].track[x].isOn = 1;

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
		if (player.track[a].stepOpen)
		{
//			// TODO
//			midiSendChordOff

			player.track[a].stepOpen = 0;
		}

	}

// Serial.println("all stop");
}

void Sequencer::switchStep(uint8_t row) //przełączamy stepy w zależności od trybu grania
{
	uint8_t x = constrain(row, MINTRACK, MAXTRACK);
	int8_t playMode = player.track[row].performancePlayMode;
	int8_t patternLength = seq[player.ramBank].track[0].length;

	if (player.performancePatternLength > -1)
	{
		patternLength = player.performancePatternLength;
	}

	if (playMode == PLAYMODE_FORWARD)
	{
		player.track[x].actual_pos++;
		if ((player.track[x].actual_pos > patternLength))
		{
			reset_actual_pos(x);

			if (row == 0 && player.songMode)
			{
				switchRamPatternsNow();
				fileManager.switchNextPatternInSong();
			}

			if ((player.onPatternEnd != NULL) && (x == MINTRACK))
				player.onPatternEnd();

		}
	}
	else if (playMode == PLAYMODE_BACKWARD)
	{
		player.track[x].actual_pos--;
		if ((player.track[x].actual_pos < 0))
		{
			reset_actual_pos(x);
			if ((player.onPatternEnd != NULL) && (x == MINTRACK))
				player.onPatternEnd();
		}
	}
	else if (playMode == PLAYMODE_PINGPONG)
	{
		if (!player.track[x].pingPongToogle)
		{
			if ((player.track[x].actual_pos >= patternLength))
			{
				player.track[x].pingPongToogle = !player.track[x].pingPongToogle;
			}
			else
			{
				player.track[x].actual_pos++;
			}

		}
		else
		{
			if ((player.track[x].actual_pos <= 1))
			{
				player.track[x].pingPongToogle = !player.track[x].pingPongToogle;
			}
			else
			{
				player.track[x].actual_pos--;
			}
		}
	}

	else if (playMode == PLAYMODE_RANDOM)
	{
		player.track[x].actual_pos = random(1, patternLength + 1);
	}

}

void Sequencer::reset_actual_pos(void)
{
	for (uint8_t row = MINTRACK; row <= MAXTRACK; row++)
	{
		player.track[row].pingPongToogle = 0;
		reset_actual_pos(row);
	}
}

void Sequencer::reset_actual_pos(uint8_t row)
{

	if (player.track[row].performancePlayMode == PLAYMODE_FORWARD)
	{
		player.track[row].actual_pos = MINSTEP;
	}

	else if (player.track[row].performancePlayMode == PLAYMODE_BACKWARD)
	{
		player.track[row].actual_pos = seq[player.ramBank].track[row].length;
	}

	else if (player.track[row].performancePlayMode == PLAYMODE_PINGPONG)
	{
		player.track[row].actual_pos = MINSTEP;
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
	//	temp_Tempo = seq[player.ramBank].tempo;
	temp_Tempo = mtProject.values.globalTempo;
	else if (config.mode == MODE_MIDICLOCK.INTERNAL_LOCK)
	temp_Tempo = config.tempoLock;
	else
		temp_Tempo = player.externalTempo;

//3125 - wyliczone niegdyś matematycznie, 12 na potrzeby dividerów
	timer_var = ((3125.0 / temp_Tempo) * (player.swing_offset + 50.0)) / 12.0;

	playTimer.begin(timerExternalVector, timer_var);

}

void Sequencer::incr_uStep(uint8_t row)
{
	if (!player.track[row].divChange)
	{
		player.track[row].uStep++;

		if (player.track[row].uStep > 48)
		{
			player.track[row].uStep = 1;
			switchStep(row);
		}
	}
}

void Sequencer::divChangeQuantize(uint8_t row)
{
	// tutaj pobieramy tempoDiv - pozostalość po sequ
//	int8_t tTempoOption = constrain(seq[player.ramBank].track[row].tempoDiv,
//									MIN_TEMPO_DIV,
//									MAX_TEMPO_DIV);
	int8_t tTempoOption = TEMPODIV_1_1;

	uint8_t tDiv = getTempoDiv(tTempoOption);

	if (player.track[row].divChange && ((nanoStep % (tDiv * 48)) == 1))
	{
		player.track[row].divChange = 0;
		player.track[row].uStep = 1;
		switchStep(row);
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

void Sequencer::send_clock(uint8_t arg)
{
// TODO: wypełnić
// TODO: wypełnić

}

void Sequencer::sendNoteOn(uint8_t track, uint8_t note, uint8_t velocity,
							uint8_t instrument)
{

	if (instrument > INSTRUMENTS_MAX)
	{
		usbMIDI.sendNoteOn(note, velocity, instrument - INSTRUMENTS_MAX);
	}
	else
	{
		instrumentPlayer[track].noteOn(instrument, note, velocity);
	}

}
void Sequencer::sendNoteOn(uint8_t track, strPattern::strTrack::strStep *step)
{

	if (step->instrument > INSTRUMENTS_MAX)
	{
		usbMIDI.sendNoteOn(step->note,
							step->velocity,
							step->instrument - INSTRUMENTS_MAX);
	}
	else
	{
		instrumentPlayer[track].noteOn(step->instrument,
										step->note,
										step->velocity,
										step->fx[0].type,
										step->fx[0].value);
	}

}

void Sequencer::sendNoteOff(uint8_t track,
							uint8_t note,
							uint8_t velocity,
							uint8_t instrument)
{

	if (instrument > INSTRUMENTS_MAX)
	{
		usbMIDI.sendNoteOff(note,
							0,
							instrument - INSTRUMENTS_MAX);
	}
	else
	{
		instrumentPlayer[track].noteOff();
	}
}
void Sequencer::sendNoteOff(uint8_t track, strPattern::strTrack::strStep *step)
{

	if (step->instrument > INSTRUMENTS_MAX)
	{
		usbMIDI.sendNoteOff(step->note,
							0,
							step->instrument - INSTRUMENTS_MAX);
	}
	else
	{
		instrumentPlayer[track].noteOff();
	}
}

void Sequencer::sendNoteOff(uint8_t track)
{

//	usbMIDI.sendNoteOff(step->note, 0, 1);

	instrumentPlayer[track].noteOff();
}

void Sequencer::blinkNote(uint8_t instrument, uint8_t note, uint8_t velocity,
							uint8_t track)
{
	if (player.blink.isOpen)
	{
		closeBlinkNote();
	}

	player.blink.isOpen = 1;
	player.blink.track = track;
	player.blink.timer = 0;
	player.blink.instrument = instrument;
	player.blink.note = note;

	if (instrument < INSTRUMENTS_COUNT)
	{
		instrumentPlayer[track].noteOff();
		instrumentPlayer[track].noteOn(instrument,
										note,
										velocity);
	}
	else
	{
		usbMIDI.sendNoteOn(note, velocity, instrument - INSTRUMENTS_COUNT);
	}
}

void Sequencer::blinkSelectedStep()
{
	strPattern::strTrack::strStep *step;
	step = &seq[player.ramBank].track[selection.firstTrack].step[selection.firstStep];

	if (step->note >= 0)
	{
		blinkNote(step->instrument,
					step->note,
					step->velocity,
					selection.firstTrack);
	}

}

void Sequencer::loadNextPattern(uint8_t patternNumber)
{
	player.jump.nextPattern = patternNumber;
	player.jump.jumpNOW = 0;

	fileManager.setLoadPattern(patternNumber);
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

			if (channel == MIDI_CHANNEL_GRID)
			{
			}
			else
			{
				step->velocity = velocity;
			}

			instrumentPlayer[sel->firstTrack].noteOff();
			instrumentPlayer[sel->firstTrack].noteOn(step->instrument,
														step->note,
														step->velocity);
		}
	}
	else
	{
		if (!isMultiSelection())
		{
			instrumentPlayer[sel->firstTrack].noteOff();
		}
	}
}
void Sequencer::setPerformancePatternLength(int8_t length)
{
	player.performancePatternLength = length - 1;
}
void Sequencer::setPerformancePatternLengthFromFxVal(int8_t val)
{
	uint8_t performancePatternLengthValues[] =
			{ 1, 2, 4, 8, 16, 32, 64, 128 };

	switch (val)
	{
	case -1:
		setPerformancePatternLength(-1);
		break;
	default:
		setPerformancePatternLength(performancePatternLengthValues[val]);

	}

}
