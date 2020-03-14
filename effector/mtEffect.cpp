#include "mtEffect.h"
#include "mtStructs.h"
#include "mtDataCopyier.h"

void mtEffect::update()
{
	switch(operationType)
	{
	case enOperationType::operationTypeLoading:		 	 updateLoad();					break;
	case enOperationType::operationTypeProcessing:		 updateProcessingSelection(); 	break;
	case enOperationType::operationTypeSaveing: 		 updateSave();					break;
	default: break;
	}
}

void mtEffect::switchEffect(mtEffect * lastEffect)
{
	lastEffect->apply.isProcessData = 0;
	memcpy(&this->processing, &lastEffect->processing, sizeof(this->processing));
	memcpy(&this->processed, &lastEffect->processed, sizeof(this->processed));
	memcpy(&this->confirmed, &lastEffect->confirmed, sizeof(this->confirmed));
	memcpy(&this->loading, &lastEffect->loading, sizeof(this->loading));
	memcpy(&this->save, &lastEffect->save, sizeof(this->save));
	memcpy(&this->apply, &lastEffect->apply, sizeof(this->apply));
	memcpy(&this->undo, &lastEffect->undo, sizeof(this->undo));
}
//***********LOAD

void mtEffect::setStartLength(uint32_t val)
{
	confirmed.area.length = val;
	confirmed.selection.length = val;
}
mtEffect::loadResult mtEffect::startLoad(uint8_t instr_idx)
{
	if(mtProject.instrument[instr_idx].sample.length > SAMPLE_EFFECTOR_LENGTH_MAX) return loadResult::instrTooLong;
	if(!mtProject.instrument[instr_idx].isActive) return loadResult::instrIsNotActive;
	if(dataCopyier->getState()) return loadResult::loadIsAlreadyActive;

	operationType = enOperationType::operationTypeLoading;

	loading.processParams.currentProgressValue = 0;
	loading.processParams.maxProgressValue = mtProject.instrument[instr_idx].sample.length;
	loading.processParams.state = 1;

	confirmed.area.length = mtProject.instrument[instr_idx].sample.length;
	confirmed.selection.length = mtProject.instrument[instr_idx].sample.length;

	dataCopyier->start(mtProject.instrument[instr_idx].sample.address, confirmed.area.addr, mtProject.instrument[instr_idx].sample.length);


	return loadResult::loadSuccessfull;

}
void mtEffect::updateLoad()
{
	if(!loading.processParams.state) return;

	int32_t blockSize = dataCopyier->update();

	if(blockSize != - 1) loading.processParams.currentProgressValue += blockSize;

	loading.processParams.state = dataCopyier->getState();
	if(!loading.processParams.state)
	{

		operationType = enOperationType::operationTypeIdle;
		processing.isLoadedData = true;
		changeSelectionRange(loading.preloadPoints.a, loading.preloadPoints.b);
		if(loading.isProcessOnEnd)
		{
			loading.isProcessOnEnd = false;
			startProcessingSelection();
		}
	}

}
bool mtEffect::getLoadState()
{
	return getState(&loading.processParams);
}
uint8_t mtEffect::getLoadProgress()
{
	return getProgress(&loading.processParams);
}

bool mtEffect::getIsLoadedData()
{
	return processing.isLoadedData;
}

void mtEffect::clearIsLoadedData()
{
	processing.isLoadedData = false;
	loading.preloadPoints.a = 0;
	loading.preloadPoints.b = 65536;
}
//***********
//***********APPLY
void mtEffect::startApply()
{
	if(!apply.isProcessData)
	{
		startProcessingSelection();
		processing.isApplyOnEnd = 1;
		return;
	}

	strMemoryAreaWithSelection tmp = processed;
	processed = confirmed;
	confirmed = tmp;

	undo.isEnable = 1;
	apply.isProcessData = 0;
}
//***********
//***********APPLY
void mtEffect::startUndo()
{
	if(!undo.isEnable) return;

	strMemoryAreaWithSelection tmp = processed;
	processed = confirmed;
	confirmed = tmp;

	undo.isEnable = 0;
}
//***********
//***********SAVE
bool mtEffect::startSave(uint8_t instr_idx)
{
	operationType = enOperationType::operationTypeSaveing;
	//todo: waiting for ziejas SD library
	return true;
}
void mtEffect::updateSave()
{
	//operationType = enOperationType::operationTypeIdle; //na zakonczenie save
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
	if(!processing.isLoadedData)
	{
		loading.isProcessOnEnd = true;
		startLoad(mtProject.values.lastUsedInstrument);
		return false;
	}


	if(processingState != enProcessingState::idle) return false;
	uint32_t expectedSelectLen = getExpectedProcessLength();
	int32_t dif = expectedSelectLen - confirmed.selection.length ;

	if(confirmed.area.length + dif > SAMPLE_EFFECTOR_LENGTH_MAX ) return false;

	processed.selection.addr = processed.area.addr + (uint32_t)(confirmed.selection.addr - confirmed.area.addr);
	processed.selection.length = expectedSelectLen;
	processed.area.length = confirmed.area.length + (processed.selection.length - confirmed.selection.length);

	processing.processParams.currentProgressValue = 0;
	processing.processParams.maxProgressValue = expectedSelectLen;
	processing.processParams.state = true;

	endProcessingState = true;

	operationType = enOperationType::operationTypeProcessing;

	return true;
}
void mtEffect::updateProcessingSelection()
{
	switch(processingState)
	{
		case enProcessingState::idle: break;
		case enProcessingState::copyingBeforeProcessing:		if(!processingWithoutCopying) {updateCopying();}		break;
		case enProcessingState::processingSelection:			updateCommonProcess();								break;
		case enProcessingState::copyingAfterProcessing:			if(!processingWithoutCopying) {updateCopying();}		break;
		default: break;
	}

	if(endProcessingState)
	{
		endProcessingState = 0;

		switch(processingState)
		{
		case enProcessingState::idle:
			if(processingWithoutCopying)
			{
				endProcessingState = true;
			}
			else
			{
				dataCopyier->start(confirmed.area.addr,
							   	   processed.area.addr,
								   (uint32_t)(confirmed.selection.addr - confirmed.area.addr) );
			}

			break;
		case enProcessingState::copyingBeforeProcessing:
			startCommonProcess();
			break;
		case enProcessingState::processingSelection:
			if(processingWithoutCopying)
			{
				endProcessingState = true;
			}
			else
			{
				dataCopyier->start((int16_t*)(confirmed.selection.addr + confirmed.selection.length) ,
								   (int16_t *)(processed.selection.addr + processed.selection.length),
								   confirmed.area.length - (confirmed.selection.length + (uint32_t)(confirmed.selection.addr - confirmed.area.addr)) );
			}
			break;
		case enProcessingState::copyingAfterProcessing:
			processing.processParams.state = false;
			apply.isProcessData = 1;
			undo.isEnable = 0;
			if(processing.isApplyOnEnd)
			{
				startApply();
				processing.isApplyOnEnd = false;
			}
			break;
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


bool mtEffect::startCommonProcess()
{
	startProcess();

	return true;
}
void mtEffect::updateCommonProcess()
{

	int32_t blockSize = updateProcess();

	if(blockSize != - 1) processing.processParams.currentProgressValue += blockSize;

	if(!getProcessState()) endProcessingState = true;
}

void mtEffect::updateCopying()
{
	int32_t blockSize = dataCopyier->update();

	if(blockSize != - 1) processing.processParams.currentProgressValue += blockSize;

	if(!dataCopyier->getState()) endProcessingState = true;
}

bool mtEffect::getIsProcessedData()
{
	return apply.isProcessData;
}

void mtEffect::clearIsProcessedData()
{
	apply.isProcessData = false;
}

uint32_t mtEffect::getAreaLengthAfterProcessing()
{
	uint32_t expectedSelectionLength = getExpectedProcessLength();
	uint32_t expectedAreaLength = confirmed.area.length + (expectedSelectionLength - confirmed.selection.length);

	return expectedAreaLength;

}
//***********PREVIEW GETTERS
int16_t * const mtEffect::getAddresToPreview()
{
	return processed.selection.addr;
}
uint32_t mtEffect::getLengthToPreview()
{
	return processed.selection.length;
}
//***********
//***********PLAY GETTERS
int16_t * mtEffect::getAddresToPlay()
{
	return confirmed.area.addr;
}
uint32_t  mtEffect::getLengthToPlay()
{
	return confirmed.area.length;
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

	length = (uint32_t)((uint32_t) b * (float)(confirmed.area.length)/MAX_16BIT);
	addressShift = (uint32_t)( (uint32_t) a * (float)(confirmed.area.length)/MAX_16BIT);

	confirmed.selection.addr = confirmed.area.addr + addressShift;
	confirmed.selection.length = length - addressShift;

	if(!processing.isLoadedData)
	{
		loading.preloadPoints.a = a;
		loading.preloadPoints.b = b;
	}

}

uint16_t mtEffect::getNewEndPoint()
{
	uint16_t startPoint = getNewStartPoint();

	return startPoint + (uint16_t)((confirmed.selection.length / (float) confirmed.area.length) * MAX_16BIT);
}
uint16_t mtEffect::getNewStartPoint()
{
	return (uint16_t)(( (uint32_t)(confirmed.selection.addr - confirmed.area.addr) / (float) confirmed.area.length) * MAX_16BIT);
}
//***********
bool mtEffect::getState(strProcessing * ptr)
{
	return ptr->state;
}

uint8_t mtEffect::getProgress(strProcessing * ptr)
{
	return 100 * (ptr->currentProgressValue/ (float) ptr->maxProgressValue);
}
//***********VIRTUAL SET PARAMS
void mtEffect::setParamiter(void * ptr, uint8_t n)
{

}
//***********
//***********VIRTUAL PROCESSING
bool mtEffect::startProcess()
{
	return true;
}
int32_t mtEffect::updateProcess()
{
	return 0;
};
bool mtEffect::getProcessState()
{
	return true;
};
uint32_t mtEffect::getExpectedProcessLength()
{
	return 0;
};
//***********
