#ifndef MTLFO_H
#define MTLFO_H

#include <stdint.h>
#include <elapsedMillis.h>

#define LFO_DEFAULT_MAX_WAVES 3
#define LFO_DEFAULT_RETE 0
#define LFO_DEFAULT_AMOUNT 4095
#define LFO_DEFAULT_WAVE 0

class LFO 
{
public:

	struct strLfo
	{
		//	wartości przekazywane do obiektu LFO
		uint16_t rate = 50;
		uint16_t wave = 0;
		uint16_t amount = 127;


		uint8_t enable;
		uint8_t sync;

	};

	//LFO(uint16_t *rate, uint16_t *wave, uint16_t *amount);
	LFO();
	void start();  
	void stop();
	void init(strLfo * _lfo);

	void setParams(uint16_t *rate, uint16_t *wave, uint16_t *amount);
	void setRate(uint16_t *rate);
	void setWave(uint16_t *wave);
	void setAmount(uint16_t *amount);
	void setSyncMode(uint8_t *sync);
	void setGlobalTempoValue(float *tempo);

	void setRateMod(float *rate_mod);
	void setWaveMod(float *wave_mod);
	void setAmountMod(float *amount_mod);

	float getOut();
	float getLastOut();
	float getSinus();
	float getSqrt();
	float getTriangle();
	float getFrequency();
	
	uint16_t getRate();
	uint16_t getWave();
	uint16_t getAmount();
	uint16_t getBPM();
	uint8_t isStarted();

	strLfo *lfo;
private:

/*	uint16_t *lfoRate = NULL;
	uint16_t *lfoWave = NULL;
	uint16_t *lfoAmount = NULL;
*/
	float mod=0;

	uint8_t *lfoSyncFlag = NULL;
//	float *lfoTempo = NULL;

	float *lfoRateMod = NULL;
	float *lfoWaveMod = NULL;
	float *lfoAmountMod = NULL;

	int16_t lfoRateValue;
	int16_t lfoRateLastValue;
	int16_t lfoWaveValue;
	int16_t lfoWaveLastValue;
	int16_t lfoAmountValue;
	int16_t lfoAmountLastValue;
	uint8_t lfoSyncLastFlag;
	float 	lfoTempoLastValue;

	float lfoRateModLastValue;
	float lfoAmountModLastValue;
	float lfoWaveModLastValue;

	elapsedMicros lfoMicros;
	uint8_t lfoStarted;
	float lfoT;
	float lfoOldTRatio;

	uint8_t lfoRateChange;
	uint8_t lfoSyncChange;
	uint8_t lfoTempoChange;
	uint8_t lfoRateModChange;

	float lfoTPlace;
	float lfoTRatio;
	float lfoFrequency;
	uint16_t lfoBPM;
	float lfoOutput;
	float lfoLastOutput;

	void calculateRatio();
	void updateRate();
	float lfoSinus();
	float lfoSqrt();
	float lfoTriangle(float shape);
};

//-----------------------------------------

#define WAVE_DEFAULT_WAVE 0
#define WAVEFORM_SIZE 256

class WaveFormGenerator
{
public:
	WaveFormGenerator();
	WaveFormGenerator(uint16_t *wave);

	int16_t getOut(float phase);
	void generateWaveform(int16_t waveform[]);
	void setWave(uint16_t *wave);
	uint16_t * getWave();

private:
	uint16_t *wavePointer = nullptr;
	uint16_t waveWave;
	float wavePhase;

	float waveSinus();
	float waveSqrt();
	float waveTriangle(float shape);
};



#define TEMPO_SYNC_LEVELS 8
#define TEMPO_SYNC_LEVELS_TYPES 3

const float tempoSyncRateLevel[TEMPO_SYNC_LEVELS] =
{
	4,
	3,
	2,
	1,
	0.5,
	0.25,
	0.125,
	0.0625
};


const float tempoSyncRateLevelTypes[TEMPO_SYNC_LEVELS_TYPES] =
{
	1.5, 		//doted
	1,			//normal
	0.666667	//triplet
};

#define TEMPO_SYNC_TYPES 20

const float tempoSyncRates[TEMPO_SYNC_TYPES] =
{
	6,
	4,
	3,
	2,
	1.5,
	1,
	0.75,
	0.5,
	0.375,
	0.333333,
	0.25,
	0.1875,
	0.166667,
	0.125,
	0.083333,
	0.0625,
	0.041667,
	0.03125,
	0.020833,
	0.015625
}; 



#endif
