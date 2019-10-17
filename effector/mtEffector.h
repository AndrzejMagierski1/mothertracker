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

typedef enum
{
	waitingForSaveInit,
	saving,
	saveDone
}save_stages_t;

class mtEffector
{


public:

	void loadSample(const char *patch);
	void play(uint16_t start, uint16_t stop);
	void playPrev();
	void stop();
	void trim(uint16_t a, uint16_t b);
	void undoTrim();
	void reverse(uint16_t start, uint16_t end);
	void undoReverse();
	void save(const char *patch);
	uint8_t saveUpdate();
	save_stages_t getSaveStatus();
	void setSaveStatus(save_stages_t status);
	void setEffects();

	friend class mtChorus;
	friend class mtFlanger;
	friend class mtDelay;
	friend class mtLimiter;
	friend class mtCompressor;
	friend class mtAmplifier;
	friend class mtBitcrusher;

	int16_t * getAddress();
	int32_t getLength();

private:
	void swap(int16_t *p1, int16_t *p2);
	void writeOutHeader();
	int16_t * currentAddress;
	int16_t * startAddress;

	int16_t * startAddressEffect;
	uint32_t affterEffectLength; // w probkach

	uint32_t ChunkSize = 0L;
	uint32_t Subchunk1Size = 16;
	uint32_t AudioFormat = 1;
	uint32_t numChannels = 1;
	uint32_t sampleRate = 44100;
	uint32_t bitsPerSample = 16;
	uint32_t byteRate = sampleRate*numChannels*(bitsPerSample/8);
	uint32_t blockAlign = numChannels*bitsPerSample/8;
	uint32_t Subchunk2Size = 0;
	int32_t fileByteSaved = 0; // w bajtach
	uint32_t NumSamples = 0;
	uint8_t byte1, byte2, byte3, byte4;

	uint32_t saveLength;
	uint32_t saveLengthMax;
	save_stages_t saveStage;

	uint32_t undoCropLength;
	int16_t *undoCropStart;

	uint16_t undoReverseStart;
	uint16_t undoReverseEnd;

	int16_t * const applyBuffer = sdram_effectsBank;
	int16_t * const previewBuffer = sdram_effectsBank + SAMPLE_EFFECTOR_LENGTH_MAX;

	FsFile file;
};

extern int16_t sdram_effectsBank[15*256*1024];
extern int16_t sdram_sampleBank[15*256*1024];
extern mtEffector effector;
#endif /* SOURCE_MTEFFECTOR_H_ */
