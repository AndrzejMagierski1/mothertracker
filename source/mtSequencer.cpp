#include <Arduino.h>

#include "mtSequencer.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"
#include "mtFileManager.h"

#include "mtMidi.h"
#include "configEditor/configEditor.h"

#include "patternEditor/patternEditor.h"
Sequencer sequencer;

#include "debugLog.h"

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

bool Sequencer::isInternalClock(void)
{
	return mtConfig.midi.clkIn == clockIn_Internal;
}
void Sequencer::handle_uStep_timer(void)
{
	/*
	 1 step sekwencji = 48 x uStep = 48 x 12 nanoStep(576)
	 6912 nanoStep = 12 stepów
	 */

	// noInterrupts();
	if (isInternalClock())
	{
		if (isPlay() || isRec())
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
		if ((nanoStep % 576 != 0) && (player.isPlay || isRec())) // && !clockustep)
		{
			handle_nanoStep(0);

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

	if ((step == 1)) // to znaczy że wywoładnie funkcji przyszło z midi clocka
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

	// stary mechanizm, na potrzeby startowania timera
	if ((nanoStep % 12 == 1) || step)
	{
		if (player.uStep == 1)
		{

			//konfig timera do zmiany czasu trwania kroku na swing

			float tempSwing;
			if (isInternalClock())
			tempSwing = seq[player.ramBank].swing;
//			else if (config.mode == MODE_MIDICLOCK.INTERNAL_LOCK)
//			{
//				tempSwing = config.swingLock;
//			}
			else
				tempSwing = 50.0;
			if ((player.swingToogle))
			player.swing_offset = 0 + tempSwing;
			else
				player.swing_offset = 100 - tempSwing;
			player.swingToogle = !player.swingToogle;

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
	strPlayer::strPlayerTrack::strSendStep & stepToSend = player.track[row].stepToSend;
	strPlayer::strPlayerTrack::strSendStep & stepSent = player.track[row].stepSent;

	if (!playerRow.isActive)
		return;

	memcpy((uint8_t *) &stepToSend, (uint8_t *) &patternStep,
			sizeof(patternStep));
	if (playerRow.uStep == 1)
	{
		stepToSend.velocity = STEP_VELO_DEFAULT;

		// init stepowy
		playerRow.isOffset = 0;
		playerRow.cancelStep = 0;
	}

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
			playerRow.rollPeriod = fx.ROLL_PERIOD_NONE;

		}
	}
	if (playerRow.noteOpen)
	{
		playerRow.noteTimer++;

		if ((playerRow.noteTimer >= playerRow.noteLength))
		{
			sendNoteOff(row,
						playerRow.stepSent.note,
						0,
						playerRow.stepSent.instrument);

			playerRow.noteOpen = 0;
		}
	}

	//	**************************
	// 	wysyłamy zegar
	//	**************************

	if ((playerRow.uStep > 0) && player.isPlay && row == 0 && !player.selectionMode)
	{
		if (((playerRow.uStep - 1) % 8) == 0)
			sendMidiClock();
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

	int16_t randomisedValue = -1;

	if (playerRow.uStep == 1)
	{
		if (patternStep.fx[0].type == fx.FX_TYPE_RANDOM_VALUE)
		{
			int16_t lowVal = constrain(
					patternStep.fx[1].value - patternStep.fx[0].value,
					getFxMin(patternStep.fx[1].type),
					getFxMax(patternStep.fx[1].type));

			int16_t hiVal = constrain(
					patternStep.fx[1].value + patternStep.fx[0].value,
					getFxMin(patternStep.fx[1].type),
					getFxMax(patternStep.fx[1].type));

			randomisedValue = random(lowVal, hiVal + 1);

		}
		else if (patternStep.fx[1].type == fx.FX_TYPE_RANDOM_VALUE)
		{
			int16_t lowVal = constrain(
					patternStep.fx[0].value - patternStep.fx[1].value,
					getFxMin(patternStep.fx[0].type),
					getFxMax(patternStep.fx[0].type));

			int16_t hiVal = constrain(
					patternStep.fx[0].value + patternStep.fx[1].value,
					getFxMin(patternStep.fx[0].type),
					getFxMax(patternStep.fx[0].type));

			randomisedValue = random(lowVal, hiVal + 1);
		}

		uint8_t fxIndex = 0;
		for (strPattern::strTrack::strStep::strFx &_fxStep : patternStep.fx)
		{
			strPattern::strTrack::strStep::strFx _fx = _fxStep;

			if (randomisedValue != -1)
			{
				_fx.value = randomisedValue;
				stepToSend.fx[fxIndex].value = _fx.value;
			}

			switch (_fx.type)
			{

			case fx.FX_TYPE_MICROMOVE:

				playerRow.isOffset = 1;
				playerRow.offsetValue = map(_fx.value + 1, 0, 100, 1, 48);

				break;
			case fx.FX_TYPE_VELOCITY:

				stepToSend.velocity = _fx.value;
				stepSent.velocity = stepToSend.velocity;

				break;
			case fx.FX_TYPE_OFF:

				if (fxIndex == playerRow.rollFxId)
				{
					playerRow.isOffset = 0;
					playerRow.cancelStep = 0;
					playerRow.rollIsOn = 0;
				}

				instrumentPlayer[row].seqFx(0, 0, fxIndex);

				break;
			case fx.FX_TYPE_CHANCE:
				if (random(0, 128) > _fx.value)
					playerRow.cancelStep = 1;

				break;

			case fx.FX_TYPE_SEND_CC_A:
				sendCC(0, _fx.value);
				break;
			case fx.FX_TYPE_SEND_CC_B:
				sendCC(1, _fx.value);
				break;
			case fx.FX_TYPE_SEND_CC_C:
				sendCC(2, _fx.value);
				break;
			case fx.FX_TYPE_SEND_CC_D:
				sendCC(3, _fx.value);
				break;
			case fx.FX_TYPE_SEND_CC_E:
				sendCC(4, _fx.value);
				break;

			case fx.FX_TYPE_PROGRAM_CHANGE:
				sendProgramChange(_fx.value);
				break;

			case fx.FX_TYPE_TEMPO:
				player.performance.tempo = float(_fx.value * 2);
				break;

			case fx.FX_TYPE_RANDOM_VELOCITY:
				stepToSend.velocity = constrain(random(0,
														_fx.value + 1),
												0,
												127);
				stepSent.velocity = stepToSend.velocity;
				break;

			default:
				break;
			}

			if (patternStep.note == STEP_NOTE_EMPTY)
			{
				// wysyłam tylko fxa jeśli nie ma nuty
				switch (_fx.type)
				{
				case fx.FX_TYPE_NONE:
					break;
				case fx.FX_TYPE_ROLL:

					playerRow.rollIsOn = 1;
					playerRow.rollFxId = fxIndex;
					playerRow.rollVal = _fx.value;
					playerRow.rollDir = getRollType(_fx.value);
					playerRow.rollPeriod = _fx.value;
					playerRow.rollVolumeOption = rollValToVolumeOption(
							_fx.value);
					if (playerRow.rollVolumeOption != rollVolumeOption_const)
					{
						playerRow.rollLength = calcStepLength(
								row, playerRow.actual_pos);

						playerRow.rollVolumeStart = stepToSend.velocity;
					}

					break;
				default:
					instrumentPlayer[row].seqFx(_fx.type, _fx.value, fxIndex);
					break;
				}
			}
			fxIndex++;
		}
	}

	// **************************
	// 		odpalamy step?
	// **************************

	if (patternStep.note != STEP_NOTE_EMPTY)
	{

		// nie-offset
		if (!playerRow.isOffset && playerRow.uStep == 1 && !playerRow.cancelStep)
		{
			// wystartuj stepa
			startStep = 1;
		}
		// offset
		else if (playerRow.isOffset && playerRow.uStep == playerRow.offsetValue && !playerRow.cancelStep)
		{
			startStep = 1;
		}
	}

	// **************************
	// 		odpalamy stepa
	// **************************
	if (startStep)
	{
		if (playerRow.stepOpen || playerRow.stepOpen)
		{
			sendNoteOff(row,
						playerRow.stepSent.note,
						0,
						playerRow.stepSent.instrument);

			playerRow.stepOpen = 0;
			playerRow.noteOpen = 0;
			playerRow.rollIsOn = 0;
			playerRow.rollPeriod = fx.ROLL_PERIOD_NONE;
		}

		// EFEKTY WŁAŚCIWE
		uint8_t fxIndex = 0;
		for (strPattern::strTrack::strStep::strFx &_fxStep : patternStep.fx)
		{
			strPattern::strTrack::strStep::strFx _fx = _fxStep;

			if (randomisedValue != -1)
				_fx.value = randomisedValue;

			switch (_fx.type)
			{
			case fx.FX_TYPE_ROLL:

				playerRow.rollIsOn = 1;
				playerRow.rollFxId = fxIndex;
				playerRow.rollVal = _fx.value;
				playerRow.rollPeriod = _fx.value;
				playerRow.rollDir = getRollType(_fx.value);
				playerRow.rollVolumeOption = rollValToVolumeOption(_fx.value);
				if (playerRow.rollVolumeOption != rollVolumeOption_const)
				{
					playerRow.rollLength = calcStepLength(
							row, playerRow.actual_pos);

					playerRow.rollVolumeStart = stepToSend.velocity;
				}

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

			default:
				break;
			}
			fxIndex -= -1;
		}

		// ustawiamy całego stepa
		playerRow.stepOpen = 1;
		playerRow.stepTimer = 0; // od tej pory timer liczy w górę

		// ustawiamy nute
		playerRow.noteOpen = 1;
		playerRow.noteLength = 9999; // w MT nie ma dugości stepa
		playerRow.noteTimer = 0; // od tej pory timer liczy w górę

		if (stepToSend.note >= 0)
		{
			playerRow.stepSent = stepToSend; // buforujemy wysłanego stepa
		}

//		debugLog.addLine("note: ");
//		debugLog.addValue(playerRow.stepSent.note);

		// jeśli rolka to nuty są krótsze od stepa
		if (playerRow.rollIsOn)
		{
			playerRow.rollPeriod = playerRow.rollVal;
			playerRow.noteLength = rollValToPeriod(playerRow.rollPeriod) / 2; // TODO: wyliczyć długość rolki
			playerRow.stepOpen = 1;
		}
		if (patternStep.note >= 0)
		{
			if (playerRow.rollDir == fx.rollType_noteRandom && playerRow.rollIsOn)
			{
				playerRow.stepToSend.note = constrain(
						random(
								playerRow.stepToSend.note - 12,
								playerRow.stepToSend.note + 12 + 1),
						0,
						127);
			}
			if (playerRow.rollIsOn && playerRow.rollVolumeOption != rollVolumeOption_const)
			{
				stepToSend.velocity = getRollVelo(row);
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
			playerRow.noteOpen = 0;
			playerRow.stepOpen = 0;
			playerRow.rollIsOn = 0;
			playerRow.rollPeriod = fx.ROLL_PERIOD_NONE;
		}
		else if (patternStep.note == STEP_NOTE_CUT)
		{
			instrumentPlayer[row].noteOff(STEP_NOTE_CUT);
		}
		else if (patternStep.note == STEP_NOTE_FADE)
		{
			instrumentPlayer[row].noteOff(STEP_NOTE_FADE);
		}
	}

	// **************************
	// 		kontynuowanie nuty
	// **************************
	if (playerRow.performanceStutter != 0)
	{
		uint8_t tempRollType = 0;

		if (playerRow.performanceStutter > 0)
		{
			tempRollType = playerRow.performanceStutter;
		}
		if (((playerRow.stepTimer % rollValToPeriod(tempRollType)) == 1))
		{
			playerRow.stepToSend = playerRow.stepSent;
			playerRow.noteOpen = 1;
			playerRow.noteTimer = 0; // od tej pory timer liczy w górę
//			sendNoteOn(row,
//						playerRow.stepToSend.note,
//						playerRow.stepToSend.velocity,
//						playerRow.stepToSend.instrument);
			sendNoteOn(row, &playerRow.stepToSend);

			playerRow.stepSent = playerRow.stepToSend;
		}
	}
	else if (playerRow.stepOpen)
	{
		if (playerRow.rollPeriod != fx.ROLL_PERIOD_NONE && playerRow.rollIsOn)
		{
			// sprawdzamy timer microstepów, czy jest wielokrotrością rolki
			if (((playerRow.stepTimer % rollValToPeriod(playerRow.rollPeriod)) == 1) && playerRow.stepTimer != 1)
			{
				playerRow.stepToSend = playerRow.stepSent;

				playerRow.noteOpen = 1;
				playerRow.noteTimer = 0; // od tej pory timer liczy w górę
				playerRow.noteLength = rollValToPeriod(playerRow.rollPeriod) / 2; // TODO: wyliczyć długość rolki

				if (playerRow.rollDir == fx.rollType_noteUp)
				{
					playerRow.stepToSend.note = constrain(
							++playerRow.stepToSend.note,
							0,
							127);
				}
				else if (playerRow.rollDir == fx.rollType_noteDown)
				{
					playerRow.stepToSend.note = constrain(
							--playerRow.stepToSend.note,
							0,
							127);
				}
				else if (playerRow.rollDir == fx.rollType_noteRandom)
				{
					playerRow.stepToSend.note = constrain(
							random(
									playerRow.stepToSend.note - 12,
									playerRow.stepToSend.note + 12 + 1),
							0,
							127);
				}

				if (playerRow.rollIsOn && playerRow.rollVolumeOption != rollVolumeOption_const)
				{
					stepToSend.velocity = getRollVelo(row);
				}

//				sendNoteOn(row,
//							playerRow.stepToSend.note,
//							playerRow.stepToSend.velocity,
//							playerRow.stepToSend.instrument);
				sendNoteOn(row, &playerRow.stepToSend);

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

uint8_t Sequencer::rollValToPeriod(uint8_t rollVal)
{

	rollVal = rollVal % (fx.ROLL_PERIOD_MAX + 1);
	switch (rollVal)
	{
	case fx.ROLL_PERIOD_NONE:
		return 0;
		break;

//	case fx.ROLL_TYPE_4_1:
//		return 192;
//	case fx.ROLL_TYPE_3_1:
//		return 144;
//	case fx.ROLL_TYPE_2_1:
//		return 96;
	case fx.ROLL_PERIOD_1_1:
		return 48;
	case fx.ROLL_PERIOD_1_2:
		return 24;
	case fx.ROLL_PERIOD_1_3:
		return 16;
		break;
	case fx.ROLL_PERIOD_1_4:
		return 12;
		break;
	case fx.ROLL_PERIOD_1_6:
		return 8;
		break;
	case fx.ROLL_PERIOD_1_8:
		return 6;
		break;
	case fx.ROLL_PERIOD_1_12:
		return 4;
		break;
	case fx.ROLL_PERIOD_1_16:
		return 3;
		break;
	default:
		return 0;
	}
	return 0;
}

void Sequencer::play(void)
{
	engine.clearReverb();
	reset_actual_pos();

	player.isStop = 0;
	player.isPlay = 1;
	nanoStep = 1;

	player.uStep = 1;
	for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
	{
		player.track[a].uStep = 1;
	}

	sendMidiStart();

//	player.metronome_timer = 1;

}

void Sequencer::playSelection(void) // potrzebuje aktualnego zaznaczenia
{
	player.songMode = 0;
	player.selectionMode = 1;

	reset_actual_pos();

	player.isStop = 0;
	player.isPlay = 1;
	nanoStep = 1;

	player.uStep = 1;
	for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
	{
		player.track[a].uStep = 1;
		player.track[a].actual_pos = selection.firstStep;
		player.track[a].isActive = 0;
	}

	for (uint8_t a = selection.firstTrack; a <= selection.lastTrack; a++)
	{
		player.track[a].isActive = 1;
	}

	player.globalPos = selection.firstStep;

}

float Sequencer::getPlaySelectionProgress(void) // potrzebuje aktualnego zaznaczenia
{
	if (player.selectionMode && isSelectionCorrect(&selection))
	{
		strSelection * s = &selection;
		float retVal = (float(player.globalPos - s->firstStep) /
				float(s->lastStep - s->firstStep)) * 100.0f;
		return constrain(retVal, 0.0f, 100.0f);
	}
	else
	{
		return 0.0f;
	}
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

	uint8_t patternToStart = fileManager.getSongPattern(fromPos);
	if (patternToStart == mtProject.values.actualPattern)
	{
		fileManager.savePattern(patternToStart);
	}
	else
	{
		fileManager.loadPattern(patternToStart);
	}

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
		if (player.track[row].stepSent.instrument > INSTRUMENTS_MAX)
		{
			sendMidiNoteOn(
					player.track[row].stepSent.note,
					0,
					player.track[row].stepSent.instrument - INSTRUMENTS_MAX);
		}
	}
}

void Sequencer::stop(void)
{

	send_allNotesOff();

	player.isPlay = 0;
	player.isStop = 1;
	player.isREC = 0;
	player.uStep = 0;
	player.selectionMode = 0;

	nanoStep = 1;

	for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
	{
		player.track[a].uStep = 0;
		player.track[a].makeJump = 0;

		player.track[a].rollIsOn = 0;
		player.track[a].isActive = 1;

		player.track[a].performanceSourcePattern = -1;
	}
//	player.changeBank = 0;

	player.swingToogle = 1;

	reset_actual_pos();

	allNoteOffs();

	player.performance.tempo = 0.0;

	engine.endAllFx();
	sendMidiStop();
}

void Sequencer::rec(void)
{
	player.isREC = 1;
	playPattern();
}

void Sequencer::allNoteOffs(void)
{
	for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
	{

		player.track[a].stepOpen = 0;
		player.track[a].noteOpen = 0;
		player.track[a].recOpen = 0;
		instrumentPlayer[a].noteOff();

	}
}

void Sequencer::switchStep(uint8_t row) //przełączamy stepy w zależności od trybu grania
{
	uint8_t x = constrain(row, MINTRACK, MAXTRACK);
	int8_t playMode = player.track[row].performancePlayMode;
	int8_t patternLength = seq[player.ramBank].track[0].length;

	if (player.performance.patternLength > -1)
	{
		patternLength = player.performance.patternLength;
	}
	// liczymy globalny licznik stepa
	if (row == 0)
	{
		player.globalPos++;

		if (player.selectionMode)
		{
			if (player.globalPos > selection.lastStep)
			{
				stop();
				player.onPatternEnd();
			}
		}

		if ((player.globalPos > patternLength))
		{
			player.globalPos = 0;

		}
	}

	if (playMode == PLAYMODE_FORWARD)
	{
		player.track[x].actual_pos++;
		if ((player.track[x].actual_pos > patternLength))
		{
			reset_actual_pos(x);
//			bool isNextPatternAvailable = 0; // jeśli 0 to song sie skonczyl

			if (player.track[x].performanceSourcePattern != -1)
			{
				enterPerformanceMode();
				fileManager.loadTrack(player.track[x].performanceSourcePattern,
										x);
				cancelFxes(x);

				player.track[x].performanceSourcePattern = -1;

			}

			if (x == MAXTRACK)
			{
				bool isNextPatternAvailable = 0; // jeśli 0 to song sie skonczyl

				if (player.songMode)
				{
					reset_actual_pos();
					switchRamPatternsNow();
					isNextPatternAvailable =
							fileManager.switchNextPatternInSong();
				}

				if ((player.onPatternEnd != NULL) && isNextPatternAvailable)
				player.onPatternEnd();

				else if ((player.onSongEnd != NULL) && !isNextPatternAvailable)
				{
					player.onPatternEnd();
					player.onSongEnd();
				}

				if (player.songMode && !isNextPatternAvailable)
				{
					stop();
				}
			}

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
void Sequencer::alignToGlobalPos()
{
	for (uint8_t row = MINTRACK; row <= MAXTRACK; row++)
	{
		alignToGlobalPos(row);
	}
}
void Sequencer::alignToGlobalPos(uint8_t row)
{
	player.track[row].actual_pos = player.globalPos;
}
void Sequencer::reset_actual_pos(void)
{
	for (uint8_t row = MINTRACK; row <= MAXTRACK; row++)
	{
		player.track[row].pingPongToogle = 0;
		reset_actual_pos(row);
	}

	player.globalPos = 0;
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

//uint8_t Sequencer::getLongRollVelo(uint8_t rollCurve, float progress)
//{
//	progress = constrain(progress, 0, 100);
//	uint8_t retVal = 1;
//
//	if (rollCurve == ROLL_CURVE_FLAT)
//	{
//		retVal = 127;
//	}
//
//	else if (rollCurve == ROLL_CURVE_INCREMENTAL)
//	{
//		retVal = (progress / 100) * 127;
//	}
//
//	else if (rollCurve == ROLL_CURVE_DECREMENTAL)
//	{
//		retVal = ((100 - progress) / 100) * 127;
//	}
//
//	else if (rollCurve == ROLL_CURVE_INC_DEC)
//	{
//		if (progress <= 50)
//		{
//			retVal = (progress / 50) * 127;
//		}
//		else
//		{
//			retVal = ((100 - progress) / 50) * 127;
//		}
//	}
//	else if (rollCurve == ROLL_CURVE_DEC_INC)
//	{
//		if (progress <= 50)
//		{
//			retVal = ((50 - progress) / 50) * 127;
//		}
//		else
//		{
//			retVal = ((progress - 50) / 50) * 127;
//		}
//	}
//
//	else if (rollCurve == ROLL_CURVE_RANDOM)
//	{
//		retVal = random(0, 127);
//	}
//
//	return retVal > 0 ? retVal : 1;
//}

float Sequencer::getActualTempo()
{
	float temp_Tempo;

	if (player.performance.tempo > 0.0 && player.performance.tempo < MAX_TEMPO)
	temp_Tempo = player.performance.tempo;

	else if (isInternalClock())
	temp_Tempo = mtProject.values.globalTempo;

	else
		temp_Tempo = player.externalTempo;

	return temp_Tempo;
}

void Sequencer::init_player_timer(void) // MT::refreshTimer
{

	float timer_var = 0;

//3125 - wyliczone niegdyś matematycznie, 12 na potrzeby dividerów
	timer_var = ((3125.0 / getActualTempo()) * (player.swing_offset + 50.0)) / 12.0;

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

uint8_t Sequencer::isPlay(void)
{
	return player.isPlay;
}

uint8_t Sequencer::isRec(void)
{
	return player.isREC;
}

uint8_t Sequencer::isStop(void)
{
	return player.isStop;
}

void Sequencer::sendNoteOn(uint8_t track, uint8_t note, uint8_t velocity,
							uint8_t instrument)
{

	if (instrument > INSTRUMENTS_MAX)
	{
		sendMidiNoteOn(note, velocity, instrument - INSTRUMENTS_MAX);

	}
	else
	{
		instrumentPlayer[track].noteOn(instrument, note, velocity);
	}

}
void Sequencer::sendNoteOn(uint8_t track,
							strPlayer::strPlayerTrack::strSendStep *step)
{

	if (step->instrument > INSTRUMENTS_MAX)
	{
		if (step->velocity == -1)
		{
//			todo:
			uint8_t velo = mtProject.values.midiInstrument[step->instrument - INSTRUMENTS_MAX].velocity;

			sendMidiNoteOn(step->note,
							velo,
							step->instrument - INSTRUMENTS_MAX);
		}
		else
		{
			sendMidiNoteOn(step->note,
							step->velocity,
							step->instrument - INSTRUMENTS_MAX);
		}
	}
	else
	{
		instrumentPlayer[track].noteOn(step->instrument,
										step->note,
										step->velocity,
										step->fx[0].type,
										step->fx[0].value,
										step->fx[1].type,
										step->fx[1].value);
	}

}

void Sequencer::sendNoteOff(uint8_t track,
							uint8_t note,
							uint8_t velocity,
							uint8_t instrument)
{

	if (instrument > INSTRUMENTS_MAX)
	{
		sendMidiNoteOff(note,
						0,
						instrument - INSTRUMENTS_MAX);
	}
	else
	{
		instrumentPlayer[track].noteOff();
	}
}
void Sequencer::sendNoteOff(uint8_t track,
							strPlayer::strPlayerTrack::strSendStep *step)
{

	if (step->instrument > INSTRUMENTS_MAX)
	{
		sendMidiNoteOff(step->note,
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

void Sequencer::blinkNote(uint8_t instrument, uint8_t note, int8_t velocity,
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
		// todo: pobrać velo z audio engine
		if (velocity < 0) velocity = 120;

		sendMidiNoteOn(note, velocity, instrument - INSTRUMENTS_COUNT);
	}
}

void Sequencer::blinkSelectedStep()
{
	strPattern::strTrack::strStep *step;
	step = &seq[player.ramBank].track[selection.firstTrack].step[selection.firstStep];

	if (step->note >= 0)
	{
		if (player.blink.isOpen)
		{
			closeBlinkNote();
		}

		player.blink.isOpen = 1;
		player.blink.track = selection.firstTrack;
		player.blink.timer = 0;
		player.blink.instrument = step->instrument;
		player.blink.note = step->note;

		if (step->instrument < INSTRUMENTS_COUNT)
		{
			instrumentPlayer[selection.firstTrack].noteOn(step->instrument,
															step->note,
															120,
															step->fx[0].type,
															step->fx[0].value,
															step->fx[1].type,
															step->fx[1].value);
		}
		else
		{
			sendMidiNoteOn(step->note, 120,
							step->instrument - INSTRUMENTS_COUNT);
		}
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
		if (isEditMode())
		{
			strPattern::strTrack::strStep *step = &seq[player.ramBank].track[sel->firstTrack].step[sel->firstStep];
			if (!isMultiSelection())
			{

				if (step->note == STEP_NOTE_EMPTY)
				{
					step->instrument = mtProject.values.lastUsedInstrument;
				}
				step->note = note;
			}

			instrumentPlayer[sel->firstTrack].noteOff();
			instrumentPlayer[sel->firstTrack].noteOn(
					mtProject.values.lastUsedInstrument,
					note,
					STEP_VELO_DEFAULT);
		}
		else if (isRec())
		{
			for (uint8_t tr = getActualTrack(); tr < 8; tr++)
			{
				strPattern::strTrack::strStep *step = &getActualPattern()->track[tr].step[player.track[0].actual_pos];
				if (step->note == STEP_NOTE_EMPTY &&
						step->fx[0].type == 0 &&
						step->fx[1].type == 0 &&
						!player.track[tr].noteOpen)
				{
					step->note = note;
					step->instrument = mtProject.values.lastUsedInstrument;

					player.track[tr].stepSent.note = note;
					player.track[tr].noteOpen = 1;
					player.track[tr].recOpen = 1;

					instrumentPlayer[tr].noteOff();
					instrumentPlayer[tr].noteOn(
												step->instrument,
												step->note,
												STEP_VELO_DEFAULT);

					step->fx[0].type = fx.FX_TYPE_MICROMOVE;
					step->fx[0].value = player.uStep;
					break;
				}
			}
		}
		else // czyli playMode
		{
			for (uint8_t tr = sel->firstTrack; tr < 8; tr++)
			{
				if (!player.track[tr].noteOpen)
				{

//					player.track[tr].stepSent.note = note;
					player.track[tr].noteOpen = 1;
					player.track[tr].noteLength = 9999;
					player.track[tr].recOpen = note;

					instrumentPlayer[tr].noteOff();
					instrumentPlayer[tr].noteOn(
							mtProject.values.lastUsedInstrument,
							note,
							STEP_VELO_DEFAULT);
					Serial.printf("noteON tr %d\n", tr);
					break;
				}
			}
		}
	}
	else // czyli noteOff
	{
		if (isEditMode())
		{
			if (!isMultiSelection())
			{
				instrumentPlayer[sel->firstTrack].noteOff();
			}
		}
		else if (isRec())
		{
			for (uint8_t tr = 0; tr < 8; tr++)
			{
				if (player.track[tr].noteOpen
						&& player.track[tr].stepSent.note == note
						&& player.track[tr].recOpen)
				{
					instrumentPlayer[tr].noteOff();
					player.track[tr].noteOpen = 0;
					player.track[tr].recOpen = 0;

					strPattern::strTrack::strStep *step = &getActualPattern()->track[tr].step[player.track[0].actual_pos];

					if (step->note == STEP_NOTE_EMPTY)
					{
						step->note = STEP_NOTE_OFF;
					}
					else
					{
						if (player.track[0].actual_pos < getActualPattern()->track[0].length)
						{
							step = &getActualPattern()->track[tr].step[player.track[0].actual_pos + 1];
							step->note = STEP_NOTE_OFF;
						}
						else
						{
							step = &getActualPattern()->track[tr].step[0];
							step->note = STEP_NOTE_OFF;
						}
					}
					break;
				}
			}
		}
		else // czyli playMode
		{
			for (uint8_t tr = 0; tr < 8; tr++)
			{
				if (player.track[tr].noteOpen
						&& player.track[tr].recOpen)
				{
					instrumentPlayer[tr].noteOff();
					player.track[tr].noteOpen = 0;
					player.track[tr].recOpen = 0;
					Serial.printf("\tnoteOFF tr %d\n", tr);
					break;
				}
			}
		}
	}
}

void Sequencer::cancelFxes()
{
	player.performance.tempo = 0.0;

	for (uint8_t a = 0; a <= MAXTRACK; a++)
	{
		player.track[a].rollIsOn = 0;
	}
}
void Sequencer::cancelFxes(int8_t track)
{
	player.track[track].rollIsOn = 0;
}

void Sequencer::setPerformancePatternLength(int8_t length)
{
	player.performance.patternLength = length - 1;
}
void Sequencer::setPerformancePatternLengthFromFxVal(int8_t val)
{
	int16_t performancePatternLengthValues[] =
			{ -1, 1, 2, 4, 8, 16, 32, 64, 128 };

	setPerformancePatternLength(performancePatternLengthValues[val]);

}

void Sequencer::setTrackToLoadOnSwitch(uint8_t track, uint8_t sourcePattern)
{
	player.track[track].performanceSourcePattern = sourcePattern;
}

uint8_t Sequencer::getRollType(uint8_t value)
{
	return (value / (fx.ROLL_PERIOD_MAX + 1));
}

uint8_t Sequencer::rollValToVolumeOption(uint8_t value)
{
	switch (getRollType(value))
	{
	case fx.rollType_volUp:
		return rollVolumeOption_up;
	case fx.rollType_volDown:
		return rollVolumeOption_down;
	default:
		return rollVolumeOption_const;
	}
}
/*
 playerRow.noteTimer,
 playerRow.rollVolumeOption,
 playerRow.rollLength
 */
uint8_t Sequencer::getRollVelo(uint8_t row)
{
	strPlayer::strPlayerTrack & playerRow = player.track[row];
	int8_t volMax = 1;
	if (playerRow.rollVolumeStart < 0)
	{
		volMax = getInstrumentVelo(player.track[row].stepToSend.instrument);
	}
	else
	{
		volMax = playerRow.rollVolumeStart;
	}

	float progress = ((float) playerRow.stepTimer / (playerRow.rollLength * 48));

	switch (playerRow.rollVolumeOption)
	{
	case rollVolumeOption_up:
		return volMax * progress;
	case rollVolumeOption_down:
		return volMax * (1.0f - progress);
	default:
		break;
	}

	return 127;
}

uint8_t Sequencer::getInstrumentVelo(uint8_t ins)
{
	if (ins >= INSTRUMENTS_COUNT)
	{
		return mtProject.values.midiInstrument[ins - INSTRUMENTS_COUNT].velocity;
	}
	else
	{
		return mtProject.instrument[ins].volume;
	}
	return 0;
}

void Sequencer::feedExternalTempo(float setTempo)
{
	player.externalTempo = (3 * player.externalTempo + setTempo) / 4;
}
