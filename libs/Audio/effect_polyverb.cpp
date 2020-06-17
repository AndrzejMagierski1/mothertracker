// Copyright 2020 Wojciech Jakóbczyk (jakobczyk.woj@gmail.com)

#include "effect_polyverb.h"

extern uint16_t sdram_reverbMemory[32768];

AudioEffectPolyverb::AudioEffectPolyverb() : AudioStream(2, inputQueueArray)
{
  buffer = sdram_reverbMemory; //static_cast<uint16_t*>(malloc(reverb.GetBufferSize()));
  reverb.Init(buffer);

  reverb.SetInputGain(0.1f);
  reverb.SetDiffusion(0.625f);
  setTime(0.7f);
  setDamp(0.2f);
  setAmount(0.8f);
}

AudioEffectPolyverb::~AudioEffectPolyverb() {
//  free(buffer);
	buffer = nullptr;
}

void AudioEffectPolyverb::update(void)
{
  audio_block_t *block_left;
  audio_block_t *block_right;

  block_left = receiveWritable(0);
  if (block_left == NULL)
    return;
  block_right = receiveWritable(1);
  if (block_right == NULL) {
    release(block_left);
    return;
  }

  reverb.Process(block_left->data, block_right->data, AUDIO_BLOCK_SAMPLES);

  transmit(block_left, 0);
  transmit(block_right, 1);

  release(block_left);
  release(block_right);
}

void AudioEffectPolyverb::reset(void)
{
  reverb.Reset();
}

bool AudioEffectPolyverb::isActive(void)
{
  return reverb.IsActive();
}

void AudioEffectPolyverb::setActiveThreshold(float threshold) {
  reverb.SetActiveThreshold(threshold);
}
