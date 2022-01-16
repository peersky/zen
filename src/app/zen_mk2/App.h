/*
 MIT License

 Copyright (c) 2021 Tim Pechersky

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#pragma once

#define ZEN_USE_DYNAMIC_ALLOCATION 0

#include "../../modules/dsp/Interpolator.h"
#include "../../modules/utils/tables.h"
#include "../../modules/zen.h"
#include "AppWrapper.h"

//ZEN API
void setDebugStuff(float *ch1, float *ch2, size_t size);
float ZENTest_tick(float input);
void ZENTest_processBlock(const float **in, float **out, int chan_num, size_t size);
void ZENTest_init(float sampleRate, int blocksize);
void ZENTest_block(void);
void ZENTest_end(void);
void ZENTest_noteOn(int midiNoteNumber, float velocity);
void ZENTest_noteOff(int midiNoteNumber);