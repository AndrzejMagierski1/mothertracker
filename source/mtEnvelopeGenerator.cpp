#include "mtEnvelopeGenerator.h"
//#include "arduino.h"
#include <stdint.h>

envelopeGenerator::envelopeGenerator()
{

};

// void envelopeGenerator::init(void)
// {
// 	return;
// }


void envelopeGenerator::init(strEnv * _env)
{
	envelope = _env;
}

void envelopeGenerator::start()
{
	// Serial.println("envelope.start()");
	keyPressed = 1;
}

void envelopeGenerator::stop()
{
	keyPressed = 0;
}

void envelopeGenerator::kill()
{
	envTemp.phase = phase_nothing;
	envTemp.timer = 0;
	envTemp.killOutput = envTemp.output;

	keyPressed = 0;
}

void envelopeGenerator::calc()
{

	// values
	float amount 		= envelope->amount ; 	// float -1 - 1
	float sustain 		= envelope->sustain ;	// float 0-1

	// time-dependent values
	float delay 		= (float)envelope->delay * timeMul;
	float attack 		= (float)envelope->attack * timeMul;
	float hold 			= 0;// float(*_hold * 	2) 		* timeMul;
	float decay 		= (float)envelope->decay * timeMul;
	float release 		= (float)envelope->release * timeMul;

	bool _loop 			= envelope->loop;


	if (envTemp.phase == phase_nothing)
	{
		if (keyPressed)
		{
			envTemp.phase = phase_predelay;
		}
		else
		{
			envTemp.timer = 0;
		}
	}

	if (envTemp.phase == phase_predelay)
	{
		if (keyPressed)
		{
			// etap trwa
			if (envTemp.timer < delay)
			{
				envTemp.phase = phase_predelay;
			}
			// etap skończony
			else
			{
				envTemp.timer = 0;
				envTemp.phase++;
			}
		}
		else
		{
			envTemp.timer = 0;
			envTemp.phase = 0;
		}
	}
	//	ATTACK
	if (envTemp.phase == phase_attack)
	{
		if (keyPressed)
		{
			// inkrementujemy timer
			// envTemp.timer += calcInt;

			// etap trwa
			if (envTemp.timer < attack)
			{
				envTemp.phase = phase_attack;
			}
			// etap skończony
			else
			{
				envTemp.timer = 0;
				envTemp.phase++;
			}
		}
		// jesli puszczony przechodzi od razu do release
		else
		{
			envTemp.timer = 0;
			envTemp.phase = phase_release;
		}
	}
	//	HOLD
	if (envTemp.phase == phase_hold)
	{
		// wciśnięty, hold krótki
		if (keyPressed && (hold < (MAX_HOLD * MAX_HOLD * timeMul)))
		{
			// inkrementujemy timer
			// envTemp.timer += calcInt;

			// etap trwa
			if (envTemp.timer <= hold)
			{
				envTemp.phase = phase_hold;
			}
			// etap skończony
			else
			{
				envTemp.timer = 0;
				envTemp.phase++;
			}
		}
		// wciśnięty, hold nieskończony
		else if (keyPressed && (hold >= (MAX_HOLD * MAX_HOLD * timeMul)))
		{
			envTemp.timer = 1;
			envTemp.phase = phase_hold;
		}

		// puszczony, hold krótki
		else if (!keyPressed && (hold < (MAX_HOLD * MAX_HOLD * timeMul)))
		{
			envTemp.timer = 0;
			envTemp.phase++;
		}
		// puszczony, hold nieskończony
		else if (!keyPressed && (hold >= (MAX_HOLD * MAX_HOLD * timeMul)))
		{
			envTemp.timer = 0;
			envTemp.phase = phase_release;
		}
	}
	// DECAY
	if (envTemp.phase == phase_decay)
	{
		if (keyPressed)
		{
			// etap trwa
			if (envTemp.timer <= decay)
			{
				envTemp.phase = phase_decay;
			}
			// etap skończony
			else
			{
				envTemp.timer = 0;
				envTemp.phase++;
			}
		}

		// jesli puszczony przechodzi od razu do release
		else
		{
			envTemp.timer = 0;
			envTemp.phase = phase_release;
		}
	}

	// SUSTAIN
	if (envTemp.phase == phase_sustain)
	{
		// wciśnięty
		if (keyPressed && !_loop)
		{
			envTemp.timer = 1;
			envTemp.phase = phase_sustain;
		}

		else if (keyPressed && _loop)
		{
			envTemp.timer = 0;
			envTemp.phase = phase_release;
		}
		// puszczony, przechodzimy do release
		else
		{
			envTemp.timer = 0;
			envTemp.phase = phase_release;
		}


	}

	// RELEASE
	if (envTemp.phase == phase_release)
	{
		// etap trwa
		if (envTemp.timer <= release)
		{
			envTemp.phase = phase_release;
		}

		else if(_loop)
		{
			envTemp.timer = 0;
			envTemp.phase = phase_predelay;	
		}
		// etap skończony
		else
		{
			envTemp.timer = 0;
			envTemp.phase = phase_nothing;
		}
	}

	// #ifdef ADSR_DEBUG
	// 	Serial.print(envTemp.timer);
	// #endif

	float tempOutput = 0;

	switch (envTemp.phase)
	{
	case phase_nothing:



		envTemp.tempOutput = 0;
		envTemp.maxOutput = envTemp.tempOutput;

		break;
	case phase_predelay:

		// #ifdef ADSR_DEBUG
		// 		Serial.print(" predelay");
		// #endif

		envTemp.tempOutput = envTemp.killOutput;

		envTemp.maxOutput = envTemp.tempOutput;

		break;
	case phase_attack:

		// #ifdef ADSR_DEBUG
		// 		Serial.print(" attack");
		// #endif

		// tempOutput = (envTemp.timer / attack) * amount ;
		tempOutput = envTemp.killOutput + ((envTemp.timer / attack) * (amount-envTemp.killOutput)) ;

		envTemp.tempOutput = constrain(tempOutput, envTemp.killOutput, MAX_OUTPUT);
		envTemp.maxOutput = envTemp.tempOutput;

		break;

	case phase_hold:
		// #ifdef ADSR_DEBUG
		// 		Serial.print(" hold");
		// #endif
		envTemp.tempOutput = amount;
		envTemp.maxOutput = envTemp.tempOutput;

		break;

	case phase_decay:
		// #ifdef ADSR_DEBUG
		// 		Serial.print(" decay");
		// #endif
		tempOutput = (((1 - (envTemp.timer / decay)) * (1 - sustain) + sustain)) * amount;// * (amount / MAX_ADSR_PAR);
		// tempOutput = tempOutput * (amount > 0 ? 1 : -1);
		envTemp.tempOutput = constrain(tempOutput, MIN_OUTPUT, MAX_OUTPUT);
		envTemp.maxOutput = envTemp.tempOutput;

		break;

	case phase_sustain:
		// #ifdef ADSR_DEBUG
		// 		Serial.print(" sustain");
		// #endif
		tempOutput = sustain * amount;
		envTemp.tempOutput = constrain(tempOutput, MIN_OUTPUT, MAX_OUTPUT);
		envTemp.maxOutput = envTemp.tempOutput;


		break;
	case phase_release:

		// #ifdef ADSR_DEBUG
		// 		Serial.print(" release");
		// #endif
		if (release > 0)
		{
			tempOutput = (1 - (envTemp.timer / release)) * envTemp.maxOutput;
		}
		else
		{
			tempOutput = 0;
		}
		envTemp.tempOutput = constrain(tempOutput, MIN_OUTPUT, MAX_OUTPUT);
		break;

	default:
		break;
	}

	// #ifdef ADSR_DEBUG
	// 	Serial.print(" - ");
	// 	Serial.print(envTemp.timer);
	// #endif

	// jesli modulacja targetuje w ten adsr
	// if (modTarget == (ch + 1))
	// {
	// 	envTemp.output = constrain(envTemp.tempOutput + mod, MIN_OUTPUT, MAX_OUTPUT);
	// }
	// else
	// {
	envTemp.output = constrain(envTemp.tempOutput, MIN_OUTPUT, MAX_OUTPUT);
	// }


	// Serial.print("output: ");
	// Serial.println(envTemp.output, 3);

	// #ifdef ADSR_DEBUG
	// 	Serial.println();
	// #endif
}


float envelopeGenerator::fMap(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


float envelopeGenerator::getOut()
{
	if(!envelope->enable) return 0.0;
	
	calc();
	return (envTemp.output);
}

float envelopeGenerator::getLastOut()
{
	// calc();
	if(!envelope->enable) return 0.0;

	return (envTemp.output);
}




