#include "mtAudioEngine.h"

AudioPlayMemory          playMem[8];
AudioEffectEnvelope      envelopeAmp[8];
envelopeGenerator		 envelopeFilter[8];
AudioFilterStateVariable filter[8];
AudioAmplifier           amp[8];
AudioMixer8				 mixerL,mixerR;
AudioOutputI2S           i2s1;

AudioConnection          connect1(&playMem[0], 0, &filter[0], 0);
AudioConnection          connect2(&playMem[1], 0, &filter[1], 0);
AudioConnection          connect3(&playMem[2], 0, &filter[2], 0);
AudioConnection          connect4(&playMem[3], 0, &filter[3], 0);
AudioConnection          connect5(&playMem[4], 0, &filter[4], 0);
AudioConnection          connect6(&playMem[5], 0, &filter[5], 0);
AudioConnection          connect7(&playMem[6], 0, &filter[6], 0);
AudioConnection          connect8(&playMem[7], 0, &filter[7], 0);

AudioConnection          connect9(&filter[0], 0, &envelopeAmp[0], 0);
AudioConnection          connect10(&filter[1], 0, &envelopeAmp[1], 0);
AudioConnection          connect11(&filter[2], 0, &envelopeAmp[2], 0);
AudioConnection          connect12(&filter[3], 0, &envelopeAmp[3], 0);
AudioConnection          connect13(&filter[4], 0, &envelopeAmp[4], 0);
AudioConnection          connect14(&filter[5], 0, &envelopeAmp[5], 0);
AudioConnection          connect15(&filter[6], 0, &envelopeAmp[6], 0);
AudioConnection          connect16(&filter[7], 0, &envelopeAmp[7], 0);

AudioConnection          connect17(&envelopeAmp[0],&amp[0]);
AudioConnection          connect18(&envelopeAmp[1], &amp[1]);
AudioConnection          connect19(&envelopeAmp[2], &amp[2]);
AudioConnection          connect20(&envelopeAmp[3], &amp[3]);
AudioConnection          connect21(&envelopeAmp[4], &amp[4]);
AudioConnection          connect22(&envelopeAmp[5], &amp[5]);
AudioConnection          connect23(&envelopeAmp[6], &amp[6]);
AudioConnection          connect24(&envelopeAmp[7], &amp[7]);

AudioConnection          connect25(&amp[0], 0, &mixerL, 0);
AudioConnection          connect26(&amp[1], 0, &mixerL, 1);
AudioConnection          connect27(&amp[2], 0, &mixerL, 2);
AudioConnection          connect28(&amp[3], 0, &mixerL, 3);
AudioConnection          connect29(&amp[4], 0, &mixerL, 4);
AudioConnection          connect30(&amp[5], 0, &mixerL, 5);
AudioConnection          connect31(&amp[6], 0, &mixerL, 6);
AudioConnection          connect32(&amp[7], 0, &mixerL, 7);


AudioConnection          connect33(&amp[0], 0, &mixerR, 0);
AudioConnection          connect34(&amp[1], 0, &mixerR, 1);
AudioConnection          connect35(&amp[2], 0, &mixerR, 2);
AudioConnection          connect36(&amp[3], 0, &mixerR, 3);
AudioConnection          connect37(&amp[4], 0, &mixerR, 4);
AudioConnection          connect38(&amp[5], 0, &mixerR, 5);
AudioConnection          connect39(&amp[6], 0, &mixerR, 6);
AudioConnection          connect40(&amp[7], 0, &mixerR, 7);

AudioConnection         connect41(&mixerL, 0, &i2s1, 1);
AudioConnection         connect42(&mixerR, 0, &i2s1, 0);



	instrumentEngine instrumentPlayer[8];
	strMtModAudioEngine modAudioEngine[8];
	audioEngine engine;

	void audioEngine::init()
	{
		for(int i=0;i<8; i++)
		{
			instrumentPlayer[i].init(&playMem[i],&envelopeFilter[i],&filter[i],&envelopeAmp[i], &amp[i], i);
		}

	}
	void audioEngine::update()
	{
		for(int i=0; i<8; i++)
		{
			instrumentPlayer[i].update();
		}
	}

	void instrumentEngine::init(AudioPlayMemory * playMem,envelopeGenerator* envFilter,AudioFilterStateVariable * filter,
			AudioEffectEnvelope * envAmp, AudioAmplifier * amp, uint8_t panCh)
	{
		playMemPtr=playMem;
		envelopeAmpPtr=envAmp;
		envelopeFilterPtr=envFilter;
		filterPtr=filter;
		ampPtr=amp;
		numPanChannel=panCh;
		switch(panCh)
		{
		case 0:
			conFilterToAmpPtr=&connect9;
			conPlayToFilterPtr=&connect1;
			break;
		case 1:
			conFilterToAmpPtr=&connect10;
			conPlayToFilterPtr=&connect2;
			break;
		case 2:
			conFilterToAmpPtr=&connect11;
			conPlayToFilterPtr=&connect3;
			break;
		case 3:
			conFilterToAmpPtr=&connect12;
			conPlayToFilterPtr=&connect4;
			break;
		case 4:
			conFilterToAmpPtr=&connect13;
			conPlayToFilterPtr=&connect5;
			break;
		case 5:
			conFilterToAmpPtr=&connect14;
			conPlayToFilterPtr=&connect6;
			break;
		case 6:
			conFilterToAmpPtr=&connect15;
			conPlayToFilterPtr=&connect7;
			break;
		case 7:
			conFilterToAmpPtr=&connect16;
			conPlayToFilterPtr=&connect8;
			break;
		default:
			break;
		}

	}

	uint8_t instrumentEngine :: play(strStep * step,strMtModAudioEngine * mod)
	{
		uint8_t status;
		float gainL=0,gainR=0;


		actualStepPtr=step;
		actualInstrPtr=NULL;
		/*================================================ENVELOPE AMP==========================================*/
		envelopeAmpPtr->delay(mtProject.instrument[step->instrumentIndex].envelope[envAmp].delay);
		envelopeAmpPtr->attack(mtProject.instrument[step->instrumentIndex].envelope[envAmp].attack);
		envelopeAmpPtr->hold(mtProject.instrument[step->instrumentIndex].envelope[envAmp].hold);
		envelopeAmpPtr->decay(mtProject.instrument[step->instrumentIndex].envelope[envAmp].decay);
		envelopeAmpPtr->sustain(mtProject.instrument[step->instrumentIndex].envelope[envAmp].sustain);
		envelopeAmpPtr->release(mtProject.instrument[step->instrumentIndex].envelope[envAmp].release);
		/*======================================================================================================*/
		/*================================================ENVELOPE FILTER=======================================*/

		if(mtProject.instrument[step->instrumentIndex].filterEnable == filterOn)
		{
			envelopeFilterPtr->init(&mtProject.instrument[step->instrumentIndex].envelope[envFilter]);
		/*======================================================================================================*/
		/*================================================FILTER================================================*/
			filterConnect();
			changeFilterType(mtProject.instrument[step->instrumentIndex].filterType);
			filterPtr->resonance(mtProject.instrument[step->instrumentIndex].resonance);


			filterPtr->setCutoff(mtProject.instrument[step->instrumentIndex].cutOff);

		}
		else if(mtProject.instrument[step->instrumentIndex].filterEnable == filterOff) filterDisconnect();





		/*======================================================================================================*/
		/*==================================================GAIN================================================*/
		ampPtr->gain( (step->volume/100.0) * mtProject.instrument[step->instrumentIndex].envelope[envAmp].amount);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/

		gainL=mtProject.instrument[step->instrumentIndex].panning/100.0;
		gainR=(100-mtProject.instrument[step->instrumentIndex].panning)/100.0;

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
		/*======================================================================================================*/

		status = playMemPtr->play(step,mod);
		envelopeAmpPtr->noteOn();
		if(mtProject.instrument[step->instrumentIndex].envelope[envFilter].enable == envelopeOn) envelopeFilterPtr->start();

		return status;

	}
	uint8_t instrumentEngine :: play(strInstrument * instr,strMtModAudioEngine * mod, int8_t note)
	{
		uint8_t status=0;

		actualStepPtr=NULL;
		actualInstrPtr=instr;
		/*================================================ENVELOPE AMP==========================================*/
		envelopeAmpPtr->delay(instr->envelope[envAmp].delay);
		envelopeAmpPtr->attack(instr->envelope[envAmp].attack);
		envelopeAmpPtr->hold(instr->envelope[envAmp].hold);
		envelopeAmpPtr->decay(instr->envelope[envAmp].decay);
		envelopeAmpPtr->sustain(instr->envelope[envAmp].sustain);
		envelopeAmpPtr->release(instr->envelope[envAmp].release);
		/*======================================================================================================*/
		/*================================================ENVELOPE FILTER=======================================*/
		if(instr->filterEnable == filterOn)
		{
			envelopeFilterPtr->init(&instr->envelope[envFilter]);
			filterConnect();

		/*======================================================================================================*/
		/*================================================FILTER================================================*/

			changeFilterType(instr->filterType);
			filterPtr->resonance(instr->resonance);



			filterPtr->setCutoff(instr->cutOff);
		}
		else if(instr->filterEnable == filterOff) filterDisconnect();




		/*======================================================================================================*/
		/*==================================================GAIN================================================*/
		ampPtr->gain(instr->envelope[envAmp].amount);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/
		mixerL.gain(numPanChannel,instr->panning/100.0);
		mixerR.gain(numPanChannel,(100-instr->panning)/100.0);


		status = playMemPtr->play(instr,mod,1.0,note);
		envelopeAmpPtr->noteOn();

		/*======================================================================================================*/

		if(instr->envelope[envFilter].enable == envelopeOn) envelopeFilterPtr->start();


		return status;

	}


	uint8_t instrumentEngine :: change(strStep * step, strMtModAudioEngine * mod)
	{
		uint8_t status=0;
		float gainL=0,gainR=0;
		float filterMod=0;

		envelopeAmpPtr->sustain(mtProject.instrument[step->instrumentIndex].envelope[envAmp].sustain);
		/*================================================FILTER================================================*/
		if(mtProject.instrument[step->instrumentIndex].filterEnable == filterOn)
		{
			filterPtr->resonance(mtProject.instrument[step->instrumentIndex].resonance);

			if(mtProject.instrument[actualStepPtr->instrumentIndex].envelope[envFilter].enable == envelopeOn)
			{
					filterMod+=envelopeFilterPtr->getOut();
			}

			filterPtr->setCutoff(mtProject.instrument[step->instrumentIndex].cutOff + filterMod);
		}

		/*======================================================================================================*/

		/*==================================================GAIN================================================*/
		ampPtr->gain( (step->volume/100.0) * mtProject.instrument[step->instrumentIndex].envelope[envAmp].amount);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/
		gainL=mtProject.instrument[step->instrumentIndex].panning/100.0;
		gainR=(100-mtProject.instrument[step->instrumentIndex].panning)/100.0;

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
		/*======================================================================================================*/

		status=playMemPtr->setMod(step,mod);


		return status;
	}
	uint8_t instrumentEngine :: change(strInstrument * instr, strMtModAudioEngine * mod)
	{
		uint8_t status=0;
		float filterMod=0;
		envelopeAmpPtr->sustain(instr->envelope[envAmp].sustain);

		/*================================================FILTER================================================*/

		if(instr->filterEnable == filterOn)
		{
			filterPtr->resonance(instr->resonance);

			if(instr->envelope[envFilter].enable == envelopeOn)
			{
					filterMod+=envelopeFilterPtr->getOut();
			}
			filterPtr->setCutoff(instr->cutOff + filterMod);
		}

		/*======================================================================================================*/
		/*==================================================GAIN================================================*/
		ampPtr->gain(instr->envelope[envAmp].amount);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/
		mixerL.gain(numPanChannel,instr->panning/100.0);
		mixerR.gain(numPanChannel,(100-instr->panning)/100.0);
		/*======================================================================================================*/

		ampPtr->gain(instr->envelope[envAmp].amount);
		status=playMemPtr->setMod(instr,mod,24);

		return status;
	}

	void instrumentEngine:: update()
	{
		float filterMod=0;

		if(envelopeAmpPtr->endRelease()) playMemPtr->stop();
		if(actualStepPtr)
		{
			if(mtProject.instrument[actualStepPtr->instrumentIndex].envelope[envFilter].enable == envelopeOn)
			{

					filterMod+=envelopeFilterPtr->getOut();

				filterPtr->setCutoff(mtProject.instrument[actualStepPtr->instrumentIndex].cutOff + filterMod);
			}

		}


		if(actualInstrPtr)
		{
			if(actualInstrPtr->envelope[envFilter].enable == envelopeOn)
			{

					filterMod+=envelopeFilterPtr->getOut();

				filterPtr->setCutoff(actualInstrPtr->cutOff + filterMod);
			}
		}

	}



	void instrumentEngine :: stop()
	{
		envelopeAmpPtr->noteOff();
		envelopeFilterPtr->stop();

	}

	void instrumentEngine :: changeFilterType(uint8_t type)
	{
		conFilterToAmpPtr->disconnect();
		if(type == lowPass) conFilterToAmpPtr->src_index=0;
		else if(type == bandPass) conFilterToAmpPtr->src_index=1;
		else if(type ==  highPass) conFilterToAmpPtr->src_index=2;
		conFilterToAmpPtr->connect();
	}

	void instrumentEngine :: filterDisconnect()
	{
		//conFilterToAmpPtr->disconnect();
		conPlayToFilterPtr->disconnect();

		conPlayToFilterPtr->src=playMemPtr;
		conPlayToFilterPtr->dst=envelopeAmpPtr;
		conPlayToFilterPtr->src_index=0;
		conPlayToFilterPtr->dest_index=0;

		conPlayToFilterPtr->connect();
	}

	void instrumentEngine :: filterConnect()
	{
		conFilterToAmpPtr->disconnect();
		conPlayToFilterPtr->disconnect();

		conPlayToFilterPtr->src=playMemPtr;
		conPlayToFilterPtr->dst=filterPtr;
		conPlayToFilterPtr->src_index=0;
		conPlayToFilterPtr->dest_index=0;



		conFilterToAmpPtr->connect();
		conPlayToFilterPtr->connect();

	}

	float instrumentEngine :: fmap(float x, float in_min, float in_max, float out_min, float out_max)
	{
	  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

