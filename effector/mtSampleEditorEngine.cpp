#include "mtSampleEditorEngine.h"
#include "mtEffect.h"
#include "mtEffectDelay.h"
#include "mtEffectBitcrusher.h"
#include "mtEffectCut.h"
#include "mtEffectReverse.h"
#include "mtEffectAmplifier.h"
#include "mtEffectChorus.h"

mtEffect * sampleEditorEffect[editorEffectMax] =
{
		&sampleEditorCut,
		&sampleEditorReverse,
		&sampleEditorAmplifier,
		&sampleEditorDelay,
		&sampleEditorBitcrusher,
		&sampleEditorChorus
};

//******************************DELAY

namespace delayParams
{
	const char * const labelText[2] =
	{
		(const char * const)"Feedback",
		(const char * const)"Time"
	};

	const char paramsType[2] = {'f','d'};

	int iUpConstrain[2] = { 0  , 5000};
	float fUpConstrain[2]= {0.95, 0 };

	int iDownConstrain[2] = { 0 , 1};
	float fDownConstrain[2]= {0.01, 0.0};

	float changeStep[2] = { 0.01 , 5};
	float displayMult[2] = {1, 1};

	const char * const afterValueText[2] =
	{
		nullptr,
		(const char * const)" ms"
	};
}

namespace bitcrusherParams
{
	const char * const labelText[2] =
	{
		(const char * const)"Bits",
		(const char * const)"Rate"
	};

	const char paramsType[2] = {'d','d'};

	int iUpConstrain[2] = { 16 , 44100};
	float fUpConstrain[2]= {0.0, 0.0};

	int iDownConstrain[2] = { 1 , 500};
	float fDownConstrain[2]= {0.0, 0.00};

	float changeStep[2] = { 1 , 100};
	float displayMult[2] = {1, 1};

	const char * const afterValueText[2] =
	{
		nullptr,
		(const char * const)" Hz"
	};
}

namespace amplifierParams
{
	const char * const labelText[1] =
	{
		(const char * const)"Amplifier"
	};

	const char paramsType[1] = {'f'};

	int iUpConstrain[1] = {0};
	float fUpConstrain[1]= {5.0};

	int iDownConstrain[1] = {0};
	float fDownConstrain[1]= {0.01};

	float changeStep[1] = { 0.01};
	float displayMult[1] = {1};

	const char * const afterValueText[1] =
	{
		nullptr,
	};
}

namespace chorusParams
{
	const char * const labelText[2] =
	{
		(const char * const)"Depth",
		(const char * const)"Voices"
	};

	const char paramsType[2] = {'d','d'};

	int iUpConstrain[2] = { 4096 , 8};
	float fUpConstrain[2]= {0.0, 0.0};

	int iDownConstrain[2] = { 128 , 1};
	float fDownConstrain[2]= {0.0, 0.00};

	float changeStep[2] = { 128 , 1};
	float displayMult[2] = {1, 1};

	const char * const afterValueText[2] =
	{
		nullptr,
		nullptr
	};
}
//******************************
