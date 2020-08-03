#ifndef LIBS_AUDIO_EFFECT_LIMITER_H_
#define LIBS_AUDIO_EFFECT_LIMITER_H_

#include "Arduino.h"
#include "AudioStream.h"

constexpr uint8_t LIMITER_DELAY = 5;

class AudioEffectLimiter :
public AudioStream
{
public:
  AudioEffectLimiter(void):
  AudioStream(1,inputQueueArray)
  { }

  void begin(uint16_t ts, uint16_t a, uint16_t r);
  void setThreshold(uint16_t threshold);
  void setAttack(uint16_t attack);
  void setRelease(float release);
  virtual void update(void);

private:
  audio_block_t *inputQueueArray[1];
  int16_t buffer[LIMITER_DELAY];
  uint16_t threshold;
  float g=1.0;
  uint16_t xpeak;
  float attack;
  float releaseTime;
  uint16_t a;
  float coeff;
  float f;
};



#endif /* LIBS_AUDIO_EFFECT_LIMITER_H_ */
