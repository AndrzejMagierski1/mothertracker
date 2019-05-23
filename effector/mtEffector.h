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

class mtEffector
{
public:

	void loadSample(const char *patch);
	void play(uint16_t start, uint16_t stop);
	void playPrev();
	void stop();
	void trim(uint16_t a, uint16_t b);
	void save(const char *patch);
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

	FsFile file;
};

extern int16_t sdram_effectsBank[4*1024*1024];
extern int16_t sdram_sampleBank[4*1024*1024];
extern mtEffector effector;
#endif /* SOURCE_MTEFFECTOR_H_ */
