#ifndef EFFECTOR_MTSAMPLEEDITORENGINE_H_
#define EFFECTOR_MTSAMPLEEDITORENGINE_H_
#include "mtEffect.h"

enum enSampleEditorEffect
{
	editorEffectDelay,
	editorEffectBitcrusher,

//**********************//
	editorEffectMax
};


extern mtEffect * sampleEditorEffect[editorEffectMax];


#endif /* EFFECTOR_MTSAMPLEEDITORENGINE_H_ */
