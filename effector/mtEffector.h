#ifndef SOURCE_MTEFFECTOR_H_
#define SOURCE_MTEFFECTOR_H_

#include "mtFileManager.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"
#include "mtEffectorChorus.h"
#include "mtEffectorFlanger.h"
#include "mtEffectorDelay.h"
#include "mtEffectorLimiter.h"
#include "mtEffectorCompressor.h"
#include "mtEffectorAmplifier.h"
#include "mtEffectorBitcrusher.h"

#define UNDO_QUEUE		10



class mtEffector
{
public:
	mtEffector()
	{
		startAddressEffect = previewBuffer;
	}

	enum struct enSaveStatus
	{
		waitingForSaveInit,
		saving,
		saveDone
	};

	void loadSample(const char *patch);
	void play(uint16_t start, uint16_t stop, uint8_t pad);
	void playPrev(uint8_t pad);
	void stop(uint8_t pad);
	void trim(uint16_t a, uint16_t b);
	void undoTrim();
	void reverse(uint16_t start, uint16_t end);
	void undoReverse();
	void save(const char *patch);
	uint8_t saveUpdate();
	enSaveStatus getSaveStatus();
	void setSaveStatus(enSaveStatus status);
	void setEffects();
	void clearMainBuffer();

	friend class mtChorus;
	friend class mtFlanger;
	friend class mtDelay;
	friend class mtLimiter;
	friend class mtCompressor;
	friend class mtAmplifier;
	friend class mtBitcrusher;

	int16_t * getAddress();
	int32_t getLength();

	int32_t getEffectLength();
	int16_t *getEffectAddress();

private:
	void swap(int16_t *p1, int16_t *p2);
	void writeOutHeader();
	int16_t * currentAddress;
	int16_t * startAddress;

	int16_t * startAddressEffect;
	uint32_t affterEffectLength; // w probkach


	int32_t bytesSaved = 0; // w bajtach

	uint32_t saveLength;
	uint32_t saveLengthMax;
	enSaveStatus saveStatus;

	uint32_t undoCropLength;
	int16_t *undoCropStart;

	uint16_t undoReverseStart;
	uint16_t undoReverseEnd;

	int16_t * const applyBuffer = sdram_effectsBank;
	int16_t * const previewBuffer = sdram_effectsBank + SAMPLE_EFFECTOR_LENGTH_MAX;

	SdFile file;
};

extern int16_t sdram_effectsBank[15*256*1024];
extern int16_t sdram_sampleBank[15*256*1024];
extern mtEffector effector;
#endif /* SOURCE_MTEFFECTOR_H_ */
