#ifndef LIBS_AUDIO_EFFECT_SHORTDELAY_H_
#define LIBS_AUDIO_EFFECT_SHORTDELAY_H_
#include "Arduino.h"
#include "AudioStream.h"

extern int16_t* sdram_ptrSampleBank;
extern int16_t* sdram_ptrEffectsBank;
extern int16_t* sdram_ptrDelayMemory;

constexpr uint8_t MAX_SHORT_DELAY_VOICES = 12;
constexpr uint16_t MAX_SHORT_DELAY_TIME = 3500;

class AudioEffectShortDelay : public AudioStream
{
public:
  AudioEffectShortDelay(void):
  AudioStream(1,inputQueueArray)
  {
	  begin(0.5,500,false,false,0);
	  blockUpdate();
  }

  void begin(float f, uint32_t t, bool pp, bool se, uint8_t sr);
  void setFeedback(float f);
  void setTime(uint16_t t);
  void setRate(uint8_t r);
  void setPingpongEnable(bool pp);
  void setSyncEnable(bool se);
  virtual void update(void);
  void clear();
  void blockUpdate();
  void unblockUpdate();
  void startFadeCount();
  bool getIsFaded();

private:
  int16_t * const startDelayLine = sdram_ptrDelayMemory;
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
  bool isSync;
  uint8_t syncRate;
  uint32_t processBufCounter;
  uint32_t processBufCounterFadeConstrain;
};

#endif /* LIBS_AUDIO_EFFECT_SHORTDELAY_H_ */
