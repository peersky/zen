/*
	This is reposnsible should implement API for Juce Standalone/Plugin targets
*/

#pragma once

#include <JuceHeader.h>
#include "../../modules/ui/SpectrumAnalyzer.h"
#include "App.h"
#include "stdlib.h"

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

inline void setSlider(SlidersEnum slider, String name, float min, float max, float step, float skew, String suffix);
bool getButtonState(String name);
void setButtonState(String name, bool on);
int getComboBoxState(String name);
void setComboBoxState(String name, int idx);
void setSliderModelValue(String name, float val);
void setSliderValue(String name, float val);

void JuceAppInit(double sampleRate, int samplesPerBlock);
void JuceAppNoteOn(int midiNoteNumber, float velocity);
void JuceAppNoteOff(int midiNoteNumber);

void setLabelValue(float (&pVal)[SLIDER_NUM_ENUM]);

void JuceApp_processBlock(const float **in, float **out, int chan_num, size_t size);
