#ifndef AUDIOENGINE_MTAUDIOENGINECONNECTIONSANDGLOBALOBJECTS_H_
#define AUDIOENGINE_MTAUDIOENGINECONNECTIONSANDGLOBALOBJECTS_H_

#include "Audio.h"

//********************** INPUTS
extern AudioInputI2S				i2sIn;
extern AudioMixer4              	inputChannelsToMonoMixer;
//********************** PREVIEW OR TEST
extern AudioPlaySdWav           	player16BitWavFromSD;
extern AudioPlaySdWavFloat		 	playerFloatWavFromSD;
extern AudioPlaySdWav24bit 	 		player24BitWavFromSD;
extern AudioSynthWaveform		 	testWaveform;
extern AudioPlayMemory 		 		metronomeTick;
//********************** TRACK STREAM
extern AudioPlayMemory          	trackMemoryPlayer[8];
extern AudioAmplifier           	trackAmplifier[8];
extern AudioFilterStateVariable 	trackFilter[8];
//********************** TRACK MODIFICATORS
extern envelopeGenerator		 	envelopeFilter[8];
extern envelopeGenerator		 	envelopeWtPosition[8];
extern envelopeGenerator		 	envelopeGranPosition[8];
extern envelopeGenerator		 	envelopePanning[8];
extern envelopeGenerator		 	envelopeFinetune[8];
//********************** SEND MIX
extern AudioMixer10			 		preDelayMixer,preReverbMixer;
//********************** SEND PROCESSING
extern AudioEffectShortDelay	 	shortDelay;
extern AudioEffectPolyverb		 	polyverb;
//********************** MIX
extern AudioMixer10			 		leftChannelTraksMixer,rightChannelTracksMixer;
//********************** MIXED STREAM
extern AudioEffectLimiter		 	limiter[2];
extern AudioBitDepth			 	bitDepthControl[2];
//********************** OUTPUTS
extern AudioOutputI2S           	i2sOut;
extern AudioRecordQueue         	queueOfRecordedData;
extern AudioRecordQueue		 		queueOfLeftChannelExportedData, queueOfRightChannelExportedData;
//********************** ANALYZE
extern AudioAnalyzeRMS			 	trackRMS[8];
extern AudioAnalyzeRMS			 	delayRMS[2];
extern AudioAnalyzeRMS			 	polyverbRMS[2];
extern AudioAnalyzeRMS			 	analogInputSignalRMS;
extern AudioAnalyzeRMS			 	exportLeftChannelRMS, exportRightChannelRMS;
extern AudioAnalyzeRMS			 	mixedTracksLeftChannelRMS, mixedTracksRightChannelRMS;
//********************** SOURCE(STREAM/PREVIEW OR TEST) SWITCHING MIX
extern AudioMixer10            		leftChannelSourceSwitchingMixer,rightChannelSourceSwitchingMixer;
//**********************



#endif /* AUDIOENGINE_MTAUDIOENGINECONNECTIONSANDGLOBALOBJECTS_H_ */
