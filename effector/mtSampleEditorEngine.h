#ifndef EFFECTOR_MTSAMPLEEDITORENGINE_H_
#define EFFECTOR_MTSAMPLEEDITORENGINE_H_
#include "mtEffect.h"

enum enSampleEditorEffect
{
	editorEffectCut,
	editorEffectReverse,
	editorEffectAmplifier,
	editorEffectDelay,
	editorEffectBitcrusher,

//**********************//
	editorEffectMax
};


extern mtEffect * sampleEditorEffect[editorEffectMax];

namespace delayParams
{
//******************DELAY PARAMS
	extern const char * const labelText[2];
	extern const char paramsType[2];
	extern int iUpConstrain[2];
	extern float fUpConstrain[2];
	extern int iDownConstrain[2];
	extern float fDownConstrain[2];
	extern float changeStep[2];
	extern float displayMult[2];
	extern const char * const afterValueText[2];
//*********************
}

namespace bitcrusherParams
{
//******************BITCRUSHER PARAMS
	extern const char * const labelText[2];
	extern const char paramsType[2];
	extern int iUpConstrain[2];
	extern float fUpConstrain[2];
	extern int iDownConstrain[2];
	extern float fDownConstrain[2];
	extern float changeStep[2];
	extern float displayMult[2];
	extern const char * const afterValueText[2];
//*********************
}
namespace amplifierParams
{
extern const char * const labelText[1];
extern const char paramsType[1];
extern int iUpConstrain[1];
extern float fUpConstrain[1];
extern int iDownConstrain[1];
extern float fDownConstrain[1];
extern float changeStep[1];
extern float displayMult[1];
extern const char * const afterValueText[1];
}

#endif /* EFFECTOR_MTSAMPLEEDITORENGINE_H_ */
