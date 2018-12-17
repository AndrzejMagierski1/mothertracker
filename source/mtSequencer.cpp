/*
 * mtSequencer.cpp
 *
 *  Created on: 14 gru 2018
 *      Author: krzys
 */

#include "mtSequencer.h"

void Sequencer::init_player_timer()
{

	float timer_var = 0;
	float temp_Tempo;

	// szukamy tempa do zadania
	if (clockMode == defs.CLOCKMODE_INTERNAL)
		temp_Tempo = tempo;
	else if (clockMode == defs.CLOCKMODE_INTERNAL_LOCK)
		temp_Tempo = tempoLock;
	else
		temp_Tempo = tempoExternal;

	//3125 - wyliczone niegdyś matematycznie, 12 na potrzeby dividerów
	timer_var = ((3125.0 / temp_Tempo) * (player.swingOffset + 50.0)) / 12.0;

//	playTimer.begin(timerVector, timer_var); TODO: set vector do tego miejsca

}

void todo(){};

void Sequencer::timerVector()
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
	if ((clockMode == defs.CLOCKMODE_INTERNAL)
			|| (clockMode == defs.CLOCKMODE_INTERNAL_LOCK))
	{
		// handle_uStep(0);
		// play_uStepEmulate(0);
		if (player.isPlay || player.isRec)
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
		if ((timerTick % 576 != 0) && (player.isPlay || player.isRec)) // && !clockustep)
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

void Sequencer::handle_uStep12(uint8_t step)
{
	if (player.isPlay)
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
						int8_t tTempoOption = constrain(seq[player.ramBank].row[a].tempoDiv,
														defs.TEMPODIV_MIN,
														defs.TEMPODIV_MAX);
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
			if (!player.isRec)
			{
				for (uint8_t a = 1; a <= 8; a++)
				{
					int8_t tTempoDiv = constrain(
							seq[player.ramBank].row[a].tempoDiv, defs.TEMPODIV_MIN,
							defs.TEMPODIV_MAX);
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
				if (!player.isRec)
				{
					float tempSwing;
					if (config.mode == MODE_MIDICLOCK_INTERNAL)
						tempSwing = seq[player.ramBank].swing;
					else if (config.mode == MODE_MIDICLOCK_INTERNAL_LOCK)
						tempSwing = config.swingLock;
					else
						tempSwing = 50.0;

					if ((player.swingToogle))
						player.swingOffset = 0 + tempSwing;
					else
						player.swingOffset = 100 - tempSwing;
					player.swingToogle = !player.swingToogle;
				}
				else
				{
					player.swingOffset = 50.0;
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

uint8_t Sequencer::play_uStep(uint8_t row)
{
	uint8_t anythingSent = 0;
	uint8_t x = constrain(row, 1, 8);
	strRow & tempSeqRow = seq[player.ramBank].row[x];
	strPlayerRow & tempPlayerRow = player.row[x];

	// TU LICZYMY CZAS DO KONCA NUTY
	if (player.row[x].note_length_timer > 0)//ZMIANA
	{
		player.row[x].note_length_timer --;
		if ( player.row[x].note_length_timer < 0 ) player.row[x].note_length_timer = 0; //bzdura ale huj, zostawie
	}
	// wyłączamy nutę jeśli się skończyła
	if ((player.row[x].note_length_timer <= 1) && (player.row[x].noteOn_sent) && !player.row[x].recNoteOpen)
	{
		// GASIMY
		if (player.row[x].note_sent <= 127)
		{
			// Serial.println("gasimy");

//			todo
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
//			todo
//			send_clock(x);
			// Serial.println("clock");
			// Serial.println(player.uStepInd[x]);
		}
	}



	// uint8_t temp_hitMode, temp_isOn, temp_offset;//, temp_isGhost;
	const uint8_t temp_hitMode 		= seq[player.ramBank].row[x].step[player.row[x].actual_pos].hitMode;
	const uint8_t temp_rowIsOn 		= (seq[player.ramBank].row[x].flags & MASK_ROW_ON) > 0;
	const uint8_t temp_stepIsOn		= seq[player.ramBank].row[x].step[player.row[x].actual_pos].isOn;

	const uint8_t temp_rollMode 	= val2roll( seq[player.ramBank].row[x].step[player.row[x].actual_pos].hitMode);
	const uint8_t temp_rollCurve 	= seq[player.ramBank].row[x].step[player.row[x].actual_pos].rollCurve;
	const uint8_t temp_offset 		= seq[player.ramBank].row[x].step[player.row[x].actual_pos].offset;
	const uint8_t temp_stepVelo 	= seq[player.ramBank].row[x].step[player.row[x].actual_pos].velocity;
	// temp_isGhost 	= player.row[x].step[player.row[x].actual_pos].isGhost;


	if (temp_hitMode >= 1 && temp_rowIsOn && temp_stepIsOn)
	{
		//

		// TU GRAMY
		if (((temp_hitMode != HITMODE_OFFSET) && ((player.uStepInd[x] % (48 / temp_rollMode)) == 1))
		        || ((temp_hitMode == HITMODE_OFFSET) && (player.uStepInd[x] == temp_offset)))
		{

			if ((player.uStepInd[x] % 48) == 1)
			{
				player.row[x].rollLength = 0;
				player.row[x].rollCounter = 0;
				// Serial.print("cycki1 ");
			}
			// tempPlayerRow.rollLength = 0;

			const uint8_t tempNote 	=	tempSeqRow.step[player.row[x].actual_pos].note;
			const uint8_t tempChord =	tempSeqRow.step[player.row[x].actual_pos].chord;
			const uint8_t tempMod 	=	tempSeqRow.step[player.row[x].actual_pos].modulation;
			const uint8_t tempScale =	tempSeqRow.scale;
			const uint8_t tempRoot 	=	tempSeqRow.rootNote;
			const uint8_t tempLength =	tempSeqRow.step[player.row[x].actual_pos].length1 + 1;


			// sprawdzam czy początek rolki
			if ((temp_hitMode != HITMODE_OFFSET) && (temp_hitMode > 1) && ((player.uStepInd[x] % 48) == 1))
			{
				player.row[x].rollLength = tempLength;
				player.row[x].rollStep   = player.row[x].actual_pos;
				// Serial.print("cycki2 ");

				// Serial.print("<");
			}
			else if ((temp_hitMode != HITMODE_OFFSET) && (temp_hitMode == 1))
			{
				player.row[x].rollStep   = player.row[x].actual_pos;
			}


			// procent postępu rolki
			const float rollProgress = (float(player.uStepInd[row]) / (float(tempLength) * 48)) * 100;
			// Serial.print("progress: ");
			// Serial.println(rollProgress);

			float tempVelo = temp_hitMode > 1 ? getLongRollVelo(temp_rollCurve, rollProgress) * ((float)temp_stepVelo / 127) : temp_stepVelo;

			tempVelo = (tempVelo / MAX_VELO_TRACK) * seq[player.ramBank].row[x].trackVelo;






			//wyciszamy jezeli bylo cos wczesniej
			if (player.row[x].noteOn_sent)
			{
				// midiSendNoteOff(player.row[x].note_sent,
				//                 0,
				//                 player.row[x].channel_sent,
				//                 player.row[x].midiOut_sent);


//				todo();
//				midiSendChordOff(player.row[x].note_sent,
//				                 player.row[x].chord_sent,
//				                 0,
//				                 player.row[x].channel_sent,
//				                 player.row[x].midiOut_sent,
//				                 player.row[x].scale_sent,
//				                 player.row[x].scaleRoot_sent);

				// usbMIDI.send_now();



			}


			//gramy

			if (player.row[x].rollLength > 0) ++player.row[x].rollCounter;

			if (tempNote <= MAX_NOTE_TRACK && !player.row[x].divChange)
			{

				player.row[x].note_sent = constrain( (int16_t) tempNote + (int16_t) getNextRollNoteOffset(x), MIN_NOTE_STEP, MAX_NOTE_STEP);
				// Serial.println(player.row[x].note_sent);
				midiSendChordOn(constrain(player.row[x].note_sent, MIN_NOTE_STEP, MAX_NOTE_STEP) ,
				                tempChord,
				                uint8_t(tempVelo),
				                seq[player.ramBank].row[x].channel,
				                seq[player.ramBank].row[x].midiOut,
				                tempScale,
				                tempRoot);

				anythingSent = 1;

				// usbMIDI.send_now();




//				todo();
//				player.row[x].midiOut_sent = seq[player.ramBank].row[x].midiOut;
//				player.row[x].channel_sent = seq[player.ramBank].row[x].channel;



				// jesli wystapila zmiana wysylamy mod
//				if (tempMod != NULL_MOD)
//				{
//					midiSendCC(seq[player.ramBank].row[x].cc,
//					           tempMod,
//					           seq[player.ramBank].row[x].channel,
//					           seq[player.ramBank].row[x].midiOut);
//					// player.row[x].lastMod = tempMod;
//				}
			}

			else if (tempNote == NOTE_JUMPTO)
			{
				uint8_t can_i_jump = 1;
				for (uint8_t a = 1; a <= 8; a++) if (player.row[a].makeJump)can_i_jump = 0;

				if (can_i_jump)
				{
					seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1 = 0; // moment przed koncem, zeby nie zaświecała kolejna dioda

					player.row[x].lastMod = tempMod;    // mod mówi gdzie skaczemy

					setLoadBank2Ram(tempMod);

					player.row[x].makeJump = 1;
				}

				// JUMP TO po odliczeniu długości nuty

			}
			else if (tempNote == NOTE_JUMPTO_NEXT)
			{

				uint8_t can_i_jump = 1;
				for (uint8_t a = 1; a <= 8; a++) if (player.row[a].makeJump)can_i_jump = 0;

				if (can_i_jump)
				{
					seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1 = 0; // moment przed koncem, zeby nie zaświecała kolejna dioda

					// player.row[x].lastMod = tempMod;    // mod mówi gdzie skaczemy

					setLoadBank2Ram(player.actualBank < 255 ? player.actualBank + 1 : 0);

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
				case GATEMODE_NORMAL:
					player.row[x].note_length_timer = (48 / temp_rollMode) - 2;
					break;

				case GATEMODE_MEDIUM:
					player.row[x].note_length_timer = (48 / temp_rollMode) / 2;
					break;

				case GATEMODE_SHORT:
					player.row[x].note_length_timer = 1;
					break;

				case GATEMODE_EXTRASHORT:
					player.row[x].note_length_timer = 1;
					break;

				default:
					player.row[x].note_length_timer = (seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1 + 1) * 48 - 3;
				}
			}

			else
			{
				// player.row[x].note_length_timer = seq[player.ramBank].row[x].step[player.row[x].actual_pos].length * 48 - 3;
				// if (seq[player.ramBank].row[x].playMode == PLAYMODE_FORWARD) player.row[x].note_length_timer = 10;

				switch (seq[player.ramBank].row[x].gateMode)
				{
				case GATEMODE_NORMAL:
					player.row[x].note_length_timer = (seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1 + 1) * 48 - 3;
					break;

				case GATEMODE_MEDIUM:
					player.row[x].note_length_timer = (seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1 + 1) * 48 - 24;
					break;

				case GATEMODE_SHORT:
					player.row[x].note_length_timer = (seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1 + 1) * 48 - 36;
					break;

				case GATEMODE_EXTRASHORT:
					player.row[x].note_length_timer = (seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1 + 1) * 48 - 43;
					break;

				default:
					player.row[x].note_length_timer = (seq[player.ramBank].row[x].step[player.row[x].actual_pos].length1 + 1) * 48 - 3;
				}
			}


			if (tempNote == NOTE_JUMPTO) player.row[x].note_length_timer -= 1; // zeby diody nastepne po jumpie nie zaswiecily
		}
	}

	else if (tempPlayerRow.rollLength > 0)
	{
		// pobieramy dane
		const uint8_t ghostedStep 		= tempPlayerRow.rollStep;
		// const uint8_t ghost_hitMode 	= seq[player.ramBank].row[x].step[ghostedStep].hitMode;
		const uint8_t ghost_rollMode 	= val2roll( seq[player.ramBank].row[x].step[ghostedStep].hitMode);

		const uint8_t ghost_stepIsOn 	= val2roll( seq[player.ramBank].row[x].step[ghostedStep].isOn);



		const uint8_t temp_rowIsOn 		= (seq[player.ramBank].row[x].flags & MASK_ROW_ON) > 0;
		const uint8_t ghostedRollCurve 	= seq[player.ramBank].row[x].step[ghostedStep].rollCurve;
		const uint8_t ghostedStepLength = seq[player.ramBank].row[x].step[ghostedStep].length1 + 1;


		if ((player.uStepInd[row] % 48) == 1)
		{
			--tempPlayerRow.rollLength;
		}

		if (ghostedStep == 0)
		{
			tempPlayerRow.rollLength = 0;
			tempPlayerRow.rollCounter = 0;
		}
		if (((player.uStepInd[row] % (48 / ghost_rollMode)) == 1) && tempPlayerRow.rollLength)
		{
			if (ghost_stepIsOn >= 1 && temp_rowIsOn)
			{

				if (player.row[x].rollLength > 0) ++player.row[x].rollCounter;


				// procent postępu rolki
				const float rollProgress = (float((ghostedStepLength - tempPlayerRow.rollLength) * 48 + player.uStepInd[row]) / (float(ghostedStepLength) * 48)) * 100;

				// TU GRAMY
				if (((ghost_rollMode != HITMODE_OFFSET) && ((player.uStepInd[row] % (48 / ghost_rollMode)) == 1))
				        || ((ghost_rollMode == HITMODE_OFFSET) && (player.uStepInd[row] == temp_offset)))
				{
					const uint8_t tempNote 		=	tempSeqRow.step[ghostedStep].note;
					const uint8_t tempStepVelo 	=	tempSeqRow.step[ghostedStep].velocity;
					const uint8_t tempChord 	=	tempSeqRow.step[ghostedStep].chord;
					// const uint8_t tempMod 	=	tempSeqRow.step[ghostedStep].modulation;
					const uint8_t tempScale 	=	tempSeqRow.trackScale;
					const uint8_t tempRoot 		=	tempSeqRow.rootNote;



					float tempVelo = getLongRollVelo(ghostedRollCurve, rollProgress) * ((float)tempStepVelo / 127);
					tempVelo = (tempVelo / MAX_VELO_TRACK) * seq[player.ramBank].row[x].trackVelo;

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
						player.row[x].note_sent = constrain( (int16_t) tempNote + (int16_t) getNextRollNoteOffset(x), MIN_NOTE_STEP, MAX_NOTE_STEP);
						// Serial.println(player.row[x].note_sent);
						midiSendChordOn(constrain(player.row[x].note_sent, MIN_NOTE_STEP, MAX_NOTE_STEP),
						                tempChord,
						                uint8_t(tempVelo),
						                seq[player.ramBank].row[x].channel,
						                seq[player.ramBank].row[x].midiOut,
						                tempScale,
						                tempRoot);

						anythingSent = 1;

						player.row[x].midiOut_sent = seq[player.ramBank].row[x].midiOut;
						player.row[x].channel_sent = seq[player.ramBank].row[x].channel;
					}

					player.row[x].chord_sent = tempChord;
					player.row[x].scale_sent = tempScale;
					player.row[x].scaleRoot_sent = tempRoot;
					player.row[x].noteOn_sent = 1;


					if ((ghost_rollMode >= 1) && (ghost_rollMode != HITMODE_OFFSET))
					{
						switch (seq[player.ramBank].row[x].gateMode)
						{
						case GATEMODE_NORMAL:
							player.row[x].note_length_timer = (48 / ghost_rollMode) - 2;
							break;

						case GATEMODE_MEDIUM:
							player.row[x].note_length_timer = (48 / ghost_rollMode) / 2;
							break;

						case GATEMODE_SHORT:
							player.row[x].note_length_timer = 1;
							break;

						case GATEMODE_EXTRASHORT:
							player.row[x].note_length_timer = 1;
							break;

						default:
							player.row[x].note_length_timer = (seq[player.ramBank].row[x].step[ghostedStep].length1 + 1) * 48 - 3;
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
				if (debug.player) Serial.println("zaraz...");

				player.isPlay = 1;
				player.uStep = 1;
				for (uint8_t a = 1; a <= 8; a++)
				{
					player.uStepInd[a] = 1;
				}
				//player.metronome_timer = 1;

				player.rec_intro_step = 0;
				player.rec_intro_timer = 0;

				change_grid_view_mode(VIEWMODE_SEQUENCE);

				if (debug.player) Serial.println("nagrywam");

				reset_ruler_blink();
			}
		}



	}
	else if (!player.isREC)
	{
		player.rec_intro_timer = 0;
		player.rec_intro_step = 0;
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


// TODO uzupelnic
void Sequencer::flushNotes(){

}


void Sequencer::divChangeQuantize(uint8_t row)
{
	int8_t tTempoOption = constrain(seq[player.ramBank].row[row].tempoDiv, MIN_TEMPO_DIV, MAX_TEMPO_DIV);
	uint8_t tDiv      = getTempoDiv(tTempoOption);

	if (player.row[row].divChange && ((timerTick % (tDiv * 48)) == 1))
	{
		player.row[row].divChange = 0;
		player.uStepInd[row] = 1;
		switchStep(row);
	}
}



void Sequencer::switchStep(uint8_t row)   //przełączamy stepy w zależności od trybu grania
{
	uint8_t x = constrain(row, 1, 8);





	if (player.isREC && player.row[x].recNoteOpen)
	{
		player.row[x].recNoteLength++;
		seq[player.ramBank].row[x].step[player.row[x].recNoteStep].length1 = player.row[x].recNoteLength - 1;

	}




	if (player.row[x].goToStep)
	{
		player.row[x].actual_pos = player.row[x].goToStep;
		player.row[x].goToStep = 0;
	}
	else if (player.row[x].makeJump)
	{
		for (uint8_t a = 1 ; a <= 8; a++)
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
			if ((player.row[x].actual_pos > seq[player.ramBank].row[x].length) || player.row[x].return2start)
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
				if ((player.row[x].actual_pos >= seq[player.ramBank].row[x].length ) || player.row[x].return2start)
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

		else if (seq[player.ramBank].row[x].playMode == PLAYMODE_RANDOM)
		{
			player.row[x].actual_pos = random(1, seq[player.ramBank].row[x].length + 1);
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


uint8_t Sequencer::val2roll(uint8_t val)
{
	if (val <= sizeof(arrVal2roll))
	{
		return arrVal2roll[val];
	}
	else if (val == HITMODE_OFFSET) return HITMODE_OFFSET; // wyjatek dla nudge //TODO
	else
	{
		return 1;
	}

}
