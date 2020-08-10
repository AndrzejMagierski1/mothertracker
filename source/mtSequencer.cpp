#include <Arduino.h>

#include "mtSequencer.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"
//#include "mtFileManager.h"
#include "fileManager.h"
#include "sampleRecorder/sampleRecorder.h"

#include "core/songTimer.h"
#include "mtMidi.h"
#include "configEditor/configEditor.h"

#include "patternEditor/patternEditor.h"

#include "core/interfacePopups.h"

Sequencer sequencer;

#include "debugLog.h"

extern audioEngine engine;

extern uint8_t externalClockRunning;

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
	return (mtConfig.midi.clkIn == clockIn_Internal) ||
			player.selectionMode;
}

extern cSampleRecorder sampleRecorder;
cSampleRecorder *SR = &sampleRecorder;
//SR->recordInProgressFlag ==1

//recorderConfig.source
void Sequencer::handle_uStep_timer(void)
{
	/*
	 1 step sekwencji = 48 x uStep = 48 x 12 nanoStep(576)
	 6912 nanoStep = 12 stepów
	 */

	// noInterrupts();
	if (SR->recordInProgressFlag == 1 || player.preRecMetronomeActive)
	{
		if (SR->recorderConfig.source == SR->sourceTypeMicLG ||
				SR->recorderConfig.source == SR->sourceTypeMicHG ||
				SR->recorderConfig.source == SR->sourceTypeLineIn ||
				(player.preRecMetronomeActive))
		{
			if (nanoStep % 576 == 0)
			{
				//rekonfig timera
				init_player_timer();

				if ((isMetronomeActive() || player.preRecMetronomeActive) &&
						player.extRecMetronomeStep % (getMetronomeDenominator() * getMetronomeNumerator()) == 0)
				{
					engine.makeMetronomeTick(1);
				}
				else if ((isMetronomeActive() || player.preRecMetronomeActive) &&
						player.extRecMetronomeStep % getMetronomeDenominator() == 0)
				{
					engine.makeMetronomeTick(0);
				}

				player.extRecMetronomeStep++;
				if(player.preRecMetronomeActive && player.extRecMetronomeStep>16)
				{
					player.preRecMetronomeActive = 0;
					playPattern();
				}
			}

			nanoStep++;
			if (nanoStep > 6912)
			{
				nanoStep = 1;
				nanoStepMultiplier++;
			}
		}
	}
	else
	{
		player.extRecMetronomeStep = 0;
	}

	if (isInternalClock())
	{
		if (isPlay())
		{
			handle_nanoStep(0);
			nanoStep++;
			if (nanoStep > 6912)
			{
				nanoStep = 1;
				nanoStepMultiplier++;
			}
		}
	}
	else // external clock
	{
		// warunek blokujący przejście do kolejnego stepa
		if ((nanoStep % 576 != 0) && (player.isPlay )) // && !clockustep)
		{
			handle_nanoStep(0);

			nanoStep++;
			if (nanoStep > 6912)
			{
				nanoStep = 1;
				nanoStepMultiplier++;
			}
		}
		else
		{
			// clockFuck = 0;
		}
	}
}

void Sequencer::handle_nanoStep(uint8_t forceStepSwitchFromExtClk)
{

	if ((forceStepSwitchFromExtClk == 1)) // to znaczy że wywoładnie funkcji przyszło z midi clocka
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
	if ((nanoStep % 12 == 1) || forceStepSwitchFromExtClk)
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

	if (forceStepSwitchFromExtClk == 1)
	{
		nanoStep++;
		if (nanoStep > 6912)
		{
			nanoStep = 1;
			nanoStepMultiplier++;
		}

	}

}

void Sequencer::play_microStep(uint8_t row)
{
	if (row > MAXTRACK)
		return;
	strPattern::strTrack &patternRow = seq[player.ramBank].track[row];
	strPlayer::strPlayerTrack &playerRow = player.track[row];

	strPattern::strTrack::strStep &patternStep = patternRow.step[playerRow.actual_pos];
	strPlayer::strPlayerTrack::strSendStep &stepToSend = player.track[row].stepToSend;
	strPlayer::strPlayerTrack::strSendStep &stepSent = player.track[row].stepSent;

	bool forceFirstRollWhenNoNote = 0; // potrzebne jak rolka wchodzi jako efekt bez nuty

	if (row == 0 &&
			playerRow.uStep == 1 &&
			isRec())
	{
		if (isMetronomeActive() &&
				playerRow.actual_pos % (getMetronomeDenominator() * getMetronomeNumerator()) == 0)
		{
			engine.makeMetronomeTick(1);
		}
		else if (isMetronomeActive() &&
				playerRow.actual_pos % getMetronomeDenominator() == 0)
		{
			engine.makeMetronomeTick(0);
		}
	}



	memcpy((uint8_t*) &stepToSend, (uint8_t*) &patternStep,
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
	if (playerRow.noteOpen && playerRow.noteSource == -1)
	{
		playerRow.noteTimer++;

		if ((playerRow.noteTimer >= playerRow.noteLength) ||
				(isTrackEngineMuted(row) && playerRow.stepSent.instrument > INSTRUMENTS_MAX))
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
	if (playerRow.uStep == 1 && player.isPlay && row == 0 )
	{

		if (((playerRow.actual_pos == patternRow.length)) && player.sequencialSwitch_isArmed > 0)
		{
			loadNextPattern(player.sequencialSwitch_pattern);
		}
		else if (((playerRow.actual_pos == patternRow.length)) && player.songMode)
		{
			loadNextPattern(newFileManager.getNextSongPattern());
		}
	}

	// ************************************
	// 			jesli zmutowany albo nieaktywny(playSelection)
	//			to reszta sie nie wykonuje
	// ************************************

	if (!playerRow.isActive)
			return;
	if (isTrackEngineMuted(row))
		return;


	// ************************************
	// 		 PRE EFEKTY i operacje na uStep == 1
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

//		uint8_t fxIndex = 0;
		uint8_t noMoFx = 0;
		uint8_t setBreakPattern = 0;
//		for (strPattern::strTrack::strStep::strFx &_fxStep : patternStep.fx)
		for (int8_t fxIndex = 1; fxIndex >= 0; fxIndex--)
		{
			strPattern::strTrack::strStep::strFx _fx = patternStep.fx[fxIndex];

			if (randomisedValue != -1)
			{
				_fx.value = randomisedValue;
				stepToSend.fx[fxIndex].value = _fx.value;
			}

			switch (_fx.type)
			{

			case fx.FX_TYPE_MICROMOVE:
				killFxOnSlot(row, fxIndex);
				noMoFx = 1;

				playerRow.isOffset = 1;
				playerRow.offsetValue = map(_fx.value + 1, 0, 100, 1, 48);
				playerRow.offsetValue = constrain(playerRow.offsetValue,
													0,
													47);

				break;
			case fx.FX_TYPE_VELOCITY:
				if (patternStep.note >= 0)
				{
					killFxOnSlot(row, fxIndex);
					noMoFx = 1;

					stepToSend.velocity = _fx.value;
					stepSent.velocity = stepToSend.velocity;
				}
				break;
			case fx.FX_TYPE_OFF:
				killFxOnSlot(row, fxIndex);
				noMoFx = 1;

				if (fxIndex == playerRow.rollFxId)
				{
					playerRow.isOffset = 0;
					playerRow.cancelStep = 0;
					playerRow.rollIsOn = 0;
				}

				instrumentPlayer[row].seqFx(0, 0, fxIndex);

				break;
			case fx.FX_TYPE_CHANCE:
				killFxOnSlot(row, fxIndex);
				noMoFx = 1;

				if (random(0, getFxMax(fx.FX_TYPE_CHANCE)) > _fx.value)
					playerRow.cancelStep = 1;

				break;

			case fx.FX_TYPE_SEND_CC_A:
				killFxOnSlot(row, fxIndex);
				noMoFx = 1;

				sendCC(0, _fx.value);

				break;
			case fx.FX_TYPE_SEND_CC_B:
				killFxOnSlot(row, fxIndex);
				noMoFx = 1;

				sendCC(1, _fx.value);

				break;
			case fx.FX_TYPE_SEND_CC_C:
				killFxOnSlot(row, fxIndex);
				noMoFx = 1;

				sendCC(2, _fx.value);

				break;
			case fx.FX_TYPE_SEND_CC_D:
				killFxOnSlot(row, fxIndex);
				noMoFx = 1;

				sendCC(3, _fx.value);

				break;
			case fx.FX_TYPE_SEND_CC_E:
				killFxOnSlot(row, fxIndex);
				noMoFx = 1;

				sendCC(4, _fx.value);

				break;

			case fx.FX_TYPE_PROGRAM_CHANGE:
				killFxOnSlot(row, fxIndex);
				noMoFx = 1;

				sendProgramChange(_fx.value);

				break;

			case fx.FX_TYPE_TEMPO:
				killFxOnSlot(row, fxIndex);
				noMoFx = 1;

				player.performance.tempo = float(_fx.value * 2);
				player.performance.tempoSource = fxIndex;
				break;
			case fx.FX_TYPE_BREAK_PATTERN:
				killFxOnSlot(row, fxIndex);
				noMoFx = 1;

				setBreakPattern = 1;
				player.breakPattern = 1;
				break;

			case fx.FX_TYPE_RANDOM_VELOCITY:
				if (patternStep.note >= 0)
				{
					killFxOnSlot(row, fxIndex);
					noMoFx = 1;

					stepToSend.velocity = constrain(random(0,
															_fx.value + 1),
													0,
													127);
					stepSent.velocity = stepToSend.velocity;
				}
				break;

			default:
				break;
			}

			if (patternStep.note == STEP_NOTE_EMPTY && !noMoFx)
			{
				// wysyłam tylko fxa jeśli nie ma nuty
				switch (_fx.type)
				{
				case fx.FX_TYPE_NONE:
					break;
				case fx.FX_TYPE_ROLL:
					killFxOnSlot(row, fxIndex);

					playerRow.stepTimer = 1; // trza wyzerować na potrzeby przeliczania volume
					forceFirstRollWhenNoNote = 1;

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

						playerRow.rollVolumeStart = stepSent.velocity;
					}

					break;
				default:
					killFxOnSlot(row, fxIndex);
					instrumentPlayer[row].seqFx(_fx.type, _fx.value, fxIndex);
					break;
				}
			}
//			fxIndex++;
		}

		if (patternStep.note == STEP_NOTE_OFF)
		{
//			playerRow.isOffset = 1;
//			playerRow.offsetValue = 10;

		}

		if (setBreakPattern && player.songMode)
		{
			loadNextPattern(newFileManager.getNextSongPattern());
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
		if (playerRow.stepOpen && playerRow.stepToSend.note >= 0)
		{
			if(playerRow.stepToSend.instrument > INSTRUMENTS_MAX)
			{
				sendNoteOff(row,
							playerRow.stepSent.note,
							0,
							playerRow.stepSent.instrument);
			}


			playerRow.stepOpen = 0;
			playerRow.noteOpen = 0;
			playerRow.rollIsOn = 0;
			playerRow.rollPeriod = fx.ROLL_PERIOD_NONE;
		}

		// EFEKTY WŁAŚCIWE
//		uint8_t fxIndex = 0;
//		for (strPattern::strTrack::strStep::strFx &_fxStep : patternStep.fx)
		for (int8_t fxIndex = 1; fxIndex >= 0; fxIndex--)
		{
			strPattern::strTrack::strStep::strFx _fx = patternStep.fx[fxIndex];

			if (randomisedValue != -1)
				_fx.value = randomisedValue;

			switch (_fx.type)
			{
			case fx.FX_TYPE_ROLL:
				killFxOnSlot(row, fxIndex);

				playerRow.rollIsOn = 1;
				playerRow.rollFxId = fxIndex;
				playerRow.rollVal = _fx.value;
				playerRow.rollDir = getRollType(_fx.value);
				playerRow.rollPeriod = _fx.value;
				playerRow.rollVolumeOption = rollValToVolumeOption(_fx.value);
				if (playerRow.rollVolumeOption != rollVolumeOption_const)
				{
					playerRow.rollLength = calcStepLength(
							row, playerRow.actual_pos);

					playerRow.rollVolumeStart = stepToSend.velocity;
				}

				break;

			case fx.FX_TYPE_RANDOM_NOTE:
				killFxOnSlot(row, fxIndex);
				for (uint8_t a = 0; a < 100; a++)
				{
					stepToSend.note = constrain(
							random(patternStep.note - _fx.value,
									patternStep.note + _fx.value + 1),
							0,
							127);
					if (isInScale(stepToSend.note,
									mtConfig.values.padBoardRootNote,
									mtConfig.values.padBoardRootNote))
						break;
				}
				break;
			case fx.FX_TYPE_RANDOM_INSTRUMENT:
				killFxOnSlot(row, fxIndex);
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
//			fxIndex -= -1;
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
//				Serial.println(stepToSend.velocity);
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
			playerRow.noteOpen = 0;
		}
		else if (patternStep.note == STEP_NOTE_FADE)
		{
			instrumentPlayer[row].noteOff(STEP_NOTE_FADE);
			playerRow.noteOpen = 0;
		}
	}

	// **************************
	// 		kontynuowanie nuty
	// **************************
	// performance stutter
	if (playerRow.performanceStutter != 0)
	{
		uint8_t tempRollType = 0;

		if (playerRow.performanceStutter > 0)
		{
			tempRollType = playerRow.performanceStutter;
		}
		if (((playerRow.stepTimer % stutterValToPeriod(tempRollType)) == 1))
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
	// rolka z patternu
	else if (playerRow.stepOpen)
	{
		if (playerRow.rollPeriod != fx.ROLL_PERIOD_NONE && playerRow.rollIsOn)
		{
//			if(forceFirstRollWhenNoNote)
			// sprawdzamy timer microstepów, czy jest wielokrotrością rolki
			if (((playerRow.stepTimer % rollValToPeriod(playerRow.rollPeriod)) == 1) &&
					(playerRow.stepTimer != 1 || forceFirstRollWhenNoNote))
			{
				playerRow.stepToSend = playerRow.stepSent;
				playerRow.stepToSend.fx[0].type = fx.FX_TYPE_ROLL;

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
//					Serial.println(stepToSend.velocity);
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

Sequencer::strPattern* Sequencer::getPattern()
{
	return &seq[player.ramBank];
}

uint16_t Sequencer::rollValToPeriod(int8_t rollVal)
{

	rollVal = rollVal % (fx.ROLL_PERIOD_MAX + 1);
	switch (rollVal)
	{
	case fx.ROLL_PERIOD_NONE:
		return 0;
		break;

//	case fx.ROLL_PERIOD_16_1:
//		return 16 * 48;
//	case fx.ROLL_PERIOD_12_1:
//		return 12 * 48;
//	case fx.ROLL_PERIOD_8_1:
//		return 8 * 48;
//	case fx.ROLL_PERIOD_6_1:
//		return 6 * 48;
//	case fx.ROLL_PERIOD_4_1:
//		return 192;
//	case fx.ROLL_PERIOD_3_1:
//		return 144;
//	case fx.ROLL_PERIOD_2_1:
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

uint16_t Sequencer::stutterValToPeriod(int8_t rollVal)
{

	rollVal = rollVal % (fx.STUTTER_PERIOD_MAX + 1);
	switch (rollVal)
	{
	case fx.STUTTER_PERIOD_NONE:
		return 0;
		break;

	case fx.STUTTER_PERIOD_16_1:
		return 16 * 48;
	case fx.STUTTER_PERIOD_12_1:
		return 12 * 48;
	case fx.STUTTER_PERIOD_8_1:
		return 8 * 48;
	case fx.STUTTER_PERIOD_6_1:
		return 6 * 48;
	case fx.STUTTER_PERIOD_4_1:
		return 192;
	case fx.STUTTER_PERIOD_3_1:
		return 144;
	case fx.STUTTER_PERIOD_2_1:
		return 96;

	case fx.STUTTER_PERIOD_1_1:
		return 48;
	case fx.STUTTER_PERIOD_1_2:
		return 24;
	case fx.STUTTER_PERIOD_1_3:
		return 16;
		break;
	case fx.STUTTER_PERIOD_1_4:
		return 12;
		break;
	case fx.STUTTER_PERIOD_1_6:
		return 8;
		break;
	case fx.STUTTER_PERIOD_1_8:
		return 6;
		break;
	case fx.STUTTER_PERIOD_1_12:
		return 4;
		break;
	case fx.STUTTER_PERIOD_1_16:
		return 3;
		break;
	default:
		return 0;
	}
	return 0;
}

void Sequencer::play(void)
{
	play(0);

}
void Sequencer::play(uint8_t fromPos)
{
	fromPos = constrain(fromPos, 0, MAXSTEP);

	songTimer.start();
	engine.endAllFx();
	engine.clearDelay();
	engine.clearReverb();

	reset_actual_pos();

	player.isStop = 0;
	player.isPlay = 1;
	nanoStep = 1;
	nanoStepMultiplier = 0;

	player.uStep = 1;
	for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
	{
		player.track[a].uStep = 1;
		player.track[a].actual_pos = fromPos;
	}

	sendMidiStart();

	if (!isInternalClock())
	{
		externalClockRunning = 1;
	}

//	player.metronome_timer = 1;

}

void Sequencer::playSelection(void) // potrzebuje aktualnego zaznaczenia
{
	if (!isSelectionCorrect(&selection)) return;;

	reset_actual_pos();
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
	player.songMode = 0;
	player.selectionMode = 1;

	nanoStep = 1;
	nanoStepMultiplier = 0;

	player.uStep = 1;

	player.globalPos = selection.firstStep;

	player.isStop = 0;
	player.isPlay = 1;
}

float Sequencer::getPlaySelectionProgress(void) // potrzebuje aktualnego zaznaczenia
{
	if (player.selectionMode && isSelectionCorrect(&selection))
	{
		strSelection *s = &selection;
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
	playPattern(0);
}
void Sequencer::playPattern(uint8_t fromPos)
{
	player.songMode = 0;
	play(fromPos);
}
void Sequencer::playSong(void)
{
	newFileManager.saveWorkspacePatternNow(mtProject.values.actualPattern);
	newFileManager.loadWorkspacePatternNow(
			newFileManager.resetToFirstSongPattern());
	switchRamPatternsNow();

	player.songMode = 1;
	play();
}
void Sequencer::playSong(uint8_t fromPos)
{

	uint8_t patternToStart = newFileManager.getSongPattern(fromPos);
	newFileManager.setSongPos(fromPos);

	newFileManager.saveWorkspacePatternNow(mtProject.values.actualPattern);

	if (patternToStart != mtProject.values.actualPattern)
	{
		newFileManager.loadWorkspacePatternNow(patternToStart);
		switchRamPatternsNow();
	}

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
			sendMidiNoteOff(
					player.track[row].stepSent.note,
					0,
					player.track[row].stepSent.instrument - INSTRUMENTS_MAX);
		}
	}
}
// zatrzymuje nuty wywolane z palca
void Sequencer::stopManualNotes(void)
{
	for (uint8_t tr = 0; tr < 8; tr++)
	{
		if (player.track[tr].noteOpen
				&& player.track[tr].recOpen)
		{
			if (player.track[tr].stepSent.instrument > INSTRUMENTS_MAX)
			{
				sendMidiNoteOff(
						player.track[tr].stepSent.note,
						0,
						player.track[tr].stepSent.instrument - INSTRUMENTS_MAX);
			}
			else
			{
				instrumentPlayer[tr].noteOff();
			}
			player.track[tr].noteOpen = 0;
			player.track[tr].recOpen = 0;
			player.track[tr].noteSource = -1;
		}
	}
}

void Sequencer::stop(void)
{

	songTimer.stop();
	stopManualNotes();
	send_allNotesOff();

	player.isPlay = 0;
	player.isStop = 1;
	player.isREC = 0;
	player.uStep = 0;
	player.selectionMode = 0;

	player.preRecMetronomeActive = 0;

	externalClockRunning = 0;

	player.breakPattern = 0;

	sequencialSwitch_Reset();

	nanoStep = 0;
	nanoStepMultiplier = 0;

	for (uint8_t a = MINTRACK; a <= MAXTRACK; a++)
	{
		player.track[a].uStep = 0;
		player.track[a].makeJump = 0;

		player.track[a].rollIsOn = 0;
		player.track[a].isActive = 1;

		if (isPerformanceMode())
		{
			if (player.track[a].performanceSourcePattern != -1)
			{
				setTrackToLoadOnSwitch(
						a,
						player.track[a].performanceSourcePattern);
				switchPerformanceTrackNow(a);
			}
		}
		else
		{
			player.track[a].performanceSourcePattern = -1;
		}
	}
//	player.changeBank = 0;

	player.swingToogle = 0;

	reset_actual_pos();

	allNoteOffs();

	player.performance.tempo = 0.0;

	sendMidiStop();
}

void Sequencer::rec(void)
{
	player.isREC = 1;
	if (isPreRollActive())
	{
		player.preRecMetronomeActive = 1;
		strPopupStyleConfig popupConfig {
				1,					// time
				800 / 2 - 150,		// x
				480 / 2 - 50,		// y
				400,				// w
				70,				// h
				0xffffff,			// lineColor[4];
				0xffffff,
				0xffffff,
				0xffffff,
				controlStyleCenterX | controlStyleCenterY,		//lineStyle[4];
				controlStyleCenterX | controlStyleCenterY,
				controlStyleCenterX | controlStyleCenterY,
				controlStyleCenterX | controlStyleCenterY };

		mtPopups.config(0, &popupConfig);
		mtPopups.show(0, "Recording in 4.. 3.. 2.. 1..");
	}
	else
	{
		playPattern();
	}
}
void Sequencer::recOff(void)
{
	player.isREC = 0;
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

	if (player.performanceMode)
	{
		patternLength = player.performance.trackLength[row];
	}

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
		if ((player.track[x].actual_pos > patternLength) || (player.breakPattern))
		{

			reset_actual_pos(x);
//			bool isNextPatternAvailable = 0; // jeśli 0 to song sie skonczyl

			if (player.track[x].performanceSourcePattern != -1)
			{

				switchPerformanceTrackNow(x);
				cancelFxes(x);

				player.track[x].performanceSourcePattern = -1;

			}

			if (x == MAXTRACK)
			{
				bool isNextPatternAvailable = 0; // jeśli 0 to song sie skonczyl
				player.breakPattern = 0;

				if (player.sequencialSwitch_isArmed > 0)
				{
					player.songMode = 0;

					reset_actual_pos();
					switchRamPatternsNow();
					player.sequencialSwitch_pattern = 0;
					player.sequencialSwitch_isArmed = 0;
				}
				else if (player.songMode)
				{
					reset_actual_pos();
					switchRamPatternsNow();
					isNextPatternAvailable =
							newFileManager.switchNextPatternInSong();
				}

				if ((player.onPatternEnd != NULL) && isNextPatternAvailable)
				player.onPatternEnd();

				else if ((player.onSongEnd != NULL) && !isNextPatternAvailable)
				{
					player.onPatternEnd();
					player.onSongEnd();
//					stop();
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

	if (player.performance.tempo > 0.0 && player.performance.tempo <= MAX_TEMPO)
	temp_Tempo = player.performance.tempo;

	else if (isInternalClock())
	temp_Tempo = mtProject.values.globalTempo;

	else
		temp_Tempo = player.externalTempo;

	return temp_Tempo;
}

uint32_t Sequencer::getSeqTimer()
{
	return nanoStep + nanoStepMultiplier * 6912;
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

void Sequencer::sendNoteOn(uint8_t track,
							strPlayer::strPlayerTrack::strSendStep *step)
{
	if (step->instrument > INSTRUMENTS_MAX)
	{
		if (step->velocity == -1)
		{
			uint8_t velo = mtProject.values.midiInstrument[step->instrument - INSTRUMENTS_COUNT].velocity;

			sendMidiNoteOn(step->note,
							velo,
							step->instrument - INSTRUMENTS_MAX);
		}
		else
		{
			sendMidiNoteOn(step->note,
							map(step->velocity, 0, 100, 0, 127),
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

//void Sequencer::blinkNote(uint8_t instrument, uint8_t note, int8_t velocity,
//							uint8_t track)
//{
//	if (player.blink.isOpen)
//	{
//		closeBlinkNote();
//	}
//
//	player.blink.isOpen = 1;
//	player.blink.track = track;
//	player.blink.timer = 0;
//	player.blink.instrument = instrument;
//	player.blink.note = note;
//
//	if (instrument < INSTRUMENTS_COUNT)
//	{
//		instrumentPlayer[track].noteOff();
//		instrumentPlayer[track].noteOn(instrument,
//										note,
//										velocity);
//	}
//	else
//	{
//		// todo: pobrać velo z audio engine
//		if (velocity < 0) velocity = 120;
//
//		sendMidiNoteOn(note, velocity, instrument - INSTRUMENTS_COUNT);
//	}
//}

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
//	player.jump.jumpNOW = 0;

	newFileManager.loadWorkspacePattern(patternNumber);
}
// handleNote dla sytuacji gdzie nr pada jest nieistotny
//void Sequencer::handleNote(byte channel, byte note, byte velocity)
//{
//	handleNote(channel, note, velocity, handleNoteSource_irrelevant);
//}

void Sequencer::handleNoteOn(byte channel, // channel jesli midi, albo pochodzenie grida np. GRID_OUTSIDE_PATTERN itd
		byte note,
		byte velocity,
		int16_t source) // nr pada, jesli midi to source = nuta+100,
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;

	if (isEditMode() && channel != GRID_OUTSIDE_PATTERN)
	{
		newFileManager.setPatternStructChanged(
												mtProject.values.actualPattern);

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
				map(velocity, 0, 127, 0, 100),
				0,
				0, 0, 0); //magiczne zera
		engine.setLastUsedVoice(sel->firstTrack);
	}
	else if (isRec())
	{
		newFileManager.setPatternStructChanged(
												mtProject.values.actualPattern);

		for (uint8_t tr = getActualTrack(); tr < 8; tr++)
		{
			strPattern::strTrack::strStep *step = &getActualPattern()->track[tr].step[player.track[0].actual_pos];
			if (step->note == STEP_NOTE_EMPTY &&
					step->fx[0].type == 0 &&
					step->fx[1].type == 0 &&
					(source < 0 ? !player.track[tr].noteOpen : 1) && // jesli nagrywamy instrument, nie patrz na otwarte nuty
					!isTrackEngineMuted(tr)) // omijamy zmutowane tracki // stop mutantom
			{
				step->note = note;
				if (source < 0)
				{
					step->instrument = mtProject.values.lastUsedInstrument;
				}
				else
				{
					step->instrument = source;
				}

				player.track[tr].stepSent.note = note;
				player.track[tr].stepSent.instrument = step->instrument;

				player.track[tr].noteOpen = 1;
				player.track[tr].recOpen = 1;

				if (step->instrument > INSTRUMENTS_MAX)
				{
					uint8_t velo = mtProject.values.midiInstrument[step->instrument - INSTRUMENTS_COUNT].velocity;

					sendMidiNoteOn(
									step->note,
									velo,
									step->instrument - INSTRUMENTS_MAX);
				}
				else
				{

					instrumentPlayer[tr].noteOff();
					instrumentPlayer[tr].noteOn(
												step->instrument,
												step->note,
												map(velocity, 0, 127, 0, 100),
												0, 0, 0, 0); //magiczne zera
				}

				engine.setLastUsedVoice(tr);
				if (mtConfig.general.recOptions == recOptions_microtimingAndVelocity ||
						mtConfig.general.recOptions == recOptions_microtiming)
				{
					step->fx[0].type = fx.FX_TYPE_MICROMOVE;
					step->fx[0].value = map(player.uStep + 1, 1, 48, 0,
											100);
				}

				if (mtConfig.general.recOptions == recOptions_microtimingAndVelocity ||
						mtConfig.general.recOptions == recOptions_velocity)
				{
					step->fx[1].type = fx.FX_TYPE_VELOCITY;
					if (velocity >= 0)
					{
						step->fx[1].value = map(velocity,0,127,0,100);

					}
					else
					{
						step->fx[1].value = getInstrumentVolume(
								mtProject.values.lastUsedInstrument);
					}

				}
				break;
			}
		}
	}
	else // czyli playMode / dogrywanie / wgrywanie akordów w edit
	{
		for (uint8_t tr = sel->firstTrack; tr < 8; tr++)
		{
			if (!player.track[tr].noteOpen && !isTrackEngineMuted(tr))
			{

				player.track[tr].noteOpen = 1;
				player.track[tr].noteLength = 9999;
				player.track[tr].recOpen = note;
				player.track[tr].noteSource = source;

				player.track[tr].stepSent.note = note;
				player.track[tr].stepSent.instrument = mtProject.values.lastUsedInstrument;

				if (mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX)
				{
					uint8_t velo = mtProject.values.midiInstrument[mtProject.values.lastUsedInstrument - INSTRUMENTS_COUNT].velocity;

					sendMidiNoteOn(
							note,
							velo,
							mtProject.values.lastUsedInstrument - INSTRUMENTS_MAX);

				}
				else
				{

					instrumentPlayer[tr].noteOff();
					instrumentPlayer[tr].noteOn(
							mtProject.values.lastUsedInstrument,
							note,
							map(velocity, 0, 127, 0, 100),
							0,
							0,
							0,
							0); //magiczne zera

					engine.setLastUsedVoice(tr);
				}

				if (isMultiSelectionOnOneLine() &&
						isEditMode() &&
						tr >= sel->firstTrack &&
						tr <= sel->lastTrack)
				{
					strPattern::strTrack::strStep *step =
							&getActualPattern()->track[tr].step[sel->firstStep];

					step->note = note;
					step->instrument = mtProject.values.lastUsedInstrument;
				}

				//					Serial.printf("noteON tr %d\n", tr);
				break;
			}
		}
	}

}
void Sequencer::handleNoteOff(byte channel, // channel jesli midi, albo pochodzenie grida np. GRID_OUTSIDE_PATTERN itd
		byte note,
		byte velocity,
		int16_t source) // nr pada, jesli midi to source = nuta+100,
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;

	if (isEditMode() && channel != GRID_OUTSIDE_PATTERN)
	{
		if (!isMultiSelection())
		{
			instrumentPlayer[sel->firstTrack].noteOff();
		}
	}
	else if (isRec())
	{
		newFileManager.setPatternStructChanged(
												mtProject.values.actualPattern);

		for (uint8_t tr = 0; tr < 8; tr++)
		{
			if (player.track[tr].noteOpen
					&& player.track[tr].stepSent.note == note
					&& player.track[tr].recOpen)
			{

				if (player.track[tr].stepSent.instrument > INSTRUMENTS_MAX)
				{

					sendMidiNoteOff(
							player.track[tr].stepSent.note,
							0,
							player.track[tr].stepSent.instrument - INSTRUMENTS_MAX);
				}
				else
				{
					instrumentPlayer[tr].noteOff();
				}

				player.track[tr].noteOpen = 0;
				player.track[tr].recOpen = 0;

				strPattern::strTrack::strStep *step = &getActualPattern()->track[tr].step[player.track[0].actual_pos];

				if (source < 0) // tylko wtedy wstawiamy offy
				{
					if (step->note == STEP_NOTE_EMPTY)
					{
						step->note = STEP_NOTE_OFF;
						if (mtConfig.general.recOptions == recOptions_microtimingAndVelocity ||
								mtConfig.general.recOptions == recOptions_microtiming)
						{
							step->fx[0].type = fx.FX_TYPE_MICROMOVE;
							step->fx[0].value = map(player.uStep, 1, 48, 0,
													100);
						}

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
					&& player.track[tr].recOpen
					&& player.track[tr].noteSource == source)
			{

				if (mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX)
				{
					sendMidiNoteOff(
							note,
							0,
							mtProject.values.lastUsedInstrument - INSTRUMENTS_MAX);
				}
				else
				{
					instrumentPlayer[tr].noteOff();
				}
				player.track[tr].noteOpen = 0;
				player.track[tr].recOpen = 0;
				player.track[tr].noteSource = -1;
				//					Serial.printf("\tnoteOFF tr %d\n", tr);
				break;
			}
		}

	}

}

// 1 jesli jest najmniej jedna otwarta nuta recOpen
uint8_t Sequencer::noMoreRecOpen()
{
	uint8_t noMore = 1;
	for (uint8_t a = 0; a < 8; a++)
	{
		if (player.track[a].recOpen)
		{
			noMore = 0;
			break;
		}
	}
	return noMore;
}

//void Sequencer::handleNote(byte channel, // channel jesli midi, albo pochodzenie grida np. GRID_OUTSIDE_PATTERN itd
//		byte note,
//		byte velocity,
//		int16_t source) // nr pada, jesli midi to source = nuta+100,
//{
//
//	if (velocity > 0)
//	{
//		handleNoteOn(channel,
//						note,
//						velocity,
//						source);
//	}
//	else
//	{
//		handleNoteOff(channel,
//						note,
//						velocity,
//						source);
//	}
//
//}

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

void Sequencer::killFxOnSlot(uint8_t row, uint8_t slot)
{

	if (player.track[row].rollFxId == slot)
	{
		player.track[row].rollIsOn = 0;
	}

//	if (player.performance.tempoSource == slot)
//	{
//		player.performance.tempo = 0.0f;
//	}
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
// laduje track do bufora i czeka na switch
void Sequencer::setTrackToLoadOnSwitch(uint8_t track, uint8_t sourcePattern)
{
	if (!player.performanceMode)
	{
		enterPerformanceMode();
	}

	newFileManager.loadPerformanceTrackToBuffer(sourcePattern, track);

	player.track[track].performanceSourcePattern = sourcePattern;
}
// laduje track do bufora i przelacza od razu
void Sequencer::setTrackToLoadNow(uint8_t track, uint8_t sourcePattern)
{
	if (!player.performanceMode)
	{
		enterPerformanceMode();
	}

	newFileManager.loadPerformanceTrackToBuffer(sourcePattern, track);

	switchPerformanceTrackNow(track);
}

void Sequencer::setPerformanceTrackLength(uint8_t track, uint8_t length)
{
	if (track >= 8) return;
	if (length > MAXSTEP) return;

	player.performance.trackLength[track] = length;
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
	strPlayer::strPlayerTrack &playerRow = player.track[row];
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

uint8_t Sequencer::getInstrumentVelo(uint8_t ins) // 0-127
{
	if (ins >= INSTRUMENTS_COUNT)
	{
		return mtProject.values.midiInstrument[ins - INSTRUMENTS_COUNT].velocity;
	}
	else
	{
		return map(mtProject.instrument[ins].volume, 0, 100, 0, 127);
	}
	return 0;
}
uint8_t Sequencer::getInstrumentVolume(uint8_t ins) // 0-100
{
	if (ins >= INSTRUMENTS_COUNT)
	{
		return map(mtProject.values.midiInstrument[ins - INSTRUMENTS_COUNT].velocity,0,127,0,100);
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

void Sequencer::switchRamPatternsNow()
{
	cancelFxes();
	player.ramBank = !player.ramBank;
	mtProject.values.actualPattern = newFileManager.getLoadedPatternIndex();
}

extern Sequencer::strPattern fileManagerPerformancePatternBuffer;

// podmienia zaladowane w performance tracki
void Sequencer::switchPerformanceTrackNow(uint8_t trackToSwitch)
{

	Sequencer::strPattern *patternTo = sequencer.getActualPattern();
	Sequencer::strPattern *patternFrom = &fileManagerPerformancePatternBuffer;

	patternTo->track[trackToSwitch] = patternFrom->track[trackToSwitch];

	setPerformanceTrackLength(trackToSwitch, patternFrom->track[trackToSwitch].length);

	player.track[trackToSwitch].performanceSourcePattern = -1;

}

uint8_t Sequencer::isMetronomeActive()
{
	return mtConfig.metronome.state > 0;
}
uint8_t Sequencer::isPreRollActive()
{
	return mtConfig.metronome.preRoll == 1;
}
uint8_t Sequencer::getMetronomeNumerator()
{
	return mtConfig.metronome.timeSignatureNumerator + 1;
}
uint8_t Sequencer::getMetronomeDenominator()
{
	return mtConfig.metronome.timeSignatureDenominator + 1;
}

void Sequencer::setMidiInVoiceMode(enMidiInVoiceMode mode)
{
	player.midiInVoiceMode = mode;
}

uint8_t Sequencer::getMidiInVoiceMode()
{
	return player.midiInVoiceMode;
}

// zmiania wartosc sequencialSwitch, potrzebuje uzbrojenia setReady()
void Sequencer::sequencialSwitch_changeNextPattern(int8_t delta)
{
	if (player.sequencialSwitch_pattern == 0)
	{
		player.sequencialSwitch_pattern = mtProject.values.actualPattern;
	}

	player.sequencialSwitch_pattern = constrain(
			player.sequencialSwitch_pattern + delta,
			PATTERN_INDEX_MIN,
			PATTERN_INDEX_MAX);

//	Serial.printf("sequentialPatternChange: %d\n",
//					player.sequencialSwitch_pattern);

}

// uzbraja sequencialSwitch
void Sequencer::sequencialSwitch_SetReady()
{
	player.sequencialSwitch_isArmed = 1;

	if (player.sequencialSwitch_pattern == mtProject.values.actualPattern)
	{
		sequencialSwitch_Reset();
	}
}

// anuluje sequencialSwitch
void Sequencer::sequencialSwitch_Reset()
{
	player.sequencialSwitch_pattern = 0;
	player.sequencialSwitch_isArmed = 0;
}

// getuje nastepny pattern, jesli 0 to nie ma zmiany
uint16_t Sequencer::sequencialSwitch_GetNext()
{
	return player.sequencialSwitch_pattern;
}
