#ifndef EFFECTOR_MTSAMPLEEDITORENGINE_H_
#define EFFECTOR_MTSAMPLEEDITORENGINE_H_
#include "mtEffect.h"
#include "mtEffectDelay.h"

enum enSampleEditorEffect
{
	editorEffectDelay,


//**********************//
	editorEffectMax
};


extern mtEffect * sampleEditorEffect[editorEffectMax];


#endif /* EFFECTOR_MTSAMPLEEDITORENGINE_H_ */
