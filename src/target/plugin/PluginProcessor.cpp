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

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ZenAudioProcessorEditor::ZenAudioProcessorEditor(ZenAudioProcessor &p)
    : AudioProcessorEditor(&p), uicomponent_(p), processor_(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(p.getSavedSize().getX(), p.getSavedSize().getY());
    addAndMakeVisible(uicomponent_);
}

ZenAudioProcessorEditor::~ZenAudioProcessorEditor()
{
}

//==============================================================================
void ZenAudioProcessorEditor::paint(Graphics &g)
{
    g.fillAll(Colours::white);
}

void ZenAudioProcessorEditor::resized()
{
    uicomponent_.setBounds(getBounds());
}

AudioSampleBuffer spectrumBuffer;
//==============================================================================
void ZenAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    gain_ = 0.0f;
    timer_ = 0;
    start_ = false;
    ramp_ = false;

    JuceAppInit(sampleRate, samplesPerBlock);
    outputAnalyser_.setupAnalyser(int(sampleRate), float(sampleRate));
    //	outputAnalyser.stopThread(1000);
    spectrumBuffer.setSize(2, samplesPerBlock);
    setDebugStuff(spectrumBuffer.getWritePointer(0), spectrumBuffer.getWritePointer(1), spectrumBuffer.getNumSamples());
    spectrumBuffer.setSize(2, samplesPerBlock);
}

//====================================BLOCK=======================================

void ZenAudioProcessor::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    MidiMessage m;
    for (MidiMessageMetadata metadata : midiMessages)
    {
        m = metadata.getMessage();
        int noteNumber = m.getNoteNumber();
        float velocity = m.getFloatVelocity();

        if (m.isNoteOn())
        {
            JuceAppNoteOn(noteNumber, velocity);
        }
        else if (m.isNoteOff())
        {
            JuceAppNoteOff(noteNumber);
        }
        else
        {
        }
    }

    //   const float* inPointerL = buffer.getReadPointer (0);
    //   const float* inPointerR = buffer.getReadPointer (1);
    //
    //    float* outPointerL = buffer.getWritePointer( 0);
    //    float* outPointerR = buffer.getWritePointer( 1);

    const float **in = buffer.getArrayOfReadPointers();
    float **out = buffer.getArrayOfWritePointers();

    //    for (int samp = 0; samp < buffer.getNumSamples(); ++samp)
    //    {
    //        outPointerL[samp] = ZENTest_tick( (inPointerL[samp] + inPointerR[samp]) * 0.5f )/10.0f;
    //        outPointerR[samp] = outPointerL[samp];
    //    }

    JuceApp_processBlock(in, out, 2, buffer.getNumSamples());
    if (buffer.getNumSamples() != spectrumBuffer.getNumSamples())
    {
        spectrumBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples());
        setDebugStuff(spectrumBuffer.getWritePointer(0), spectrumBuffer.getWritePointer(1), buffer.getNumSamples());
    }

    //	ZENTest_tick( (inPointerL[samp] + inPointerR[samp]) * 0.5f )/10.0f;

    if (getActiveEditor() != nullptr)
        outputAnalyser_.addAudioData(spectrumBuffer, 0, getTotalNumOutputChannels());
}

//==============================================================================

ZenAudioProcessor::ZenAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
}

ZenAudioProcessor::~ZenAudioProcessor()
{
    outputAnalyser_.stopThread(1000);
}

//==============================================================================
const String ZenAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ZenAudioProcessor::acceptsMidi() const
{
    return true;
}

bool ZenAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

double ZenAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ZenAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int ZenAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ZenAudioProcessor::setCurrentProgram(int index)
{
}

const String ZenAudioProcessor::getProgramName(int index)
{
    return String();
}

void ZenAudioProcessor::changeProgramName(int index, const String &newName)
{
}

void ZenAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    // ZENTest_end();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ZenAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

//==============================================================================
bool ZenAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor *ZenAudioProcessor::createEditor()
{
    return new ZenAudioProcessorEditor(*this);
}

//==============================================================================
void ZenAudioProcessor::getStateInformation(MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ZenAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void ZenAudioProcessor::createAnalyserPlot(juce::Path &p, const juce::Rectangle<int> bounds, float minFreq)
{
    outputAnalyser_.createPath(p, bounds.toFloat(), minFreq);
}

bool ZenAudioProcessor::checkForNewAnalyserData()
{
    return outputAnalyser_.checkForNewData();
}

juce::Point<int> ZenAudioProcessor::getSavedSize() const
{
    return editorSize_;
}

void ZenAudioProcessor::setSavedSize(const juce::Point<int> &size)
{
    editorSize_ = size;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new ZenAudioProcessor();
}
