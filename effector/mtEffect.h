#ifndef EFFECTOR_MTEFFECT_H_
#define EFFECTOR_MTEFFECT_H_

#include "stdint.h"
#include "mtDataCopyier.h"

constexpr uint32_t LOAD_BLOCK_SIZE_IN_SAMPLES = 8192;
extern const uint32_t SAMPLE_EFFECTOR_LENGTH_MAX;
extern int16_t sdram_effectsBank[15*256*1024];

class mtEffect
{
public:

	mtEffect();

//***********LOAD
	bool startLoad(uint8_t instr_idx);
	void updateLoad();
	bool getLoadState();
	uint8_t getLoadProgress();
//***********
//***********APPLY
	void startApply();
//***********
//***********SAVE
	bool startSave(uint8_t instr_idx);
	void updateSave();
	bool getSaveState();
	uint8_t getSaveProgress();
//***********
//***********PROCESSING

	enum enProcessingState
	{
		idle,
		copyingBeforeProcessing,
		processingSelection,
		copyingAfterProcessing
	};

	uint8_t processingState;

	bool 	endProcessingState = false;
	bool	startProcessingSelection();
	void 	updateProcessingSelection();
	bool	startCopying();
	uint8_t getProcessSelectionProgress();
	bool getProcessSelectionState();
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
	virtual void updateProcess();
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
		bool isApplyOnEnd = false;
		strProcessing processParams;
	} processing;

	bool isUpdateProcessing = true;

	mtDataCopyier dataCopyier;

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

	strProcessing loadProcessParams;

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



	bool getState(strProcessing * ptr);
	uint8_t getProgress(strProcessing * ptr);
};




#endif /* EFFECTOR_MTEFFECT_H_ */
