#ifndef EFFECTOR_MTSAMPLEEDITORENGINE_H_
#define EFFECTOR_MTSAMPLEEDITORENGINE_H_
#include "mtEffect.h"

enum enSampleEditorEffect
{
	editorEffectNormalizer,
	editorEffectCut,
	editorEffectReverse,
	editorEffectAmplifier,
	editorEffectDelay,
	editorEffectBitcrusher,
	editorEffectChorus,
	editorEffectFlanger,
	editorEffectLimiter,
	editorEffectCompresor,
	editorEffectWavetableSmoother,


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
	extern const char displayType[2];
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
	extern const char displayType[2];
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
	extern const char displayType[1];
}

namespace chorusParams
{
//******************CHORUS PARAMS
	extern const char * const labelText[2];
	extern const char paramsType[2];
	extern int iUpConstrain[2];
	extern float fUpConstrain[2];
	extern int iDownConstrain[2];
	extern float fDownConstrain[2];
	extern float changeStep[2];
	extern float displayMult[2];
	extern const char * const afterValueText[2];
	extern const char displayType[2];
//*********************
}

namespace flangerParams
{
//******************CHORUS PARAMS
	extern const char * const labelText[4];
	extern const char paramsType[4];
	extern int iUpConstrain[4];
	extern float fUpConstrain[4];
	extern int iDownConstrain[4];
	extern float fDownConstrain[4];
	extern float changeStep[4];
	extern float displayMult[4];
	extern const char * const afterValueText[4];
	extern const char displayType[4];
//*********************
}

namespace limiterParams
{
//******************CHORUS PARAMS
	extern const char * const labelText[3];
	extern const char paramsType[3];
	extern int iUpConstrain[3];
	extern float fUpConstrain[3];
	extern int iDownConstrain[3];
	extern float fDownConstrain[3];
	extern float changeStep[3];
	extern float displayMult[3];
	extern const char * const afterValueText[3];
	extern const char displayType[3];
//*********************
}

namespace compressorParams
{
//******************CHORUS PARAMS
	extern const char * const labelText[4];
	extern const char paramsType[4];
	extern int iUpConstrain[4];
	extern float fUpConstrain[4];
	extern int iDownConstrain[4];
	extern float fDownConstrain[4];
	extern float changeStep[4];
	extern float displayMult[4];
	extern const char * const afterValueText[4];
	extern const char displayType[4];
//*********************
}

namespace wavetableSmootherParams
{
//******************CHORUS PARAMS
	extern const char * const labelText[2];
	extern const char paramsType[2];
	extern int iUpConstrain[2];
	extern float fUpConstrain[2];
	extern int iDownConstrain[2];
	extern float fDownConstrain[2];
	extern float changeStep[2];
	extern float displayMult[2];
	extern const char * const afterValueText[2];
	extern const char displayType[2];
//*********************
}

namespace normalizerParams
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
	extern const char displayType[1];
}

#endif /* EFFECTOR_MTSAMPLEEDITORENGINE_H_ */
