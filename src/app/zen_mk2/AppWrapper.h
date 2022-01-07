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

// #include "../JuceLibraryCode/JuceHeader.h"
#include <JuceHeader.h>
#include "../../modules/dsp/Interpolator.h"
#include "../../modules/utils/tables.h"
#include "../../modules/zen.h"
#include "stdlib.h"

enum SlidersEnum
{
	SLIDER_DELAY = 0,
	SLIDER_SPREAD,
	SLIDER_FEEDBACK,
	SLIDER_LFO_FREQ,
	SLIDER_MODULATION_DEPTH,
	SLIDER_FILTER_CUTOFF,
	SLIDER_FILTER_RESO,
	SLIDER_REVERB_DECAY,
	SLIDER_REVERB_SIZE,
	SLIDER_NUM_ENUM
};

extern std::vector<juce::String> cSliderNames;

extern std::vector<juce::String> cButtonNames;

extern std::vector<juce::String> cComboBoxNames;

extern std::vector<juce::String> cWaveformTypes;

extern std::vector<float> cSliderValues;

extern std::vector<float> cSliderModelValues;

extern std::vector<bool> cButtonStates;

extern std::vector<int> cComboBoxStates;

extern std::vector<float> cSliderSteps;
extern std::vector<float> cSliderRangesMin;
extern std::vector<float> cSliderRangesMax;
extern std::vector<float> cSliderSkew;
extern std::vector<juce::String> cSliderSuffix;
extern std::vector<float> cLabelValues;
extern std::vector<juce::String> cLabelNames;

extern Array<AudioBuffer<float>> loadedAudio;

void printSliderValues(void);
bool getButtonState(String name);
void setButtonState(String name, bool on);
int getComboBoxState(String name);
void setComboBoxState(String name, int idx);
void setSliderModelValue(String name, float val);
void setSliderValue(String name, float val);
//float getSliderValue(String name);
float getRandomFloat(void);
//void setLabelValue_f(float **pVal);

template <typename T>
inline void setLabelValue(T *pVal)
{
	for (int i = 0; i < 4; i++)
	{
		cLabelValues[i] = (float)pVal[i];
	}
}

//template <String &name>
inline float getSliderValue(SlidersEnum i)
{
	float value = 0.0f;
	value = cSliderValues[i];

	if (i == SLIDER_FEEDBACK)
	{
		float range = cSliderRangesMax[i] - cSliderRangesMin[i];
		float invStep = 4095.0f / range;
		float indexf = value * invStep;
		size_t index = (size_t)indexf;
		return zen_Feedback_Shape[index];
	}

	return value;
}
//template <String &name>
inline void setSlider(SlidersEnum slider, String name, float min, float max, float step, float skew, String suffix)
{
	cSliderNames[slider] = name;
	cSliderRangesMax[slider] = max;
	cSliderRangesMin[slider] = min;
	cSliderSteps[slider] = max * step;
	cSliderSkew[slider] = skew;
	cSliderSuffix[slider] = suffix;
}

inline void slidersConstruct()
{

	float step12b = 1.0f / 4096.0f;
	setSlider(SLIDER_DELAY, "Delay", 0, 3000, step12b, 0.5f, "ms");
	setSlider(SLIDER_SPREAD, "Spread", 0, 50, step12b, 0.5f, "ms");
	setSlider(SLIDER_FEEDBACK, "Feedback", 0, 1.1, step12b, 0.5f, "%");
	setSlider(SLIDER_LFO_FREQ, "LFO", 0, 1000, step12b, 0.5f, "Hz");
	setSlider(SLIDER_FILTER_RESO, "Q", 0, 1000, step12b, 0.5f, "");
	setSlider(SLIDER_REVERB_SIZE, "Size", 0, 1, step12b, 0.5f, "");
	setSlider(SLIDER_REVERB_DECAY, "Decay", 0, 1, step12b, 0.5f, "");
	setSlider(SLIDER_FILTER_CUTOFF, "Fc", 0, 10000, step12b, 0.5f, "Hz");
	setSlider(SLIDER_MODULATION_DEPTH, "M Depth", 0, 1, step12b, 1, "%");
}

inline void setSliderValue(SlidersEnum slider, float val)
{
	cSliderValues[slider] = val;
}
