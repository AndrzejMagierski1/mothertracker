
#ifndef INTERFACE_INTERFACEFXDESC_H_
#define INTERFACE_INTERFACEFXDESC_H_


const char fxDescriptions[FX_COUNT][300] =
{
	//Sequencer::strFxConsts::enFxType::FX_TYPE_NONE,
	{
			"Indicates that there's no effect used on the desired step."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_OFF,
	{
			"Toggle off the effect used on previous step."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_VELOCITY,
	{
			"Sets the Velocity of an \nInstrument.\n\nFrom 0% to 100%."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_PANNING,
	{
			"Sets the Panning of an \nInstrument.\n\nFrom -50 (L) to +50 (R)."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_MICROTUNING,
	{
			"Sets the microtuning \nadjustments."
			"\n"
			"\nFrom -99 cents \nto +99 cents."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_GLIDE,
	{
			"Sets the time it takes for the pitch of previous note to travel to the pitch of corresponding note."
			"\n"
			"\n"
			"From 0-49% (0-1s) to 50-100% (1-15s)."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_TEMPO,
	{
			"Changes pattern tempo. Handy to create custom swings or grooves."
			"\n"
			"\n"
			"From 10% to 400%."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_MICROMOVE,
	{
			"Slightly moves (nudges) forward the step position."
			"\n"
			"\n"
			"From 0% to 100%."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_CHANCE,
	{
			"Sets a chance of a step being played."
			"\n"
			"\n"
			"From 0% to 100%."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL,
	{
			"Sets the type and value of the Roll (step divider) parameter."
			"\n"
			"\nFrom 0 to 16 for each Roll type."
			"\nR - regular Roll."
			"\nRv - decreasing volume \nRoll."
			"\nRV - increasing volume \nRoll."
			"\nRn - decreasing note Roll."
			"\nRN - increasing note Roll."
			"\nRR - random note Roll."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_RANDOM_NOTE,
	{
			"Sets the range of randomisation for Note with each sequencer pass."
			"\n"
			"\n"
			"From +/- 0 to +/- 100"
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_RANDOM_INSTRUMENT,
	{
			"Sets the range of randomisation for Instrument number with each sequencer pass."
			"\n"
			"\n"
			"From +/- 0 to +/- 47"
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_RANDOM_VALUE,
	{
			"Sets the range of randomisation for the FX that is in the same step with each sequencer pass. For example, if randomiser value is 10 and affected FX value i 100, the algorithm will pick the value from 90 to 110."
			"\n"
			"\n"
			"From +/- 0 to +/-255."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_RANDOM_VELOCITY,
	{
			"Sets the range of randomisation for Velocity with each sequencer pass."
			"\n"
			"\n"
			"From +/- 0 to +/- 100"
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_REVERSE_PLAYBACK,
	{
			"Changes the playback direction of the sample."
			"\n"
			"\n"
			"Values: <<< and >>>."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_POSITION,
	{
			"In relation to the chosen sample playback type - changes the position of: Sample start or Wavetable position or Granular \nposition."
			"\n"
			"\n"
			"From 0% to 100%."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_SAMPLE_SLICE,
	{
			"Sets the slice number of a instrument with a Slice playback type."
			"\n"
			"\n"
			"From 1 to 48."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_VOLUME_LFO,
	{
			"Sets the LFO rate for instrument Volume. LFO is synced to project \nTempo."
			"\n"
			"\n"
			"Values: 6; 4; 3; 2; 3/2; 1; 3/4; 1/2; 3/8; 1/3; 1/4; 316 (3/16); 1/6; 1/8; /12 (1/12); /16 (1/16); /24 (1/24); /32 (1/32); /48 (1/48); /64 (1/64)."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_PANNING_LFO,
	{
			"Sets the LFO rate for Panning. LFO is synced to project Tempo."
			"\n"
			"\n"
			"Values: 6; 4; 3; 2; 3/2; 1; 3/4; 1/2; 3/8; 1/3; 1/4; 316 (3/16); 1/6; 1/8; /12 (1/12); /16 (1/16); /24 (1/24); /32 (1/32); /48 (1/48); /64 (1/64)."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_FILTER_LFO,
	{
			"Sets the LFO rate for the selected Filter type. LFO is synced to project Tempo."
			"\n"
			"\n"
			"Values: 6; 4; 3; 2; 3/2; 1; 3/4; 1/2; 3/8; 1/3; 1/4; 316 (3/16); 1/6; 1/8; /12 (1/12); /16 (1/16); /24 (1/24); /32 (1/32); /48 (1/48); /64 (1/64)."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_POSITION_LFO,
	{
			"Sets the LFO rate for the selected playback type (Wavetable, Granular). Position in respect to the project tempo (BPM)."
			"\n"
			"\n"
			"Values: 6; 4; 3; 2; 3/2; 1; 3/4; 1/2; 3/8; 1/3; 1/4; 316 (3/16); 1/6; 1/8; /12 (1/12); /16 (1/16); /24 (1/24); /32 (1/32); /48 (1/48); /64 (1/64)."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_POSITION_LFO,
	{
			"Sets the LFO rate for instrument Finetune. LFO is synced to project Tempo."
			"\n"
			"\n"
			"Values: 6; 4; 3; 2; 3/2; 1; 3/4; 1/2; 3/8; 1/3; 1/4; 316 (3/16); 1/6; 1/8; /12 (1/12); /16 (1/16); /24 (1/24); /32 (1/32); /48 (1/48); /64 (1/64)."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_FILTER_LOWPASS,
	{
			"Sets the cutoff amount of the Low-pass Filter."
			"\n"
			"\n"
			"From 0% to 100%."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_FILTER_BANDPASS,
	{
			"Sets the cutoff amount of the Band-pass Filter."
			"\n"
			"\n"
			"From 0% to 100%."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_FILTER_HIGHPASS,
	{
			"Sets the cutoff amount of the High-pass Filter."
			"\n"
			"\n"
			"From 0% to 100%."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_REVERB_SEND,
	{
			"Sets the amount of Delay send."
			"\n"
			"\n"
			"From 0% to 100%."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_A,
	{
			"Sends MIDI CC Value set to A in the Config."
			"\n"
			"\n"
			"From 0 to 127."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_B,
	{
			"Sends MIDI CC Value set to B in the Config."
			"\n"
			"\n"
			"From 0 to 127."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_C,
	{
			"Sends MIDI CC Value set to C in the Config."
			"\n"
			"\n"
			"From 0 to 127."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_D,
	{
			"Sends MIDI CC Value set to D in the Config."
			"\n"
			"\n"
			"From 0 to 127."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_E,
	{
			"Sends MIDI CC Value set to E in the Config."
			"\n"
			"\n"
			"From 0 to 127."
	},
	//Sequencer::strFxConsts::enFxType::FX_TYPE_PROGRAM_CHANGE,
	{
			"Sends MIDI Program Change Value."
			"\n"
			"\n"
			"From 0 to 127."
	},
//	//Sequencer::strFxConsts::enFxType::FX_TYPE_PROGRAM_CHANGE,
//	{
//			"Sends MIDI Program Change Value."
//			"\n"
//			"\n"
//			"From 0 to 127."
//	},
//	//Sequencer::strFxConsts::enFxType::FX_TYPE_PROGRAM_CHANGE,
//	{
//			"Sends MIDI Program Change Value."
//			"\n"
//			"\n"
//			"From 0 to 127."
//	},
//	//Sequencer::strFxConsts::enFxType::FX_TYPE_PROGRAM_CHANGE,
//	{
//			"Sends MIDI Program Change Value."
//			"\n"
//			"\n"
//			"From 0 to 127."
//	},
//	//Sequencer::strFxConsts::enFxType::FX_TYPE_PROGRAM_CHANGE,
//	{
//			"Sends MIDI Program Change Value."
//			"\n"
//			"\n"
//			"From 0 to 127."
//	},
//	//Sequencer::strFxConsts::enFxType::FX_TYPE_PROGRAM_CHANGE,
//	{
//			"Sends MIDI Program Change Value."
//			"\n"
//			"\n"
//			"From 0 to 127."
//	},
//	//Sequencer::strFxConsts::enFxType::FX_TYPE_PROGRAM_CHANGE,
//	{
//			"Sends MIDI Program Change Value."
//			"\n"
//			"\n"
//			"From 0 to 127."
//	},
//	//Sequencer::strFxConsts::enFxType::FX_TYPE_PROGRAM_CHANGE,
//	{
//			"Sends MIDI Program Change Value."
//			"\n"
//			"\n"
//			"From 0 to 127."
//	},

};


#endif /* INTERFACE_INTERFACEFXDESC_H_ */
