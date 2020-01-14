
#include <mtLFO.h>


// global tempo value
static float *lfoGlobalTempoValue = NULL;

//-------------------------------------------------------
/*LFO::LFO(uint16_t *rate, uint16_t *wave, uint16_t *amount)
{
	setRate(rate);
	setWave(wave);
	setAmount(amount);
	lfoRateChange = 0;
	lfoOldTRatio = 0;
	lfoStarted = 0;
	//lfoRateModSet = 0;
	//lfoWaveModSet = 0;
	//lfoAmountModSet = 0;
}
*/
LFO::LFO()
{
	lfoRateChange = 0;
	lfoOldTRatio = 0;
	lfoStarted = 0;
	lfoRateValue = 0;
	//lfoRateModSet = 0;
	//lfoWaveModSet = 0;
	//lfoAmountModSet = 0;
}

void LFO::start()
{
	lfoMicros = 0;
	lfoStarted = 1;
}

void LFO::stop()
{
	lfoRateChange = 0;
	lfoOldTRatio = 0;
	lfoStarted = 0;
}

//-------------------------------------------------------

void LFO::init(strLfo * _lfo)
{


	lfo = _lfo;
	setRateMod(&mod);
	setWaveMod(&mod);
	setAmountMod(&mod);
}

void LFO::setSyncMode(uint8_t *sync)
{
	lfoSyncFlag = sync;
}

void LFO::setGlobalTempoValue(float *tempo)
{
	lfoGlobalTempoValue = tempo;
}

void LFO::setRateMod(float *rate_mod)
{
	lfoRateMod = rate_mod;
}

void LFO::setWaveMod(float *wave_mod)
{
	lfoWaveMod = wave_mod;
}

void LFO::setAmountMod(float *amount_mod)
{
	lfoAmountMod = amount_mod;	
}

float LFO::getFrequency()
{
	return lfoFrequency;
}

uint16_t LFO::getBPM()
{
	return lfoBPM;
}

//-------------------------------------------------------
void LFO::updateRate()
{
	// sprawdzenie wskaznikow rate
	if(lfoRateMod == NULL || lfo == NULL) return;

	// sprawdzenie wskaznika synca oraz zmiany wzgledem poaprzedniej wartosci
	if(lfoSyncLastFlag != lfo->sync) 
	{
		lfoSyncChange = 1;
		lfoSyncLastFlag = lfo->sync;
	}

	// sprawdzenie wskaznika tempa oraz zmiany wzgledem poaprzedniej wartosci
	if(lfoGlobalTempoValue)
	{
		if(lfoTempoLastValue != *lfoGlobalTempoValue) 
		{
			lfoTempoChange = 1;
			lfoTempoLastValue = *lfoGlobalTempoValue;
			//Serial.print("tempo change: ");
			//Serial.println(lfoTempoLastValue);
		}
	}

	// sprawdzanie rate mod change
	if(lfoRateModLastValue != (*lfoRateMod))
	{
		lfoRateModLastValue = *lfoRateMod;
		lfoRateModChange = 1;
	}


 	// sprawdzenie zmiany rate wzgledem poaprzednich wartosci
	if(lfoRateModChange || lfoRateLastValue != lfo->rate)
	{
		lfoRateLastValue = lfo->rate;

		// sprawdzenie czy rate jest modyfikowany
		if(lfoRateModLastValue != 0)
		{
			lfoRateValue = lfoRateLastValue + (lfoRateModLastValue * 4095.0);
			if(lfoRateValue < 0) lfoRateValue = 0;
			else if(lfoRateValue > 4095) lfoRateValue = 4095;
		}
		else
		{
			lfoRateValue = lfoRateLastValue;
		}

		lfoRateChange = 1;
	}

	// w przypadku zmian przeliczenie wartosci okresu (lfoT)
	if(lfoRateChange || lfoSyncChange || (lfoSyncLastFlag && lfoTempoChange))
	{
		if(lfoSyncLastFlag) // synchroniacja rate z tempem
		{
			//uint8_t actual_sync_level = (lfoRateValue*((TEMPO_SYNC_LEVELS*TEMPO_SYNC_LEVELS_TYPES)-1))/4095; // 0-23
			//lfoFrequency = ((lfoTempoLastValue/240.0)*tempoSyncRateLevelTypes[actual_sync_level%TEMPO_SYNC_LEVELS_TYPES])
			//				/tempoSyncRateLevel[actual_sync_level/TEMPO_SYNC_LEVELS_TYPES];

			uint8_t actual_sync_level = (lfoRateValue*(TEMPO_SYNC_TYPES-1))/4095; // 0-19

			lfoFrequency = (lfoTempoLastValue/240.0)/tempoSyncRates[actual_sync_level];
			//Serial.print("freq change:  ");
			//Serial.println(lfoFrequency);
		}
		else // bez synchronizacji, obliczenie wartosci z zakresu 0.1-30 Hz
		{
			if(lfoRateValue <= 100) 		lfoFrequency = 0.016;
			else if(lfoRateValue <= 1200) 	lfoFrequency = ((lfoRateValue-100)*0.984)/1100 + 0.016;
			else if(lfoRateValue < 1300) 	lfoFrequency = 1;
			else if(lfoRateValue < 4096)	lfoFrequency = ((lfoRateValue-1300)*29.0)/(2795)+1;  // 4095-1300=2795
		}

		lfoSyncChange = 0;

		lfoTempoChange = 0;
		lfoRateChange = 1;
		lfoBPM = lfoFrequency*60;
		lfoT = 1000000.0/lfoFrequency;
		//Serial.print("t change:     ");
		//Serial.println(lfoT);
		//Serial.println();
 	}
}

void LFO::calculateRatio()
{
	if(!lfoRateModChange && lfoRateChange)
	{
		// jesli zmienilo sie rate ale nie zmienilo rateMod
		// obliczenie nowego czasu dla poprzedniego ratio
		// aby wartosc na wyjsciu byla plynnie zmieniona bez skoku
		lfoTPlace = lfoTRatio * lfoT;
		lfoRateModChange = 0;
	}
	else 
	{
		lfoTPlace = lfoMicros;
	}

	lfoRateChange = 0;
	lfoRateModChange = 0;

	// obliczenie aktualnego czasu w okresie z reszty dzielenia calkowitego czasu przez okres 
	lfoTPlace = fmod(lfoTPlace, lfoT);

	// zapisanie do timera aktyalnego czasu w okresie
	lfoMicros = lfoTPlace;

	// obliczenie aktualnej pozycji w okresie 0.0 - 1.0 na podstawie aktualnego czasu i wartosci okresu
	lfoTRatio = lfoTPlace / lfoT;
}

float LFO::lfoSinus()
{
	return sinf((2 * PI) * lfoTRatio);
}

float LFO::lfoSqrt()
{
	if(lfoTRatio < 0.5) return 1.0;
	return -1.0;
}

float LFO::lfoTriangle(float shape)
{
	if(shape == 0) return ((1-lfoTRatio)*2)-1;
	if(shape == 1) return (lfoTRatio<0.5)?(lfoTRatio*2):((lfoTRatio-1)*2);
	else if(lfoTRatio < shape/2) return ((lfoTRatio/shape)*2);
	else if(lfoTRatio > 1-shape/2) return (lfoTRatio-(1-shape/2))/(1-(1-shape/2))-1;
	else return (2-((2*lfoTRatio-shape)/(1-shape)))-1;
	return 0;
}

//-------------------------------------------------------
float LFO::getOut()
{
	if(!lfo->enable)
	{
		return 0.0;
	}

	if(lfoAmountMod == NULL || lfoWaveMod == NULL || !lfoStarted) return 0;

	if(lfoAmountModLastValue != (*lfoAmountMod) || lfoAmountLastValue != lfo->amount)
	{
		lfoAmountLastValue = lfo->amount;
 
		if((*lfoAmountMod) != 0)
		{
			lfoAmountModLastValue = (*lfoAmountMod);
			lfoAmountValue = lfoAmountLastValue + (lfoAmountModLastValue * 4095.0);
			if(lfoAmountValue < 0) lfoAmountValue = 0;
			else if(lfoAmountValue > 4095) lfoAmountValue = 4095;
		}
		else
		{
			lfoAmountValue = lfoAmountLastValue;
		}
	}

	updateRate();
	calculateRatio();

	if(lfoWaveModLastValue != (*lfoWaveMod) || lfoWaveLastValue != lfo->wave)
	{
		lfoWaveLastValue = lfo->wave;

		if((*lfoWaveMod) != 0)
		{
			lfoWaveModLastValue = (*lfoWaveMod);
			lfoWaveValue = lfoWaveLastValue + (lfoWaveModLastValue * 4095.0);
			if(lfoWaveValue < 0) lfoWaveValue = 0;
			else if(lfoWaveValue > 4095) lfoWaveValue = 4095;
		}
		else
		{
			lfoWaveValue = lfoWaveLastValue;
		}
	}

	if(lfoWaveValue == 0)
	{
		lfoOutput = lfoSinus();
	}
	else if(lfoWaveValue < 1023)
	{
		lfoOutput = (lfoSqrt()*lfoWaveValue)/1023.0 + lfoSinus()*(1-(lfoWaveValue/1023.0));
	}
	else if(lfoWaveValue == 1023)
	{
		lfoOutput = lfoSqrt();
	}
	else if(lfoWaveValue < 2047)
	{
		lfoOutput =  lfoTriangle(0)*((lfoWaveValue-1023)/1023.0) + lfoSqrt()*(1-((lfoWaveValue-1023)/1023.0));
	}
	else if(lfoWaveValue >= 2047)
	{
		lfoOutput = lfoTriangle((lfoWaveValue-2047)/2047.0);
	}
	lfoLastOutput = lfoOutput*( (lfoAmountValue <= 2000 ? lfoAmountValue/10 : map(lfoAmountValue, 2001 , 4095, 201 , 4095)) /4095.0);

	return lfoLastOutput;
}


float LFO::getLastOut()
{
	if(!lfo->enable)
	{
		return 0.0;
	}
	
	return lfoLastOutput;
}

//-------------------------------------------------------
float LFO::getSinus()
{
	if(!lfoStarted) return 0;
	calculateRatio();
	return lfoSinus()*((lfo->amount)/4095.0);
}

float LFO::getSqrt()
{
	if(!lfoStarted) return 0;
	calculateRatio();
	return lfoSqrt()*((lfo->amount)/4095.0);
}

float LFO::getTriangle()
{
	if(!lfoStarted) return 0;
	calculateRatio();
	return lfoTriangle(32)*((lfo->amount)/4095.0);
}

//#############################################################################
//#############################################################################
//#############################################################################

WaveFormGenerator::WaveFormGenerator()
{
	waveWave = WAVE_DEFAULT_WAVE;
}

WaveFormGenerator::WaveFormGenerator(uint16_t *wave)
{
	waveWave = WAVE_DEFAULT_WAVE;
	if(wavePointer != nullptr) setWave(wavePointer);
}

float WaveFormGenerator::waveSinus()
{
	return sinf((2 * PI) * wavePhase);
}

float WaveFormGenerator::waveSqrt()
{
	if(wavePhase < 0.5) return 1.0;
	return -1.0;
}

float WaveFormGenerator::waveTriangle(float shape)
{
	if(shape == 0) return ((1-wavePhase)*2)-1;
	if(shape == 1) return (wavePhase<0.5)?(wavePhase*2):((wavePhase-1)*2);
	else if(wavePhase < shape/2) return ((wavePhase/shape)*2);
	else if(wavePhase > 1-shape/2) return (wavePhase-(1-shape/2))/(1-(1-shape/2))-1;
	else return (2-((2*wavePhase-shape)/(1-shape)))-1;
	return 0;
}
/*
float WaveFormGenerator::getOut(float phase)
{
	wavePhase = phase;

	if(waveWave == 0)
	{
		return waveSinus();
	}
	else if(waveWave < 1023)
	{
		return 	(waveSqrt()*waveWave)/1023.0 + waveSinus()*(1-(waveWave/1023.0));
	}
	else if(waveWave == 1023)
	{
		return waveSqrt();
	}
	else if(waveWave < 2047)
	{
		return waveTriangle(0)*((waveWave-1023)/1023.0) + waveSqrt()*(1-((waveWave-1023)/1023.0));
	}
	else if(waveWave >= 2047)
	{
		return waveTriangle((waveWave-2047)/2048.0);
	}
}
*/
int16_t WaveFormGenerator::getOut(float phase)
{
	wavePhase = phase;
	waveWave = *wavePointer;

	if(waveWave == 0)
	{
		return waveSinus()*32767;
	}
	else if(waveWave < 1023)
	{
		return ((waveSqrt()*waveWave)/1023.0 + waveSinus()*(1-(waveWave/1023.0)))*32767;
	}
	else if(waveWave == 1023)
	{
		return waveSqrt()*32767;
	}
	else if(waveWave < 2047)
	{
		return (waveTriangle(0)*((waveWave-1023)/1023.0) + waveSqrt()*(1-((waveWave-1023)/1023.0)))*32767;
	}
	else if(waveWave >= 2047)
	{
		return waveTriangle((waveWave-2047)/2048.0)*32767;
	}
	return 0;
}

void WaveFormGenerator::generateWaveform(int16_t waveform[])
{
	for(uint16_t i = 0; i<WAVEFORM_SIZE; i++) 
	{
		waveform[i] = getOut(i/(WAVEFORM_SIZE-1.0));
	}
}

void WaveFormGenerator::setWave(uint16_t *wave)
{
	if(wavePointer != wave)
	{
		wavePointer = wave;
	}
}

uint16_t * WaveFormGenerator::getWave()
{
	return wavePointer;
}

uint8_t LFO::isStarted()
{
	return lfoStarted;
}
