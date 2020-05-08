//#include "mtWavetableSmoother.h"
//
//mtEffectWavetableSmoother sampleEditorWavetableSmoother;
//
//
////***********VIRTUAL SET PARAMS
//void mtEffectWavetableSmoother::setParamiter(void * ptr, uint8_t n)
//{
//	switch(n)
//	{
//		case 0 :
//			effectWavetableSmootherParams.windowLength = *((int*)ptr);
//			break;
//		case 1 :
//			effectWavetableSmootherParams.windowSize = *((int*)ptr);
//			break;
//		default: break;
//	}
//}
////***********
////***********VIRTUAL PROCESSING
//bool mtEffectWavetableSmoother::startProcess()
//{
//	if(effectWavetableSmootherParams.windowLength== 0 || effectWavetableSmootherParams.windowSize == 0)
//	{
//		return false;
//	}
//
//
//	dstAddr = processed.selection.addr;
//	srcAddr = confirmed.selection.addr;
//
//	processedSamples = 0;
//	length = effectWavetableSmootherParams.windowLength * effectWavetableSmootherParams.windowSize;
//	state = true;
//	return true;
//
//}
//int32_t mtEffectWavetableSmoother::updateProcess()
//{
//	if(!state) return -1;
//
//	int32_t processedBlockLength = 0;
//
//
//
//		int16_t windowDivider=0;
//		uint16_t numberOfWindows=0;
//		uint16_t missingWindows=0;
//		float windowsControl=0;
//		float windowsCounter=0;
//		uint16_t buforCounter=0;
//
//		uint32_t sampleCounter=0;
//		uint32_t sampleConstrain = STANDARD_WAVETABLE_WINDOWS_NUMBER*STANDARD_WAVETABLE_WINDOWS_NUMBER;
//
//
//		if(numberOfWindows == STANDARD_WAVETABLE_WINDOWS_NUMBER)
//		{
//			while ( wavfile.available() )
//			{
//				bufferLength = wavfile.read(buf16, 512);
//				for(int i=0; i< 256; i++)
//				{
//					if(bufferLength <= i ) *buf=0;
//					else *buf=buf16[i];
//					buf++;
//				}
//			}
//		}
//		else if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
//		{
//			missingWindows = STANDARD_WAVETABLE_WINDOWS_NUMBER - numberOfWindows;
//			windowsControl = (float) STANDARD_WAVETABLE_WINDOWS_NUMBER / (missingWindows+1);
//
//
//			while ( wavfile.available() )
//			{
//				if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
//				{
//					bufferLength=wavfile.read(buf16_1024, 2048);
//					for(int i=0; i< 1024; i++)
//					{
//						if(bufferLength <= i ) *buf=0;
//						else *buf=buf16_1024[i];
//						buf++;
//						currentWave[i]=buf16_1024[i];
//					}
//					sampleCounter+=1024;
//					buforCounter++;
//				}
//				else
//				{
//					bufferLength=wavfile.read(nextWave, 2048);
//					windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);
//
//					for(uint8_t i=0;i<windowDivider;i++)
//					{
//						for(uint16_t j=0; j< 1024; j++)
//						{
//							*buf=currentWave[j]+(((nextWave[j]-currentWave[j])/(windowDivider+1))*i);
//							buf++;
//						}
//						sampleCounter+=1024;
//					}
//					if(sampleCounter >= sampleConstrain) break;
//					for(int i=0; i< 1024; i++)
//					{
//						if(bufferLength <= i ) *buf=0;
//						else *buf=nextWave[i];
//						buf++;
//
//						currentWave[i]=nextWave[i];
//					}
//					sampleCounter+=1024;
//					buforCounter++;
//					buforCounter+=(windowDivider);
//					windowsCounter+=(windowDivider*windowsControl);
//				}
//			}
//
//		}
//
//		return STANDARD_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
//
//	for(uint32_t i = 0; i < 8192; i++)
//	{
//		if( ( sourceVoicePointer[lastVoiceIdx] < lastSrcAddr ) && (processedSamples <= SAMPLE_EFFECTOR_LENGTH_MAX))
//		{
//			int32_t currentSum = 0;
//
//			for(uint8_t i = firstActiveVoiceIdx ; i <= lastActiveVoiceIdx ; i ++ )
//			{
//				currentSum+= *sourceVoicePointer[i] * feedbackVoiceMult[i];
//				sourceVoicePointer[i]++;
//			}
//
//			if(currentSum > MAX_SIGNED_16BIT) currentSum = MAX_SIGNED_16BIT;
//			else if(currentSum < MIN_SIGNED_16BIT) currentSum = MIN_SIGNED_16BIT;
//
//			*currentDstAddr++ = (int16_t)currentSum;
//			processedBlockLength++;
//			processedSamples++;
//
//			if((processedSamples > lastIndexInVoice[lastActiveVoiceIdx])) lastActiveVoiceIdx ++;
//			if(sourceVoicePointer[firstActiveVoiceIdx] >= lastSrcAddr) firstActiveVoiceIdx ++;
//
//
//			if(lastActiveVoiceIdx > MAX_DELAY_VOICE_NUMBER - 1) lastActiveVoiceIdx = MAX_DELAY_VOICE_NUMBER - 1;
//			if(firstActiveVoiceIdx > MAX_DELAY_VOICE_NUMBER - 1) firstActiveVoiceIdx = MAX_DELAY_VOICE_NUMBER - 1;
//		}
//		else
//		{
//			state = false;
//			processed.selection.length = processedSamples; //redundancy
//			processed.area.length = confirmed.area.length + (processed.selection.length - confirmed.selection.length); //redundancy
//			break;
//		}
//	}
//
//	return processedBlockLength;
//}
//bool mtEffectWavetableSmoother::getProcessState()
//{
//	return state;
//}
//uint32_t mtEffectWavetableSmoother::getExpectedProcessLength()
//{
//	return  effectWavetableSmootherParams.windowLength * effectWavetableSmootherParams.windowSize;
//}
//
