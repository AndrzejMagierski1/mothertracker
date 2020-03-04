#ifndef EFFECTOR_MTSAMPLEEDITORENGINE_CPP_
#define EFFECTOR_MTSAMPLEEDITORENGINE_CPP_
#include "mtSampleEditorEngine.h"
#include "mtEffect.h"
#include "mtEffectDelay.h"

mtEffect * sampleEditorEffect[editorEffectMax] =
{
		&sampleEditorDelay
};

#endif /* EFFECTOR_MTSAMPLEEDITORENGINE_CPP_ */
