#include <Arduino.h>

#include "mtSequencer.h"

extern Sequencer sequencer;

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
								seq[player.ramBank].row[a].tempoDiv,
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
							seq[player.ramBank].row[a].tempoDiv, MIN_TEMPO_DIV,
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
	strBank::strTrack & patternRow = seq[player.ramBank].row[row];
	strPlayer::strPlayerTrack & playerRow = player.row[row];

	strBank::strTrack::strStep & patternStep = patternRow.step[playerRow.actual_pos];
//	strPlayer::strPlayerTrack::strPlayerStep & playerStep = playerRow.step[playerRow.actual_pos];

	// TU LICZYMY CZAS DO KONCA NUTY
	if (playerRow.note_length_timer > 0)
		playerRow.note_length_timer--;

	// wyłączamy nutę jeśli się skończyła
	if ((playerRow.note_length_timer <= 1) && (playerRow.noteOn_sent) && !playerRow.recNoteOpen)
	{
		sendNoteOff(row,
					&playerRow.stepSent);
		playerRow.noteOn_sent = 0;
	}

	// wysyłamy zegar
	// TODO: ogarnąć warunek na mniej skomplikowany
	if ((playerRow.uStep > 0) && player.isPlay)
	{
		if (((playerRow.uStep - 1) % 8) == 0)
			send_clock(row);
	}

	if (patternRow.isOn && patternStep.isOn)
	{
		// gasimy jeśli jakaś nuta wisi
		// dodać warunek jeżli przesunięcie
//		if(playerRow.noteOn_sent) sendNoteOff(row, &playerRow.stepSent);

		boolean sendNote = 0;
		if (patternStep.fx[0].type != fx.FX_TYPE_OFFSET &&
				playerRow.uStep == 1)
		{
			sendNote = 1;
			if (playerRow.noteOn_sent) sendNoteOff(row, &playerRow.stepSent);
			sendNoteOn(row, &patternStep);
		}
		else if (patternStep.fx[0].type == fx.FX_TYPE_OFFSET &&
				playerRow.uStep == patternStep.fx[0].value_u16)
		{
			sendNote = 1;
			if (playerRow.noteOn_sent) sendNoteOff(row, &playerRow.stepSent);
			sendNoteOn(row, &patternStep);
		}

		if (sendNote)
		{

			playerRow.noteOn_sent = 1;
			playerRow.note_length_timer = patternStep.length1;
			playerRow.stepSent = patternStep; // buforujemy wysłanego stepa
		}
	}

	/*
	 if (temp_hitMode >= 1 && temp_rowIsOn && temp_stepIsOn)
	 {
	 //

	 // TU GRAMY
	 if (((temp_hitMode != HITMODE_OFFSET) && ((playerRow.uStep % (48 / temp_rollMode)) == 1)) || ((temp_hitMode == HITMODE_OFFSET) && (playerRow.uStep == temp_offset)))
	 {

	 if ((playerRow.uStep % 48) == 1)
	 {
	 playerRow.rollLength = 0;
	 playerRow.rollCounter = 0;
	 // Serial.print("cycki1 ");
	 }
	 // tempPlayerRow.rollLength = 0;

	 const uint8_t tempNote = patternRow.step[playerRow.actual_pos].note;

	 //			const uint8_t tempMod = patternRow.step[playerRow.actual_pos].modulation;
	 const uint8_t tempScale = patternRow.trackScale;
	 const uint8_t tempRoot = patternRow.rootNote;
	 const uint8_t tempLength = patternRow.step[playerRow.actual_pos].length1 + 1;

	 // sprawdzam czy początek rolki
	 if ((temp_hitMode != HITMODE_OFFSET) && (temp_hitMode > 1) && ((playerRow.uStep % 48) == 1))
	 {
	 playerRow.rollLength = tempLength;
	 playerRow.rollStep = playerRow.actual_pos;
	 // Serial.print("cycki2 ");

	 // Serial.print("<");
	 }
	 else if ((temp_hitMode != HITMODE_OFFSET) && (temp_hitMode == 1))
	 {
	 playerRow.rollStep = playerRow.actual_pos;
	 }

	 // procent postępu rolki
	 const float rollProgress = (float(player.row[row].uStep) / (float(
	 tempLength) * 48)) * 100;
	 // Serial.print("progress: ");
	 // Serial.println(rollProgress);

	 float tempVelo =
	 +
	 temp_hitMode > 1 ? getLongRollVelo(
	 temp_rollCurve,
	 rollProgress) * ((float) temp_stepVelo / 127) : temp_stepVelo;

	 tempVelo = (tempVelo / MAX_VELO_TRACK) * seq[player.ramBank].row[x].trackVelo;

	 //wyciszamy jezeli bylo cos wczesniej
	 if (playerRow.noteOn_sent)
	 {

	 // midiSendNoteOff(playerRow.note_sent,
	 //                 0,
	 //                 playerRow.channel_sent,
	 //                 playerRow.midiOut_sent);

	 sendNoteOff(
	 x,
	 &seq[player.ramBank].row[x].step[playerRow.actual_pos]);

	 // usbMIDI.send_now();

	 }
	 //delay(2);

	 //gramy

	 if (playerRow.rollLength > 0)
	 ++playerRow.rollCounter;

	 if (tempNote <= MAX_NOTE_TRACK && !playerRow.divChange)
	 {

	 playerRow.note_sent = constrain(
	 (int16_t ) tempNote + (int16_t ) getNextRollNoteOffset(
	 x),
	 MIN_NOTE_STEP, MAX_NOTE_STEP);
	 // Serial.println(playerRow.note_sent);

	 sendNoteOn(
	 x,
	 &seq[player.ramBank].row[x].step[playerRow.actual_pos]);

	 // usbMIDI.send_now();

	 playerRow.midiOut_sent = seq[player.ramBank].row[x].midiOut;
	 playerRow.channel_sent = seq[player.ramBank].row[x].channel;

	 // jesli wystapila zmiana wysylamy mod
	 //				if (tempMod != NULL_MOD)
	 //				{
	 //					midiSendCC(seq[player.ramBank].row[x].cc, tempMod,
	 //								seq[player.ramBank].row[x].channel,
	 //								seq[player.ramBank].row[x].midiOut);
	 //					// playerRow.lastMod = tempMod;
	 //				}
	 }

	 else if (tempNote == NOTE_JUMPTO)
	 {
	 uint8_t can_i_jump = 1;
	 for (uint8_t a = MINROW; a <= MAXROW; a++)
	 if (player.row[a].makeJump)
	 can_i_jump = 0;

	 if (can_i_jump)
	 {
	 seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 = 0; // moment przed koncem, zeby nie zaświecała kolejna dioda

	 //					playerRow.lastMod = tempMod;  // mod mówi gdzie skaczemy

	 //					setLoadBank2Ram(tempMod);

	 playerRow.makeJump = 1;
	 }

	 // JUMP TO po odliczeniu długości nuty

	 }
	 else if (tempNote == NOTE_JUMPTO_NEXT)
	 {

	 uint8_t can_i_jump = 1;
	 for (uint8_t a = MINROW; a <= MAXROW; a++)
	 if (player.row[a].makeJump)
	 can_i_jump = 0;

	 if (can_i_jump)
	 {
	 seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 = 0; // moment przed koncem, zeby nie zaświecała kolejna dioda

	 // playerRow.lastMod = tempMod;    // mod mówi gdzie skaczemy

	 setLoadBank2Ram(
	 player.actualBank < 255 ? player.actualBank + 1 : 0);

	 playerRow.makeJump = 1;
	 }

	 }

	 playerRow.scale_sent = tempScale;
	 playerRow.scaleRoot_sent = tempRoot;
	 playerRow.noteOn_sent = 1;

	 if ((temp_hitMode > 1) && (temp_hitMode != HITMODE_OFFSET))
	 {
	 switch (seq[player.ramBank].row[x].gateMode)
	 {
	 case GATEMODE.NORMAL:
	 playerRow.note_length_timer = (48 / temp_rollMode) - 2;
	 break;

	 case GATEMODE.MEDIUM:
	 playerRow.note_length_timer = (48 / temp_rollMode) / 2;
	 break;

	 case GATEMODE.SHORT:
	 playerRow.note_length_timer = 1;
	 break;

	 case GATEMODE.EXTRASHORT:
	 playerRow.note_length_timer = 1;
	 break;

	 default:
	 playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 3;
	 }
	 }

	 else
	 {
	 // playerRow.note_length_timer = seq[player.ramBank].row[x].step[playerRow.actual_pos].length * 48 - 3;
	 // if (seq[player.ramBank].row[x].playMode == PLAYMODE_FORWARD) playerRow.note_length_timer = 10;

	 switch (seq[player.ramBank].row[x].gateMode)
	 {
	 case GATEMODE.NORMAL:
	 playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 3;
	 break;

	 case GATEMODE.MEDIUM:
	 playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 24;
	 break;

	 case GATEMODE.SHORT:
	 playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 36;
	 break;

	 case GATEMODE.EXTRASHORT:
	 playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 43;
	 break;

	 default:
	 playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 3;
	 }
	 }

	 if (tempNote == NOTE_JUMPTO)
	 playerRow.note_length_timer -= 1; // zeby diody nastepne po jumpie nie zaswiecily
	 }
	 }

	 else if (playerRow.rollLength > 0)
	 {
	 // pobieramy dane
	 const uint8_t ghostedStep = playerRow.rollStep;
	 // const uint8_t ghost_hitMode 	= seq[player.ramBank].row[x].step[ghostedStep].hitMode;
	 const uint8_t ghost_rollMode = val2roll(
	 seq[player.ramBank].row[x].step[ghostedStep].hitMode);

	 const uint8_t ghost_stepIsOn = val2roll(
	 seq[player.ramBank].row[x].step[ghostedStep].isOn);

	 const uint8_t temp_rowIsOn = seq[player.ramBank].row[x].isOn;
	 const uint8_t ghostedRollCurve = seq[player.ramBank].row[x].step[ghostedStep].rollCurve;
	 const uint8_t ghostedStepLength = seq[player.ramBank].row[x].step[ghostedStep].length1 + 1;

	 if ((player.row[row].uStep % 48) == 1)
	 {
	 --playerRow.rollLength;
	 }

	 if (ghostedStep == 0)
	 {
	 playerRow.rollLength = 0;
	 playerRow.rollCounter = 0;
	 }
	 if (((player.row[row].uStep % (48 / ghost_rollMode)) == 1) && playerRow.rollLength)
	 {
	 if (ghost_stepIsOn >= 1 && temp_rowIsOn)
	 {

	 if (playerRow.rollLength > 0)
	 ++playerRow.rollCounter;

	 // procent postępu rolki
	 const float rollProgress = (float(
	 (ghostedStepLength - playerRow.rollLength) * 48 + player.row[row].uStep) / (float(
	 ghostedStepLength) * 48)) * 100;

	 // TU GRAMY
	 if (((ghost_rollMode != HITMODE_OFFSET) && ((player.row[row].uStep % (48 / ghost_rollMode)) == 1)) || ((ghost_rollMode == HITMODE_OFFSET) && (player.row[row].uStep == temp_offset)))
	 {
	 const uint8_t tempNote = patternRow.step[ghostedStep].note;
	 const uint8_t tempStepVelo = patternRow.step[ghostedStep].velocity;
	 //					const uint8_t tempChord = tempSeqRow.step[ghostedStep].chord;
	 // const uint8_t tempMod 	=	tempSeqRow.step[ghostedStep].modulation;
	 const uint8_t tempScale = patternRow.trackScale;
	 const uint8_t tempRoot = patternRow.rootNote;

	 float tempVelo = getLongRollVelo(ghostedRollCurve,
	 rollProgress) * ((float) tempStepVelo / 127);
	 tempVelo = (tempVelo / MAX_VELO_TRACK) * seq[player.ramBank].row[x].trackVelo;

	 //wyciszamy jezeli bylo cos wczesniej
	 if (playerRow.noteOn_sent)
	 {
	 sendNoteOn(
	 x,
	 &seq[player.ramBank].row[x].step[playerRow.actual_pos]);
	 }
	 //gramy
	 if (tempNote <= MAX_NOTE_TRACK)
	 {
	 playerRow.note_sent = constrain(
	 (int16_t ) tempNote + (int16_t ) getNextRollNoteOffset(
	 x),
	 MIN_NOTE_STEP, MAX_NOTE_STEP);
	 // Serial.println(playerRow.note_sent);
	 sendNoteOn(
	 x,
	 &seq[player.ramBank].row[x].step[playerRow.actual_pos]);

	 playerRow.midiOut_sent = seq[player.ramBank].row[x].midiOut;
	 playerRow.channel_sent = seq[player.ramBank].row[x].channel;
	 }

	 //					playerRow.chord_sent = tempChord;
	 playerRow.scale_sent = tempScale;
	 playerRow.scaleRoot_sent = tempRoot;
	 playerRow.noteOn_sent = 1;

	 if ((ghost_rollMode >= 1) && (ghost_rollMode != HITMODE_OFFSET))
	 {
	 switch (seq[player.ramBank].row[x].gateMode)
	 {
	 case GATEMODE.NORMAL:
	 playerRow.note_length_timer = (48 / ghost_rollMode) - 2;
	 break;

	 case GATEMODE.MEDIUM:
	 playerRow.note_length_timer = (48 / ghost_rollMode) / 2;
	 break;

	 case GATEMODE.SHORT:
	 playerRow.note_length_timer = 1;
	 break;

	 case GATEMODE.EXTRASHORT:
	 playerRow.note_length_timer = 1;
	 break;

	 default:
	 playerRow.note_length_timer = (seq[player.ramBank].row[x].step[ghostedStep].length1 + 1) * 48 - 3;
	 }
	 }
	 }
	 }

	 // wyłączamy przedłużenie rolki
	 if ((playerRow.rollLength == 1) && (player.row[row].uStep == 48))
	 {
	 playerRow.rollLength = 0;
	 playerRow.rollCounter = 0;
	 // Serial.println(">");
	 }
	 }
	 else
	 {

	 }
	 }
	 */
}

//
//void Sequencer::play_microStep_kopia(uint8_t row)
//{
//
////	uint8_t x = constrain(row, MINROW, MAXROW);
//	if (row > MAXROW)
//		return;
//
//	strBank::strTrack & patternRow = seq[player.ramBank].row[x];
//	strPlayer::strPlayerTrack & playerRow = player.row[x];
//
////	strBank::strRow::strStep & patternStep = patternRow.step[]
//
//	// TU LICZYMY CZAS DO KONCA NUTY
//	if (playerRow.note_length_timer > 0)
//	{
//		playerRow.note_length_timer--;
//		if (playerRow.note_length_timer < 0)
//			playerRow.note_length_timer = 0; //bzdura ale huj, zostawie
//	}
//
//	// wyłączamy nutę jeśli się skończyła
//	if ((playerRow.note_length_timer <= 1) && (playerRow.noteOn_sent) && !playerRow.recNoteOpen)
//	{
//		// GASIMY
//		if (playerRow.note_sent <= 127)
//		{
//			// Serial.println("gasimy");
//
//			/*
//			 sendNoteOff(
//			 x,
//			 playerRow.note_sent);*/
//
//		}
//
//		playerRow.noteOn_sent = 0;
//	}
//
//	// wysyłamy zegar
//	if ((playerRow.uStep > 0) && player.isPlay)
//	{
//		if (((playerRow.uStep - 1) % 8) == 0)
//		{
//			send_clock (x);
//		}
//	}
//
//	const uint8_t temp_hitMode = seq[player.ramBank].row[x].step[playerRow.actual_pos].hitMode;
//	const uint8_t temp_rowIsOn = seq[player.ramBank].row[x].isOn;
//	const uint8_t temp_stepIsOn = seq[player.ramBank].row[x].step[playerRow.actual_pos].isOn;
//
//	const uint8_t temp_rollMode = val2roll(
//			seq[player.ramBank].row[x].step[playerRow.actual_pos].hitMode);
//	const uint8_t temp_rollCurve = seq[player.ramBank].row[x].step[playerRow.actual_pos].rollCurve;
//	const uint8_t temp_offset = seq[player.ramBank].row[x].step[playerRow.actual_pos].offset;
//	const uint8_t temp_stepVelo = seq[player.ramBank].row[x].step[playerRow.actual_pos].velocity;
//
//	if (temp_hitMode >= 1 && temp_rowIsOn && temp_stepIsOn)
//	{
//		//
//
//		// TU GRAMY
//		if (((temp_hitMode != HITMODE_OFFSET) && ((playerRow.uStep % (48 / temp_rollMode)) == 1)) || ((temp_hitMode == HITMODE_OFFSET) && (playerRow.uStep == temp_offset)))
//		{
//
//			if ((playerRow.uStep % 48) == 1)
//			{
//				playerRow.rollLength = 0;
//				playerRow.rollCounter = 0;
//				// Serial.print("cycki1 ");
//			}
//			// tempPlayerRow.rollLength = 0;
//
//			const uint8_t tempNote = patternRow.step[playerRow.actual_pos].note;
//
////			const uint8_t tempMod = patternRow.step[playerRow.actual_pos].modulation;
//			const uint8_t tempScale = patternRow.trackScale;
//			const uint8_t tempRoot = patternRow.rootNote;
//			const uint8_t tempLength = patternRow.step[playerRow.actual_pos].length1 + 1;
//
//			// sprawdzam czy początek rolki
//			if ((temp_hitMode != HITMODE_OFFSET) && (temp_hitMode > 1) && ((playerRow.uStep % 48) == 1))
//			{
//				playerRow.rollLength = tempLength;
//				playerRow.rollStep = playerRow.actual_pos;
//				// Serial.print("cycki2 ");
//
//				// Serial.print("<");
//			}
//			else if ((temp_hitMode != HITMODE_OFFSET) && (temp_hitMode == 1))
//			{
//				playerRow.rollStep = playerRow.actual_pos;
//			}
//
//			// procent postępu rolki
//			const float rollProgress = (float(player.row[row].uStep) / (float(
//					tempLength) * 48)) * 100;
//			// Serial.print("progress: ");
//			// Serial.println(rollProgress);
//
//			float tempVelo =
//					+
//							temp_hitMode > 1 ? getLongRollVelo(temp_rollCurve,
//																rollProgress) * ((float) temp_stepVelo / 127) : temp_stepVelo;
//
//			tempVelo = (tempVelo / MAX_VELO_TRACK) * seq[player.ramBank].row[x].trackVelo;
//
//			//wyciszamy jezeli bylo cos wczesniej
//			if (playerRow.noteOn_sent)
//			{
//
//				// midiSendNoteOff(playerRow.note_sent,
//				//                 0,
//				//                 playerRow.channel_sent,
//				//                 playerRow.midiOut_sent);
//
//				sendNoteOff(
//						x,
//						&seq[player.ramBank].row[x].step[playerRow.actual_pos]);
//
//				// usbMIDI.send_now();
//
//			}
//			//delay(2);
//
//			//gramy
//
//			if (playerRow.rollLength > 0)
//				++playerRow.rollCounter;
//
//			if (tempNote <= MAX_NOTE_TRACK && !playerRow.divChange)
//			{
//
//				playerRow.note_sent = constrain(
//						(int16_t ) tempNote + (int16_t ) getNextRollNoteOffset(
//								x),
//						MIN_NOTE_STEP, MAX_NOTE_STEP);
//				// Serial.println(playerRow.note_sent);
//
//				sendNoteOn(
//						x,
//						&seq[player.ramBank].row[x].step[playerRow.actual_pos]);
//
//				// usbMIDI.send_now();
//
//				playerRow.midiOut_sent = seq[player.ramBank].row[x].midiOut;
//				playerRow.channel_sent = seq[player.ramBank].row[x].channel;
//
//				// jesli wystapila zmiana wysylamy mod
////				if (tempMod != NULL_MOD)
////				{
////					midiSendCC(seq[player.ramBank].row[x].cc, tempMod,
////								seq[player.ramBank].row[x].channel,
////								seq[player.ramBank].row[x].midiOut);
////					// playerRow.lastMod = tempMod;
////				}
//			}
//
//			else if (tempNote == NOTE_JUMPTO)
//			{
//				uint8_t can_i_jump = 1;
//				for (uint8_t a = MINROW; a <= MAXROW; a++)
//					if (player.row[a].makeJump)
//						can_i_jump = 0;
//
//				if (can_i_jump)
//				{
//					seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 = 0; // moment przed koncem, zeby nie zaświecała kolejna dioda
//
////					playerRow.lastMod = tempMod;  // mod mówi gdzie skaczemy
//
////					setLoadBank2Ram(tempMod);
//
//					playerRow.makeJump = 1;
//				}
//
//				// JUMP TO po odliczeniu długości nuty
//
//			}
//			else if (tempNote == NOTE_JUMPTO_NEXT)
//			{
//
//				uint8_t can_i_jump = 1;
//				for (uint8_t a = MINROW; a <= MAXROW; a++)
//					if (player.row[a].makeJump)
//						can_i_jump = 0;
//
//				if (can_i_jump)
//				{
//					seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 = 0; // moment przed koncem, zeby nie zaświecała kolejna dioda
//
//					// playerRow.lastMod = tempMod;    // mod mówi gdzie skaczemy
//
//					setLoadBank2Ram(
//							player.actualBank < 255 ? player.actualBank + 1 : 0);
//
//					playerRow.makeJump = 1;
//				}
//
//			}
//
//			playerRow.scale_sent = tempScale;
//			playerRow.scaleRoot_sent = tempRoot;
//			playerRow.noteOn_sent = 1;
//
//			if ((temp_hitMode > 1) && (temp_hitMode != HITMODE_OFFSET))
//			{
//				switch (seq[player.ramBank].row[x].gateMode)
//				{
//				case GATEMODE.NORMAL:
//					playerRow.note_length_timer = (48 / temp_rollMode) - 2;
//					break;
//
//				case GATEMODE.MEDIUM:
//					playerRow.note_length_timer = (48 / temp_rollMode) / 2;
//					break;
//
//				case GATEMODE.SHORT:
//					playerRow.note_length_timer = 1;
//					break;
//
//				case GATEMODE.EXTRASHORT:
//					playerRow.note_length_timer = 1;
//					break;
//
//				default:
//					playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 3;
//				}
//			}
//
//			else
//			{
//				// playerRow.note_length_timer = seq[player.ramBank].row[x].step[playerRow.actual_pos].length * 48 - 3;
//				// if (seq[player.ramBank].row[x].playMode == PLAYMODE_FORWARD) playerRow.note_length_timer = 10;
//
//				switch (seq[player.ramBank].row[x].gateMode)
//				{
//				case GATEMODE.NORMAL:
//					playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 3;
//					break;
//
//				case GATEMODE.MEDIUM:
//					playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 24;
//					break;
//
//				case GATEMODE.SHORT:
//					playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 36;
//					break;
//
//				case GATEMODE.EXTRASHORT:
//					playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 43;
//					break;
//
//				default:
//					playerRow.note_length_timer = (seq[player.ramBank].row[x].step[playerRow.actual_pos].length1 + 1) * 48 - 3;
//				}
//			}
//
//			if (tempNote == NOTE_JUMPTO)
//				playerRow.note_length_timer -= 1; // zeby diody nastepne po jumpie nie zaswiecily
//		}
//	}
//
//	else if (playerRow.rollLength > 0)
//	{
//		// pobieramy dane
//		const uint8_t ghostedStep = playerRow.rollStep;
//		// const uint8_t ghost_hitMode 	= seq[player.ramBank].row[x].step[ghostedStep].hitMode;
//		const uint8_t ghost_rollMode = val2roll(
//				seq[player.ramBank].row[x].step[ghostedStep].hitMode);
//
//		const uint8_t ghost_stepIsOn = val2roll(
//				seq[player.ramBank].row[x].step[ghostedStep].isOn);
//
//		const uint8_t temp_rowIsOn = seq[player.ramBank].row[x].isOn;
//		const uint8_t ghostedRollCurve = seq[player.ramBank].row[x].step[ghostedStep].rollCurve;
//		const uint8_t ghostedStepLength = seq[player.ramBank].row[x].step[ghostedStep].length1 + 1;
//
//		if ((player.row[row].uStep % 48) == 1)
//		{
//			--playerRow.rollLength;
//		}
//
//		if (ghostedStep == 0)
//		{
//			playerRow.rollLength = 0;
//			playerRow.rollCounter = 0;
//		}
//		if (((player.row[row].uStep % (48 / ghost_rollMode)) == 1) && playerRow.rollLength)
//		{
//			if (ghost_stepIsOn >= 1 && temp_rowIsOn)
//			{
//
//				if (playerRow.rollLength > 0)
//					++playerRow.rollCounter;
//
//				// procent postępu rolki
//				const float rollProgress = (float(
//						(ghostedStepLength - playerRow.rollLength) * 48 + player.row[row].uStep) / (float(
//						ghostedStepLength) * 48)) * 100;
//
//				// TU GRAMY
//				if (((ghost_rollMode != HITMODE_OFFSET) && ((player.row[row].uStep % (48 / ghost_rollMode)) == 1)) || ((ghost_rollMode == HITMODE_OFFSET) && (player.row[row].uStep == temp_offset)))
//				{
//					const uint8_t tempNote = patternRow.step[ghostedStep].note;
//					const uint8_t tempStepVelo = patternRow.step[ghostedStep].velocity;
////					const uint8_t tempChord = tempSeqRow.step[ghostedStep].chord;
//					// const uint8_t tempMod 	=	tempSeqRow.step[ghostedStep].modulation;
//					const uint8_t tempScale = patternRow.trackScale;
//					const uint8_t tempRoot = patternRow.rootNote;
//
//					float tempVelo = getLongRollVelo(ghostedRollCurve,
//														rollProgress) * ((float) tempStepVelo / 127);
//					tempVelo = (tempVelo / MAX_VELO_TRACK) * seq[player.ramBank].row[x].trackVelo;
//
//					//wyciszamy jezeli bylo cos wczesniej
//					if (playerRow.noteOn_sent)
//					{
//						sendNoteOn(
//								x,
//								&seq[player.ramBank].row[x].step[playerRow.actual_pos]);
//					}
//					//gramy
//					if (tempNote <= MAX_NOTE_TRACK)
//					{
//						playerRow.note_sent = constrain(
//								(int16_t ) tempNote + (int16_t ) getNextRollNoteOffset(
//										x),
//								MIN_NOTE_STEP, MAX_NOTE_STEP);
//						// Serial.println(playerRow.note_sent);
//						sendNoteOn(
//								x,
//								&seq[player.ramBank].row[x].step[playerRow.actual_pos]);
//
//						playerRow.midiOut_sent = seq[player.ramBank].row[x].midiOut;
//						playerRow.channel_sent = seq[player.ramBank].row[x].channel;
//					}
//
////					playerRow.chord_sent = tempChord;
//					playerRow.scale_sent = tempScale;
//					playerRow.scaleRoot_sent = tempRoot;
//					playerRow.noteOn_sent = 1;
//
//					if ((ghost_rollMode >= 1) && (ghost_rollMode != HITMODE_OFFSET))
//					{
//						switch (seq[player.ramBank].row[x].gateMode)
//						{
//						case GATEMODE.NORMAL:
//							playerRow.note_length_timer = (48 / ghost_rollMode) - 2;
//							break;
//
//						case GATEMODE.MEDIUM:
//							playerRow.note_length_timer = (48 / ghost_rollMode) / 2;
//							break;
//
//						case GATEMODE.SHORT:
//							playerRow.note_length_timer = 1;
//							break;
//
//						case GATEMODE.EXTRASHORT:
//							playerRow.note_length_timer = 1;
//							break;
//
//						default:
//							playerRow.note_length_timer = (seq[player.ramBank].row[x].step[ghostedStep].length1 + 1) * 48 - 3;
//						}
//					}
//				}
//			}
//
//			// wyłączamy przedłużenie rolki
//			if ((playerRow.rollLength == 1) && (player.row[row].uStep == 48))
//			{
//				playerRow.rollLength = 0;
//				playerRow.rollCounter = 0;
//				// Serial.println(">");
//			}
//		}
//		else
//		{
//
//		}
//	}
//}

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
	for (int8_t col = 1; col <= seq[player.ramBank].row[ghost.cnt1].length;
			col++)
	{
		int8_t motherGhost = col; // numer stepa który jest matką ghostów
		uint8_t temp_row_lenght = seq[player.ramBank].row[ghost.cnt1].length;
//		uint8_t temp_hitMode = seq[player.ramBank].row[ghost.cnt1].step[col].hitMode;
		uint8_t temp_isOn = seq[player.ramBank].row[ghost.cnt1].step[col].isOn;

		uint8_t temp_length = seq[player.ramBank].row[ghost.cnt1].step[col].length1 + 1;

		if (temp_length > 1 && temp_isOn)
		{
			while (temp_length > 1 && !seq[player.ramBank].row[ghost.cnt1].step[col + 1 + ((col + 1 > temp_row_lenght) * -temp_row_lenght)].isOn)
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

void Sequencer::send_allNotesOff(void)
{
//	for (uint8_t row = 1; row <= MAXROW; row++)
//	{
//		midiSendCC( seq[player.ramBank].row[row].channel,
//		            CC_ALL_NOTES_OFF,
//		            CC_VAL_NOTES_OFF,
//		            seq[player.ramBank].row[row].midiOut);
//	}
}

void Sequencer::stop(void)
{

	if (player.isStop)
	{
		send_allNotesOff();
	}

//	send_stop();

	player.isPlay = 0;
	player.isStop = 1;
	player.isREC = 0;
	player.uStep = 0;

	nanoStep = 1;

	for (uint8_t a = MINROW; a <= MAXROW; a++)
	{
		player.row[a].uStep = 0;
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
	for (uint8_t y = MINROW; y <= MAXROW; y++)
	{
		player.row[y].lastMod = 128;
	}
}

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
		seq[bank].row[row].length = DEFAULT_ROW_LEN;
		seq[bank].row[row].rootNote = DEFAULT_ROW_NOTE;
		seq[bank].row[row].trackVelo = MAX_VELO_TRACK;
		seq[bank].row[row].defaultMod = DEFAULT_MOD;
		seq[bank].row[row].channel = DEFAULT_ROW_CHANNEL;
		seq[bank].row[row].cc = DEFAULT_CC;
//		seq[bank].row[row].flags = 1;
		seq[bank].row[row].isOn = 1;
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

	clearStep(x, row, player.ramBank);
}

void Sequencer::clearStep(uint8_t x, uint8_t row, uint8_t bank)
{
	strBank::strTrack & tempRow = seq[bank].row[row];
	strBank::strTrack::strStep & step = tempRow.step[x];

//	step.hitMode = 1;
	step.isOn = 0;
//	step.chord = MIN_CHORD;
	step.velocity = MAX_VELO_STEP;
//	step.modulation = DEFAULT_MOD;
	step.rollCurve = MIN_STEP_ROLL_VAR;

	step.rollNoteCurve = MIN_STEP_ROLL_NOTE_VAR;
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
	return seq[player.ramBank].row[row].isOn;
}



void Sequencer::loadDefaultSequence(void)
{
	for (uint8_t x = MINROW; x <= MAXROW; x++)
	{
		seq[player.ramBank].row[x].rootNote = 35 + x;
		seq[player.ramBank].row[x].channel = x;
		seq[player.ramBank].row[x].isOn = 1;

		seq[player.ramBank].row[x].length = 16;
		for (uint8_t y = MINSTEP; y <= MAXSTEP; y++)
		{
			seq[player.ramBank].row[x].step[y].isOn = 0;
		}
	}

	seq[player.ramBank].row[0].step[0].isOn = 1;
//	seq[player.ramBank].row[0].step[0].hitMode = 1;
	seq[player.ramBank].row[0].step[0].note = 42;
	seq[player.ramBank].row[0].step[0].length1 = 10;

	seq[player.ramBank].row[0].step[1].isOn = 1;
//	seq[player.ramBank].row[0].step[1].hitMode = 1;
	seq[player.ramBank].row[0].step[1].note = 45;
	seq[player.ramBank].row[0].step[1].length1 = 30;

	seq[player.ramBank].row[0].step[2].isOn = 1;
//	seq[player.ramBank].row[0].step[2].hitMode = 1;
	seq[player.ramBank].row[0].step[2].note = 46;
	seq[player.ramBank].row[0].step[2].length1 = 30;
	seq[player.ramBank].row[0].step[2].fx[0].isOn = 1;
	seq[player.ramBank].row[0].step[2].fx[0].type = fx.FX_TYPE_OFFSET;
	seq[player.ramBank].row[0].step[2].fx[0].value_u16 = 10;

}



void Sequencer::allNoteOffs(void)
{
	for (uint8_t a = MINROW; a <= MAXROW; a++)
	{
		if (player.row[a].noteOn_sent)
		{
//			// TODO
//			midiSendChordOff

			player.row[a].noteOn_sent = 0;
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
		seq[player.ramBank].row[x].step[player.row[x].recNoteStep].length1 = player.row[x].recNoteLength - 1;

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
				if ((player.row[x].actual_pos >= seq[player.ramBank].row[x].length) || player.row[x].return2start)
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
	for (uint8_t row = MINROW; row <= MAXROW; row++)
	{
		player.row[row].pingPongToogle = 0;
		reset_actual_pos(row);
	}
}

void Sequencer::reset_actual_pos(uint8_t row)
{

	if (seq[player.ramBank].row[row].playMode == PLAYMODE_FORWARD)
	{
		player.row[row].actual_pos = MINSTEP;
	}

	else if (seq[player.ramBank].row[row].playMode == PLAYMODE_BACKWARD)
	{
		player.row[row].actual_pos = seq[player.ramBank].row[row].length;
	}

	else if (seq[player.ramBank].row[row].playMode == PLAYMODE_PINGPONG)
	{
		player.row[row].actual_pos = MINSTEP;
	}

}

int8_t Sequencer::getLastRollNoteOffset(uint8_t row)
{
	return player.row[row].lastRollNote;
}


int8_t Sequencer::getNextRollNoteOffset(uint8_t row)
{

	int8_t & retVal = player.row[row].lastRollNote;

//	if (player.row[row].rollStep == 0)
//		return 0;
//
//	strBank::strTrack::strStep & step = seq[player.ramBank].row[row].step[player.row[row].rollStep];
//
//	if (step.rollNoteCurve == ROLL_CURVE.FLAT)
//	return 0;
//	else if (step.rollNoteCurve == ROLL_CURVE.INCREMENTAL)
//	return player.row[row].rollCounter - 1;
//	else if (step.rollNoteCurve == ROLL_CURVE.DECREMENTAL)
//	return -player.row[row].rollCounter + 1;
//
//	else if (step.rollNoteCurve == ROLL_CURVE.INC_DEC)
//	{
//		uint8_t total = (step.length1 + 1) * val2roll(step.hitMode);
//
//		if (player.row[row].rollCounter < ((total / 2) + 1))
//		{
//			retVal = player.row[row].rollCounter - 1;
//		}
//		else
//		{
//			retVal = total - player.row[row].rollCounter;
//		}
//	}
//
//	else if (step.rollNoteCurve == ROLL_CURVE.DEC_INC)
//	{
//		uint8_t total = (step.length1 + 1) * val2roll(step.hitMode);
//
//		if (player.row[row].rollCounter < ((total / 2) + 1))
//		{
//			retVal = -player.row[row].rollCounter + 1;
//		}
//		else
//		{
//			retVal = -(total - player.row[row].rollCounter + 1) + 1;
//		}
//
//	}
//
//	else if (step.rollNoteCurve == ROLL_CURVE.RANDOM)
//	{
//	}

	retVal = random(-6, 6);
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
	int8_t tTempoOption = constrain(seq[player.ramBank].row[row].tempoDiv,
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




void Sequencer::copy_step(uint8_t from_step, uint8_t from_track,
							uint8_t to_step,
							uint8_t to_track)
{
	from_step = constrain(from_step, MINSTEP, MAXSTEP);
	from_track = constrain(from_track, MINROW, MAXROW);

	to_step = constrain(to_step, MINSTEP, MAXSTEP);
	to_track = constrain(to_track, MINROW, MAXROW);

	seq[player.ramBank].row[to_track].step[to_step] = seq[player.ramBank].row[from_track].step[from_step];
}

void Sequencer::copy_row(uint8_t from, uint8_t to)
{
	from = constrain(from, MINROW, MAXROW);
	to = constrain(to, MINROW, MAXROW);
	seq[player.ramBank].row[to] = seq[player.ramBank].row[from];
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
	seq[player.ramBank].row[row].step[step].isOn = !seq[player.ramBank].row[row].step[step].isOn;
	seq[player.ramBank].row[row].step[step].note = DEFAULT_ROW_NOTE;
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
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_ARP_UP:
		return fx.FX_VAL_TYPE_U16;
		break;

	case fx.FX_TYPE_ARP_DOWN:
		return fx.FX_VAL_TYPE_U16;
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

	case fx.FX_TYPE_VOL_ROLL:
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
		return fx.FX_VAL_TYPE_U16;
		break;
	}
}

void Sequencer::sendNoteOn(uint8_t track, strBank::strTrack::strStep *step)
{
	Serial.printf("track %d\nnoteOn:\t%d\nvelo:\t%d\ninstr:\t%d\n\n",
					track,
					step->note,
					step->velocity,
					step->instrument);

	usbMIDI.sendNoteOn(step->note, step->velocity, 1);

}
void Sequencer::sendNoteOff(uint8_t track, strBank::strTrack::strStep *step)
{
	Serial.printf("\ttrack %d\n\tnoteOff:\t%d\n\tvelo:\t%d\n\tinstr:\t%d\n\n",
					track,
					step->note,
					step->velocity,
					step->instrument);

	usbMIDI.sendNoteOff(step->note, 0, 1);

}
