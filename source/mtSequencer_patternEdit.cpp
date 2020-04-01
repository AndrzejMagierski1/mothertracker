#include <Arduino.h>

#include "mtSequencer.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"
//#include "mtFileManager.h"
#include "debugLog.h"

#include "patternEditor/patternEditor.h"
#include "keyScanner.h"
#include "core/interfaceDefs.h"
extern Sequencer sequencer;

void fromToSwap(int16_t &from, int16_t &to)
{
	if (to < from)
	{
		uint8_t buff;
		buff = from;
		from = to;
		to = buff;
	}
}

void Sequencer::fillRandomNotes(int16_t fillStep,
								int16_t scale,
								int16_t from,
								int16_t to)
{
	fromToSwap(from, to);

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *step;
	uint8_t root = mtConfig.values.padBoardRootNote;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];
			if (isStepToFillNote(step, offset, fillStep))
			{
				if (step->note == STEP_NOTE_EMPTY)
				{
					step->instrument = mtProject.values.lastUsedInstrument;
				}
				for (uint8_t a = 0; a <= 100; a -= -1)
				{
					step->note = random(from, to + 1);
					if (isInScale(step->note, root, scale)) break;
				}
			}
		}
	}
}
void Sequencer::fillLinearNotes(int16_t fillStep,
								int16_t scale,
								int16_t from,
								int16_t to)
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	strPattern::strTrack::strStep *step;
//	uint8_t scale = mtProject.values.padBoardScale;
	uint8_t root = mtConfig.values.padBoardRootNote;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];
			if (isStepToFillNote(step, offset, fillStep))
			{
				if (step->note == STEP_NOTE_EMPTY)
				{
					step->instrument = mtProject.values.lastUsedInstrument;
				}
				step->note = map(offset + sel->firstStep,
									sel->firstStep,
									sel->lastStep,
									from,
									to);
				for (uint8_t a = 0; a < 12; a++)
				{
					if (isInScale(step->note, root, scale)) break;
					step->note++;
				}

			}
		}
	}
}
void Sequencer::fillLinearInstruments(int16_t fillStep,
										int16_t from,
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
void Sequencer::fillLinearFx(int16_t fxIndex,
								int16_t fillStep,
								int16_t fxName,
								int16_t fromVal,
								int16_t toVal)
{
	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (fxIndex > FX_SLOTS_MAX) fxIndex = 0;

	int16_t fxType = constrain(interfaceGlobals.fxNameToId(fxName),
								-1,
								FX_COUNT - 1);

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
				// przez float ladniej sie mapuje
				float tempFloat = map(float(offset + sel->firstStep),
										sel->firstStep,
										sel->lastStep,
										fromVal,
										toVal);
				step->fx[fxIndex].value = tempFloat;

				step->fx[fxIndex].type =
						(fxType >= 0) ? fxType : randomFx();
				Serial.println(step->fx[fxIndex].type);

				step->fx[fxIndex].value = constrain(
						step->fx[fxIndex].value,
						getFxMin(step->fx[fxIndex].type),
						getFxMax(step->fx[fxIndex].type));

			}
		}
	}
}
uint8_t Sequencer::randomFx()
{
	uint8_t retVal;
	for (uint8_t a = 0; a < 100; a++)
	{
		retVal = random(2, FX_MAX_FOR_RANDOM + 1); // bo 1 to off a nie chcemy
		if (
		retVal != fx.FX_TYPE_TEMPO &&
				retVal != fx.FX_TYPE_R11 &&
				retVal != fx.FX_TYPE_FADE &&
				retVal != fx.FX_TYPE_CUT &&
				retVal != fx.FX_TYPE_R4 &&
				retVal != fx.FX_TYPE_R5 &&
				retVal != fx.FX_TYPE_R10 &&
				retVal != fx.FX_TYPE_R30 &&
				retVal != fx.FX_TYPE_R31 &&
				retVal != fx.FX_TYPE_R22

				) break;

	}
	return retVal;
}
void Sequencer::fillRandomFx(int16_t fxIndex,
								int16_t fillStep,
								int16_t fxName,
								int16_t fromVal,
								int16_t toVal)
{
	fromToSwap(fromVal, toVal);

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (fxIndex > FX_SLOTS_MAX) fxIndex = 0;

	int16_t fxType = interfaceGlobals.fxNameToId(fxName);

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
				step->fx[fxIndex].type =
						(fxType >= 0) ? fxType : randomFx();

				step->fx[fxIndex].value = constrain(
						step->fx[fxIndex].value,
						getFxMin(step->fx[fxIndex].type),
						getFxMax(step->fx[fxIndex].type));
				;
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

	else if (fillStep == fillStepRandom
			&& random(0, 10) > 6) return true;

	else if (fillStep == fillStepNoFx &&
			step->fx[fxIndex].type == 0) return true;

	else if (fillStep == fillStepFx &&
			step->fx[fxIndex].type != 0) return true;

	else if (fillStep == fillStepNoNote &&
			step->note < 0) return true;

	else if (fillStep == fillStepNote &&
			step->note >= 0) return true;

	return false;
}
bool Sequencer::isStepToFillNote(strPattern::strTrack::strStep *step,
									uint8_t offset,
									int16_t fillStep)
{
	if (fillStep > 0 && offset % fillStep == 0) return true;

	else if (fillStep == fillStepRandom
			&& random(0, 10) > 6) return true;

	else if (fillStep == fillStepNoFx &&
			step->fx[0].type == 0 &&
			step->fx[1].type == 0) return true;

	else if (fillStep == fillStepFx &&
			(step->fx[0].type != 0 ||
					step->fx[1].type != 0)) return true;

	else if (fillStep == fillStepNoNote &&
			step->note < 0) return true;

	else if (fillStep == fillStepNote &&
			step->note >= 0) return true;

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

void Sequencer::invertSelectedSteps(uint8_t elements)
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

			switch (elements)
			{
			case ELEMENTS_ALL_NO_PREFERENCES:
				buffStep = *stepA;
				*stepA = *stepB;
				*stepB = buffStep;
				break;
			case ELEMENTS_FX1:

				buffStep = *stepA;
				stepA->fx[0] = stepB->fx[0];
				stepB->fx[0] = buffStep.fx[0];

				break;

			case ELEMENTS_FX2:

				buffStep = *stepA;
				stepA->fx[1] = stepB->fx[1];
				stepB->fx[1] = buffStep.fx[1];

				break;

			case ELEMENTS_INSTRUMENTS:
				case ELEMENTS_NOTES:

				buffStep = *stepA;
				stepA->note = stepB->note;
				stepB->note = buffStep.note;
				stepA->instrument = stepB->instrument;
				stepB->instrument = buffStep.instrument;

				break;
			default:
				break;
			}
		}
	}

}
//void Sequencer::loadDefaultTrack(uint8_t row, uint8_t bank)
//{
//
//	seq[bank].track[row].length = DEFAULT_PATTERN_LENGTH - 1;
//
//	for (uint8_t x = 0; x <= MAXSTEP; x++)
//	{
//		clearStep(x, row, bank);
//	}
//}

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
//					blinkNote(step->instrument,
//								step->note,
//								STEP_VELO_DEFAULT,
//								t);
					if (!(player.isPlay && !player.selectionMode))
						playSelection();
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
//						blinkNote(step->instrument,
//									step->note,
//									STEP_VELO_DEFAULT,
//									t);
						if (!(player.isPlay && !player.selectionMode))
							playSelection();

					}
				}
			}

		}
	}
}

//void Sequencer::changeSelectionVolume(int16_t value)
//{
//
//	strSelection *sel = &selection;
//	if (!isSelectionCorrect(sel)) return;
//
//	strPattern::strTrack::strStep *step;
//
//	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
//	{
//		for (uint8_t s = sel->firstStep, offset = 0;
//				s <= sel->lastStep;
//				s++, offset++)
//		{
//			step = &seq[player.ramBank].track[t].step[s];
//
//			if (!isMultiSelection())
//			{
//				step->velocity = constrain(step->velocity + value, -1,
//											STEP_VELO_MAX);
//				return;
//			}
//			else if (step->velocity >= 0)
//			{
//				step->velocity = constrain(step->velocity + value, 0,
//											STEP_VELO_MAX);
//			}
//		}
//	}
//}
void Sequencer::changeSelectionFxValue(uint8_t fxIndex, int16_t value)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (fxIndex > FX_SLOTS_MAX) return;

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			step->fx[fxIndex].value = constrain(
					step->fx[fxIndex].value + value,
					getFxMin(step->fx[fxIndex].type),
					getFxMax(step->fx[fxIndex].type));

//			step->fx[0].value2 = 1;
			if (!isMultiSelection() && step->fx[fxIndex].type == 0)
			{
				step->fx[fxIndex].type = interfaceGlobals.fxNameToId(
						mtProject.values.lastUsedFx);
				step->fx[fxIndex].value = getFxDefault(step->fx[fxIndex].type);
			}
			else
			{
				mtProject.values.lastUsedFx = interfaceGlobals.fxIdToName(
						step->fx[fxIndex].type);
			}

			if (!(player.isPlay && !player.selectionMode))
				playSelection();

		}
	}
}
void Sequencer::setSelectionFxValueByPad(uint8_t fxIndex, int16_t pad)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (fxIndex > FX_SLOTS_MAX) return;

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			if (!isMultiSelection() && step->fx[fxIndex].type == 0)
			{
				step->fx[fxIndex].type = interfaceGlobals.fxNameToId(
						mtProject.values.lastUsedFx);
			}

			step->fx[fxIndex].value = map(pad, 0, 47,
											getFxMin(step->fx[fxIndex].type),
											getFxMax(step->fx[fxIndex].type));

			if (!isMultiSelection() &&
					step->fx[fxIndex].type != 0
					&& !isRec()
					&& !isPlay())
			{
				playSelection();
			}
		}
	}
}
void Sequencer::changeSelectionFxType(uint8_t index, int16_t value)
{

	if (value > 0) value = 1;
	else if (value < 0) value = -1;

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (index > FX_SLOTS_MAX) return;

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];
			// jeśli off

			uint8_t name = interfaceGlobals.fxIdToName(step->fx[index].type);
			name += value;

			step->fx[index].type = constrain(interfaceGlobals.fxNameToId(name),
												0,
												47);
			step->fx[index].value = constrain(
												step->fx[index].value,
												getFxMin(step->fx[index].type),
												getFxMax(step->fx[index].type));
		}
	}
}
void Sequencer::setSelectionFxType(uint8_t fxIndex, int16_t value)
{

	strSelection *sel = &selection;
	if (!isSelectionCorrect(sel)) return;
	if (fxIndex > FX_SLOTS_MAX) return;

	strPattern::strTrack::strStep *step;

	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
	{
		for (uint8_t s = sel->firstStep, offset = 0;
				s <= sel->lastStep;
				s++, offset++)
		{
			step = &seq[player.ramBank].track[t].step[s];

			step->fx[fxIndex].type = value;
			step->fx[fxIndex].value = constrain(
					step->fx[fxIndex].value,
					getFxMin(step->fx[fxIndex].type),
					getFxMax(step->fx[fxIndex].type));
		}
	}
}

uint8_t Sequencer::getInstrumentPosInOrder(int8_t value)
{
	for (uint8_t a = 0; a < 48 + 16; a++)
	{
		if (changeInstrumentOrder[a] == value)
		{
			return a;
		}
	}
	return 0;
}
uint8_t Sequencer::getInstrumentFromOrder(int8_t value)
{
	value = constrain(value,
						0,
						47 + 16);
	return changeInstrumentOrder[value];
}
uint8_t Sequencer::changeInstrumentInSpecificOrder(int8_t actualValue,
													int16_t delta)
{
	delta = constrain(delta, -5, 5);
	return constrain(
			getInstrumentFromOrder(
					getInstrumentPosInOrder(actualValue) + delta),
			0,
			INSTRUMENTS_MAX + 16);
}

void Sequencer::changeSelectionInstrument(int16_t value)
{

//	value = constrain(value, -1, 1);

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

					step->instrument = changeInstrumentInSpecificOrder(
							step->instrument, value);

					if (!(player.isPlay && !player.selectionMode))
						playSelection();

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
					step->instrument = changeInstrumentInSpecificOrder(
							step->instrument, value);
				}

			}

		}
	}
	if (!(player.isPlay && !player.selectionMode))
		playSelection();
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

//					blinkNote(step->instrument,
//								step->note,
//								STEP_VELO_DEFAULT,
//								t);
					if (!isRec()
							&& !isPlay())
					{
						playSelection();
					}

					mtProject.values.lastUsedInstrument = step->instrument;
				}

				return;
			}
			else
			{
				if (step->note >= 0)
				{
					step->instrument = value;
					//mtProject.values.lastUsedInstrument = step->instrument;
				}
			}
		}
	}

	if (!isRec()
			&& !isPlay())
	{
		playSelection();
	}
}
//void Sequencer::setSelectionVelocity(int16_t value)
//{
//
//	strSelection *sel = &selection;
//	if (!isSelectionCorrect(sel)) return;
//
//	strPattern::strTrack::strStep *step;
//
//	for (uint8_t t = sel->firstTrack; t <= sel->lastTrack; t++)
//	{
//		for (uint8_t s = sel->firstStep, offset = 0;
//				s <= sel->lastStep;
//				s++, offset++)
//		{
//			step = &seq[player.ramBank].track[t].step[s];
//
//			if (isSingleSelection(sel))
//			{
//				step->velocity = value;
//
//				if (step->note >= 0)
//				{
//					blinkNote(step->instrument,
//								step->note,
//								step->velocity,
//								t);
//				}
//				return;
//			}
//			else
//			{
//				if (step->note >= 0)
//				{
//					step->velocity = value;
//
//				}
//			}
//		}
//	}
//}
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

				if (step->note == STEP_NOTE_EMPTY)
				{
					step->instrument = mtProject.values.lastUsedInstrument;
				}
				step->note = value;

				if (step->note >= 0
						&& !isRec()
						&& !isPlay())
				{
//					blinkNote(step->instrument,
//								step->note,
//								STEP_VELO_DEFAULT,
//								t);
					playSelection();
				}
				return;
			}
			else
			{
				if (step->note != STEP_NOTE_EMPTY)
				{
					step->note = value;
				}
			}
		}
	}

	if (/*step->note >= 0
			&&*/ !isRec()
			&& !isPlay())
	{
//					blinkNote(step->instrument,
//								step->note,
//								STEP_VELO_DEFAULT,
//								t);
		playSelection();
	}

}

void Sequencer::clearStep(uint8_t x, uint8_t row)
{
	clearStep(x, row, player.ramBank);
}

void Sequencer::clearStep(uint8_t x, uint8_t row, uint8_t bank)
{
	strPattern::strTrack *tempRow = &seq[bank].track[row];
	strPattern::strTrack::strStep *step = &tempRow->step[x];

	clearStep(step, 0);
}

void Sequencer::clearStep(strPattern::strTrack::strStep *step)
{
	clearStep(step, 0);
}

void Sequencer::clearStep(strPattern::strTrack::strStep *step,
							uint8_t elements)
{
	switch (elements)
	{
	case ELEMENTS_ALL_NO_PREFERENCES:
		step->note = STEP_NOTE_EMPTY;
		step->instrument = 0;
		step->fx[0].type = 0;
		step->fx[1].type = 0;
		break;
	case ELEMENTS_FX1:
		step->fx[0].type = 0;
		break;
	case ELEMENTS_FX2:
		step->fx[1].type = 0;
		break;
	case ELEMENTS_INSTRUMENTS:
		case ELEMENTS_NOTES:
		step->note = STEP_NOTE_EMPTY;
		step->instrument = 0;
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
void Sequencer::clearSelected(strSelection *selection, uint8_t elements)
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

void Sequencer::setPatternHeader(strPattern *patt)
{
	patt->header.id_file[0] = 'K';
	patt->header.id_file[1] = 'S';

	patt->header.fwVersion[0] = FV_VER_1;
	patt->header.fwVersion[1] = FV_VER_2;
	patt->header.fwVersion[2] = FV_VER_3;
	patt->header.fwVersion[3] = FV_VER_1;

	patt->header.type = fileTypePattern;

	patt->header.fileStructureVersion[0] = PATTERN_FILE_VERSION;
	patt->header.fileStructureVersion[1] = PATTERN_FILE_VERSION;
	patt->header.fileStructureVersion[2] = PATTERN_FILE_VERSION;
	patt->header.fileStructureVersion[3] = PATTERN_FILE_VERSION;

	patt->header.size = sizeof(strPattern);
}

void Sequencer::clearPattern(strPattern *patt)
{

	patt->swing = DEFAULT_SWING;
	patt->rezerwa1 = 0;
	patt->rezerwa2 = 0;
	patt->rezerwa3 = 0;
	patt->rezerwa4 = 0;

	setPatternHeader(patt);

	for (strPattern::strTrack &track : patt->track)
	{
		track.length = DEFAULT_PATTERN_LENGTH - 1;

		for (strPattern::strTrack::strStep &step : track.step)
		{
			clearStep(&step);
		}
	}

}

void Sequencer::clearSingleTrack(strPattern::strTrack *track)
{
	for (uint8_t i = 0; i < MAXSTEP; i++)
	{
		clearStep(&track->step[i]);
	}
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
				case ELEMENTS_FX1:
					stepTo->fx[0] = stepFrom->fx[0];
					break;
				case ELEMENTS_FX2:
					stepTo->fx[1] = stepFrom->fx[1];
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
uint8_t Sequencer::getCopySelectionHeight()
{
	if (isSelectionCorrect(&copySelection))
	{
		return copySelection.lastStep - copySelection.firstStep + 1;
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

int16_t Sequencer::getFxMax(uint8_t fxID)
{
	switch (fxID)
	{
	case fx.FX_TYPE_VELOCITY:
		return 100;
	case fx.FX_TYPE_PANNING:
		return 100;
	case fx.FX_TYPE_MICROTUNING:
		return 198;

	case fx.FX_TYPE_BREAK_PATTERN:
		return 1;

	case fx.FX_TYPE_MICROMOVE:
		return 100;
	case fx.FX_TYPE_TEMPO:
		return 200;
	case fx.FX_TYPE_OFF:
		return 0;
	case fx.FX_TYPE_RANDOM_INSTRUMENT:
		return 47;
	case fx.FX_TYPE_RANDOM_VALUE:
		return 255;
	case fx.FX_TYPE_SAMPLE_SLICE:
		return 47;
	case fx.FX_TYPE_PROGRAM_CHANGE:
		return 127;
	case fx.FX_TYPE_REVERSE_PLAYBACK:
		return 1;
	case fx.FX_TYPE_ROLL:

		return (fx.ROLL_PERIOD_MAX + 1) * (fx.rollNoteDir_max + 1) - 1;

	case fx.FX_TYPE_SEND_CC_A:
		case fx.FX_TYPE_SEND_CC_B:
		case fx.FX_TYPE_SEND_CC_C:
		case fx.FX_TYPE_SEND_CC_D:
		case fx.FX_TYPE_SEND_CC_E:
		return 127;

	case fx.FX_TYPE_FILTER_LFO:
		case fx.FX_TYPE_PANNING_LFO:
		case fx.FX_TYPE_POSITION_LFO:
		return 24;

	case fx.FX_TYPE_VOLUME_LFO:
		return 19;

	default:
		return 100;
	}
}

int16_t Sequencer::getFxMin(uint8_t fxID)
{
	switch (fxID)
	{
	case fx.FX_TYPE_MICROMOVE:
		return 0;
	case fx.FX_TYPE_SAMPLE_SLICE:
		return 0;
	case fx.FX_TYPE_TEMPO:
		return 5;
	case fx.FX_TYPE_PANNING:
		return 0;
	case fx.FX_TYPE_BREAK_PATTERN:
		return 1;

	case fx.FX_TYPE_ROLL:
		return fx.ROLL_PERIOD_MIN;

	default:
		return 0;
	}
}

int16_t Sequencer::getFxDefault(uint8_t fxID)
{

	switch (fxID)
	{
	case fx.FX_TYPE_OFF:
		return 0;
	case fx.FX_TYPE_TEMPO:
		return 60;

	case fx.FX_TYPE_ROLL:
		return 1;

	case fx.FX_TYPE_BREAK_PATTERN:
		return 1;

	case fx.FX_TYPE_PANNING:
		return 50;
	case fx.FX_TYPE_MICROTUNING:
		return 99;

	default:
		return 0;
	}
}

int16_t Sequencer::rollValueToPeriod(int16_t value)
{
	switch (value)
	{
	case fx.ROLL_PERIOD_1_1:
		return 1;
	case fx.ROLL_PERIOD_1_2:
		return 2;
	case fx.ROLL_PERIOD_1_3:
		return 3;
	case fx.ROLL_PERIOD_1_4:
		return 4;
	case fx.ROLL_PERIOD_1_6:
		return 6;
	case fx.ROLL_PERIOD_1_8:
		return 8;
	case fx.ROLL_PERIOD_1_12:
		return 12;
	case fx.ROLL_PERIOD_1_16:
		return 16;
	default:
		return 0;
	}
}

int16_t Sequencer::getFxValueCorrection(uint8_t type, uint8_t value)
{

	switch (type)
	{
	case fx.FX_TYPE_TEMPO:
		return value * 2;
		break;
	case fx.FX_TYPE_PANNING:
		return value - 50;
		break;
	case fx.FX_TYPE_MICROTUNING:
		return value - 99;
		break;
	case fx.FX_TYPE_SAMPLE_SLICE: // jawnie od 1, programowo od 0
		return value + 1;
		break;

	default:
		return value;
	}
}

const char lfoAmpLabels[20][4] =
		{
				"  6",
				"  4",
				"  3",
				"  2",
				"3/2",
				"  1",
				"3/4",
				"1/2",
				"3/8",
				"1/3",
				"1/4",
				"316",
				"1/6",
				"1/8",
				"/12",
				"/16",
				"/24",
				"/32",
				"/48",
				"/64"
		};

const char lfoSpeedLabels[25][4] =
		{
				"32",
				"24",
				"16",
				"12",
				"8",
				"6",
				"4",
				"3",
				"2",
				"3/2",
				"1",
				"3/4",
				"1/2",
				"3/8",
				"1/3",
				"1/4",
				"316",
				"1/6",
				"1/8",
				"/12",
				"/16",
				"/24",
				"/32",
				"/48",
				"/64"
		};

void Sequencer::makeFxValLabel(char *ptr, uint8_t fxID, uint8_t track,
								uint8_t step)
{
	strPattern::strTrack::strStep *actualStep = &getActualPattern()->track[track].step[step];

	makeFxValLabel(ptr, actualStep->fx[fxID].type, actualStep->fx[fxID].value);

}
void Sequencer::makeFxValLabel(char *ptr, uint8_t fxType, uint8_t value)
{
	makeFxValLabel(ptr, fxType, value, fxValLabelAlign_right);
}

void Sequencer::makeFxValLabel(char *ptr, uint8_t fxType, uint8_t value,
								enFxValLabelAlign align)
{

	int16_t val = getFxValueCorrection(fxType, value);

	switch (fxType)
	{
	case fx.FX_TYPE_ROLL:
		sprintf(ptr,
				"%c%2u",
				getRollTypeChar(value),
				rollValueToPeriod(value % (fx.ROLL_PERIOD_MAX + 1)));
		break;

	case fx.FX_TYPE_FILTER_LFO:
		case fx.FX_TYPE_PANNING_LFO:
		case fx.FX_TYPE_POSITION_LFO:
		strcpy(ptr, lfoSpeedLabels[val]);
		break;

	case fx.FX_TYPE_VOLUME_LFO:
		strcpy(ptr, lfoAmpLabels[val]);
		break;

	case fx.FX_TYPE_OFF:
		strcpy(ptr, "OFF");
		break;

	case fx.FX_TYPE_REVERSE_PLAYBACK:
		strcpy(ptr, val > 0 ? "<<<" : ">>>");
		break;

	case fx.FX_TYPE_BREAK_PATTERN:
		strcpy(ptr, val > 0 ? "BRK" : "---");
		break;

	default:
		if (abs(val) < 10 && (align == fxValLabelAlign_center))
		{
			sprintf(ptr,
					"%2i",
					val
					);
		}
		else
		{
			sprintf(ptr,
					"%3i",
					val
					);
		}
		break;
	}

}

char Sequencer::getRollTypeChar(uint8_t val)
{
	switch (val / (fx.ROLL_PERIOD_MAX + 1))
	{
	case fx.rollType_const:
		return ' ';
	case fx.rollType_noteUp:
		return 'N';
	case fx.rollType_noteDown:
		return 'n';
	case fx.rollType_noteRandom:
		return 'R';
	case fx.rollType_volDown:
		return 'v';
	case fx.rollType_volUp:
		return 'V';
	default:
		break;
	}
	return '?';
}
uint8_t Sequencer::calcStepLength(uint8_t track, uint8_t step)
{
	uint8_t trackLen = getActualPattern()->track[0].length;

	uint8_t b = 1;
	for (uint8_t a = step + 1; a <= trackLen; a++, b++)
	{
		if (getActualPattern()->track[track].step[a].note != STEP_NOTE_EMPTY)
		{
			break;
		}
	}

	return b;
}

uint8_t Sequencer::isTrackEngineMuted(uint8_t track)
{
	if (track > MAXTRACK)
	{
		return 0;
	}
	return mtProject.values.trackMute[track];
}

