#ifndef LIBS_AUDIO_EFFECT_DECLICK_H_
#define LIBS_AUDIO_EFFECT_DECLICK_H_

#include "Arduino.h"
#include "AudioStream.h"

class AudioEffectDeclick :
public AudioStream
{
public:
  AudioEffectDeclick(void):
  AudioStream(1,inputQueueArray)
  { }
  virtual void update(void);

  void setNextBufferCrossfade() { needCrossfade = true; };

private:
  audio_block_t *inputQueueArray[1];

  int16_t lastSample;
  bool needCrossfade;
};



#endif /* LIBS_AUDIO_EFFECT_LIMITER_H_ */
