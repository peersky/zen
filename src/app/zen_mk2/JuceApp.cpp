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

#include <JuceHeader.h>
#include "AppWrapper.h"
#include "JuceApp.hpp"
#include "../../modules/utils/tables.h"
#include <algorithm>

#define MYTEST 0
#define FM 1

std::vector<juce::String> cButtonNames = std::vector<juce::String>{
    "load"};

std::vector<juce::String> cSliderNames(SLIDER_NUM_ENUM);
std::vector<juce::String> cLabelNames = std::vector<juce::String>{
    "Freq",
    "Alpha",
    "Beta",
    "Omega",
};

std::vector<juce::String> cComboBoxNames = std::vector<juce::String>{

};

std::vector<juce::String> cWaveformTypes = std::vector<juce::String>{

};

std::vector<float> cSliderModelValues(cSliderNames.size());
std::vector<float> cSliderValues(cSliderNames.size());
std::vector<float> cSliderSteps(cSliderNames.size());
std::vector<float> cSliderSkew(cSliderNames.size());
std::vector<float> cSliderRangesMin(cSliderNames.size());
std::vector<juce::String> cSliderSuffix(cSliderNames.size());
std::vector<float> cSliderRangesMax(cSliderNames.size());
std::vector<bool> cButtonStates(cButtonNames.size());
std::vector<int> cComboBoxStates(cComboBoxNames.size());
std::vector<float> cLabelValues(cLabelNames.size());
Array<AudioBuffer<float>> loadedAudio;

void setSliderModelValue(String name, float val)
{
    for (int i = 0; i < cSliderNames.size(); i++)
    {
        if (name == cSliderNames[i])
        {
            cSliderModelValues[i] = val;
            break;
        }
    }
}

void printSliderValues(void)
{
    for (int i = 0; i < cSliderNames.size(); i++)
    {
    }
}

bool getButtonState(String name)
{
    for (int i = 0; i < cButtonNames.size(); i++)
    {
        if (name == cButtonNames[i])
        {
            return cButtonStates[i];
        }
    }

    return false;
}

void setButtonState(String name, bool on)
{
    for (int i = 0; i < cButtonNames.size(); i++)
    {
        if (name == cButtonNames[i])
        {
            cButtonStates[i] = on;
            break;
        }
    }
}

int getComboBoxState(String name)
{
    for (int i = 0; i < cComboBoxNames.size(); i++)
    {
        if (name == cComboBoxNames[i])
        {
            return cComboBoxStates[i];
        }
    }
    return -1;
}

void setComboBoxState(String name, int idx)
{
    DBG("set state: " + name + " " + String(idx));
    for (int i = 0; i < cComboBoxNames.size(); i++)
    {
        if (name == cComboBoxNames[i])
        {
            cComboBoxStates[i] = idx;
            break;
        }
    }
}

void setSliderValue(String name, float val)
{
    for (int i = 0; i < cSliderNames.size(); i++)
    {
        if (name == cSliderNames[i])
        {
            cSliderValues[i] = val;
            break;
        }
    }
}

void setLabelValue_f(float **pVal)
{
    for (int i = 0; i < 4; i++)
    {
        cLabelValues[i] = *pVal[i];
    }
}

float getRandomFloat(void)
{
    return ((float)rand() / RAND_MAX);
}

void getZenValues()
{
}

void slidersConstruct()
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

inline void setSliderValue(SlidersEnum slider, float val)
{
    cSliderValues[slider] = val;
}

//template <String &name>
float getSliderValue(SlidersEnum i)
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

void setLabelValue(float (&pVal)[SLIDER_NUM_ENUM])
{
    for (int i = 0; i < 4; i++)
    {
        cLabelValues[i] = (float)pVal[i];
    }
}

void JuceAppInit(double sampleRate, int samplesPerBlock)
{
    setSliderValue(SLIDER_DELAY, 100);
    setSliderValue(SLIDER_SPREAD, 0);
    setSliderValue(SLIDER_FEEDBACK, 0.1);
    setSliderValue(SLIDER_LFO_FREQ, 440);
    setSliderValue(SLIDER_MODULATION_DEPTH, 0);
    setSliderValue(SLIDER_FILTER_CUTOFF, 440);
    setSliderValue(SLIDER_FILTER_RESO, 10);
    setSliderValue(SLIDER_REVERB_DECAY, 0.5);
    setSliderValue(SLIDER_REVERB_SIZE, 0);

    ZENTest_init(sampleRate, samplesPerBlock);
}

void JuceAppNoteOn(int midiNoteNumber, float velocity)
{
}
void JuceAppNoteOff(int midiNoteNumber)
{
}

void JuceApp_processBlock(const float **in, float **out, int chan_num, size_t size)
{

    ZENTest_processBlock(in, out, chan_num, size);
}

float *bufferPtr1, *bufferPtr2;
int bufferSize;

void setDebugStuff(float *ptr1, float *ptr2, int size)
{
    printf("setDebugStuff\n");
    bufferSize = size;
    bufferPtr1 = ptr1;
    bufferPtr2 = ptr2;
}
void sendToAnalyzer(float *ptr, int ch)
{
    // printf("sendToAnalyzer\n");
    memcpy(bufferPtr1, ptr, bufferSize);
}