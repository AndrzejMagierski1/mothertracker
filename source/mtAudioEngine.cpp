#include "mtAudioEngine.h"

AudioPlayMemory          playMem1;
AudioOutputI2S           i2s1;

AudioConnection          patchCord1(playMem1, 0, i2s1, 0);
AudioConnection          patchCord2(playMem1, 0, i2s1, 1);



