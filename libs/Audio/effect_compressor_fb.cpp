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

 
//******************************************************************/
//                A u d i o E f f e c t C o m p r e s s o r
// Written by Jim (hyperdyne) August 2016

#include "effect_compressor_fb.h"
#include "utility/dspinst.h"
#include "arm_math.h"

void AudioEffectCompressor::update(void)
{
	audio_block_t *block;
	audio_block_t *b_new;
	
	block = receiveReadOnly();
	if (!block) return;
	
	//passthru if not enabled
	if (!enabled)
	{
		transmit(block);
		release(block);
		return;
	}
	
	b_new = allocate();
	if(b_new)
	{
		if(upd_sem)
		{
	        if(cn >= c0)
			{		
    		    gain = pow(cn/c0, cratio);
			}
			else
			{
				gain = 1.0;
			}
			upd_sem = 0;
		}
		
		for(int i=0; i<AUDIO_BLOCK_SAMPLES; i++)
		{		
			b_new->data[i] = (q15_t)(block->data[i] * gain);
		}
		
		transmit(b_new);
		release(b_new);
	}
	
	release(block);
}


