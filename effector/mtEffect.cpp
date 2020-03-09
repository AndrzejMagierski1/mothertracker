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
		if(loading.isProcessOnEnd) startProcessingSelection();
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
	uint32_t expectedSelectLen = getExpectedProcessLength(confirmed.selection.length);
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
		case enProcessingState::copyingBeforeProcessing:	updateCopying();		break;
		case enProcessingState::processingSelection:		updateCommonProcess();	break;
		case enProcessingState::copyingAfterProcessing:		updateCopying();		break;
		default: break;
	}

	if(endProcessingState)
	{
		endProcessingState = 0;

		switch(processingState)
		{
		case enProcessingState::idle:
			dataCopyier->start(confirmed.area.addr,
							   processed.area.addr,
							  (uint32_t)(confirmed.selection.addr - confirmed.area.addr) );
			break;
		case enProcessingState::copyingBeforeProcessing:
			startCommonProcess();
			break;
		case enProcessingState::processingSelection:
			dataCopyier->start((int16_t*)(confirmed.selection.addr + confirmed.selection.length) ,
							   (int16_t *)(processed.selection.addr + processed.selection.length),
							   confirmed.area.length - (confirmed.selection.length + (uint32_t)(confirmed.selection.addr - confirmed.area.addr)) );
			break;
		case enProcessingState::copyingAfterProcessing:
			processing.processParams.state = false;
			apply.isProcessData = 1;
			undo.isEnable = 0;
			if(processing.isApplyOnEnd) startApply();
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
}

uint16_t mtEffect::getNewEndPoint()
{
	return (confirmed.selection.length * MAX_16BIT) / confirmed.area.length;
}
uint16_t mtEffect::getNewStartPoint()
{
	return ( (uint32_t)(confirmed.selection.addr - confirmed.area.addr) * MAX_16BIT) / confirmed.area.length;
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
uint32_t mtEffect::getExpectedProcessLength(uint32_t selectLen)
{
	return 0;
};
//***********
