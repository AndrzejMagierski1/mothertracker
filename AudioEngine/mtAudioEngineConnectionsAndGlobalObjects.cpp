#include "Audio.h"

//********************** INPUTS
AudioInputI2S            i2sIn;
AudioMixer4              inputChannelsToMonoMixer;
//********************** PREVIEW OR TEST
AudioPlaySdWav           player16BitWavFromSD;
AudioPlaySdWavFloat		 playerFloatWavFromSD;
AudioPlaySdWav24bit 	 player24BitWavFromSD;
AudioSynthWaveform		 testWaveform;
AudioPlayMemory 		 metronomeTick;
//********************** TRACK STREAM
AudioPlayMemory          trackMemoryPlayer[8];
AudioAmplifier           trackAmplifier[8];
AudioFilterStateVariable trackFilter[8];
//********************** TRACK MODIFICATORS
envelopeGenerator		 envelopeFilter[8];
envelopeGenerator		 envelopeWtPosition[8];
envelopeGenerator		 envelopeGranPosition[8];
envelopeGenerator		 envelopePanning[8];
envelopeGenerator		 envelopeFinetune[8];
//********************** SEND MIX
AudioMixer10			 preDelayMixer,preReverbMixer;
//********************** SEND PROCESSING
AudioEffectShortDelay	 shortDelay;
AudioEffectPolyverb		 polyverb;
//********************** MIX
AudioMixer10			 leftChannelTraksMixer,rightChannelTracksMixer;
//********************** MIXED STREAM
AudioEffectLimiter		 limiter[2];
AudioBitDepth			 bitDepthControl[2];
//********************** OUTPUTS
AudioOutputI2S           i2sOut;
AudioRecordQueue         queueOfRecordedData;
AudioRecordQueue		 queueOfLeftChannelExportedData, queueOfRightChannelExportedData;
//********************** ANALYZE
AudioAnalyzeRMS			 trackRMS[8];
AudioAnalyzeRMS			 delayRMS[2];
AudioAnalyzeRMS			 polyverbRMS[2];
AudioAnalyzeRMS			 analogInputSignalRMS;
AudioAnalyzeRMS			 exportLeftChannelRMS, exportRightChannelRMS;
AudioAnalyzeRMS			 mixedTracksLeftChannelRMS, mixedTracksRightChannelRMS;
//********************** SOURCE(STREAM/PREVIEW OR TEST) SWITCHING MIX
AudioMixer10             leftChannelSourceSwitchingMixer,rightChannelSourceSwitchingMixer;
//**********************

//********************** MAIN AUDIO STREAM CONNECTIONS
AudioConnection          connectTrackMemoryPlayerToTrackFilter[8] =
{
		AudioConnection(&trackMemoryPlayer[0], 0, &trackFilter[0], 0),
		AudioConnection(&trackMemoryPlayer[1], 0, &trackFilter[1], 0),
		AudioConnection(&trackMemoryPlayer[2], 0, &trackFilter[2], 0),
		AudioConnection(&trackMemoryPlayer[3], 0, &trackFilter[3], 0),
		AudioConnection(&trackMemoryPlayer[4], 0, &trackFilter[4], 0),
		AudioConnection(&trackMemoryPlayer[5], 0, &trackFilter[5], 0),
		AudioConnection(&trackMemoryPlayer[6], 0, &trackFilter[6], 0),
		AudioConnection(&trackMemoryPlayer[7], 0, &trackFilter[7], 0)
};

AudioConnection          connectTrackFilterToTrackAmplifier[8] =
{
		AudioConnection(&trackFilter[0], 0, &trackAmplifier[0], 0),
		AudioConnection(&trackFilter[1], 0, &trackAmplifier[1], 0),
		AudioConnection(&trackFilter[2], 0, &trackAmplifier[2], 0),
		AudioConnection(&trackFilter[3], 0, &trackAmplifier[3], 0),
		AudioConnection(&trackFilter[4], 0, &trackAmplifier[4], 0),
		AudioConnection(&trackFilter[5], 0, &trackAmplifier[5], 0),
		AudioConnection(&trackFilter[6], 0, &trackAmplifier[6], 0),
		AudioConnection(&trackFilter[7], 0, &trackAmplifier[7], 0)
};

AudioConnection          connectTrackAmplifierToLeftChannelMixer[8] =
{
		AudioConnection(&trackAmplifier[0], 0, &leftChannelTraksMixer, 0),
		AudioConnection(&trackAmplifier[1], 0, &leftChannelTraksMixer, 1),
		AudioConnection(&trackAmplifier[2], 0, &leftChannelTraksMixer, 2),
		AudioConnection(&trackAmplifier[3], 0, &leftChannelTraksMixer, 3),
		AudioConnection(&trackAmplifier[4], 0, &leftChannelTraksMixer, 4),
		AudioConnection(&trackAmplifier[5], 0, &leftChannelTraksMixer, 5),
		AudioConnection(&trackAmplifier[6], 0, &leftChannelTraksMixer, 6),
		AudioConnection(&trackAmplifier[7], 0, &leftChannelTraksMixer, 7)
};

AudioConnection          connectTrackAmplifierToRightChannelMixer[8] =
{
		AudioConnection(&trackAmplifier[0], 0, &rightChannelTracksMixer, 0),
		AudioConnection(&trackAmplifier[1], 0, &rightChannelTracksMixer, 1),
		AudioConnection(&trackAmplifier[2], 0, &rightChannelTracksMixer, 2),
		AudioConnection(&trackAmplifier[3], 0, &rightChannelTracksMixer, 3),
		AudioConnection(&trackAmplifier[4], 0, &rightChannelTracksMixer, 4),
		AudioConnection(&trackAmplifier[5], 0, &rightChannelTracksMixer, 5),
		AudioConnection(&trackAmplifier[6], 0, &rightChannelTracksMixer, 6),
		AudioConnection(&trackAmplifier[7], 0, &rightChannelTracksMixer, 7)
};

AudioConnection          connectTrackFilterToPreDelayMixer[8] =
{
		AudioConnection(&trackFilter[0], 0, &preDelayMixer, 0),
		AudioConnection(&trackFilter[1], 0, &preDelayMixer, 1),
		AudioConnection(&trackFilter[2], 0, &preDelayMixer, 2),
		AudioConnection(&trackFilter[3], 0, &preDelayMixer, 3),
		AudioConnection(&trackFilter[4], 0, &preDelayMixer, 4),
		AudioConnection(&trackFilter[5], 0, &preDelayMixer, 5),
		AudioConnection(&trackFilter[6], 0, &preDelayMixer, 6),
		AudioConnection(&trackFilter[7], 0, &preDelayMixer, 7)
};

AudioConnection          connectTrackFilterToPreReverbMixer[8] =
{
		AudioConnection(&trackFilter[0], 0, &preReverbMixer, 0),
		AudioConnection(&trackFilter[1], 0, &preReverbMixer, 1),
		AudioConnection(&trackFilter[2], 0, &preReverbMixer, 2),
		AudioConnection(&trackFilter[3], 0, &preReverbMixer, 3),
		AudioConnection(&trackFilter[4], 0, &preReverbMixer, 4),
		AudioConnection(&trackFilter[5], 0, &preReverbMixer, 5),
		AudioConnection(&trackFilter[6], 0, &preReverbMixer, 6),
		AudioConnection(&trackFilter[7], 0, &preReverbMixer, 7)
};

AudioConnection          connectPreDelayMixerToDelay(&preDelayMixer,&shortDelay);

AudioConnection          connectPreReverbMixerToReverb[2] =
{
		AudioConnection(&preReverbMixer,0,&polyverb,0),
		AudioConnection(&preReverbMixer,0,&polyverb,1)
};

AudioConnection          connectDelayToTracksMixer[2] =
{
		AudioConnection(&shortDelay, 0, &leftChannelTraksMixer, 8),
		AudioConnection(&shortDelay, 1, &rightChannelTracksMixer, 8)
};

AudioConnection          connectReverbToTracksMixer[2] =
{
		AudioConnection(&polyverb,0,&leftChannelTraksMixer,9),
		AudioConnection(&polyverb,1,&rightChannelTracksMixer,9)
};

AudioConnection          connectTracksMixertoBitDepth[2] =
{
		AudioConnection(&leftChannelTraksMixer, &bitDepthControl[0]),
		AudioConnection(&rightChannelTracksMixer, &bitDepthControl[1])
};

AudioConnection          connectBitDepthToLimiter[2] =
{
		AudioConnection(&bitDepthControl[0], 0, &limiter[0], 0),
		AudioConnection(&bitDepthControl[1], 0, &limiter[1], 0)
};

AudioConnection          connectLimiterToSourceSwitchingMixer[2] =
{
		AudioConnection(&limiter[0], 0, &leftChannelSourceSwitchingMixer, 0),
		AudioConnection(&limiter[1], 0, &rightChannelSourceSwitchingMixer, 0)
};
//********************** PREVIEW OR TEST AUDIO STREAM CONNECTIONS

AudioConnection          connectPlayer16BitWavFromSDToSourceSwitchingMixer[2] =
{
		AudioConnection(&player16BitWavFromSD, 0, &leftChannelSourceSwitchingMixer, 1),
		AudioConnection(&player16BitWavFromSD, 0, &rightChannelSourceSwitchingMixer, 1)
};

AudioConnection          connectPlayerFloatWavFromSDToSourceSwitchingMixer[2] =
{
		AudioConnection(&playerFloatWavFromSD,0,&leftChannelSourceSwitchingMixer,2),
		AudioConnection(&playerFloatWavFromSD,0,&rightChannelSourceSwitchingMixer,2)
};

AudioConnection          connectPlayer24BitWavFromSDToSourceSwitchingMixer[2] =
{
		AudioConnection(&player24BitWavFromSD,0,&leftChannelSourceSwitchingMixer,3),
		AudioConnection(&player24BitWavFromSD,0,&rightChannelSourceSwitchingMixer,3)
};

AudioConnection          connectTestWaveformToSourceSwitchingMixer[2] =
{
		AudioConnection(&testWaveform, 0, &rightChannelSourceSwitchingMixer, 4),
		AudioConnection(&testWaveform, 0, &leftChannelSourceSwitchingMixer, 4)
};

AudioConnection          connectMetronomeTickToSourceSwitchingMixer[2] =
{
		AudioConnection(&metronomeTick, 0, &rightChannelSourceSwitchingMixer, 5),
		AudioConnection(&metronomeTick, 0, &leftChannelSourceSwitchingMixer, 5)
};

//********************** ANALYZE CONNECTIONS
AudioConnection          connectTrackAmplifierToTrackRMS[8] =
{
		AudioConnection(&trackAmplifier[0], &trackRMS[0]),
		AudioConnection(&trackAmplifier[1], &trackRMS[1]),
		AudioConnection(&trackAmplifier[2], &trackRMS[2]),
		AudioConnection(&trackAmplifier[3], &trackRMS[3]),
		AudioConnection(&trackAmplifier[4], &trackRMS[4]),
		AudioConnection(&trackAmplifier[5], &trackRMS[5]),
		AudioConnection(&trackAmplifier[6], &trackRMS[6]),
		AudioConnection(&trackAmplifier[7], &trackRMS[7])
};

AudioConnection			 connectDelayToRMS[2] =
{
		AudioConnection(&shortDelay, 0, &delayRMS[0], 0),
		AudioConnection(&shortDelay, 1, &delayRMS[1], 0)
};

AudioConnection			 connectReverbToRMS[2] =
{
		AudioConnection(&polyverb, 0, &polyverbRMS[0], 0),
		AudioConnection(&polyverb, 1, &polyverbRMS[1], 0)
};

AudioConnection			 connectTrackMixersToRMS[2] =
{
		AudioConnection(&leftChannelTraksMixer, &mixedTracksLeftChannelRMS),
		AudioConnection(&rightChannelTracksMixer, &mixedTracksRightChannelRMS)
};

AudioConnection			 connectSourceSwitchingMixerToRMS[2] =
{
		AudioConnection(&leftChannelSourceSwitchingMixer, &exportLeftChannelRMS),
		AudioConnection(&rightChannelSourceSwitchingMixer, &exportRightChannelRMS)
};
//********************** STREAM TO OUTPUT CONNECTIONS

AudioConnection			 connectSourceSwitchingMixerToI2S[2] =
{
		AudioConnection(&rightChannelSourceSwitchingMixer, 0, &i2sOut, 0),
		AudioConnection(&leftChannelSourceSwitchingMixer, 0, &i2sOut, 1)
};

AudioConnection			 connectSourceSwitchingMixerToQueueOfExportedData[2] =
{
		AudioConnection(&leftChannelSourceSwitchingMixer, &queueOfLeftChannelExportedData),
		AudioConnection(&rightChannelSourceSwitchingMixer, &queueOfRightChannelExportedData)
};
//********************** INPUT SIGNAL RECORDING CONNECTIONS
AudioConnection			 connectI2SToMonoMixer[2] =
{
		AudioConnection(&i2sIn, 0, &inputChannelsToMonoMixer, 0),
		AudioConnection(&i2sIn, 1, &inputChannelsToMonoMixer, 1)
};

AudioConnection			 connectMonoMixerToQueueOfRecordedData[2] =
{
		AudioConnection(&inputChannelsToMonoMixer, &queueOfRecordedData),
		AudioConnection(&inputChannelsToMonoMixer, &analogInputSignalRMS)
};
//**********************
