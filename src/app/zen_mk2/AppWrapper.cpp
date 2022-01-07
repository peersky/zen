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


#include "AppWrapper.h"
#include <algorithm>

#define MYTEST 0
#define FM     1

std::vector<juce::String> cButtonNames =  std::vector<juce::String>
{
    "load"
};

std::vector<juce::String> cSliderNames(SLIDER_NUM_ENUM);
std::vector<juce::String> cLabelNames =  std::vector<juce::String>
{
	"Freq",
	"Alpha",
	"Beta",
	"Omega",
};


std::vector<juce::String> cComboBoxNames =  std::vector<juce::String>
{
    
};

std::vector<juce::String> cWaveformTypes =  std::vector<juce::String>
{
    
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
	for (int i = 0; i<4; i++)
	{
		cLabelValues[i] = *pVal[i];
	}
	
	
}




float getRandomFloat(void)
{
    return ((float)rand()/RAND_MAX);
}

void getZenValues()
{
	
}
