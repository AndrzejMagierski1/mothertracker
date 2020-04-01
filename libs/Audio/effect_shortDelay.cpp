#include "effect_shortDelay.h"
#include "sdram.h"
#include "mtSequencer.h"
static int16_t zeroblock[AUDIO_BLOCK_SAMPLES] = {};

constexpr uint32_t MAX_DELAYLINE_LENGTH = (MAX_SHORT_DELAY_VOICES - 1) * MAX_SHORT_DELAY_TIME * 44.1f;


const float tempoSyncRatesShortDelay[15] =
{
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
};



void AudioEffectShortDelay::begin(float f, uint32_t t, bool pp, bool se, uint8_t sr)
{
	isPingpong = pp;
	isSync = se;
	setTime(t);
	setFeedback(f);
	setRate(sr);
	currentTail = startDelayLine;

	unblockUpdate();
}
void AudioEffectShortDelay::setFeedback(float f)
{
	if(f > 0.7f) f = 0.7f;
	else if(f < 0.0f) f = 0.0f;

	uint8_t maxVoiceNumber = 0;
	float feedbackGain = 1.0;

	while(feedbackGain > 0.01f)
	{
		feedbackGain *= f;
		maxVoiceNumber++;
		if(maxVoiceNumber == MAX_SHORT_DELAY_VOICES) break;
	}
	__disable_irq();
	delayVoicesNumber = maxVoiceNumber - 1;

	for(uint8_t i = 1 ; i<= delayVoicesNumber; i++)
	{
		if(i==1) feedbackVoiceMult[i-1] = f * 100; //(uint8_t)(100 * pow(f,i));
		else feedbackVoiceMult[i-1] = feedbackVoiceMult[i-2] * f;

		feedbackVoiceShift[i-1] = i * timeInSamples;
	}
	delaylineLength = delayVoicesNumber * timeInSamples;
	if(bufferedDataLength > delaylineLength) bufferedDataLength = delaylineLength;
	__enable_irq();
}
void AudioEffectShortDelay::setTime(uint16_t t)
{
	if(isSync) return;

	if(t > MAX_SHORT_DELAY_TIME) t = MAX_SHORT_DELAY_TIME;
	timeInSamples = (uint32_t)(t * 44.1f) ;
	__disable_irq();
	for(uint8_t i = 1 ; i <= delayVoicesNumber; i++)
	{
		feedbackVoiceShift[i-1] = i * timeInSamples;
	}
	delaylineLength = delayVoicesNumber * timeInSamples;
	if(bufferedDataLength > delaylineLength) bufferedDataLength = delaylineLength;
	__enable_irq();
}

void AudioEffectShortDelay::setRate(uint8_t r)
{
	if(!isSync) return;

	float tempo = sequencer.getActualTempo();

	float freq = (tempo/15.0f); // *4,bo  beat = 4 step i /60, bo min = 60s
	float time = (1000/freq) * tempoSyncRatesShortDelay[r];// w ms


	timeInSamples = (uint32_t)(time * 44.1f) ;
	__disable_irq();
	for(uint8_t i = 1 ; i <= delayVoicesNumber; i++)
	{
		feedbackVoiceShift[i-1] = i * timeInSamples;
	}
	delaylineLength = delayVoicesNumber * timeInSamples;
	if(bufferedDataLength > delaylineLength) bufferedDataLength = delaylineLength;
	__enable_irq();
}
void AudioEffectShortDelay::setPingpongEnable(bool pp)
{
	isPingpong = pp;
}
void AudioEffectShortDelay::setSyncEnable(bool se)
{
	isSync = se;
}

void AudioEffectShortDelay::clear()
{
	bufferedDataLength = 0;
	currentTail = startDelayLine;
}

void AudioEffectShortDelay::blockUpdate()
{
	noRefresh = true;
}
void AudioEffectShortDelay::unblockUpdate()
{
	noRefresh = false;
}

void AudioEffectShortDelay::update(void)
{
	if(noRefresh == true) return;
	audio_block_t *sblock, *dblockL, *dblockR = nullptr ;
    int16_t *sbuf, *dbufL, *dbufR = nullptr;

    sblock = receiveReadOnly(0);
    dblockL = allocate();

    if(dblockL) dbufL = dblockL->data;
    else
    {
    	if(sblock) release(sblock);
    	return;
    }

    if(isPingpong)
    {
    	dblockR = allocate();
        if(dblockR) dbufR = dblockR->data;
        else
        {
        	if(sblock) release(sblock);
        	if(dblockL) release(dblockL);
        	return;
        }
    }

    if(sblock) sbuf = sblock->data;
    else
    {
    	sbuf = zeroblock;
    }

    if(isPingpong)
    {
    	int32_t outL = 0;
    	int32_t outR = 0;
		int16_t * wrapConstrain = startDelayLine + MAX_DELAYLINE_LENGTH;

		for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			outL = 100 * (*sbuf);
			outR = outL;

			*currentTail++ = *sbuf++;
			if(currentTail > wrapConstrain ) currentTail = startDelayLine;

			if(bufferedDataLength < MAX_DELAYLINE_LENGTH )
			{
				bufferedDataLength++;
			}

			for(int i = 0; i < delayVoicesNumber; i++)
			{
				if( bufferedDataLength >= feedbackVoiceShift[i])
				{
					int16_t * currentPointer = currentTail - feedbackVoiceShift[i]; //delayline shift
					if(currentPointer < startDelayLine ) currentPointer += MAX_DELAYLINE_LENGTH; // wrap

					if(i%2)
					{
						outL += feedbackVoiceMult[i] * (*currentPointer); //apply gain
					}
					else
					{
						outR += feedbackVoiceMult[i] * (*currentPointer); //apply gain
					}

				}
			}
			outL /= 100;
			outR /= 100;

			if(outL > 32767) *dbufL = 32767;
			else if(outL < -32768) *dbufL = -32768;
			else *dbufL = (int16_t)outL;
			dbufL++;


			if(outR > 32767) *dbufR = 32767;
			else if(outR < -32768) *dbufR = -32768;
			else *dbufR = (int16_t)outR;
			dbufR++;
		}

		transmit(dblockL,0);
		transmit(dblockR,1);
		if (sblock) release(sblock);
		release(dblockL);
		release(dblockR);
    }
    else
    {
        int32_t out = 0;
        int16_t * wrapConstrain = startDelayLine + MAX_DELAYLINE_LENGTH;

        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
        	out = 100 * (*sbuf);
        	*currentTail++ = *sbuf++;
        	if(currentTail > wrapConstrain ) currentTail = startDelayLine;

        	if(bufferedDataLength < MAX_DELAYLINE_LENGTH )
        	{
        		bufferedDataLength++;
        	}

        	for(int i = 0; i < delayVoicesNumber; i++)
        	{
        		if( bufferedDataLength >= feedbackVoiceShift[i])
        		{
        			int16_t * currentPointer = currentTail - feedbackVoiceShift[i]; //delayline shift
        			if(currentPointer < startDelayLine ) currentPointer += MAX_DELAYLINE_LENGTH; // wrap

        			out += feedbackVoiceMult[i] * (*currentPointer); //apply gain
        		}
        	}
        	out /= 100;

        	if(out > 32767) *dbufL = 32767;
        	else if(out < -32768) *dbufL = -32768;
        	else *dbufL = (int16_t)out;
        	dbufL++;
        }

        transmit(dblockL,0);
        transmit(dblockL,1);
        if (sblock) release(sblock);
        release(dblockL);
    }


}
