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

#include "filter_variable.h"
#include "utility/dspinst.h"

// State Variable Filter (Chamberlin) with 2X oversampling
// http://www.musicdsp.org/showArchiveComment.php?ArchiveID=92

// The fast 32x32 with rshift32 discards 2 bits, which probably
// never matter.
//#define MULT(a, b) (int32_t)(((int64_t)(a) * (b)) >> 30)
#define MULT(a, b) (multiply_32x32_rshift32_rounded(a, b) << 2)

// It's very unlikely anyone could hear any difference, but if you
// care deeply about numerical precision in seldom-used cases,
// uncomment this to improve the control signal accuracy
//#define IMPROVE_HIGH_FREQUENCY_ACCURACY

// This increases the exponential approximation accuracy from
// about 0.341% error to only 0.012% error, which probably makes
// no audible difference.
//#define IMPROVE_EXPONENTIAL_ACCURACY

#if defined(KINETISK)

const float filterFreq[478] =
{
20.000000 , 20.279190 , 20.562277 , 20.849315 , 21.140361 , 21.435469 , 21.734697 , 22.038102 , 22.345743 , 22.657678 , 22.973967 , 23.294672 ,
23.619853 , 23.949574 , 24.283898 , 24.622888 , 24.966611 , 25.315132 , 25.668518 , 26.026837 , 26.390158 , 26.758551 , 27.132087 , 27.510836 ,
27.894873 , 28.284271 , 28.679105 , 29.079450 , 29.485384 , 29.896985 , 30.314331 , 30.737504 , 31.166583 , 31.601652 , 32.042795 , 32.490096 ,
32.943641 , 33.403517 , 33.869812 , 34.342617 , 34.822023 , 35.308120 , 35.801003 , 36.300766 , 36.807506 , 37.321320 , 37.842306 , 38.370565 ,
38.906198 , 39.449308 , 40.000000 , 40.558379 , 41.124553 , 41.698630 , 42.280722 , 42.870939 , 43.469395 , 44.076205 , 44.691486 , 45.315355 ,
45.947934 , 46.589343 , 47.239706 , 47.899148 , 48.567795 , 49.245777 , 49.933222 , 50.630264 , 51.337036 , 52.053674 , 52.780316 , 53.517102 ,
54.264173 , 55.021673 , 55.789747 , 56.568542 , 57.358210 , 58.158901 , 58.970769 , 59.793970 , 60.628663 , 61.475007 , 62.333166 , 63.203305 ,
64.085590 , 64.980192 , 65.887281 , 66.807034 , 67.739625 , 68.685235 , 69.644045 , 70.616240 , 71.602006 , 72.601532 , 73.615012 , 74.642639 ,
75.684612 , 76.741130 , 77.812396 , 78.898616 , 80.000000 , 81.116758 , 82.249106 , 83.397261 , 84.561443 , 85.741877 , 86.938789 , 88.152409 ,
89.382971 , 90.630711 , 91.895868 , 93.178687 , 94.479413 , 95.798296 , 97.135591 , 98.491553 , 99.866444 , 101.260528 , 102.674072 , 104.107348 ,
105.560633 , 107.034204 , 108.528346 , 110.043345 , 111.579493 , 113.137085 , 114.716420 , 116.317801 , 117.941537 , 119.587940 , 121.257325 ,
122.950015 , 124.666333 , 126.406610 , 128.171180 , 129.960383 , 131.774563 , 133.614067 , 135.479250 , 137.370470 , 139.288090 , 141.232479 ,
143.204011 , 145.203065 , 147.230024 , 149.285279 , 151.369223 , 153.482259 , 155.624792 , 157.797233 , 160.000000 , 162.233517 , 164.498212 ,
166.794522 , 169.122886 , 171.483754 , 173.877578 , 176.304819 , 178.765942 , 181.261422 , 183.791737 , 186.357374 , 188.958826 , 191.596593 ,
194.271182 , 196.983106 , 199.732888 , 202.521055 , 205.348144 , 208.214697 , 211.121266 , 214.068409 , 217.056692 , 220.086691 , 223.158987 ,
226.274170 , 229.432840 , 232.635603 , 235.883075 , 239.175880 , 242.514651 , 245.900029 , 249.332665 , 252.813220 , 256.342361 , 259.920767 ,
263.549126 , 267.228134 , 270.958500 , 274.740940 , 278.576180 , 282.464959 , 286.408023 , 290.406130 , 294.460048 , 298.570557 , 302.738447 ,
306.964518 , 311.249583 , 315.594465 , 320.000000 , 324.467034 , 328.996425 , 333.589043 , 338.245773 , 342.967508 , 347.755156 , 352.609637 ,
357.531884 , 362.522843 , 367.583474 , 372.714748 , 377.917652 , 383.193185 , 388.542363 , 393.966212 , 399.465776 , 405.042110 , 410.696287 ,
416.429394 , 422.242531 , 428.136818 , 434.113385 , 440.173382 , 446.317973 , 452.548340 , 458.865679 , 465.271206 , 471.766150 , 478.351760 ,
485.029301 , 491.800058 , 498.665331 , 505.626440 , 512.684722 , 519.841534 , 527.098251 , 534.456268 , 541.917000 , 549.481879 , 557.152361 ,
564.929918 , 572.816045 , 580.812259 , 588.920096 , 597.141115 , 605.476894 , 613.929036 , 622.499166 , 631.188931 , 640.000000 , 648.934067 ,
657.992849 , 667.178087 , 676.491546 , 685.935016 , 695.510312 , 705.219274 , 715.063768 , 725.045687 , 735.166947 , 745.429495 , 755.835303 ,
766.386371 , 777.084726 , 787.932425 , 798.931551 , 810.084220 , 821.392574 , 832.858787 , 844.485063 , 856.273635 , 868.226770 , 880.346764 ,
892.635946 , 905.096680 , 917.731359 , 930.542411 , 943.532299 , 956.703519 , 970.058603 , 983.600116 , 997.330662 , 1011.252879 , 1025.369443 ,
1039.683067 , 1054.196502 , 1068.912537 , 1083.834000 , 1098.963759 , 1114.304721 , 1129.859835 , 1145.632091 , 1161.624519 , 1177.840193 ,
1194.282229 , 1210.953788 , 1227.858073 , 1244.998333 , 1262.377862 , 1280.000000 , 1297.868134 , 1315.985698 , 1334.356174 , 1352.983092 ,
1371.870032 , 1391.020624 , 1410.438548 , 1430.127537 , 1450.091373 , 1470.333894 , 1490.858991 , 1511.670607 , 1532.772742 , 1554.169452 ,
1575.864849 , 1597.863103 , 1620.168440 , 1642.785149 , 1665.717575 , 1688.970126 , 1712.547270 , 1736.453539 , 1760.693527 , 1785.271893 ,
1810.193360 , 1835.462717 , 1861.084822 , 1887.064598 , 1913.407038 , 1940.117205 , 1967.200232 , 1994.661324 , 2022.505758 , 2050.738887 ,
2079.366135 , 2108.393004 , 2137.825074 , 2167.668000 , 2197.927517 , 2228.609442 , 2259.719671 , 2291.264182 , 2323.249038 , 2355.680386 ,
2388.564458 , 2421.907576 , 2455.716145 , 2489.996665 , 2524.755724 , 2560.000000 , 2595.736268 , 2631.971396 , 2668.712348 , 2705.966184 ,
2743.740064 , 2782.041248 , 2820.877097 , 2860.255073 , 2900.182746 , 2940.667789 , 2981.717981 , 3023.341213 , 3065.545484 , 3108.338904 ,
3151.729698 , 3195.726205 , 3240.336881 , 3285.570298 , 3331.435150 , 3377.940252 , 3425.094540 , 3472.907078 , 3521.387054 , 3570.543786 ,
3620.386720 , 3670.925435 , 3722.169644 , 3774.129196 , 3826.814077 , 3880.234410 , 3934.400464 , 3989.322648 , 4045.011517 , 4101.477773 ,
4158.732269 , 4216.786008 , 4275.650147 , 4335.335999 , 4395.855035 , 4457.218884 , 4519.439341 , 4582.528363 , 4646.498075 , 4711.360771 ,
4777.128917 , 4843.815151 , 4911.432291 , 4979.993331 , 5049.511447 , 5120.000000 , 5191.472537 , 5263.942792 , 5337.424696 , 5411.932368 ,
5487.480128 , 5564.082496 , 5641.754193 , 5720.510147 , 5800.365493 , 5881.335578 , 5963.435963 , 6046.682427 , 6131.090968 , 6216.677808 ,
6303.459396 , 6391.452410 , 6480.673761 , 6571.140596 , 6662.870300 , 6755.880503 , 6850.189081 , 6945.814157 , 7042.774109 , 7141.087572 ,
7240.773439 , 7341.850870 , 7444.339289 , 7548.258393 , 7653.628153 , 7760.468821 , 7868.800928 , 7978.645296 , 8090.023033 , 8202.955547 ,
8317.464539 , 8433.572017 , 8551.300295 , 8670.671999 , 8791.710069 , 8914.437768 , 9038.878682 , 9165.056726 , 9292.996150 , 9422.721542 ,
9554.257833 , 9687.630302 , 9822.864582 , 9959.986662 , 10099.022894 , 10240.000000 , 10382.945073 , 10527.885585 , 10674.849391 , 10823.864735 ,
10974.960256 , 11128.164992 , 11283.508387 , 11441.020294 , 11600.730985 , 11762.671155 , 11926.871925 , 12093.364853 , 12262.181935 , 12433.355616 ,
12606.918793 , 12782.904821 , 12961.347522 , 13142.281191 , 13325.740600 , 13511.761006 , 13700.378161 , 13891.628313 , 14085.548218 , 14282.175143 ,
14481.546879 , 14683.701740 , 14888.678577

};


void AudioFilterStateVariable::update_fixed(const int16_t *in,
	int16_t *lp, int16_t *bp, int16_t *hp)
{
	const int16_t *end = in + AUDIO_BLOCK_SAMPLES;
	int32_t input, inputprev;
	int32_t lowpass, bandpass, highpass;
	int32_t lowpasstmp, bandpasstmp, highpasstmp;
	int32_t fmult, damp;

	fmult = setting_fmult;
	damp = setting_damp;
	inputprev = state_inputprev;
	lowpass = state_lowpass;
	bandpass = state_bandpass;
	do {
		input = (*in++) << 12;
		lowpass = lowpass + MULT(fmult, bandpass);
		highpass = ((input + inputprev)>>1) - lowpass - MULT(damp, bandpass);
		inputprev = input;
		bandpass = bandpass + MULT(fmult, highpass);
		lowpasstmp = lowpass;
		bandpasstmp = bandpass;
		highpasstmp = highpass;
		lowpass = lowpass + MULT(fmult, bandpass);
		highpass = input - lowpass - MULT(damp, bandpass);
		bandpass = bandpass + MULT(fmult, highpass);
		lowpasstmp = signed_saturate_rshift(lowpass+lowpasstmp, 16, 13);
		bandpasstmp = signed_saturate_rshift(bandpass+bandpasstmp, 16, 13);
		highpasstmp = signed_saturate_rshift(highpass+highpasstmp, 16, 13);
		*lp++ = lowpasstmp;
		*bp++ = bandpasstmp;
		*hp++ = highpasstmp;
	} while (in < end);
	state_inputprev = inputprev;
	state_lowpass = lowpass;
	state_bandpass = bandpass;
}


void AudioFilterStateVariable::update_variable(const int16_t *in,
	 int16_t *ctl, int16_t *lp, int16_t *bp, int16_t *hp)
{
	const int16_t *end = in + AUDIO_BLOCK_SAMPLES;
	int32_t input, inputprev, control;
	int32_t lowpass, bandpass, highpass;
	int32_t lowpasstmp, bandpasstmp, highpasstmp;
	int32_t fcenter, fmult, damp, octavemult;
	int32_t n;
	fcenter = setting_fcenter;
	octavemult = setting_octavemult;
	damp = setting_damp;
	inputprev = state_inputprev;
	lowpass = state_lowpass;
	bandpass = state_bandpass;
	do {
		// compute fmult using control input, fcenter and octavemult
		control = *ctl++;          // signal is always 15 fractional bits
		control *= octavemult;     // octavemult range: 0 to 28671 (12 frac bits)
		n = control & 0x7FFFFFF;   // 27 fractional control bits
		#ifdef IMPROVE_EXPONENTIAL_ACCURACY
		// exp2 polynomial suggested by Stefan Stenzel on "music-dsp"
		// mail list, Wed, 3 Sep 2014 10:08:55 +0200
		int32_t x = n << 3;
		n = multiply_accumulate_32x32_rshift32_rounded(536870912, x, 1494202713);
		int32_t sq = multiply_32x32_rshift32_rounded(x, x);
		n = multiply_accumulate_32x32_rshift32_rounded(n, sq, 1934101615);
		n = n + (multiply_32x32_rshift32_rounded(sq,
			multiply_32x32_rshift32_rounded(x, 1358044250)) << 1);
		n = n << 1;
		#else
		// exp2 algorithm by Laurent de Soras
		// http://www.musicdsp.org/showone.php?id=106
		n = (n + 134217728) << 3;
		n = multiply_32x32_rshift32_rounded(n, n);
		n = multiply_32x32_rshift32_rounded(n, 715827883) << 3;
		n = n + 715827882;
		#endif
		n = n >> (6 - (control >> 27)); // 4 integer control bits
		fmult = multiply_32x32_rshift32_rounded(fcenter, n);
		if (fmult > 5378279) fmult = 5378279;
		fmult = fmult << 8;
		// fmult is within 0.4% accuracy for all but the top 2 octaves
		// of the audio band.  This math improves accuracy above 5 kHz.
		// Without this, the filter still works fine for processing
		// high frequencies, but the filter's corner frequency response
		// can end up about 6% higher than requested.
		#ifdef IMPROVE_HIGH_FREQUENCY_ACCURACY
		// From "Fast Polynomial Approximations to Sine and Cosine"
		// Charles K Garrett, http://krisgarrett.net/
		fmult = (multiply_32x32_rshift32_rounded(fmult, 2145892402) +
			multiply_32x32_rshift32_rounded(
			multiply_32x32_rshift32_rounded(fmult, fmult),
			multiply_32x32_rshift32_rounded(fmult, -1383276101))) << 1;
		#endif
		// now do the state variable filter as normal, using fmult
		input = (*in++) << 12;
		lowpass = lowpass + MULT(fmult, bandpass);
		highpass = ((input + inputprev)>>1) - lowpass - MULT(damp, bandpass);
		inputprev = input;
		bandpass = bandpass + MULT(fmult, highpass);
		lowpasstmp = lowpass;
		bandpasstmp = bandpass;
		highpasstmp = highpass;
		lowpass = lowpass + MULT(fmult, bandpass);
		highpass = input - lowpass - MULT(damp, bandpass);
		bandpass = bandpass + MULT(fmult, highpass);
		lowpasstmp = signed_saturate_rshift(lowpass+lowpasstmp, 16, 13);
		bandpasstmp = signed_saturate_rshift(bandpass+bandpasstmp, 16, 13);
		highpasstmp = signed_saturate_rshift(highpass+highpasstmp, 16, 13);
		*lp++ = lowpasstmp;
		*bp++ = bandpasstmp;
		*hp++ = highpasstmp;
	} while (in < end);
	state_inputprev = inputprev;
	state_lowpass = lowpass;
	state_bandpass = bandpass;
}


void AudioFilterStateVariable::update(void)
{
	audio_block_t *input_block=NULL, *control_block=NULL;
	audio_block_t *lowpass_block=NULL, *bandpass_block=NULL, *highpass_block=NULL;

	input_block = receiveReadOnly(0);
	control_block = receiveReadOnly(1);
	if (!input_block) {
		if (control_block) release(control_block);
		return;
	}
	lowpass_block = allocate();
	if (!lowpass_block) {
		release(input_block);
		if (control_block) release(control_block);
		return;
	}
	bandpass_block = allocate();
	if (!bandpass_block) {
		release(input_block);
		release(lowpass_block);
		if (control_block) release(control_block);
		return;
	}
	highpass_block = allocate();
	if (!highpass_block) {
		release(input_block);
		release(lowpass_block);
		release(bandpass_block);
		if (control_block) release(control_block);
		return;
	}

	if (control_block) {
		update_variable(input_block->data,
			 control_block->data,
			 lowpass_block->data,
			 bandpass_block->data,
			 highpass_block->data);
		release(control_block);
	} else {
		update_fixed(input_block->data,
			 lowpass_block->data,
			 bandpass_block->data,
			 highpass_block->data);
	}
	release(input_block);
	transmit(lowpass_block, 0);
	release(lowpass_block);
	transmit(bandpass_block, 1);
	release(bandpass_block);
	transmit(highpass_block, 2);
	release(highpass_block);
	return;
}

#elif defined(KINETISL)

void AudioFilterStateVariable::update(void)
{
	audio_block_t *block;

	block = receiveReadOnly(0);
	if (block) release(block);
	block = receiveReadOnly(1);
	if (block) release(block);
}

#endif

