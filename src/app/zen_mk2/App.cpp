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

#include "App.h"
#include <math.h>

zen::AudioInstance ZenInstance;

float gain;
float dtime;
bool buttonState;
int ratio = 2;
float x = 0.0f;
float y = 0.0f;
float a, b, c, d;

float *bufIn;
float *bufOut;

#define MSIZE 2500000
float memory[MSIZE];

//zen::Wave *wave3;
zen::Wave wave(ZenInstance);
zen::Wave lfo(ZenInstance);
zen::Interpolator sliderInterpolator[SLIDER_NUM_ENUM];
//zenInterpolate_t phaseInterpolator, fInterpolator, filterInterpolator;
zen::Svf filterTest;

#define NUM_DELAY_CHANNELS 2
#define NUM_REVERB_CHANNELS 8
zen::Delay<float, 48000, 2> stereoDelay(ZenInstance);
zen::Reverb<float, 144000, 2, 4> stereoReverb(ZenInstance);


float delta;

#define ZEN_BLOCK_SIZE 64

float slider2_block[ZEN_BLOCK_SIZE];
float slider3_block[ZEN_BLOCK_SIZE];

float sliders_blocks[SLIDER_NUM_ENUM][2048];
float delay_offsets_[NUM_DELAY_CHANNELS][2048] = {0};
float **delay_offsets = new float *[NUM_DELAY_CHANNELS];
float wave_out[2048];

// crazy idea with feedback chains
zen::SimpleCompressor<float> feedbackCompressor;
typedef float (zen::SimpleCompressor<float>::*CompressorTick)(float newValue);
CompressorTick fbCompressorTickerPtr = &zen::SimpleCompressor<float>::tick;

zen::Delay<float, 48000, 2>::delayProps delayProperties;

float feedbackChain0(float input) // Use a typedef for pointer-to-member types
{

	float ans = ZEN_CALL_MEMBER_FN(feedbackCompressor, fbCompressorTickerPtr)(input);
	return ans;
}

float feedbackChain1(float input) // Use a typedef for pointer-to-member types
{

	float ans = ZEN_CALL_MEMBER_FN(feedbackCompressor, fbCompressorTickerPtr)(input);
	return ans;
}

typedef float (*feedbackChainType)(float input);

feedbackChainType feedbackChains[NUM_DELAY_CHANNELS] = {
	feedbackChain0,
	feedbackChain1};
// End of crazy idea with feedback chains

void ZENTest_init(float sampleRate, int blockSize)
{

	for (int i = 0; i < NUM_DELAY_CHANNELS; i++)
	{
		delay_offsets[i] = delay_offsets_[i];
	}

	slidersConstruct();
	ZenInstance.prepareToPlay(sampleRate, blockSize, &getRandomFloat);
	wave.prepareToPlay(memory, 256, zen::WAVETABLE_SINE);
	lfo.prepareToPlay(memory, 256, zen::WAVETABLE_SINE);

	wave.setFreq(440);
	lfo.setFreq(1);

	filterTest.Init();
	stereoDelay.prepareToPlay();
	stereoReverb.prepareToPlay();
	feedbackCompressor.prepareToPlay(2.5f, 0.5f);
	for (int ch = 0; ch < NUM_DELAY_CHANNELS; ch++)
	{

		stereoDelay.setChannelDelay(500, ch);
		stereoDelay.setChainProcessor(feedbackChains[ch], ch);
	}

	stereoDelay.setDelayType(zen::delayTypes::DIGITAL);

	feedbackCompressor.prepareToPlay(2.5f, 0.5f);

	stereoReverb.setChannelDelay(1, 0);
	stereoReverb.setChannelDelay(3, 1);

	stereoReverb.setDelayType(zen::reverbTypes::one);

	sliderInterpolator[SLIDER_DELAY].prepareToPlay(100, 1);
	sliderInterpolator[SLIDER_SPREAD].prepareToPlay(0, 1);
	sliderInterpolator[SLIDER_FEEDBACK].prepareToPlay(0, 1);
	sliderInterpolator[SLIDER_LFO_FREQ].prepareToPlay(10, 1);
	sliderInterpolator[SLIDER_MODULATION_DEPTH].prepareToPlay(0, 1);
	sliderInterpolator[SLIDER_FILTER_CUTOFF].prepareToPlay(440, 1);
	sliderInterpolator[SLIDER_FILTER_RESO].prepareToPlay(10, 1);
	sliderInterpolator[SLIDER_REVERB_DECAY].prepareToPlay(0.5, 1);
	sliderInterpolator[SLIDER_REVERB_SIZE].prepareToPlay(0, 1);
}

size_t getZenBlockNum(size_t JUCE_BlockSize)
{
	size_t blocksNum = JUCE_BlockSize > ZEN_BLOCK_SIZE ? JUCE_BlockSize / ZEN_BLOCK_SIZE : 1;
	return JUCE_BlockSize % ZEN_BLOCK_SIZE == 0 ? blocksNum : blocksNum + 1;
}

void setDebugStuff(float *ch1, float *ch2, size_t size)
{
	// setDebugAnalyser(ch1, ch2, size);
}
void ZENTest_processBlock(const float **in, float **out, int chan_num, size_t size)
{

	for (int slider_num = 0; slider_num < SLIDER_NUM_ENUM; slider_num++)
	{
		sliderInterpolator[slider_num].processBlock(getSliderValue((SlidersEnum)slider_num), sliders_blocks[slider_num], size, 1);
	}
	memcpy(delay_offsets[1], sliders_blocks[SLIDER_SPREAD], 4 * size);
	stereoReverb.setDecay(getSliderValue(SLIDER_REVERB_DECAY));
	float waveFreq = 440;
	wave.setFreq(waveFreq);
	float LFOVals[size];
	lfo.processBlock(LFOVals, sliders_blocks[SLIDER_LFO_FREQ], size);
	// wave.processBlock(out[0], sliders_blocks[SLIDER_LFO_FREQ], size);
	// memcpy(out[0], in[0], 4 * size);
	// memcpy(out[1], out[0], 4 * size);

	// stereoReverb.setFeedbackType(zen::FEEDBACK_TYPE_PING_PONG);
	// stereoReverb.processBlock((const float **)out, out, sliders_blocks[SLIDER_DELAY], (float **)delay_offsets, size, sliders_blocks[SLIDER_FEEDBACK], 0.5f);
	delayProperties.delay = sliders_blocks[SLIDER_DELAY];
	delayProperties.feedback = sliders_blocks[SLIDER_FEEDBACK];
	float preDelay[1] = {0};
	float outGain[1] = {1};
	delayProperties.outGain = outGain;
	delayProperties.preDelay = preDelay;
	delayProperties.nextChannelOffset[0] = delay_offsets[0];
	delayProperties.nextChannelOffset[1] = delay_offsets[1];
	delayProperties.io = out;
	delayProperties.size = size;

	stereoDelay.processBlock(delayProperties);
}

int firstFrame = 1;
bool lastState = false, lastPlayState = false;

int lastNote;
