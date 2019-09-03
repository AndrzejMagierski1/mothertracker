#include <Arduino.h>

#include "mtSequencer.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"
#include "mtFileManager.h"

#include "patternEditor.h"
extern Sequencer sequencer;

void fromToSwap(uint8_t & from, uint8_t & to)
{
	if (to < from)
	{
		uint8_t buff;
		buff = from;
		from = to;
		to = buff;
	}
}

void Sequencer::fillRandomNotes(uint8_t fillStep, uint8_t from, uint8_t to)
{
	fromToSwap(from, to);

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			if (offset % fillStep == 0)
			{
				step = &seq[player.ramBank].track[t].step[s];
				step->note = random(from, to + 1);
				step->instrument = mtProject.values.lastUsedInstrument;
			}
		}
	}
}
void Sequencer::fillLinearNotes(uint8_t fillStep, uint8_t from, uint8_t to)
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			if (offset % fillStep == 0)
			{
				step = &seq[player.ramBank].track[t].step[s];
				step->note = map(offset + sel->firstStep,
									sel->firstStep,
									sel->lastStep,
									from,
									to);
				step->instrument = mtProject.values.lastUsedInstrument;
			}
		}
	}
}
void Sequencer::fillLinearInstruments(uint8_t fillStep, uint8_t from,
										uint8_t to)
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			if (offset % fillStep == 0)
			{
				step = &seq[player.ramBank].track[t].step[s];
				step->instrument = map(offset + sel->firstStep,
										sel->firstStep,
										sel->lastStep,
										from,
										to);
				if (step->note == STEP_NOTE_EMPTY)
				{
					step->note = STEP_NOTE_DEFAULT;
				}

			}
		}
	}
}

void Sequencer::fillRandomInstruments(uint8_t fillStep, uint8_t from,
										uint8_t to)
{
	fromToSwap(from, to);

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			if (offset % fillStep == 0)
			{
				step = &seq[player.ramBank].track[t].step[s];
				step->instrument = random(from, to + 1);

				if (step->note == STEP_NOTE_EMPTY)
				{
					step->note = STEP_NOTE_DEFAULT;
				}
			}
		}
	}
}

void Sequencer::fillLinearVelocity(uint8_t fillStep, uint8_t from,
									uint8_t to)
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			if (offset % fillStep == 0)
			{
				step = &seq[player.ramBank].track[t].step[s];
				step->velocity = map(offset + sel->firstStep,
										sel->firstStep,
										sel->lastStep,
										from,
										to);

			}
		}
	}
}

void Sequencer::fillRandomVelocity(uint8_t fillStep, uint8_t from,
									uint8_t to)
{
	fromToSwap(from, to);
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			if (offset % fillStep == 0)
			{
				step = &seq[player.ramBank].track[t].step[s];
				step->velocity = random(from, to + 1);

			}
		}
	}
}

void Sequencer::randomSelectedNotes(uint8_t from, uint8_t to, uint8_t scale)
{
	fromToSwap(from, to);

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];
			if (step->note >= 0)
			{
				step->note = random(from, to + 1);
			}
		}
	}
}

void Sequencer::randomSelectedInstruments(uint8_t from, uint8_t to)
{
	fromToSwap(from, to);

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];
			if (step->note >= 0)
			{
				step->instrument = random(from, to + 1);
			}
		}
	}
}
void Sequencer::randomSelectedVelo(uint8_t from, uint8_t to)
{
	fromToSwap(from, to);

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];
			if (step->velocity >= 0 || step->note >= 0)
			{
				step->velocity = random(from, to + 1);
			}
		}
	}
}
void Sequencer::invertSelectedSteps()
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *stepA;
	strPattern::strTrack::strStep *stepB;
	strPattern::strTrack::strStep buffStep;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t a = sel->firstStep, b = sel->lastStep;
				a <= (sel->lastStep - sel->firstStep) / 2;
				a++, b--)
		{
			stepA = &seq[player.ramBank].track[t].step[a];
			stepB = &seq[player.ramBank].track[t].step[b];
			buffStep = *stepA;
			*stepA = *stepB;
			*stepB = buffStep;
		}
	}

}
void Sequencer::loadDefaultTrack(uint8_t row, uint8_t bank)
{

	for (uint8_t x = 0; x <= MAXSTEP; x++)
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

void Sequencer::changeSelectionNote(int16_t value)
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			if (isSingleSelection(sel))
			{
				if (step->note == STEP_NOTE_EMPTY && value > 0)
				{
					step->instrument = mtProject.values.lastUsedInstrument;
					step->note = STEP_NOTE_DEFAULT;
				}
				else if (step->note == STEP_NOTE_EMPTY)
				{
					step->instrument = mtProject.values.lastUsedInstrument;
					step->note = constrain(step->note + value,
											-2,
											MAX_NOTE_STEP);
				}
				else
				{
					step->note = constrain(step->note + value,
											-2,
											MAX_NOTE_STEP);
				}

				if (step->note >= 0)
				{
					blinkNote(step->instrument,
								step->note,
								step->velocity,
								t);
				}
				return;
			}
			else
			{
				if (step->note >= 0)
				{
					step->note = constrain(step->note + value,
											0,
											MAX_NOTE_STEP);
				}
			}

		}
	}
}

void Sequencer::changeSelectionVolume(int16_t value)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];
			step->velocity = constrain(step->velocity + value, 0,
										MAX_VELO_STEP);
		}
	}
}

void Sequencer::changeSelectionInstrument(int16_t value)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			if (isSingleSelection(sel))
			{
				if (step->note >= 0)
				{
					step->instrument = constrain(step->instrument + value, 0,
													INSTRUMENTS_COUNT);

					blinkNote(step->instrument,
								step->note,
								step->velocity,
								t);

					mtProject.values.lastUsedInstrument = step->instrument;
				}
				else if (step->note == STEP_NOTE_EMPTY)
				{
					step->note = STEP_NOTE_DEFAULT;
					step->instrument = mtProject.values.lastUsedInstrument;
				}

				return;
			}
			else
			{
				if (step->note >= 0)
				{
					step->instrument = constrain(step->instrument + value, 0,
													INSTRUMENTS_COUNT);

				}

			}

		}
	}
}
void Sequencer::setSelectionInstrument(int16_t value)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			if (isSingleSelection(sel))
			{
				if (step->note == STEP_NOTE_EMPTY)
				{
					step->note = STEP_NOTE_DEFAULT;
				}

				if (step->note >= 0)
				{
					step->instrument = value;

					blinkNote(step->instrument,
								step->note,
								step->velocity,
								t);

					mtProject.values.lastUsedInstrument = step->instrument;
				}

				return;
			}
			else
			{
				if (step->note >= 0)
				{
					step->instrument = value;

				}
			}
		}
	}
}
void Sequencer::setSelectionVelocity(int16_t value)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			if (isSingleSelection(sel))
			{
				step->velocity = value;

				if (step->note >= 0)
				{
					blinkNote(step->instrument,
								step->note,
								step->velocity,
								t);
				}
//				else if (step->note == STEP_NOTE_EMPTY)
//				{
//					step->note = STEP_NOTE_DEFAULT;
//					step->instrument = mtProject.values.lastUsedInstrument;
//				}
				return;
			}
			else
			{
				if (step->note >= 0)
				{
					step->velocity = value;

				}
			}
		}
	}
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

void Sequencer::clearRow(uint8_t row, uint8_t bank)
{
// TODO: ograniczyć ba
	for (uint8_t x = MINSTEP; x <= MAXSTEP; x++)
	{
		clearStep(x, row, bank);
	}
}

void Sequencer::clearRow(uint8_t row)
{
	clearRow(row, player.ramBank);
}

void Sequencer::clearStep(uint8_t x, uint8_t row)
{
	clearStep(x, row, player.ramBank);
}

void Sequencer::clearStep(uint8_t x, uint8_t row, uint8_t bank)
{
	strPattern::strTrack * tempRow = &seq[bank].track[row];
	strPattern::strTrack::strStep * step = &tempRow->step[x];

	clearStep(step);
}

void Sequencer::clearStep(strPattern::strTrack::strStep * step)
{
//	step->isOn = 0;
	step->velocity = MAX_VELO_STEP;

	step->note = STEP_NOTE_EMPTY;
	step->instrument = 0;
	step->velocity = -1;
	step->fx[0].isOn = 0;

}

void Sequencer::clearSelected()
{
	clearSelected(&selection);
}
void Sequencer::clearSelected(strSelection * selection)
{
	if (!isSelectionCorrect(selection)) return;

	for (uint8_t t = selection->firstTrack; t <= selection->lastTrack; t++)
	{
		for (uint8_t s = selection->firstStep; s <= selection->lastStep; s++)
		{
			clearStep(&seq[player.ramBank].track[t].step[s]);
		}
	}
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

	for (uint8_t row = MINTRACK; row <= MAXTRACK; row++)
	{
		loadDefaultTrack(row, bank);
	}

}

void Sequencer::initPattern(uint8_t pattern) // czyści pattern z flasha
{
// czyścimy bank 3
	loadDefaultBank(pattern);

//	if (pattern == player.actualBank)
//	{
//		loadDefaultBank(1);
//		loadDefaultBank(0);
//	}

// flashuje pattern ramem z 3 pozycji
// 	TODO
//	flash_bank(pattern, 3);
}

void Sequencer::insert(strSelection *selection)
{
	if (!isSelectionCorrect(selection)) return;

	for (uint8_t t = selection->firstTrack; t <= selection->lastTrack; t++)
	{
		for (uint8_t s = MAXSTEP; s > selection->firstStep; s--)
		{
			seq[player.ramBank].track[t].step[s] = seq[player.ramBank].track[t].step[s - 1];
		}
		clearStep(&seq[player.ramBank].track[t].step[selection->firstStep]);
	}

}

void Sequencer::insertReversed(strSelection *selection)
{
	if (!isSelectionCorrect(selection)) return;

	for (uint8_t t = selection->firstTrack; t <= selection->lastTrack; t++)
	{
		for (uint8_t s = MINSTEP; s < selection->firstStep; s++)
		{
			seq[player.ramBank].track[t].step[s] = seq[player.ramBank].track[t].step[s + 1];
		}
		clearStep(&seq[player.ramBank].track[t].step[selection->firstStep]);
	}

}
void Sequencer::copyToBuffer()
{
	copySelectionToBuffer(&sequencer.selection, &sequencer.selectionPaste);
}
void Sequencer::pasteFromBuffer()
{
	pasteSelectionFromBuffer(&sequencer.selection, &sequencer.selectionPaste);
}

void Sequencer::copySelectionToBuffer(strSelection *from, strSelection *to)
{
	if (!isSelectionCorrect(from)) return;
	if (!isSelectionCorrect(to)) return;

//	uint8_t tracksToCopy, rowsToCopy;
//	uint8_t trackOff = 0, stepOff = 0;

//	copyTrackBuffer[0].step[0].note = 10;

//	tracksToCopy = from->lastTrack - from->firstTrack + 1;
//	rowsToCopy = from->lastStep - from->firstStep + 1;
//	Sequencer::strPattern::strTrack::strStep *stepFrom, *stepTo;

	Sequencer::strPattern::strTrack *trackToCopy, *trackToPaste;

	for (uint8_t a = 0; a <= MAXTRACK; a++)
	{
		trackToCopy = &seq[player.ramBank].track[a];
		trackToPaste = &copyTrackBuffer[a];

		*trackToPaste = *trackToCopy;
	}

}
void Sequencer::pasteSelectionFromBuffer(strSelection *from, strSelection *to)
{
	if (!isSelectionCorrect(from)) return;
	if (!isSelectionCorrect(to)) return;

	uint8_t tracksToCopy, rowsToCopy;
	uint8_t trackOff = 0, stepOff = 0; //offsety

	tracksToCopy = from->lastTrack - from->firstTrack + 1;
	rowsToCopy = from->lastStep - from->firstStep + 1;
	Sequencer::strPattern::strTrack::strStep *stepFrom, *stepTo;

//	Sequencer::strPattern::strTrack::strStep *stepToCopy, *stepToPaste;

	trackOff = 0;
	for (uint8_t t = to->firstTrack;
			t <= to->firstTrack + tracksToCopy - 1;
			t++)
	{
		stepOff = 0;
		for (uint8_t s = to->firstStep;
				s <= to->firstStep + rowsToCopy - 1;
				s++)
		{
			uint8_t trackNoFrom, stepNoFrom;
			trackNoFrom = from->firstTrack + trackOff;
			stepNoFrom = from->firstStep + stepOff;

			// czy step istnieje?
			if (stepNoFrom <= MAXSTEP
					&& trackNoFrom <= MAXTRACK
					&& s <= MAXSTEP
					&& t <= MAXTRACK)
			{

				stepFrom = &copyTrackBuffer[trackNoFrom].step[stepNoFrom];
				stepTo = &seq[player.ramBank].track[t].step[s];
				*stepTo = *stepFrom;
			}
			stepOff++;

		}
		trackOff++;
	}

}

//		stepToCopy = &seq[player.ramBank].track[a];
//		stepToPaste = &copyTrackBuffer[a];
//
//		*trackToPaste = *trackToCopy;

bool Sequencer::isSelectionCorrect(strSelection *selection)
{
	if (selection->firstStep > selection->lastStep ||
			selection->firstTrack > selection->lastTrack ||
			selection->firstStep > MAXSTEP ||
			selection->firstTrack > MAXTRACK ||
			selection->lastStep > MAXSTEP ||
			selection->lastTrack > MAXTRACK)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

bool Sequencer::isSingleSelection(strSelection *selection)
{
	if (selection->firstStep == selection->lastStep &&
			selection->firstTrack == selection->lastTrack &&
			isSelectionCorrect(selection))

	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void Sequencer::setSelection(uint8_t stepFrom,
								uint8_t trackFrom,
								uint8_t stepTo,
								uint8_t trackTo)
{
	if (stepTo < stepFrom)
	{
		selection.firstStep = stepTo;
		selection.lastStep = stepFrom;
	}
	else
	{
		selection.firstStep = stepFrom;
		selection.lastStep = stepTo;
	}

	if (trackTo < trackFrom)
	{
		selection.firstTrack = trackTo;
		selection.lastTrack = trackFrom;
	}
	else
	{
		selection.firstTrack = trackFrom;
		selection.lastTrack = trackTo;
	}

}

void Sequencer::setPasteSelection(uint8_t stepFrom,
									uint8_t trackFrom,
									uint8_t stepTo,
									uint8_t trackTo)
{
	if (stepTo < stepFrom)
	{
		selectionPaste.firstStep = stepTo;
		selectionPaste.lastStep = stepFrom;
	}
	else
	{
		selectionPaste.firstStep = stepFrom;
		selectionPaste.lastStep = stepTo;
	}

	if (trackTo < trackFrom)
	{
		selectionPaste.firstTrack = trackTo;
		selectionPaste.lastTrack = trackFrom;
	}
	else
	{
		selectionPaste.firstTrack = trackFrom;
		selectionPaste.lastTrack = trackTo;
	}

}
