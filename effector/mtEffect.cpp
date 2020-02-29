#include "mtEffect.h"
#include "mtStructs.h"

extern int16_t sdram_effectsBank[15*256*1024];

constexpr uint32_t LOAD_BLOCK_SIZE_IN_SAMPLES = 8192;

mtEffect::mtEffect(): area( sdram_effectsBank + SAMPLE_EFFECTOR_LENGTH_MAX,0),
		confirmed(sdram_effectsBank,0),
		dataCopyier(LOAD_BLOCK_SIZE_IN_SAMPLES)
{

}
//***********LOAD
bool mtEffect::startLoad(uint8_t instr_idx)
{
	if(!mtProject.instrument[instr_idx].isActive) return false;
	if(load.processParams.state) return false;

	load.currentSrcAddr = mtProject.instrument[instr_idx].sample.address;
	load.currentDstAddr = confirmedArea.addr;
	load.processParams.currentProgressValue = 0;
	load.processParams.maxProgressValue = mtProject.instrument[instr_idx].sample.length;
	load.processParams.state = true;

	return true;

}
void mtEffect::updateLoad()
{
	if(load.processParams.state)
	{
		if(load.processParams.currentProgressValue < load.processParams.maxProgressValue )
		{
			uint32_t dif = load.processParams.maxProgressValue - load.processParams.currentProgressValue;
			uint32_t blockSize =  (dif >= LOAD_BLOCK_SIZE_IN_SAMPLES) ? 2*LOAD_BLOCK_SIZE_IN_SAMPLES : dif;

			memcpy(load.currentDstAddr,load.currentSrcAddr,blockSize);

			load.processParams.currentProgressValue += blockSize;
		}
		else
		{
			load.processParams.state = 0;
		}
	}
}
bool mtEffect::getLoadState()
{
	return getState(&load.processParams);
}
uint8_t mtEffect::getLoadProgress()
{
	return getProgress(&load.processParams);
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

	strMemoryArea temp = processedArea;

	processedArea = confirmedArea;
	confirmedArea = temp;

}
//***********
//***********SAVE
bool mtEffect::startSave(uint8_t instr_idx)
{
	//todo: waiting for ziejas SD library
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

		if(endProcessingState != enProcessingState::copyingAfterProcessing) processingState++;
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
uint8_t mtEffect::getProcessProgress()
{

}

//***********PREVIEW GETTERS
int16_t * const mtEffect::getAddresToPreview()
{
	return processedArea.addr;
}
uint32_t mtEffect::getLengthToPreview()
{
	return processedArea.length;
}
//***********
//***********PLAY GETTERS
int16_t * mtEffect::getAddresToPlay()
{
	return currentSelection.addr;
}
uint32_t  mtEffect::getLengthToPlay()
{
	return currentSelection.length;
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

	length = (uint32_t)((uint32_t) b * (float)(confirmedArea.length/2)/MAX_16BIT);
	addressShift = (uint32_t)( (uint32_t) a * (float)(confirmedArea.length/2)/MAX_16BIT);

	currentSelection.addr = confirmedArea.addr + addressShift;
	currentSelection.length = length - addressShift;
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

