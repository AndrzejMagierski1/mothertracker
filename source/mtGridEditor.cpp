#include "mtGridEditor.h"
#include "mtStructs.h"

mtGridEditor gridEditor;
extern strGridPad gridPad[48];
constexpr uint8_t GRID_ROW_SIZE = 12;

void  mtGridEditor::open()
{
	selectedPad = 0;
	isConfirmedPad = false;
}

void mtGridEditor::changeSelectedPad(enChangePadDirection dir)
{
	switch(dir)
	{
	case enChangePadDirectionUp:
		if(selectedPad >= GRID_ROW_SIZE ) selectedPad -= GRID_ROW_SIZE;
		break;
	case enChangePadDirectionDown:
		if(selectedPad < (3 * GRID_ROW_SIZE) ) selectedPad += GRID_ROW_SIZE;
		break;
	case enChangePadDirectionLeft:
		if(selectedPad%12 != 0) selectedPad--;
		break;
	case enChangePadDirectionRight:
		if(selectedPad%12 != 11) selectedPad++;
		break;
	default:	break;
	}
}
void mtGridEditor::changeSelectedPad(uint8_t pad)
{
	selectedPad = pad;
}

void mtGridEditor::confirmSelectedPad()
{
	isConfirmedPad = true;
}
void mtGridEditor::cancelSelectedPad()
{
	isConfirmedPad = false;
}
void mtGridEditor::changeNote(int16_t val)
{
	if(!isConfirmedPad) return;

	if(gridPad[selectedPad].note + val > MAX_NOTE) gridPad[selectedPad].note = MAX_NOTE;
	else if(gridPad[selectedPad].note + val < MIN_NOTE) gridPad[selectedPad].note = MIN_NOTE;
	else gridPad[selectedPad].note += val;
}
void mtGridEditor::changeMicrotune(int16_t val)
{
	if(!isConfirmedPad) return;

	if(gridPad[selectedPad].microtune + val > MAX_INSTRUMENT_FINETUNE) gridPad[selectedPad].microtune = MAX_INSTRUMENT_FINETUNE;
	else if(gridPad[selectedPad].microtune + val < MIN_INSTRUMENT_FINETUNE) gridPad[selectedPad].microtune = MIN_INSTRUMENT_FINETUNE;
	else gridPad[selectedPad].microtune += val;
}
void mtGridEditor::changeLedEnable(int16_t val)
{
	if(!isConfirmedPad) return;

	if(val > 0) gridPad[selectedPad].ledEnable =  true;
	else if(val < 0) gridPad[selectedPad].ledEnable = false;
}

bool mtGridEditor::getIsConfirmedPad()
{
	return isConfirmedPad;
}
uint8_t mtGridEditor::getSelectedPad()
{
	return selectedPad;
}
