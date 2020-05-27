#include "mtSampleEditorEngine.h"
#include "mtEffect.h"
#include "mtEffectDelay.h"
#include "mtEffectBitcrusher.h"
#include "mtEffectCut.h"
#include "mtEffectReverse.h"
#include "mtEffectAmplifier.h"
#include "mtEffectChorus.h"
#include "mtEffectFlanger.h"
#include "mtEffectLimiter.h"
#include "mtEffectCompresor.h"
#include "mtWavetableSmoother.h"
#include "mtEffectNormalizer.h"

mtEffect * sampleEditorEffect[editorEffectMax] =
{
		&sampleEditorCut,
		&sampleEditorReverse,
		&sampleEditorAmplifier,
		&sampleEditorDelay,
		&sampleEditorBitcrusher,
		&sampleEditorChorus,
		&sampleEditorFlanger,
		&sampleEditorLimiter,
		&sampleEditorCompresor,
		&sampleEditorWavetableSmoother,
		&sampleEditorNormalizer
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
	float displayMult[2] = {100, 1};

	const char * const afterValueText[2] =
	{
		(const char * const)" %",
		(const char * const)" ms"
	};

	const char displayType[2] = {'d','d'};
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

	const char displayType[2] = {'d','d'};
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

	const char displayType[1] = {'f'};
}

namespace chorusParams
{
	const char * const labelText[2] =
	{
		(const char * const)"Length",
		(const char * const)"Voices"
	};

	const char paramsType[2] = {'d','d'};

	int iUpConstrain[2] = { 4096 , 8};
	float fUpConstrain[2]= {0.0, 0.0};

	int iDownConstrain[2] = { 128 , 1};
	float fDownConstrain[2]= {0.0, 0.00};

	float changeStep[2] = { 128 , 1};
	float displayMult[2] = {1/44.1, 1};

	const char * const afterValueText[2] =
	{
		" ms",
		nullptr
	};

	const char displayType[2] = {'d','d'};
}

namespace flangerParams
{
	const char * const labelText[4] =
	{
		(const char * const)"Length",
		(const char * const)"Offset",
		(const char * const)"Depth",
		(const char * const)"Rate"
	};

	const char paramsType[4] = {'d','d','d','f'};

	int iUpConstrain[4] = { 4096, 100, 8,0};
	float fUpConstrain[4]= {0.0,0.0,0.0,5.0};

	int iDownConstrain[4] = {128, 1, 1, 0};
	float fDownConstrain[4]= {0.0, 0.0, 0.0, 0.1};

	float changeStep[4] = { 128, 1, 1, 0.1};
	float displayMult[4] = {1/44.1, 1, 1, 1};

	const char * const afterValueText[4] =
	{
		(const char * const)" ms",
		(const char * const)" %",
		nullptr,
		(const char * const)" Hz"
	};

	const char displayType[4] = {'d','d','d','f'};
}

namespace limiterParams
{
	const char * const labelText[3] =
	{
		(const char * const)"Threshold",
		(const char * const)"Attack",
		(const char * const)"Release"
	};

	const char paramsType[3] = {'d','f','f'};

	int iUpConstrain[3] = { 32767, 0, 0};
	float fUpConstrain[3]= {0.0,1000.0,10.0};

	int iDownConstrain[3] = {0, 0, 0};
	float fDownConstrain[3]= {0.0, 1.0, 0.02};

	float changeStep[3] = { 32767/100.0, 1, 0.01};
	float displayMult[3] = {100/32767.0, 1, 1};

	const char * const afterValueText[3] =
	{
		(const char * const)" %",
		(const char * const)" ms",
		(const char * const)" ms"
	};

	const char displayType[3] = {'d','d','f'};
}

namespace compressorParams
{
	const char * const labelText[4] =
	{
		(const char * const)"Threshold",
		(const char * const)"Ratio",
		(const char * const)"Attack",
		(const char * const)"Release",
	};

	const char paramsType[4] = {'d','d','d','d'};

	int iUpConstrain[4] = { 32767, 10, 2600, 5000};
	float fUpConstrain[4]= {0.0,0.0,0.0, 0.0};

	int iDownConstrain[4] = {0, 1, 1, 1};
	float fDownConstrain[4]= {0.0, 0.0, 0.0, 0.0};

	float changeStep[4] = { 32767/100.0, 1, 25, 50};
	float displayMult[4] = {100/32767.0, 1, 1, 1};

	const char * const afterValueText[4] =
	{
		(const char * const)" %",
		nullptr,
		(const char * const)" ms",
		(const char * const)" ms"
	};

	const char displayType[4] = {'d','d','d','d'};
}

namespace wavetableSmootherParams
{
	const char * const labelText[2] =
	{
		(const char * const)"Window Num",
		(const char * const)"Window Len",
	};

	const char paramsType[2] = {'d','d'};

	int iUpConstrain[2] = {256, 2048};
	float fUpConstrain[2]= {0.0,0.0};

	int iDownConstrain[2] = {1, 1};
	float fDownConstrain[2]= {0.0, 0.0};

	float changeStep[2] = {1, 1};
	float displayMult[2] = {1, 1};

	const char * const afterValueText[2] =
	{
		nullptr,
		nullptr
	};

	const char displayType[2] = {'d','d'};
}

namespace normalizerParams
{
	const char * const labelText[1] =
	{
		(const char * const)"Level"
	};

	const char paramsType[1] = {'d'};

	int iUpConstrain[1] = {100};
	float fUpConstrain[1]= {0.0};

	int iDownConstrain[1] = {0};
	float fDownConstrain[1]= {0.0};

	float changeStep[1] = {1};
	float displayMult[1] = {1};

	const char * const afterValueText[1] =
	{
		"%",
	};

	const char displayType[1] = {'d'};
}

//******************************
