#include "mtSampleEditorEngine.h"
#include "mtEffect.h"
#include "mtEffectDelay.h"
#include "mtEffectBitcrusher.h"

mtEffect * sampleEditorEffect[editorEffectMax] =
{
		&sampleEditorDelay,
		&sampleEditorBitcrusher
};

