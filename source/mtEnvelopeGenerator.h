																																																																																																																																																																																																																										#include <stdint.h>
//#include "arduino.h"
#include <elapsedMillis.h>




#ifndef MT_ENVELOPEGENERATOR_H
#define MT_ENVELOPEGENERATOR_H


// #define CHANNEL_QTY 20
#define MAX_ADSR_PAR 4095

#define MAX_HOLD 4095


#define MAX_SUSTAIN 4095
#define MIN_OUTPUT -1.0
#define MAX_OUTPUT 1.0


// #define ADSR_DEBG 1

class envelopeGenerator
{

public:
	struct strEnv // do flash
	{
		float    amount 	= 1.0;
		uint16_t delay 		= 0;
		uint16_t attack 	= 20;
		uint16_t hold 		= 0;
		uint16_t decay 		= 30;
		float	 sustain 	= 1.0;
		uint16_t release 	= 50;

		uint8_t loop 		= 0;
		uint8_t enable;
	};
	
	envelopeGenerator();
	void init(void);
	void init(strEnv * _env);

	void calc();
	void calcTime();
	void start();
	void stop();
	void kill();
	void killToZero();

	float getOut();
	float getLastOut();
	// float calcOutput();
	uint8_t getPhase() { return envTemp.phase; }
	uint8_t isKeyPressed() { return keyPressed; }

	void putMidiParameter(uint8_t ch, uint8_t parNo, uint8_t value);

	void syncTrackerSeq(uint32_t val, float seqSpeed);
	void setSyncStartStep(uint16_t n);
	void setPhaseNumbers(int8_t n1, int8_t n2);
	void setSyncRate(float sync);
	
private:
	float fMap(float x, float in_min, float in_max, float out_min, float out_max);

	const float timeMul = 1000;	// parametry ADSR są mnożone przez ten czynnik
	
	bool keyPressed = 0;	// triggeruje cały envelope
	
	strEnv * envelope;

	struct strEnvelopeTemp
	{
		// tylko do RAM
		float output = 0;			// wyjście adsra
		float tempOutput = 0;		// czyste wyjście adsra, bez modu
		float maxOutput = 0;		// maksymalna osiągnięta wartość ( np w trakcie narastania attacku)

		float killOutput = 0;		// bufor warotści po kill
		elapsedMicros timer = 0;	// timer fazy
		uint8_t phase = 0;			// aktualna faza

	} envTemp;

	uint16_t startStep = 0;
	int8_t phaseNumber[2] = {-1,-1};
	float syncRate = 1;
	uint16_t periodTime = 0;
	
	// uint8_t modTarget = 0;		//	jesli 0 to nieaktywny, jesli 1 to 0 w tablicy
	// float mod = 0;				// 	tu można wpisać z zewnątrz wartość modulacji
	// nr fazy
	enum adsrPhase
	{
		phase_nothing,	//0
		phase_predelay,	//1
		phase_attack,	//2
		phase_hold,		//3
		phase_decay,	//4
		phase_sustain,	//5
		phase_release	//6
	};

	// nr cc parametru
	enum parNumber
	{
		par_predelay = 40, 	// 40
		par_attack,			// 41
		par_hold,			// 42
		par_decay,			// 43
		par_sustain,		// 44
		par_release,		// 45
		par_amount			// 46
	};

};

#endif
