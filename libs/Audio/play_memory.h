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
#include "mtWavetableTabs.h"
#include "mtNoteTabs.h"
#include "mtGranularTabs.h"

class AudioPlayMemory : public AudioStream
{
public:
	AudioPlayMemory(void) : AudioStream(0, NULL), playing(0)
	{
		envelope.state = 0;
		envelopeDelay(0.0f);  // default values...
		envelopeAttack(10.5f);
		envelopeHold(2.5f);
		envelopeDecay(35.0f);
		envelopeSustain(0.5f);
		envelopeRelease(300.0f);
		envelopeReleaseNoteOn(0.0f);
	}
	uint8_t play(uint8_t instr_idx,int8_t note); 										// dla sequencer'a
	uint8_t playForPrev(uint8_t instr_idx,int8_t n); 									// dla padboard'a - po indeksie instrumentu
	uint8_t playForPrev(int16_t * addr,uint32_t len,uint8_t type);						// dla importera (odgrywa sample z banku) + umieszczony w wewnętrznym module recordera ale nie uzywany
	uint8_t playForPrev(int16_t * addr,uint32_t len, uint8_t n,uint8_t type);			// dla padboard'a - po pamięci

	void stop(void);																	// do noteOff

	void clean(void);																	// nigdzie nie wykorzystane - ale kto wie?
	bool isPlaying(void) { return playing; }											// status odtwarzania
	virtual void update(void);
	uint16_t getPosition();
	void setInterpolationEnable(bool value);

	//****************** modyfikacja parametrow z instrumentów
	void setLP1(uint16_t value);
	void setLP2(uint16_t value);

	//******Pitch
	void setGlide(uint16_t value, int8_t currentNote,uint8_t instr_idx);
	void setFineTune(int8_t value, int8_t currentNote);
	void setTune(int8_t value, int8_t currentNote);
	//*********
	//*****Granular
	void setGranularGrainLength();
	void setGranularWave(uint8_t type);
	void setGranularLoopMode(uint8_t type);
	void setGranularPosition( uint16_t val);
	//*********
	//*****Wavetable
	void setWavetableWindow(int16_t value);
	//*********
	//*****************************************************************
	//*********Wymuszanie Parametrów z Sequencera lub PerformanceMode

	//************Reverse
	void setReverse();
	void clearReverse();
	//*********
	//************Pitch
	//Tune
	void setTuneForceFlag();
	void clearTuneForceFlag();
	void setForcedTune(int8_t value);
	//Glide
	void setGlideForceFlag();
	void clearGlideForceFlag();
	void setForcedGlide(uint16_t value);
	//Finetune
	void setFineTuneForceFlag();
	void clearFineTuneForceFlag();
	void setForcedFineTune(int8_t value);
	//*********
	//************Wavetable
	void setWavetableWindowFlag();
	void clearWavetableWindowFlag();
	void setForcedWavetableWindow(uint32_t val);
	//*********
	//************Pointy Loopów
	void setPointsForceFlag();
	void clearPointsForceFlag();
	void setForcedPoints(int32_t sp, int32_t lp1, int32_t lp2, int32_t ep); // -1 nie nadpisuje
	//*********
	//************Granular
	void setGranularPosForceFlag();
	void clearGranularPosForceFlag();
	void setForcedGranularPos(uint16_t value);
	//*********
	//************Slice
	void setForcedSlice(uint8_t value);
	void setSliceForcedFlag();
	void clearSliceForcedFlag();
	//************************************************

	void setCurrentInstrIdx(uint8_t n);

//******************************************************************ENVELOPE PUBLIC BLOCK START****************************************************************************//
	void envelopeNoteOn(uint8_t instr_idx,int8_t note);
	void envelopeNoteOff();

	void envelopeNoteOnForPrev(uint8_t instr_idx,int8_t note);
	void envelopeNoteOnForPrev(int16_t * addr,uint32_t len,uint8_t type);
	void envelopeNoteOnForPrev(int16_t * addr,uint32_t len, uint8_t n,uint8_t type);

	void envelopeSetIdle();

	void envelopeDelay(float milliseconds);
	void envelopeAttack(float milliseconds);
	void envelopeHold(float milliseconds);
	void envelopeDecay(float milliseconds);
	void envelopeSustain(float level);
	void envelopeRelease(float milliseconds);
	void envelopeSetSustain();
	void envelopeReleaseNoteOn(float milliseconds);
	void envelopeSetLoop(uint8_t state);
	void envelopeSetPassFlag(uint8_t state);
	void envelopeSetIsRandom(bool value);

	void envelopeUpdate(audio_block_t *block);
	uint8_t envelopeEndRelease();
	void envelopeClearEndReleaseFlag();

	uint8_t envelopeGetEndReleaseKill();
	void envelopeClearEndReleaseKill();
	uint8_t envelopeGetState();

	void envelopeSyncTrackerSeq(uint32_t val, float seqSpeed);
	void envelopeSetSyncStartStep(uint16_t n);
	void envelopeSetPhaseNumbers(int8_t n1, int8_t n2);
	void envelopeSetSyncRate(float sync);
	enum enEnvelopePhase
	{
		envelopePhaseIdle,
		envelopePhaseDelay,
		envelopePhaseAttack,
		envelopePhaseHold,
		envelopePhaseDecay,
		envelopePhaseSustain,
		envelopePhaseRelease,
		envelopePhaseForced
	};




//******************************************************************ENVELOPE PUBLIC BLOCK END****************************************************************************//
private:

//  PLAY OBSLUGA OGÓLNA
	void refreshStartParamiters();
	void constrainCurrentTune(uint8_t note);
	void setStartPitch(uint8_t note);
	void applyFinetuneOnPitch(uint8_t note);
	void calculateGlidePitch(uint8_t note);
	float getChangeVolumeCoef();
//	PLAY Z PODZIALEM NA TRYBY
	void playSingleShot(uint8_t instrIdx, int8_t note);
	void playLoopForward(uint8_t instrIdx, int8_t note);
	void playLoopBackward(uint8_t instrIdx, int8_t note);
	void playLoopPingpong(uint8_t instrIdx, int8_t note);
	void playSlice(uint8_t instrIdx, int8_t note);
	void playBeatSlice(uint8_t instrIdx, int8_t note);
	void playGranular(uint8_t instrIdx, int8_t note);
	void playWavetable(uint8_t instrIdx, int8_t note);
// UPDATE OBSLUGA OGÓLNA


// UPDATE Z PODZIALEM NA TRYBY
	audio_block_t * updateSingleShot();
	audio_block_t * updateLoopForward();
	audio_block_t * updateLoopBackward();
	audio_block_t * updateLoopPingpong();
	audio_block_t * updateSlice();
	audio_block_t * updateBeatSlice();
	audio_block_t * updateGranular();
	audio_block_t * updateWavetable();

//singleShot
	audio_block_t * updateSingleShotNormal();
	audio_block_t * updateSingleShotReverse();
//loopForward
	audio_block_t * updateLoopForwardNormal();
	audio_block_t * updateLoopForwardReverse();
//loopBackward
	audio_block_t * updateLoopBackwardNormal();
	audio_block_t * updateLoopBackwardReverse();
//loopPingPong
	audio_block_t * updateLoopPingPongNormal();
	audio_block_t * updateLoopPingPongReverse();
// granular

	audio_block_t * updateGranularLoopForward();
	audio_block_t * updateGranularLoopBackward();
	audio_block_t * updateGranularLoopPingPong();

	audio_block_t * updateGranularLoopForwardNormal();
	audio_block_t * updateGranularLoopForwardReverse();
	audio_block_t * updateGranularLoopBackwardNormal();
	audio_block_t * updateGranularLoopBackwardReverse();
	audio_block_t * updateGranularLoopPingPongNormal();
	audio_block_t * updateGranularLoopPingPongReverse();
/////////////////////////

	void refreshGranularPosition();

	//********** Zarządzanie
	const float * wt_notes = wt2048Note;					// wskaźnik imitujacy tablice do pitchowania wavetabli (jest przełączalny - dlatego nie jest stałym wskaznikiem)
	volatile int16_t *next;
	volatile uint32_t length;
	volatile uint8_t playing;
	uint8_t currentPlayMode;
	uint8_t loopBackwardFlag;								// kierunek playhead'a w loopie
	int8_t	lastNote = -1;									// ostatnia nuta - potrzebne przy glidach i slidach
	volatile int16_t * currentStartAddress;

	struct strSamplePoints									// pointy umieszczone w pamieci dla konkretnej probki
	{
		uint32_t start=0;
		uint32_t end=0;
		uint32_t loop1=0;
		uint32_t loop2=0;
	} pointsInSamples;

	struct strSampleConstrains								// określają granice dla licznika pitcha
	{
		uint32_t loopLength;
		uint32_t loopPoint1;
		uint32_t loopPoint2;
		uint32_t endPoint;

		uint32_t glide;

	} constrainsInSamples;

	int32_t interpolationDif;
	int16_t nextSample;
	uint32_t interpolationCondition;
	int16_t currentSampelValue;

	volatile uint32_t currentSampleLength;
	int16_t  lastSample = 0;
	uint8_t needSmoothingFlag = 0;							// ustawiana przy gwaltownej zmianie pamieci aby wygładzic przejscie

	uint8_t currentInstrIdx;
	int8_t currentInstrIdxInPlay = -1;
	int8_t lastInstrIdxInPlay = - 1;

	bool enableInterpolation = true;
	//*******
	//********* Pitch
	float pitchControl = 1;									// Glowna zmienna kontrolujaca pitch
	float fPitchCounter;									// Licznik probek uwzgledniajacy pitch - akumulacja zmienno przecinkowa
	int32_t iPitchCounter;									// zrzutowany licznik zmienno przecinkowy - odnosi sie do konkretnej probki w pamieci
	uint16_t currentGlide;
	uint32_t glideCounter;									// licznik glide'a w czasie
	float glideControl;										// zmienna opisujaca jednostke zmiany pitcha na jednostke czasu - dodawana do pitchControl
	float finetunePitchControl;									// liniowe przeliczenie fineTuna 0-100 między bierzącą nutą a kolejną(lub poprzednią)
	int8_t currentTune;
	int8_t currentFineTune;
	//********
	//********** Wavetable
	uint16_t wavetableWindowSize;
	uint32_t currentWindow;
	uint32_t waveTablePosition;

	//********
	//********** Granular
	uint8_t granularLoopType;
	uint16_t currentGranularPosition;
	const uint16_t * granularEnvelopeTab = nullptr;			// wskaznik imitujacy tablice wplywajaca na amplitude próbek

	uint8_t granularPositionRefreshFlag = 1;				// pozycja jest aktualizowana gdy nastepuje przejscie pętli(audio) i ustawiona jest ta flaga
	//********

	//*************** Zmienne wymuszajace zmiany parametrow instrumentow przez Sequencer lub Performance Mode

	struct strStackedPlay
	{
		bool enable;
		uint8_t instr_idx;
		int8_t note;
		bool isPrev;
	} stackedPlay;

	uint8_t tuneForceFlag;
	uint8_t pointsForceFlag;
	uint8_t glideForceFlag;
	uint8_t fineTuneForceFlag;
	uint8_t wavetableWindowForceFlag;
	uint8_t sliceForcedFlag;
	uint8_t granularForcedFlag;
	uint8_t reverseDirectionFlag;

	uint16_t forcedStartPoint;
	uint16_t forcedLoopPoint1;
	uint16_t forcedLoopPoint2;
	uint16_t forcedEndPoint;
	int8_t forcedTune;
	uint16_t forcedGlide;
	int8_t forcedFineTune;
	uint32_t forcedWavetableWindow;
	uint8_t forcedSlice;
	uint16_t forcedGranularPosition;

	//*****************
//******************************************************************ENVELOPE PRIVATE BLOCK START****************************************************************************//
	void envelopeSwitchPhase(uint8_t nextPhase);
	uint16_t envelopeMilliseconds2count(float milliseconds);

	struct strEnvelopeData
	{
		uint8_t endReleaseFlag = 0;
		uint8_t endKillReleaseFlag = 0;
		bool isRandom;
		audio_block_t *inputQueueArray[1];
		// state
		uint8_t  state;      // idle, delay, attack, hold, decay, sustain, release, forced
		uint16_t count;      // how much time remains in this state, in 8 sample units
		int32_t  mult_hires; // attenuation, 0=off, 0x40000000=unity gain
		int32_t  inc_hires;  // amount to change mult_hires every 8 samples

		// settings
		uint16_t delay_count;
		uint16_t attack_count;
		uint16_t hold_count;
		uint16_t decay_count;
		int32_t  sustain_mult;
		uint16_t release_count;
		uint16_t release_forced_count;
		uint8_t loopFlag = 0;
		uint8_t pressedFlag = 0;
		uint8_t passFlag = 0;
		uint32_t sample12, sample34, sample56, sample78;
		uint32_t tmp1, tmp2;

		uint16_t startStep = 0;
		int8_t phaseNumber[2] = {-1,-1};
		float syncRate = 1;
		uint16_t periodTime = 0;
	} envelope;

//******************************************************************ENVELOPE PRIVATE BLOCK END****************************************************************************//

};


#endif
