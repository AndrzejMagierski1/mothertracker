#ifndef EFFECTOR_MTEFFECT_H_
#define EFFECTOR_MTEFFECT_H_

#include "stdint.h"
#include "mtStructs.h"
class mtDataCopyier;
extern mtDataCopyier globalDataCopyier;

constexpr uint32_t SAMPLE_EFFECTOR_LENGTH_MAX =  (15*128*1024);
extern int16_t sdram_effectsBank[15*256*1024];


class mtEffect //todo: dopisac procedure przekopiowujaca struktury przy zmianie obiektu na inny
{
public:
	enum enProcessingState
	{
		idle,
		copyingBeforeProcessing,
		processingSelection,
		copyingAfterProcessing
	};

	enum loadResult
	{
		loadIsAlreadyActive,
		instrTooLong,
		instrIsNotActive,
		loadSuccessfull
	};

	enum enOperationType
	{
		operationTypeIdle,
		operationTypeLoading,
		operationTypeProcessing,
		operationTypeSaveing
	} operationType;

	void update();

//***********LOAD
	loadResult startLoad(uint8_t instr_idx);
	void updateLoad();
	bool getLoadState();
	uint8_t getLoadProgress();
	void clearIsLoadedData();
//***********
//***********APPLY
	void startApply();
//***********
//***********UNDO
	void startUndo();
//***********
//***********SAVE
	bool startSave(uint8_t instr_idx);
	void updateSave();
	bool getSaveState();
	uint8_t getSaveProgress();
//***********
//***********PROCESSING
	uint8_t processingState;

	bool 	endProcessingState = false;
	bool	startProcessingSelection();
	void 	updateProcessingSelection();
	bool	startCopying();
	uint8_t getProcessSelectionProgress();
	bool 	getProcessSelectionState();
	bool 	startCommonProcess();
	void 	updateCommonProcess();
	void 	updateCopying();
//***********PREVIEW GETTERS
	int16_t * const getAddresToPreview();
	uint32_t getLengthToPreview();
//***********
//***********PLAY GETTERS
	int16_t * getAddresToPlay();
	uint32_t  getLengthToPlay();
//***********
//***********SELECTION SET
	void changeSelectionRange(uint16_t a, uint16_t b);
//***********
//***********VIRTUAL SET PARAMS
	virtual void setParams(void * ptr);
//***********
//***********VIRTUAL PROCESSING
	virtual bool startProcess();
	virtual int32_t updateProcess();
	virtual bool getProcessState();
	virtual uint32_t getExpectedProcessLength(uint32_t selectLen);
//***********
protected:
	struct strProcessing
	{
		bool state;
		uint32_t currentProgressValue;
		uint32_t maxProgressValue;
	};

	struct strProcessingParams
	{
		bool isLoadedData = false;
		bool isApplyOnEnd = false;
		strProcessing processParams;
	} processing;


	struct strMemoryAreaWithSelection
	{
		struct strMemoryArea
		{
			int16_t * addr;
			uint32_t length;
		} area, selection;
	};

	strMemoryAreaWithSelection processed =
	{ .area = { sdram_effectsBank + SAMPLE_EFFECTOR_LENGTH_MAX, 0},
	  .selection = { sdram_effectsBank + SAMPLE_EFFECTOR_LENGTH_MAX, 0},
	};

	strMemoryAreaWithSelection confirmed =
	{ .area = { sdram_effectsBank, 0},
	  .selection = { sdram_effectsBank, 0},
	};

private:

	struct strLoadingParams
	{
		bool isProcessOnEnd = false;
		strProcessing processParams;
	} loading;


	struct strSaveParams
	{
		//todo: file - waiting for ziejas sd library
		int16_t * currentSrcAddr = nullptr;
		strProcessing processParams;
	} save;

	struct strApplyParams
	{
		bool isProcessData = false;
	} apply;

	struct strUndo
	{
		bool isEnable = false;
	} undo;

	mtDataCopyier *const dataCopyier = &globalDataCopyier;

	bool getState(strProcessing * ptr);
	uint8_t getProgress(strProcessing * ptr);
};




#endif /* EFFECTOR_MTEFFECT_H_ */
