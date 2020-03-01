#include "mtEffect.h"
#include "mtStructs.h"




mtEffect::mtEffect(): dataCopyier(LOAD_BLOCK_SIZE_IN_SAMPLES)
{

}
//***********LOAD
bool mtEffect::startLoad(uint8_t instr_idx)
{
	if(!mtProject.instrument[instr_idx].isActive) return false;



	return true;

}
void mtEffect::updateLoad()
{

}
bool mtEffect::getLoadState()
{
	return getState(&loadProcessParams);
}
uint8_t mtEffect::getLoadProgress()
{
	return getProgress(&loadProcessParams);
}
//***********
//***********APPLY
void mtEffect::startApply()
{
	if(!apply.isProcessData)
	{
		startProcess();
		processing.isApplyOnEnd = 1;
		return;
	}
}
//***********
//***********SAVE
bool mtEffect::startSave(uint8_t instr_idx)
{
	//todo: waiting for ziejas SD library
	return true;
}
void mtEffect::updateSave()
{
	//todo: waiting for ziejas SD library
}
bool mtEffect::getSaveState()
{
	return getState(&save.processParams);
}
uint8_t mtEffect::getSaveProgress()
{
	return getProgress(&save.processParams);
}
//***********
//***********PROCESSING
bool mtEffect::startProcessingSelection()
{
	if(processingState != enProcessingState::idle) return false;
	endProcessingState = true;

	return true;
}
void mtEffect::updateProcessingSelection()
{
	switch(processingState)
	{
		case enProcessingState::idle: break;
		case enProcessingState::copyingBeforeProcessing: 	 break;
		case enProcessingState::processingSelection:		 break;
		case enProcessingState::copyingAfterProcessing: 	 break;
		default: break;
	}

	if(endProcessingState)
	{
		switch(processingState)
		{
		case enProcessingState::idle: break;
		case enProcessingState::copyingBeforeProcessing: startProcess();	break;
		case enProcessingState::processingSelection:	 break;
		}

		if(processingState != enProcessingState::copyingAfterProcessing) processingState++;
		else processingState = enProcessingState::idle;
	}
}
uint8_t mtEffect::getProcessSelectionProgress()
{
	return getProgress(&processing.processParams);
}
bool mtEffect::getProcessSelectionState()
{
	return getState(&processing.processParams);
}
//***********PREVIEW GETTERS
int16_t * const mtEffect::getAddresToPreview()
{
	return processed.area.addr;
}
uint32_t mtEffect::getLengthToPreview()
{
	return processed.area.length;
}
//***********
//***********PLAY GETTERS
int16_t * mtEffect::getAddresToPlay()
{
	return confirmed.selection.addr;
}
uint32_t  mtEffect::getLengthToPlay()
{
	return confirmed.selection.length;
}
//***********
//***********SELECTION SET
void mtEffect::changeSelectionRange(uint16_t a, uint16_t b)
{
	uint32_t length;
	uint32_t addressShift;

	if(a > b)
	{
		uint16_t temp = a;
		a = b;
		b = temp;
	}

	length = (uint32_t)((uint32_t) b * (float)(confirmed.area.length/2)/MAX_16BIT);
	addressShift = (uint32_t)( (uint32_t) a * (float)(confirmed.area.length/2)/MAX_16BIT);

	confirmed.selection.addr = confirmed.area.addr + addressShift;
	confirmed.selection.length = length - addressShift;
}
//***********
bool mtEffect::getState(strProcessing * ptr)
{
	return ptr->state;
}

uint8_t mtEffect::getProgress(strProcessing * ptr)
{
	return (100 * ptr->currentProgressValue)/ ptr->maxProgressValue;
}

