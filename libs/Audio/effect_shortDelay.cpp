#include "effect_shortDelay.h"
#include "sdram.h"

static int16_t zeroblock[AUDIO_BLOCK_SAMPLES] = {};

void AudioEffectShortDelay::begin(float f, uint32_t t)
{
	setTime(t);
	setFeedback(f);
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
		feedbackVoiceMult[i-1] = f * 100; //(uint8_t)(100 * pow(f,i));
		feedbackVoiceShift[i-1] = i * timeInSamples;
		f*=f;
	}
	delaylineLength = delayVoicesNumber * timeInSamples;
	if(bufferedDataLength > delaylineLength) bufferedDataLength = delaylineLength;
	__enable_irq();
}
void AudioEffectShortDelay::setTime(uint16_t t)
{
	if(t > 3500) t = 3500;
	timeInSamples = (uint32_t)(t * 44.1) ;
	__disable_irq();
	for(uint8_t i = 1 ; i<=delayVoicesNumber; i++)
	{
		feedbackVoiceShift[i-1] = i * timeInSamples;
	}
	delaylineLength = delayVoicesNumber * timeInSamples;
	if(bufferedDataLength > delaylineLength) bufferedDataLength = delaylineLength;
	__enable_irq();
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
	audio_block_t *sblock, *dblock;
    int16_t *sbuf, *dbuf;

    sblock = receiveReadOnly(0);
    dblock = allocate();

    if(dblock) dbuf = dblock->data;
    else
    {
    	if(sblock) release(sblock);
    	return;
    }

    if(sblock) sbuf = sblock->data;
    else
    {
    	sbuf = zeroblock;
    }

    int32_t out = 0;
    int16_t * wrapConstrain = startDelayLine + delaylineLength;

    for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
    {
    	out = *sbuf;
    	*currentTail++ = *sbuf++;
    	if(currentTail > wrapConstrain ) currentTail = startDelayLine;

    	if(bufferedDataLength < delaylineLength )
    	{
    		bufferedDataLength++;
    	}
    	for(int i = 0; i < delayVoicesNumber; i++)
    	{
    		if( bufferedDataLength >= feedbackVoiceShift[i])
    		{
    			int16_t * currentPointer = currentTail - feedbackVoiceShift[i]; //delayline shift
    			if(currentPointer < startDelayLine ) currentPointer += delaylineLength; // wrap
    			out += (*currentPointer * feedbackVoiceMult[i]); //apply gain
    		}
    	}
    	out /= 100;

    	if(out > 32767) *dbuf = 32767;
    	else if(out < -32768) *dbuf = -32768;
    	else *dbuf = (int16_t)out;
    	dbuf++;
    }

    transmit(dblock,0);
    if (sblock) release(sblock);
    release(dblock);
}
