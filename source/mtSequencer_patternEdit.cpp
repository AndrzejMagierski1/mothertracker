#include <Arduino.h>

#include "mtSequencer.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"
#include "mtFileManager.h"

#include "patternEditor.h"
#include "keyScanner.h"
extern Sequencer sequencer;

void fromToSwap(int16_t & from, int16_t & to)
{
	if (to < from)
	{
		uint8_t buff;
		buff = from;
		from = to;
		to = buff;
	}
}

void Sequencer::fillRandomNotes(int16_t fillStep, int16_t from, int16_t to)
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
			if (isStepToFillNote(step, offset, fillStep))
			{
				step->note = random(from, to + 1);
				step->instrument = mtProject.values.lastUsedInstrument;
			}
		}
	}
}
void Sequencer::fillLinearNotes(int16_t fillStep, int16_t from, int16_t to)
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
			if (isStepToFillNote(step, offset, fillStep))
			{
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
void Sequencer::fillLinearInstruments(int16_t fillStep, int16_t from,
										int16_t to)
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
			if (isStepToFillNote(step, offset, fillStep))
			{
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
void Sequencer::fillLinearFx(int16_t index,
								int16_t fillStep,
								int16_t fxType,
								int16_t fromVal,
								int16_t toVal)
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (index > FX_SLOTS_MAX) index = 0; //todo

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			if (isStepToFillFx(step, offset, index, fillStep))
			{
				step->fx[index].value = map(offset + sel->firstStep,
											sel->firstStep,
											sel->lastStep,
											fromVal,
											toVal);
				step->fx[index].type = fxType;
			}
		}
	}
}
void Sequencer::fillRandomFx(int16_t fxIndex,
								int16_t fillStep,
								int16_t fxType,
								int16_t fromVal,
								int16_t toVal)
{
	fromToSwap(fromVal, toVal);

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (fxIndex > FX_SLOTS_MAX) fxIndex = 0; //todo

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			if (isStepToFillFx(step, offset, fxIndex, fillStep))
			{
				step->fx[fxIndex].value = random(fromVal, toVal + 1);
				step->fx[fxIndex].type = fxType;
			}
		}
	}
}

bool Sequencer::isStepToFillFx(strPattern::strTrack::strStep *step,
								uint8_t offset,
								uint8_t fxIndex,
								int16_t fillStep)
{
	if (fillStep > 0 && offset % fillStep == 0) return true;
	else if (fillStep == fillStepRandom && random(0, 10) > 6) return true;
	else if (fillStep == fillStepOccupied && (step->fx[fxIndex].type > 0 || step->note >= 0)) return true;
	else if (fillStep == fillStepEmpty && step->fx[fxIndex].type == 0)
		return true;

	return false;
}
bool Sequencer::isStepToFillNote(strPattern::strTrack::strStep *step,
									uint8_t offset,
									int16_t fillStep)
{
	if (fillStep > 0 && offset % fillStep == 0) return true;
	else if (fillStep == fillStepRandom && random(0, 10) > 6) return true;
	else if (fillStep == fillStepOccupied && step->note >= 0) return true;
	else if (fillStep == fillStepEmpty && step->note == STEP_NOTE_EMPTY)
		return true;

	return false;
}

void Sequencer::fillRandomInstruments(int16_t fillStep, int16_t from,
										int16_t to)
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

			if (isStepToFillNote(step, offset, fillStep))
			{
				step->instrument = random(from, to + 1);

				if (step->note == STEP_NOTE_EMPTY)
				{
					step->note = STEP_NOTE_DEFAULT;
				}
			}
		}
	}
}
//
//void Sequencer::fillLinearVelocity(int16_t fillStep, int16_t from,
//									int16_t to)
//{
//	// todo: multifx
//	strSelection *sel = &selection;
//	if (!isSelectionCorrect(sel)) return;
//	strPattern::strTrack::strStep *step;
//
//	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
//	{
//		for (uint8_t s = sel->firstStep, offset = 0;
//				s <= sel->lastStep;
//				s++, offset++)
//		{
//			step = &seq[player.ramBank].track[t].step[s];
//			if (isStepToFillNote(step, offset, fillStep))
//			{
//				step->velocity = map(offset + sel->firstStep,
//										sel->firstStep,
//										sel->lastStep,
//										from,
//										to);
//			}
//		}
//	}
//}
//
//void Sequencer::fillRandomVelocity(int16_t fillStep, int16_t from,
//									int16_t to)
//{
//	// todo: multifx
//	fromToSwap(from, to);
//	strSelection *sel = &selection;
//	if (!isSelectionCorrect(sel)) return;
//	strPattern::strTrack::strStep *step;
//
//	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
//	{
//		for (uint8_t s = sel->firstStep, offset = 0;
//				s <= sel->lastStep;
//				s++, offset++)
//		{
//			if ((offset % fillStep == 0 && fillStep > 0) ||
//					(random(0, 10) > 6 && fillStep == 0))
//			{
//				step = &seq[player.ramBank].track[t].step[s];
//				step->velocity = random(from, to + 1);
//
//			}
//		}
//	}
//}

//void Sequencer::randomSelectedNotes(int16_t from, int16_t to, int16_t scale)
//{
//	fromToSwap(from, to);
//
//	strSelection *sel = &selection;
//	if (!isSelectionCorrect(sel)) return;
//	strPattern::strTrack::strStep *step;
//
//	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
//	{
//		for (uint8_t s = sel->firstStep, offset = 0;
//				s <= sel->lastStep;
//				s++, offset++)
//		{
//			step = &seq[player.ramBank].track[t].step[s];
//			if (step->note >= 0)
//			{
//				step->note = random(from, to + 1);
//			}
//		}
//	}
//}
//
//void Sequencer::randomSelectedInstruments(int16_t from, int16_t to)
//{
//	fromToSwap(from, to);
//
//	strSelection *sel = &selection;
//	if (!isSelectionCorrect(sel)) return;
//	strPattern::strTrack::strStep *step;
//
//	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
//	{
//		for (uint8_t s = sel->firstStep, offset = 0;
//				s <= sel->lastStep;
//				s++, offset++)
//		{
//			step = &seq[player.ramBank].track[t].step[s];
//			if (step->note >= 0)
//			{
//				step->instrument = random(from, to + 1);
//			}
//		}
//	}
//}
//void Sequencer::randomSelectedVelo(int16_t from, int16_t to)
//{
//	fromToSwap(from, to);
//
//	strSelection *sel = &selection;
//	if (!isSelectionCorrect(sel)) return;
//	strPattern::strTrack::strStep *step;
//
//	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
//	{
//		for (uint8_t s = sel->firstStep, offset = 0;
//				s <= sel->lastStep;
//				s++, offset++)
//		{
//			step = &seq[player.ramBank].track[t].step[s];
//			if (step->velocity >= 0 || step->note >= 0)
//			{
//				step->velocity = random(from, to + 1);
//			}
//		}
//	}
//}
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
				a <= ((sel->lastStep - sel->firstStep) / 2) + sel->firstStep;
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

	seq[bank].track[row].length = MAXSTEP;

	for (uint8_t x = 0; x <= MAXSTEP; x++)
	{
		clearStep(x, row, bank);
	}
}

void Sequencer::changeSelectionNote(int16_t value)
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	bool blinkFirst = 1;

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
											-4,
											STEP_NOTE_MAX);
				}
				else
				{
					step->note = constrain(step->note + value,
											-4,
											STEP_NOTE_MAX);
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
											STEP_NOTE_MAX);
					if (blinkFirst)
					{
						blinkFirst = 0;
						blinkNote(step->instrument,
									step->note,
									step->velocity,
									t);

					}
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

			if (!isMultiSelection())
			{
				step->velocity = constrain(step->velocity + value, -1,
											STEP_VELO_MAX);
				return;
			}
			else if (step->velocity >= 0)
			{
				step->velocity = constrain(step->velocity + value, 0,
											STEP_VELO_MAX);
			}
		}
	}
}
void Sequencer::changeSelectionFxValue(uint8_t index, int16_t value)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (index > FX_SLOTS_MAX) index = 0; //todo

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			switch (step->fx[index].type)
			{
			case fx.FX_TYPE_NUDGE:
				step->fx[index].value = constrain(step->fx[index].value + value,
													0,
													47);
				break;
			default:
				step->fx[index].value = constrain(step->fx[index].value + value,
													0,
													127);
			}

//			step->fx[0].value2 = 1;
			if (!isMultiSelection() && step->fx[index].type == 0)
			{
				step->fx[index].type = mtProject.values.lastUsedFx;
			}
			else
			{
				mtProject.values.lastUsedFx = step->fx[index].type;
			}
		}
	}
}
void Sequencer::setSelectionFxValue(uint8_t index, int16_t value)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (index > FX_SLOTS_MAX) index = 0; //todo

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			step->fx[index].value = value;
//			step->fx[0].value2 = 1;
			if (!isMultiSelection() && step->fx[index].type == 0)
			{
				step->fx[index].type = mtProject.values.lastUsedFx;
			}
		}
	}
}
void Sequencer::changeSelectionFxType(uint8_t index, int16_t value)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (index > FX_SLOTS_MAX) index = 0; //todo

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			// jeśli off
			step->fx[index].type = constrain(step->fx[index].type + value,
												0,
												200);
		}
	}
}
void Sequencer::setSelectionFxType(uint8_t index, int16_t value)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (index > FX_SLOTS_MAX) index = 0; //todo

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			step->fx[index].type = value; //todo
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

					step->instrument = constrain(step->instrument + value,
													0,
													INSTRUMENTS_MAX + 16);
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
					step->instrument = constrain(step->instrument + value,
													0,
													INSTRUMENTS_MAX + 16);
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
void Sequencer::setSelectionNote(int16_t value)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep;
				s <= sel->lastStep;
				s++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			if (isSingleSelection(sel))
			{
				step->note = value;

				if (step->note >= 0)
				{
					blinkNote(step->instrument,
								step->note,
								step->velocity,
								t);
				}
				return;
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

//void Sequencer::clearRow(uint8_t row, uint8_t bank)
//{
//// TODO: ograniczyć ba
//	for (uint8_t x = MINSTEP; x <= MAXSTEP; x++)
//	{
//		clearStep(x, row, bank);
//	}
//}
//
//void Sequencer::clearRow(uint8_t row)
//{
//	clearRow(row, player.ramBank);
//}

void Sequencer::clearStep(uint8_t x, uint8_t row)
{
	clearStep(x, row, player.ramBank);
}

void Sequencer::clearStep(uint8_t x, uint8_t row, uint8_t bank)
{
	strPattern::strTrack * tempRow = &seq[bank].track[row];
	strPattern::strTrack::strStep * step = &tempRow->step[x];

	clearStep(step, 0);
}

void Sequencer::clearStep(strPattern::strTrack::strStep * step,
							uint8_t elements)
{
	switch (elements)
	{
	case ELEMENTS_ALL_NO_PREFERENCES:
		step->note = STEP_NOTE_EMPTY;
		step->instrument = 0;
		step->velocity = -1;
		step->fx[0].type = 0;
		step->fx[1].type = 0;
		break;
	case ELEMENTS_FXes:
		step->fx[0].type = 0;
		step->fx[1].type = 0;
		break;
	case ELEMENTS_INSTRUMENTS:
		case ELEMENTS_NOTES:
		step->note = STEP_NOTE_EMPTY;
		step->instrument = 0;
		break;
	case ELEMENTS_VELO:
		step->velocity = -1;
		break;
	default:
		break;
	}

}

void Sequencer::clearSelected()
{
	clearSelected(&selection, 0);
}
void Sequencer::clearSelected(uint8_t elements)
{
	clearSelected(&selection, elements);
}
void Sequencer::clearSelected(strSelection * selection, uint8_t elements)
{
	if (!isSelectionCorrect(selection)) return;

	for (uint8_t t = selection->firstTrack; t <= selection->lastTrack; t++)
	{
		for (uint8_t s = selection->firstStep; s <= selection->lastStep; s++)
		{
			clearStep(&seq[player.ramBank].track[t].step[s], elements);
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
		clearStep(&seq[player.ramBank].track[t].step[selection->firstStep],
					ELEMENTS_ALL_NO_PREFERENCES);
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
		clearStep(&seq[player.ramBank].track[t].step[selection->firstStep],
					ELEMENTS_ALL_NO_PREFERENCES);
	}

}
void Sequencer::backspace()
{
	if (!isSelectionCorrect(&selection)) return;

	if (selection.firstStep < 1) return;

	strPattern::strTrack::strStep *stepFrom, *stepTo;

	for (uint8_t t = selection.firstTrack; t <= selection.lastTrack; t++)
	{
		for (uint8_t s = selection.firstStep - 1;
				s < seq[player.ramBank].track[t].length;
				s++)
		{
			stepFrom = &seq[player.ramBank].track[t].step[s + 1];
			stepTo = &seq[player.ramBank].track[t].step[s];

			*stepTo = *stepFrom;
		}

		clearStep(
				&seq[player.ramBank].track[t].step[seq[player.ramBank].track[t].length],
				ELEMENTS_ALL_NO_PREFERENCES);
	}

}
void Sequencer::copyToBuffer()
{
	copySelectionToBuffer(&sequencer.copySelection, &sequencer.pasteSelection);
}
void Sequencer::pasteFromBuffer(uint8_t elements)
{
	pasteSelectionFromBuffer(&sequencer.copySelection,
								&sequencer.pasteSelection,
								elements);
}

void Sequencer::copySelectionToBuffer(strSelection *from, strSelection *to)
{
	if (!isSelectionCorrect(from)) return;
	if (!isSelectionCorrect(to)) return;

	seq[2] = seq[player.ramBank];

}
void Sequencer::pasteSelectionFromBuffer(strSelection *from, strSelection *to,
											uint8_t elements)
{
	if (!isSelectionCorrect(from)) return;
	if (!isSelectionCorrect(to)) return;

	if (elements == ELEMENTS_ALL_WITH_PREFERENCES)
	{
		seq[player.ramBank] = seq[2];
		return;
	}

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
					&& t <= MAXTRACK
					&& s <= seq[player.ramBank].track[t].length)
			{

				stepFrom = &seq[2].track[trackNoFrom].step[stepNoFrom];
				stepTo = &seq[player.ramBank].track[t].step[s];

				switch (elements)
				{
				case ELEMENTS_ALL_NO_PREFERENCES:
					*stepTo = *stepFrom;
					break;
				case ELEMENTS_NOTES:
					stepTo->note = stepFrom->note;
					stepTo->instrument = mtProject.values.lastUsedInstrument;
					break;
				case ELEMENTS_INSTRUMENTS:
					stepTo->instrument = stepFrom->instrument;
					if (stepTo->note == STEP_NOTE_EMPTY)
					{
						stepTo->note = STEP_NOTE_DEFAULT;
					}
					break;
				case ELEMENTS_VELO:
					stepTo->velocity = stepFrom->velocity;
					break;
				case ELEMENTS_FXes:
					stepTo->fx[0] = stepFrom->fx[0];
					break;
				default:
					break;
				}

			}
			stepOff++;

		}
		trackOff++;
	}

}

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

void Sequencer::setCopySelection(uint8_t stepFrom,
									uint8_t trackFrom,
									uint8_t stepTo,
									uint8_t trackTo)
{
	if (stepTo < stepFrom)
	{
		copySelection.firstStep = stepTo;
		copySelection.lastStep = stepFrom;
	}
	else
	{
		copySelection.firstStep = stepFrom;
		copySelection.lastStep = stepTo;
	}

	if (trackTo < trackFrom)
	{
		copySelection.firstTrack = trackTo;
		copySelection.lastTrack = trackFrom;
	}
	else
	{
		copySelection.firstTrack = trackFrom;
		copySelection.lastTrack = trackTo;
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
		pasteSelection.firstStep = stepTo;
		pasteSelection.lastStep = stepFrom;
	}
	else
	{
		pasteSelection.firstStep = stepFrom;
		pasteSelection.lastStep = stepTo;
	}

	if (trackTo < trackFrom)
	{
		pasteSelection.firstTrack = trackTo;
		pasteSelection.lastTrack = trackFrom;
	}
	else
	{
		pasteSelection.firstTrack = trackFrom;
		pasteSelection.lastTrack = trackTo;
	}

}

