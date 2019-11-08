/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef play_memory_h_
#define play_memory_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "mtStructs.h"

class AudioPlayMemory : public AudioStream
{
public:
	AudioPlayMemory(void) : AudioStream(0, NULL), playing(0) { }
	uint8_t play(uint8_t instr_idx,int8_t note);
	void clean(void);
	void stop(void);
	bool isPlaying(void) { return playing; }
	virtual void update(void);
	void setPlayMode(uint8_t value); //global
	void setLP1(uint16_t value); //global
	void setLP2(uint16_t value); //global
	void setGlide(uint16_t value, int8_t currentNote,uint8_t instr_idx); // global
	void setPitch(float value); // incremental
	void setSlide(uint16_t value, int8_t currentNote, int8_t slideNote,uint8_t instr_idx);//incremental
	void setFineTune(int8_t value, int8_t currentNote);
	void setWavetableWindow(int16_t value);
	void setTune(int8_t value, int8_t currentNote);
	void setReverse();
	void clearReverse();
	uint8_t playForPrev(uint8_t instr_idx,int8_t n);
	uint8_t playForPrev(int16_t * addr,uint32_t len,uint8_t type);
	uint8_t playForPrev(int16_t * addr,uint32_t len, uint8_t n,uint8_t type);
	void setWavetableWindowFlag();
	void clearWavetableWindowFlag();
	void setForcedWavetableWindow(int16_t val);
	void setTuneForceFlag();
	void clearTuneForceFlag();
	void setForcedTune(int8_t value);
	void setPointsForceFlag();
	void clearPointsForceFlag();
	void setGlideForceFlag();
	void clearGlideForceFlag();
	void setForcedGlide(uint16_t value);
	void setFineTuneForceFlag();
	void clearFineTuneForceFlag();
	void setForcedFineTune(int8_t value);
	void setForcedPoints(int32_t sp, int32_t lp1, int32_t lp2, int32_t ep); // -1 nie nadpisuje
	uint16_t getPosition();
private:
	int16_t *next;
	int16_t *beginning;
	uint32_t length;
	float pitchControl = 1;
	float fPitchCounter;
	uint32_t iPitchCounter;
	uint8_t playMode;
	volatile uint8_t playing;
	uint8_t loopBackwardFlag;
	int8_t	lastNote = -1;
	uint16_t glide;
	uint32_t glideCounter;
	uint32_t slideCounter;
	float glideControl;
	float slideControl;
	float fineTuneControl;
	int8_t currentTune;
	uint16_t wavetableWindowSize;
	uint32_t currentWindow;
	uint32_t waveTablePosition;
	uint8_t sampleType;
//    uint16_t wavetableSync;
//    uint16_t wavetablePWM;
//    uint16_t wavetableFlip;
//    uint16_t wavetableQuantize;
	uint8_t currentInstr_idx;
	int8_t currentFineTune;
	uint8_t tuneForceFlag;
	uint8_t pointsForceFlag;
	uint8_t glideForceFlag;
	uint8_t fineTuneForceFlag;
	uint8_t wavetableWindowForceFlag;


	uint8_t reverseDirectionFlag;

	uint16_t forcedStartPoint;
	uint16_t forcedLoopPoint1;
	uint16_t forcedLoopPoint2;
	uint16_t forcedEndPoint;
	int8_t forcedTune;
	uint16_t forcedGlide;
	int8_t forcedFineTune;
	uint16_t forcedWavetableWindow;

	struct strSamplePoints
	{
		uint32_t start=0;
		uint32_t end=0;
		uint32_t loop1=0;
		uint32_t loop2=0;
	} samplePoints;

	struct strSampleConstrains
	{
		uint32_t loopLength;
		uint32_t loopPoint1;
		uint32_t loopPoint2;
		uint32_t endPoint;

		uint32_t glide;
		uint32_t slide;

	} sampleConstrains;

	uint32_t startLen;
	int16_t  lastSample = 0;
	uint8_t needSmoothingFlag = 0;
};


//--------------------------------------------------------------
// przerzucone z mtStructs.h
const double notes[MAX_NOTE] =
{
		0.0312500000000,
		0.0331082216987,
		0.0350769390096,
		0.0371627223438,
		0.0393725328092,
		0.0417137454428,
		0.0441941738241,
		0.0468220961524,
		0.0496062828740,
		0.0525560259533,
		0.0556811698837,
		0.0589921445426,
		0.0625000000000,
		0.0662164433974,
		0.0701538780193,
		0.0743254446876,
		0.0787450656184,
		0.0834274908856,
		0.0883883476483,
		0.0936441923047,
		0.0992125657480,
		0.1051120519067,
		0.1113623397675,
		0.1179842890852,
		0.1250000000000,
		0.1324328867949,
		0.1403077560386,
		0.1486508893753,
		0.1574901312368,
		0.1668549817712,
		0.1767766952966,
		0.1872883846095,
		0.1984251314960,
		0.2102241038134,
		0.2227246795350,
		0.2359685781704,
		0.2500000000000,
		0.2648657735898,
		0.2806155120773,
		0.2973017787507,
		0.3149802624737,
		0.3337099635425,
		0.3535533905933,
		0.3745767692192,
		0.3968502629921,
		0.4204482076269,
		0.4454493590702,
		0.4719371563408,
		0.5000000000000,
		0.5297315471796,
		0.5612310241547,
		0.5946035575014,
		0.6299605249474,
		0.6674199270850,
		0.7071067811865,
		0.7491535384383,
		0.7937005259841,
		0.8408964152537,
		0.8908987181403,
		0.9438743126817,
		1.0000000000000,
		1.0594630943593,
		1.1224620483094,
		1.1892071150027,
		1.2599210498949,
		1.3348398541700,
		1.4142135623731,
		1.4983070768767,
		1.5874010519682,
		1.6817928305074,
		1.7817974362807,
		1.8877486253634,
		2.0000000000000,
		2.1189261887186,
		2.2449240966188,
		2.3784142300055,
		2.5198420997898,
		2.6696797083401,
		2.8284271247462,
		2.9966141537534,
		3.1748021039364,
		3.3635856610149,
		3.5635948725614,
		3.7754972507268,
		4.0,
		4.23785237743718,
		4.48984819323749,
		4.75682846001089,
		5.03968419957949,
		5.33935941668014,
		5.65685424949238,
		5.99322830750673,
		6.34960420787280,
		6.72717132202972,
		7.12718974512272,
		7.55099450145355,
		8.00000000000000,
		8.47570475487437,
		8.97969638647499,
		9.51365692002177,
		10.0793683991590,
		10.6787188333603,
		11.3137084989848,
		11.9864566150135,
		12.6992084157456,
		13.4543426440594,
		14.2543794902454,
		15.1019890029071,
		16.0000000000000,
		16.9514095097487,
		17.9593927729500,
		19.0273138400436,
		20.1587367983180,
		21.3574376667206,
		22.6274169979696,
		23.9729132300269,
		25.3984168314912,
		26.9086852881189,
		28.5087589804909,
		30.2039780058142
};


#endif
