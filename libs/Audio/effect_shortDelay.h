#ifndef LIBS_AUDIO_EFFECT_SHORTDELAY_H_
#define LIBS_AUDIO_EFFECT_SHORTDELAY_H_
#include "Arduino.h"
#include "AudioStream.h"

extern int16_t sdram_effectsBank[(15*512*1024)/2];
constexpr uint8_t MAX_SHORT_DELAY_VOICES = 12;

class AudioEffectShortDelay : public AudioStream
{
public:
  AudioEffectShortDelay(void):
  AudioStream(1,inputQueueArray)
  {
	  begin(0.5,500);
	  blockUpdate();
  }

  void begin(float f, uint32_t t);
  void setFeedback(float f);
  void setTime(uint16_t t);
  virtual void update(void);
  void clear();
  void blockUpdate();
  void unblockUpdate();

private:
  int16_t * const startDelayLine = sdram_effectsBank;
  audio_block_t *inputQueueArray[1];
  int16_t * currentTail;
  uint8_t delayVoicesNumber;
  uint32_t timeInSamples;
  uint32_t delaylineLength;
  uint32_t bufferedDataLength;
  uint8_t feedbackVoiceMult[MAX_SHORT_DELAY_VOICES];
  uint32_t feedbackVoiceShift[MAX_SHORT_DELAY_VOICES];
  bool noRefresh;
  bool isPingpong;
};

#endif /* LIBS_AUDIO_EFFECT_SHORTDELAY_H_ */
