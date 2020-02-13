/* Audio Library for Teensy 3.X
 * Copyright (c) 2018, Paul Stoffregen, paul@pjrc.com
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

// A fixed point implementation of Freeverb by Jezar at Dreampoint
//  http://blog.bjornroche.com/2012/06/freeverb-original-public-domain-code-by.html
//  https://music.columbia.edu/pipermail/music-dsp/2001-October/045433.html

#include <Arduino.h>
#include "effect_freeverb.h"
#include "utility/dspinst.h"

constexpr uint16_t RELEASE_TAB_SIZE = 500;

const float logReleaseTab[RELEASE_TAB_SIZE] =
{
		1,
		0.998614,
		0.997231,
		0.995849,
		0.994470,
		0.993092,
		0.991716,
		0.990342,
		0.988970,
		0.987600,
		0.986232,
		0.984866,
		0.98350,
		0.982139,
		0.98077,
		0.979420,
		0.978063,
		0.97670,
		0.975355,
		0.974004,
		0.972654,
		0.971307,
		0.969961,
		0.968618,
		0.967276,
		0.965936,
		0.964598,
		0.96326,
		0.961927,
		0.96059,
		0.959264,
		0.957935,
		0.956608,
		0.955282,
		0.953959,
		0.95263,
		0.951318,
		0.950000,
		0.948684,
		0.947370,
		0.946057,
		0.94474,
		0.943438,
		0.942131,
		0.940826,
		0.939522,
		0.938221,
		0.936921,
		0.935623,
		0.934327,
		0.933032,
		0.931740,
		0.930449,
		0.929160,
		0.927873,
		0.926588,
		0.92530,
		0.924022,
		0.922742,
		0.921464,
		0.920187,
		0.918912,
		0.917639,
		0.916368,
		0.915099,
		0.913831,
		0.912565,
		0.911301,
		0.910038,
		0.908778,
		0.907519,
		0.906261,
		0.905006,
		0.903752,
		0.902500,
		0.901250,
		0.900001,
		0.898755,
		0.897510,
		0.896266,
		0.895025,
		0.893785,
		0.892546,
		0.891310,
		0.890075,
		0.888842,
		0.887611,
		0.886381,
		0.885153,
		0.883927,
		0.882702,
		0.881480,
		0.880259,
		0.879039,
		0.877821,
		0.876605,
		0.875391,
		0.874178,
		0.872967,
		0.871758,
		0.870550,
		0.869344,
		0.868140,
		0.866937,
		0.865736,
		0.864537,
		0.863339,
		0.86214,
		0.860949,
		0.859756,
		0.858565,
		0.857376,
		0.856188,
		0.855002,
		0.853817,
		0.852634,
		0.851453,
		0.850274,
		0.849096,
		0.847919,
		0.846745,
		0.845572,
		0.844400,
		0.843231,
		0.842062,
		0.840896,
		0.839731,
		0.838568,
		0.837406,
		0.836246,
		0.835087,
		0.833931,
		0.832775,
		0.831622,
		0.830470,
		0.829319,
		0.828170,
		0.827023,
		0.825877,
		0.824733,
		0.823591,
		0.822450,
		0.821310,
		0.820172,
		0.819036,
		0.817902,
		0.816768,
		0.815637,
		0.814507,
		0.813379,
		0.812252,
		0.811127,
		0.810003,
		0.808881,
		0.807760,
		0.806641,
		0.80552,
		0.804408,
		0.803293,
		0.802181,
		0.801069,
		0.799960,
		0.798851,
		0.797745,
		0.796640,
		0.795536,
		0.794434,
		0.793333,
		0.792234,
		0.791137,
		0.790041,
		0.788946,
		0.78785,
		0.786762,
		0.785672,
		0.784584,
		0.783497,
		0.782411,
		0.781327,
		0.780245,
		0.779164,
		0.778085,
		0.777007,
		0.775930,
		0.77485,
		0.773782,
		0.772710,
		0.771640,
		0.770571,
		0.769503,
		0.768437,
		0.76737,
		0.766309,
		0.765248,
		0.764188,
		0.763129,
		0.762072,
		0.761016,
		0.759962,
		0.758909,
		0.757858,
		0.756808,
		0.755759,
		0.754712,
		0.753667,
		0.752623,
		0.75158,
		0.750539,
		0.749499,
		0.748461,
		0.747424,
		0.746389,
		0.745355,
		0.744322,
		0.743291,
		0.742261,
		0.741233,
		0.74020,
		0.739181,
		0.738157,
		0.737134,
		0.736113,
		0.735093,
		0.73407,
		0.733058,
		0.73204,
		0.731028,
		0.730016,
		0.729004,
		0.727994,
		0.726986,
		0.7259,
		0.724973,
		0.723969,
		0.722966,
		0.721964,
		0.720964,
		0.719965,
		0.718968,
		0.717972,
		0.71697,
		0.715984,
		0.714992,
		0.714001,
		0.713012,
		0.712025,
		0.711038,
		0.710053,
		0.709070,
		0.708087,
		0.707106,
		0.706127,
		0.705148,
		0.704172,
		0.703,
		0.702222,
		0.701249,
		0.700278,
		0.699308,
		0.698339,
		0.697371,
		0.696405,
		0.695440,
		0.694477,
		0.693515,
		0.692554,
		0.691595,
		0.690637,
		0.689680,
		0.688725,
		0.687770,
		0.686818,
		0.685866,
		0.6849,
		0.68396,
		0.683020,
		0.682073,
		0.681129,
		0.680185,
		0.679243,
		0.678302,
		0.677362,
		0.676424,
		0.675487,
		0.674551,
		0.673616,
		0.672683,
		0.671751,
		0.670821,
		0.669891,
		0.668963,
		0.668037,
		0.667111,
		0.666187,
		0.665264,
		0.66434,
		0.663422,
		0.662503,
		0.6615,
		0.660669,
		0.659753,
		0.658839,
		0.657927,
		0.657015,
		0.656105,
		0.655196,
		0.654289,
		0.653382,
		0.652477,
		0.651573,
		0.650670,
		0.649769,
		0.648869,
		0.647970,
		0.647072,
		0.646176,
		0.645281,
		0.644387,
		0.643494,
		0.642603,
		0.641712,
		0.640823,
		0.639936,
		0.639049,
		0.638164,
		0.637280,
		0.636397,
		0.635515,
		0.63463,
		0.63375,
		0.63287,
		0.632001,
		0.631126,
		0.630251,
		0.629378,
		0.628506,
		0.627635,
		0.626766,
		0.625898,
		0.625031,
		0.624165,
		0.623300,
		0.622437,
		0.621574,
		0.620713,
		0.6198,
		0.618995,
		0.618137,
		0.617281,
		0.616426,
		0.615572,
		0.614719,
		0.61386,
		0.6130,
		0.612168,
		0.611320,
		0.610473,
		0.60962,
		0.608783,
		0.607939,
		0.607097,
		0.606256,
		0.60541,
		0.604577,
		0.603740,
		0.602903,
		0.602068,
		0.601234,
		0.600401,
		0.599569,
		0.598739,
		0.597909,
		0.597081,
		0.596254,
		0.595428,
		0.594603,
		0.59377,
		0.592957,
		0.592135,
		0.591315,
		0.590496,
		0.589678,
		0.588861,
		0.588045,
		0.587230,
		0.586417,
		0.585605,
		0.58479,
		0.583983,
		0.583174,
		0.582366,
		0.581560,
		0.58075,
		0.579949,
		0.579146,
		0.57834,
		0.577542,
		0.576742,
		0.575943,
		0.575145,
		0.574349,
		0.573553,
		0.57275,
		0.571965,
		0.571173,
		0.570381,
		0.569591,
		0.568802,
		0.568014,
		0.567227,
		0.566441,
		0.565657,
		0.564873,
		0.564091,
		0.563309,
		0.562529,
		0.561749,
		0.560971,
		0.560194,
		0.559418,
		0.55864,
		0.557869,
		0.557096,
		0.556325,
		0.555554,
		0.55478,
		0.554016,
		0.553248,
		0.552482,
		0.5517,
		0.550952,
		0.550189,
		0.549427,
		0.548665,
		0.547905,
		0.547146,
		0.546388,
		0.545631,
		0.544876,
		0.544121,
		0.543367,
		0.542614,
		0.541862,
		0.541112,
		0.54036,
		0.539614,
		0.538866,
		0.538120,
		0.537374,
		0.536630,
		0.535886,
		0.483369,
		0.435999,
		0.393271,
		0.354731,
		0.3199,
		0.288610,
		0.260326,
		0.234814,
		0.211802,
		0.191046,
		0.172323,
		0.155435,
		0.140203,
		0.126463,
		0.114069,
		0.102891,
		0.092807,
		0.083712,
		0.075508,
		0.068108,
		0.06143,
		0.055413,
		0.049983,
		0.045084,
		0.040666,
		0.036681,
		0.033086,
		0.029843,
		0.026919,
		0.024281,
		0.021901,
		0.019755,
		0.017819,
		0.016072,
		0.014497,
		0.01307,
		0.011795,
		0.010639,
		0.009596,
		0.00865,
		0.007808,
		0.007042,
		0.006352,
		0.005730,
		0.005168,
		0.004662,
		0.004205,
		0.003793,
		0
};

AudioEffectFreeverb::AudioEffectFreeverb() : AudioStream(1, inputQueueArray)
{
	memset(comb1buf, 0, sizeof(comb1buf));
	memset(comb2buf, 0, sizeof(comb2buf));
	memset(comb3buf, 0, sizeof(comb3buf));
	memset(comb4buf, 0, sizeof(comb4buf));
	memset(comb5buf, 0, sizeof(comb5buf));
	memset(comb6buf, 0, sizeof(comb6buf));
	memset(comb7buf, 0, sizeof(comb7buf));
	memset(comb8buf, 0, sizeof(comb8buf));
	comb1index = 0;
	comb2index = 0;
	comb3index = 0;
	comb4index = 0;
	comb5index = 0;
	comb6index = 0;
	comb7index = 0;
	comb8index = 0;
	comb1filter = 0;
	comb2filter = 0;
	comb3filter = 0;
	comb4filter = 0;
	comb5filter = 0;
	comb6filter = 0;
	comb7filter = 0;
	comb8filter = 0;
	combdamp1 = 6553;
	combdamp2 = 26215;
	combfeeback = 27524;
	memset(allpass1buf, 0, sizeof(allpass1buf));
	memset(allpass2buf, 0, sizeof(allpass2buf));
	memset(allpass3buf, 0, sizeof(allpass3buf));
	memset(allpass4buf, 0, sizeof(allpass4buf));
	allpass1index = 0;
	allpass2index = 0;
	allpass3index = 0;
	allpass4index = 0;
}

#if 1
#define sat16(n, rshift) signed_saturate_rshift((n), 16, (rshift))
#else
static int16_t sat16(int32_t n, int rshift)
{
	n = n >> rshift;
	if (n > 32767) {
		return 32767;
	}
	if (n < -32768) {
		return -32768;
	}
	return n;
}
#endif

// TODO: move this to one of the data files, use in output_adat.cpp, output_tdm.cpp, etc
static const audio_block_t zeroblock = {
0, 0, 0, {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#if AUDIO_BLOCK_SAMPLES > 16
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#endif
#if AUDIO_BLOCK_SAMPLES > 32
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#endif
#if AUDIO_BLOCK_SAMPLES > 48
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#endif
#if AUDIO_BLOCK_SAMPLES > 64
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#endif
#if AUDIO_BLOCK_SAMPLES > 80
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#endif
#if AUDIO_BLOCK_SAMPLES > 96
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#endif
#if AUDIO_BLOCK_SAMPLES > 112
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#endif
} };


void AudioEffectFreeverb::update()
{
#if defined(KINETISK)
	const audio_block_t *block;
	audio_block_t *outblock;
	int i;
	int16_t input, bufout, output;
	int32_t sum;
	uint8_t valueSmall = 0;

	outblock = allocate();
	if (!outblock) {
		audio_block_t *tmp = receiveReadOnly(0);
		if (tmp) release(tmp);
		return;
	}
	block = receiveReadOnly(0);
	if (!block) block = &zeroblock;
	else
	{
		stopFlag = 0;
	}

	for (i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
		// TODO: scale numerical range depending on roomsize & damping
		input = sat16(block->data[i] * 8738, 17); // for numerical headroom
		sum = 0;

		bufout = comb1buf[comb1index];
		sum += bufout;
		comb1filter = sat16(bufout * combdamp2 + comb1filter * combdamp1, 15);
		comb1buf[comb1index] = sat16(input + sat16(comb1filter * combfeeback, 15), 0);
		if (++comb1index >= sizeof(comb1buf)/sizeof(int16_t)) comb1index = 0;

		bufout = comb2buf[comb2index];
		sum += bufout;
		comb2filter = sat16(bufout * combdamp2 + comb2filter * combdamp1, 15);
		comb2buf[comb2index] = sat16(input + sat16(comb2filter * combfeeback, 15), 0);
		if (++comb2index >= sizeof(comb2buf)/sizeof(int16_t)) comb2index = 0;

		bufout = comb3buf[comb3index];
		sum += bufout;
		comb3filter = sat16(bufout * combdamp2 + comb3filter * combdamp1, 15);
		comb3buf[comb3index] = sat16(input + sat16(comb3filter * combfeeback, 15), 0);
		if (++comb3index >= sizeof(comb3buf)/sizeof(int16_t)) comb3index = 0;

		bufout = comb4buf[comb4index];
		sum += bufout;
		comb4filter = sat16(bufout * combdamp2 + comb4filter * combdamp1, 15);
		comb4buf[comb4index] = sat16(input + sat16(comb4filter * combfeeback, 15), 0);
		if (++comb4index >= sizeof(comb4buf)/sizeof(int16_t)) comb4index = 0;

		bufout = comb5buf[comb5index];
		sum += bufout;
		comb5filter = sat16(bufout * combdamp2 + comb5filter * combdamp1, 15);
		comb5buf[comb5index] = sat16(input + sat16(comb5filter * combfeeback, 15), 0);
		if (++comb5index >= sizeof(comb5buf)/sizeof(int16_t)) comb5index = 0;

		bufout = comb6buf[comb6index];
		sum += bufout;
		comb6filter = sat16(bufout * combdamp2 + comb6filter * combdamp1, 15);
		comb6buf[comb6index] = sat16(input + sat16(comb6filter * combfeeback, 15), 0);
		if (++comb6index >= sizeof(comb6buf)/sizeof(int16_t)) comb6index = 0;

		bufout = comb7buf[comb7index];
		sum += bufout;
		comb7filter = sat16(bufout * combdamp2 + comb7filter * combdamp1, 15);
		comb7buf[comb7index] = sat16(input + sat16(comb7filter * combfeeback, 15), 0);
		if (++comb7index >= sizeof(comb7buf)/sizeof(int16_t)) comb7index = 0;

		bufout = comb8buf[comb8index];
		sum += bufout;
		comb8filter = sat16(bufout * combdamp2 + comb8filter * combdamp1, 15);
		comb8buf[comb8index] = sat16(input + sat16(comb8filter * combfeeback, 15), 0);
		if (++comb8index >= sizeof(comb8buf)/sizeof(int16_t)) comb8index = 0;
		output = sat16(sum * 31457, 17);

		bufout = allpass1buf[allpass1index];
		allpass1buf[allpass1index] = output + (bufout >> 1);
		output = sat16(bufout - output, 1);
		if (++allpass1index >= sizeof(allpass1buf)/sizeof(int16_t)) allpass1index = 0;

		bufout = allpass2buf[allpass2index];
		allpass2buf[allpass2index] = output + (bufout >> 1);
		output = sat16(bufout - output, 1);
		if (++allpass2index >= sizeof(allpass2buf)/sizeof(int16_t)) allpass2index = 0;

		bufout = allpass3buf[allpass3index];
		allpass3buf[allpass3index] = output + (bufout >> 1);
		output = sat16(bufout - output, 1);
		if (++allpass3index >= sizeof(allpass3buf)/sizeof(int16_t)) allpass3index = 0;

		bufout = allpass4buf[allpass4index];
		allpass4buf[allpass4index] = output + (bufout >> 1);
		output = sat16(bufout - output, 1);
		if (++allpass4index >= sizeof(allpass4buf)/sizeof(int16_t)) allpass4index = 0;

		outblock->data[i] = sat16(output * 30, 0);

		if(i>1) // bieda rozwiazanie - w debbugingu przy burczeniu wartosci sie powtarzaly - w buforze bylo kilkadziesiat powtorzen
		{
//			if(outblock->data[i] == outblock->data[i-1]) valueRepeat++;
			if( (outblock->data[i] < 300) && (outblock->data[i] > -300)) valueSmall++;
		}
	}
	if( (valueSmall > 100) && (stopFlag == 0))
	{
		stopFlag = 1;
		releaseTim = 0;
	}
	if(stopFlag == 1)
	{
		for(uint8_t i = 0; i< AUDIO_BLOCK_SAMPLES ; i++)
		{
			if((releaseTim < releaseTime))
			{
				uint16_t release_idx = (RELEASE_TAB_SIZE - 1) - (float)((releaseTime - releaseTim)/(float)releaseTime) * (RELEASE_TAB_SIZE - 1);

				outblock->data[i] *=  logReleaseTab[release_idx];
			}
			else
			{
				outblock->data[i] = 0;
				comb1buf[comb1index] = 0;
				comb2buf[comb2index] = 0;
				comb3buf[comb3index] = 0;
				comb4buf[comb4index] = 0;
				comb5buf[comb5index] = 0;
				comb6buf[comb6index] = 0;
				comb7buf[comb7index] = 0;
				comb8buf[comb8index] = 0;

				allpass1buf[allpass1index] = 0;
				allpass2buf[allpass2index] = 0;
				allpass3buf[allpass3index] = 0;
				allpass4buf[allpass4index] = 0;

				comb1filter = 0;
				comb2filter = 0;
				comb3filter = 0;
				comb4filter = 0;
				comb5filter = 0;
				comb6filter = 0;
				comb7filter = 0;
				comb8filter = 0;
			}

		}
//		memset(outblock->data,0,AUDIO_BLOCK_SAMPLES*2);
	}
	transmit(outblock);
	release(outblock);
	if (block != &zeroblock) release((audio_block_t *)block);
#elif defined(KINETISL)
	audio_block_t *block;
	block = receiveReadOnly(0);
	if (block) release(block);
#endif
}

void AudioEffectFreeverb::clearFilters()
{
	memset(comb1buf, 0, sizeof(comb1buf));
	memset(comb2buf, 0, sizeof(comb2buf));
	memset(comb3buf, 0, sizeof(comb3buf));
	memset(comb4buf, 0, sizeof(comb4buf));
	memset(comb5buf, 0, sizeof(comb5buf));
	memset(comb6buf, 0, sizeof(comb6buf));
	memset(comb7buf, 0, sizeof(comb7buf));
	memset(comb8buf, 0, sizeof(comb8buf));
	comb1index = 0;
	comb2index = 0;
	comb3index = 0;
	comb4index = 0;
	comb5index = 0;
	comb6index = 0;
	comb7index = 0;
	comb8index = 0;
	comb1filter = 0;
	comb2filter = 0;
	comb3filter = 0;
	comb4filter = 0;
	comb5filter = 0;
	comb6filter = 0;
	comb7filter = 0;
	comb8filter = 0;
	memset(allpass1buf, 0, sizeof(allpass1buf));
	memset(allpass2buf, 0, sizeof(allpass2buf));
	memset(allpass3buf, 0, sizeof(allpass3buf));
	memset(allpass4buf, 0, sizeof(allpass4buf));
	allpass1index = 0;
	allpass2index = 0;
	allpass3index = 0;
	allpass4index = 0;
}


AudioEffectFreeverbStereo::AudioEffectFreeverbStereo() : AudioStream(1, inputQueueArray)
{
	memset(comb1bufL, 0, sizeof(comb1bufL));
	memset(comb2bufL, 0, sizeof(comb2bufL));
	memset(comb3bufL, 0, sizeof(comb3bufL));
	memset(comb4bufL, 0, sizeof(comb4bufL));
	memset(comb5bufL, 0, sizeof(comb5bufL));
	memset(comb6bufL, 0, sizeof(comb6bufL));
	memset(comb7bufL, 0, sizeof(comb7bufL));
	memset(comb8bufL, 0, sizeof(comb8bufL));
	comb1indexL = 0;
	comb2indexL = 0;
	comb3indexL = 0;
	comb4indexL = 0;
	comb5indexL = 0;
	comb6indexL = 0;
	comb7indexL = 0;
	comb8indexL = 0;
	comb1filterL = 0;
	comb2filterL = 0;
	comb3filterL = 0;
	comb4filterL = 0;
	comb5filterL = 0;
	comb6filterL = 0;
	comb7filterL = 0;
	comb8filterL = 0;
	memset(comb1bufR, 0, sizeof(comb1bufR));
	memset(comb2bufR, 0, sizeof(comb2bufR));
	memset(comb3bufR, 0, sizeof(comb3bufR));
	memset(comb4bufR, 0, sizeof(comb4bufR));
	memset(comb5bufR, 0, sizeof(comb5bufR));
	memset(comb6bufR, 0, sizeof(comb6bufR));
	memset(comb7bufR, 0, sizeof(comb7bufR));
	memset(comb8bufR, 0, sizeof(comb8bufR));
	comb1indexR = 0;
	comb2indexR = 0;
	comb3indexR = 0;
	comb4indexR = 0;
	comb5indexR = 0;
	comb6indexR = 0;
	comb7indexR = 0;
	comb8indexR = 0;
	comb1filterR = 0;
	comb2filterR = 0;
	comb3filterR = 0;
	comb4filterR = 0;
	comb5filterR = 0;
	comb6filterR = 0;
	comb7filterR = 0;
	comb8filterR = 0;
	combdamp1 = 6553;
	combdamp2 = 26215;
	combfeeback = 27524;
	memset(allpass1bufL, 0, sizeof(allpass1bufL));
	memset(allpass2bufL, 0, sizeof(allpass2bufL));
	memset(allpass3bufL, 0, sizeof(allpass3bufL));
	memset(allpass4bufL, 0, sizeof(allpass4bufL));
	allpass1indexL = 0;
	allpass2indexL = 0;
	allpass3indexL = 0;
	allpass4indexL = 0;
	memset(allpass1bufR, 0, sizeof(allpass1bufR));
	memset(allpass2bufR, 0, sizeof(allpass2bufR));
	memset(allpass3bufR, 0, sizeof(allpass3bufR));
	memset(allpass4bufR, 0, sizeof(allpass4bufR));
	allpass1indexR = 0;
	allpass2indexR = 0;
	allpass3indexR = 0;
	allpass4indexR = 0;
}

void AudioEffectFreeverbStereo::update()
{
#if defined(KINETISK)
	const audio_block_t *block;
	audio_block_t *outblockL;
	audio_block_t *outblockR;
	int i;
	int16_t input, bufout, outputL, outputR;
	int32_t sum;

	block = receiveReadOnly(0);
	outblockL = allocate();
	outblockR = allocate();
	if (!outblockL || !outblockR) {
		if (outblockL) release(outblockL);
		if (outblockR) release(outblockR);
		if (block) release((audio_block_t *)block);
		return;
	}
	if (!block) block = &zeroblock;

	for (i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
		// TODO: scale numerical range depending on roomsize & damping
		input = sat16(block->data[i] * 8738, 17); // for numerical headroom
		sum = 0;

		bufout = comb1bufL[comb1indexL];
		sum += bufout;
		comb1filterL = sat16(bufout * combdamp2 + comb1filterL * combdamp1, 15);
		comb1bufL[comb1indexL] = sat16(input + sat16(comb1filterL * combfeeback, 15), 0);
		if (++comb1indexL >= sizeof(comb1bufL)/sizeof(int16_t)) comb1indexL = 0;

		bufout = comb2bufL[comb2indexL];
		sum += bufout;
		comb2filterL = sat16(bufout * combdamp2 + comb2filterL * combdamp1, 15);
		comb2bufL[comb2indexL] = sat16(input + sat16(comb2filterL * combfeeback, 15), 0);
		if (++comb2indexL >= sizeof(comb2bufL)/sizeof(int16_t)) comb2indexL = 0;

		bufout = comb3bufL[comb3indexL];
		sum += bufout;
		comb3filterL = sat16(bufout * combdamp2 + comb3filterL * combdamp1, 15);
		comb3bufL[comb3indexL] = sat16(input + sat16(comb3filterL * combfeeback, 15), 0);
		if (++comb3indexL >= sizeof(comb3bufL)/sizeof(int16_t)) comb3indexL = 0;

		bufout = comb4bufL[comb4indexL];
		sum += bufout;
		comb4filterL = sat16(bufout * combdamp2 + comb4filterL * combdamp1, 15);
		comb4bufL[comb4indexL] = sat16(input + sat16(comb4filterL * combfeeback, 15), 0);
		if (++comb4indexL >= sizeof(comb4bufL)/sizeof(int16_t)) comb4indexL = 0;

		bufout = comb5bufL[comb5indexL];
		sum += bufout;
		comb5filterL = sat16(bufout * combdamp2 + comb5filterL * combdamp1, 15);
		comb5bufL[comb5indexL] = sat16(input + sat16(comb5filterL * combfeeback, 15), 0);
		if (++comb5indexL >= sizeof(comb5bufL)/sizeof(int16_t)) comb5indexL = 0;

		bufout = comb6bufL[comb6indexL];
		sum += bufout;
		comb6filterL = sat16(bufout * combdamp2 + comb6filterL * combdamp1, 15);
		comb6bufL[comb6indexL] = sat16(input + sat16(comb6filterL * combfeeback, 15), 0);
		if (++comb6indexL >= sizeof(comb6bufL)/sizeof(int16_t)) comb6indexL = 0;

		bufout = comb7bufL[comb7indexL];
		sum += bufout;
		comb7filterL = sat16(bufout * combdamp2 + comb7filterL * combdamp1, 15);
		comb7bufL[comb7indexL] = sat16(input + sat16(comb7filterL * combfeeback, 15), 0);
		if (++comb7indexL >= sizeof(comb7bufL)/sizeof(int16_t)) comb7indexL = 0;

		bufout = comb8bufL[comb8indexL];
		sum += bufout;
		comb8filterL = sat16(bufout * combdamp2 + comb8filterL * combdamp1, 15);
		comb8bufL[comb8indexL] = sat16(input + sat16(comb8filterL * combfeeback, 15), 0);
		if (++comb8indexL >= sizeof(comb8bufL)/sizeof(int16_t)) comb8indexL = 0;

		outputL = sat16(sum * 31457, 17);
		sum = 0;

		bufout = comb1bufR[comb1indexR];
		sum += bufout;
		comb1filterR = sat16(bufout * combdamp2 + comb1filterR * combdamp1, 15);
		comb1bufR[comb1indexR] = sat16(input + sat16(comb1filterR * combfeeback, 15), 0);
		if (++comb1indexR >= sizeof(comb1bufR)/sizeof(int16_t)) comb1indexR = 0;

		bufout = comb2bufR[comb2indexR];
		sum += bufout;
		comb2filterR = sat16(bufout * combdamp2 + comb2filterR * combdamp1, 15);
		comb2bufR[comb2indexR] = sat16(input + sat16(comb2filterR * combfeeback, 15), 0);
		if (++comb2indexR >= sizeof(comb2bufR)/sizeof(int16_t)) comb2indexR = 0;

		bufout = comb3bufR[comb3indexR];
		sum += bufout;
		comb3filterR = sat16(bufout * combdamp2 + comb3filterR * combdamp1, 15);
		comb3bufR[comb3indexR] = sat16(input + sat16(comb3filterR * combfeeback, 15), 0);
		if (++comb3indexR >= sizeof(comb3bufR)/sizeof(int16_t)) comb3indexR = 0;

		bufout = comb4bufR[comb4indexR];
		sum += bufout;
		comb4filterR = sat16(bufout * combdamp2 + comb4filterR * combdamp1, 15);
		comb4bufR[comb4indexR] = sat16(input + sat16(comb4filterR * combfeeback, 15), 0);
		if (++comb4indexR >= sizeof(comb4bufR)/sizeof(int16_t)) comb4indexR = 0;

		bufout = comb5bufR[comb5indexR];
		sum += bufout;
		comb5filterR = sat16(bufout * combdamp2 + comb5filterR * combdamp1, 15);
		comb5bufR[comb5indexR] = sat16(input + sat16(comb5filterR * combfeeback, 15), 0);
		if (++comb5indexR >= sizeof(comb5bufR)/sizeof(int16_t)) comb5indexR = 0;

		bufout = comb6bufR[comb6indexR];
		sum += bufout;
		comb6filterR = sat16(bufout * combdamp2 + comb6filterR * combdamp1, 15);
		comb6bufR[comb6indexR] = sat16(input + sat16(comb6filterR * combfeeback, 15), 0);
		if (++comb6indexR >= sizeof(comb6bufR)/sizeof(int16_t)) comb6indexR = 0;

		bufout = comb7bufR[comb7indexR];
		sum += bufout;
		comb7filterR = sat16(bufout * combdamp2 + comb7filterR * combdamp1, 15);
		comb7bufR[comb7indexR] = sat16(input + sat16(comb7filterR * combfeeback, 15), 0);
		if (++comb7indexR >= sizeof(comb7bufR)/sizeof(int16_t)) comb7indexR = 0;

		bufout = comb8bufR[comb8indexR];
		sum += bufout;
		comb8filterR = sat16(bufout * combdamp2 + comb8filterR * combdamp1, 15);
		comb8bufR[comb8indexR] = sat16(input + sat16(comb8filterR * combfeeback, 15), 0);
		if (++comb8indexR >= sizeof(comb8bufR)/sizeof(int16_t)) comb8indexR = 0;

		outputR = sat16(sum * 31457, 17);

		bufout = allpass1bufL[allpass1indexL];
		allpass1bufL[allpass1indexL] = outputL + (bufout >> 1);
		outputL = sat16(bufout - outputL, 1);
		if (++allpass1indexL >= sizeof(allpass1bufL)/sizeof(int16_t)) allpass1indexL = 0;

		bufout = allpass2bufL[allpass2indexL];
		allpass2bufL[allpass2indexL] = outputL + (bufout >> 1);
		outputL = sat16(bufout - outputL, 1);
		if (++allpass2indexL >= sizeof(allpass2bufL)/sizeof(int16_t)) allpass2indexL = 0;

		bufout = allpass3bufL[allpass3indexL];
		allpass3bufL[allpass3indexL] = outputL + (bufout >> 1);
		outputL = sat16(bufout - outputL, 1);
		if (++allpass3indexL >= sizeof(allpass3bufL)/sizeof(int16_t)) allpass3indexL = 0;

		bufout = allpass4bufL[allpass4indexL];
		allpass4bufL[allpass4indexL] = outputL + (bufout >> 1);
		outputL = sat16(bufout - outputL, 1);
		if (++allpass4indexL >= sizeof(allpass4bufL)/sizeof(int16_t)) allpass4indexL = 0;

		outblockL->data[i] = sat16(outputL * 30, 0);

		bufout = allpass1bufR[allpass1indexR];
		allpass1bufR[allpass1indexR] = outputR + (bufout >> 1);
		outputR = sat16(bufout - outputR, 1);
		if (++allpass1indexR >= sizeof(allpass1bufR)/sizeof(int16_t)) allpass1indexR = 0;

		bufout = allpass2bufR[allpass2indexR];
		allpass2bufR[allpass2indexR] = outputR + (bufout >> 1);
		outputR = sat16(bufout - outputR, 1);
		if (++allpass2indexR >= sizeof(allpass2bufR)/sizeof(int16_t)) allpass2indexR = 0;

		bufout = allpass3bufR[allpass3indexR];
		allpass3bufR[allpass3indexR] = outputR + (bufout >> 1);
		outputR = sat16(bufout - outputR, 1);
		if (++allpass3indexR >= sizeof(allpass3bufR)/sizeof(int16_t)) allpass3indexR = 0;

		bufout = allpass4bufR[allpass4indexR];
		allpass4bufR[allpass4indexR] = outputR + (bufout >> 1);
		outputR = sat16(bufout - outputR, 1);
		if (++allpass4indexR >= sizeof(allpass4bufR)/sizeof(int16_t)) allpass4indexR = 0;

		outblockR->data[i] = sat16(outputL * 30, 0);
	}
	transmit(outblockL, 0);
	transmit(outblockR, 1);
	release(outblockL);
	release(outblockR);
	if (block != &zeroblock) release((audio_block_t *)block);

#elif defined(KINETISL)
	audio_block_t *block;
	block = receiveReadOnly(0);
	if (block) release(block);
#endif
}



