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

/*
 This code initially is deeply modified version of spectrum analyzer written by Daniel Walz in his Frequalizer
 */

#pragma once

#include "../../../3rd_party/JUCE/modules/juce_dsp/juce_dsp.h"
#include "../../../3rd_party/JUCE/modules/juce_gui_basics/juce_gui_basics.h"

//==============================================================================
/*
 */
template <typename Type>
class Analyser : public juce::Thread
{
public:
	Analyser() : juce::Thread("SpectrumAnalyser")
	{
		averager_.clear();
	}

	~Analyser() override = default;

	void addAudioData(const juce::AudioBuffer<Type> &buffer, int startChannel, int numChannels)
	{
		if (abstractFifo_.getFreeSpace() < buffer.getNumSamples())
			return;

		int start1, block1, start2, block2;
		abstractFifo_.prepareToWrite(buffer.getNumSamples(), start1, block1, start2, block2);
		audioFifo_.copyFrom(0, start1, buffer.getReadPointer(startChannel), block1);
		if (block2 > 0)
			audioFifo_.copyFrom(0, start2, buffer.getReadPointer(startChannel, block1), block2);

		for (int channel = startChannel + 1; channel < startChannel + numChannels; ++channel)
		{
			if (block1 > 0)
				audioFifo_.addFrom(0, start1, buffer.getReadPointer(channel), block1);
			if (block2 > 0)
				audioFifo_.addFrom(0, start2, buffer.getReadPointer(channel, block1), block2);
		}
		abstractFifo_.finishedWrite(block1 + block2);
		waitForData_.signal();
	}

	void setupAnalyser(int audioFifoSize, Type sampleRateToUse)
	{
		sampleRate_ = sampleRateToUse;
		audioFifo_.setSize(1, audioFifoSize);
		abstractFifo_.setTotalSize(audioFifoSize);

		startThread(5);
	}

	void run() override
	{
		while (!threadShouldExit())
		{
			if (abstractFifo_.getNumReady() >= fft_.getSize())
			{
				fftBuffer_.clear();

				int start1, block1, start2, block2;
				abstractFifo_.prepareToRead(fft_.getSize(), start1, block1, start2, block2);
				if (block1 > 0)
					fftBuffer_.copyFrom(0, 0, audioFifo_.getReadPointer(0, start1), block1);
				if (block2 > 0)
					fftBuffer_.copyFrom(0, block1, audioFifo_.getReadPointer(0, start2), block2);
				abstractFifo_.finishedRead((block1 + block2) / 2);

				windowing_.multiplyWithWindowingTable(fftBuffer_.getWritePointer(0), size_t(fft_.getSize()));
				fft_.performFrequencyOnlyForwardTransform(fftBuffer_.getWritePointer(0));

				juce::ScopedLock lockedForWriting(pathCreationLock_);
				averager_.addFrom(0, 0, averager_.getReadPointer(averagerPtr), averager_.getNumSamples(), -1.0f);
				averager_.copyFrom(averagerPtr, 0, fftBuffer_.getReadPointer(0), averager_.getNumSamples(), 1.0f / (averager_.getNumSamples() * (averager_.getNumChannels() - 1)));
				averager_.addFrom(0, 0, averager_.getReadPointer(averagerPtr), averager_.getNumSamples());
				if (++averagerPtr == averager_.getNumChannels())
					averagerPtr = 1;

				newDataAvailable_ = true;
			}

			if (abstractFifo_.getNumReady() < fft_.getSize())
				waitForData_.wait(100);
		}
	}

	void createPath(juce::Path &p, const juce::Rectangle<float> bounds, float minFreq)
	{
		p.clear();
		p.preallocateSpace(8 + bounds.getWidth());

		juce::ScopedLock lockedForReading(pathCreationLock_);
		const auto *fftData = averager_.getReadPointer(0);

		//I have no idea how this works but factor is scaling relationship x to index so it matches the x grid
		const auto factor = bounds.getWidth() / 10.1f;

		float prev_x = 0;
		p.startNewSubPath(bounds.getX(), binToY_(fftData[0], bounds));
		for (int i = 0; i < averager_.getNumSamples(); i += 1)
		{

			float x = indexToX_(float(i), minFreq);
			float y = binToY_(fftData[i], bounds);
			//Draw only pixels that fit in to current pixel width of a frame.
			if ((x - prev_x) >= 1.0f / factor)
			{
				p.lineTo(bounds.getX() + factor * x, y);
				prev_x = x;
			}
		}
	}

	bool checkForNewData()
	{
		auto available = newDataAvailable_.load();
		newDataAvailable_.store(false);
		return available;
	}

private:
	inline float indexToX_(float index, float minFreq) const
	{
		const auto freq = (sampleRate_ * index) / fft_.getSize();
		return (freq > minFreq) ? std::log(freq / minFreq) / std::log(2.0f) : 0.0f;
	}

	inline float binToY_(float bin, const juce::Rectangle<float> bounds) const
	{
		const float infinity = -220.0f;
		return juce::jmap(juce::Decibels::gainToDecibels(bin, infinity),
						  infinity, 6.0f, bounds.getBottom(), bounds.getY());
	}

	juce::WaitableEvent waitForData_;
	juce::CriticalSection pathCreationLock_;

	Type sampleRate_{};

	juce::dsp::FFT fft_{11};
	juce::dsp::WindowingFunction<Type> windowing_{size_t(fft_.getSize()), juce::dsp::WindowingFunction<Type>::hann, true};
	juce::AudioBuffer<float> fftBuffer_{1, fft_.getSize() * 2};

	juce::AudioBuffer<float> averager_{2, fft_.getSize() / 2};
	int averagerPtr = 1;

	juce::AbstractFifo abstractFifo_{24000};
	juce::AudioBuffer<Type> audioFifo_;

	std::atomic<bool> newDataAvailable_;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Analyser)
};
