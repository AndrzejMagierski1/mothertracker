#include "mtSampleEditorEngine.h"
#include "mtEffect.h"
#include "mtEffectDelay.h"
#include "mtEffectBitcrusher.h"

mtEffect * sampleEditorEffect[editorEffectMax] =
{
		&sampleEditorDelay,
		&sampleEditorBitcrusher
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

//******************************
