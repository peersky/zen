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

#include "UIComponent.hpp"
#include "../../modules/ui/SpectrumAnalyzer.h"
#include "../../target/plugin/PluginProcessor.h"
#include "../../app/zen_mk2/AppWrapper.h"
#include <cmath>
#include <array>

static float maxDB = 12.0f;
static float minDB = -120.0f;
juce::Path tempPath;
juce::Path zenPath;
void createZenCircle(juce::Path &p, Graphics &g)
{
	float centerX = 900;
	float centerY = 80;
	static float r = 60;
	g.setColour(juce::Colours::white);
	g.fillEllipse(centerX - r, centerY - r, 120, 120);
	p.clear();
	g.setColour(juce::Colours::black);

	static float rotation = 3 * ZEN_PI;
	rotation += 0.03f;
	float sinus = sin(rotation);
	float cosinus = cos(rotation);

	float mp = r / 2;

	int numRotations = roundToInt(rotation / (2 * ZEN_PI));
	if (numRotations % 3 == 0)
	{
		//		mp *= 0.75;
		sinus *= -sinus;
		cosinus *= -cosinus;
	}

	p.startNewSubPath(centerX, centerY);
	p.addCentredArc(centerX - sinus * mp, centerY + cosinus * mp, mp, mp, 1 * rotation, ZEN_PI, 0);
	p.addCentredArc(centerX, centerY, r, r, 1 * rotation + ZEN_PI, 0, ZEN_PI);
	p.addCentredArc(centerX + sinus * mp, centerY - cosinus * mp, mp, mp, 1 * rotation - ZEN_PI, ZEN_PI, 0);
	p.closeSubPath();
}

UIComponent::UIComponent(ZenAudioProcessor &p) : freqProcessor_(p)
{
	formatManager_.registerBasicFormats();

	for (int i = 0; i < cSliderNames.size(); i++)
	{

		sliders_.set(i, new Slider());
		sliders_[i]->setSliderStyle(juce::Slider::Rotary);
		sliders_[i]->setRange(cSliderRangesMin[i], cSliderRangesMax[i], cSliderSteps[i]);
		sliders_[i]->setName(cSliderNames[i]);
		sliders_[i]->setComponentID(String(i));
		sliders_[i]->addListener(this);
		sliders_[i]->setLookAndFeel(&otherLookAndFeel_);
		sliders_[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
		sliders_[i]->setSkewFactor(cSliderSkew[i]);
		sliders_[i]->setTextValueSuffix(cSliderSuffix[i]);
		sliders_[i]->setNumDecimalPlacesToDisplay(2);
		addAndMakeVisible(sliders_[i]);

		sliderLabels_.set(i, new Label());
		sliderLabels_[i]->setText(cSliderNames[i], juce::dontSendNotification);
		sliderLabels_[i]->attachToComponent(sliders_[i], false); // [4]
		addAndMakeVisible(sliderLabels_[i]);

		scopeSliderX_.setSliderStyle(juce::Slider::Rotary);
		scopeSliderX_.setRange(1.0f, 128.0f, 1.0f);
		scopeSliderX_.setName("X");
		scopeSliderX_.addListener(this);
		scopeSliderX_.setLookAndFeel(&otherLookAndFeel_);
		scopeSliderX_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
		scopeSliderX_.setNumDecimalPlacesToDisplay(2);
		scopeSliderX_.setValue(0.2f);
		scopeSliderX_.setTextValueSuffix("X");
		addAndMakeVisible(scopeSliderX_);

		scopeSliderY_.setSliderStyle(juce::Slider::Rotary);
		scopeSliderY_.setRange(0.001f, 1.0f, 0.001f);
		scopeSliderY_.setName("Y");
		scopeSliderY_.addListener(this);
		scopeSliderY_.setLookAndFeel(&otherLookAndFeel_);
		scopeSliderY_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
		scopeSliderY_.setNumDecimalPlacesToDisplay(2);
		scopeSliderY_.setValue(1.0f);
		scopeSliderY_.setTextValueSuffix("Y");
		addAndMakeVisible(scopeSliderY_);
	}

	for (int i = 0; i < cButtonNames.size(); i++)
	{
		buttons_.set(i, new TextButton(cButtonNames[i]));
		buttons_[i]->changeWidthToFitText();
		buttons_[i]->setButtonText(cButtonNames[i]);
		buttons_[i]->addListener(this);
		addAndMakeVisible(buttons_[i]);
	}

	for (int i = 0; i < cComboBoxNames.size(); i++)
	{
		comboBoxes_.set(i, new ComboBox());
		comboBoxes_[i]->setName(cComboBoxNames[i]);
		comboBoxes_[i]->addSeparator();
		comboBoxes_[i]->addListener(this);
		addAndMakeVisible(comboBoxes_[i]);
	}

	for (int i = 0; i < cComboBoxNames.size(); i++)
	{
		for (int j = 0; j < cWaveformTypes.size(); j++)
		{
			comboBoxes_[i]->addItem(cWaveformTypes[j], j + 1);
		}
		comboBoxes_[i]->setSelectedItemIndex(0);
	}

	for (int i = 0; i < cLabelNames.size(); i++)
	{
		labels_.set(i, new Label());
		labels_[i]->setText(cLabelNames[i], juce::dontSendNotification);
		labels_[i]->setColour(juce::Label::textColourId, juce::Colours::lightgreen);
		addAndMakeVisible(labels_[i]);
	}

	for (int i = 0; i < cLabelNames.size(); i++)
	{
		int _vi = (int)cLabelNames.size() + i;
		labels_.set(_vi, new Label());
		labels_[_vi]->setText(String(cLabelValues[i]), juce::dontSendNotification);
		labels_[_vi]->setColour(juce::Label::textColourId, juce::Colours::lightgreen);
		addAndMakeVisible(labels_[_vi]);
	}

	analyserPath_.setFill(Colours::transparentWhite);
	analyserPath_.setStrokeType(juce::PathStrokeType(1.0));
	analyserPath_.setStrokeFill(juce::Colours::silver);
	addAndMakeVisible(&analyserPath_);

	freqProcessor_.scope_.setTriggerMode(drow::TriggeredScope::TriggerMode::Up);
	addAndMakeVisible(&freqProcessor_.scope_);

	startTimerHz(20);
}

void UIComponent::timerCallback(void)
{
	freqProcessor_.scope_.setTriggerLevel(scopeSliderY_.getValue());
	int samplesPerPixel = (int)(scopeSliderX_.getValue());
	freqProcessor_.scope_.setNumSamplesPerPixel(samplesPerPixel);
	for (int i = 0; i < cSliderNames.size(); i++)

		for (int i = 0; i < cLabelNames.size(); i++)
		{
			labels_[i]->setText(cLabelNames[i], juce::dontSendNotification);
			labels_[i + (int)cLabelNames.size()]->setText(String(cLabelValues[i]), juce::dontSendNotification);
		}

	if (freqProcessor_.checkForNewAnalyserData())
	{
		{
			freqProcessor_.createAnalyserPlot(tempPath, plotFrame_, 20.0f);
			analyserPath_.setPath(tempPath);
		}
	}
	for (int i = 0; i < cSliderNames.size(); i++)
	{
		sliders_[i]->setValue(cSliderValues[i]);
	}
	repaint();

	UILabelsTypeDef &ref = getUIValues();
	setLabelValue(ref);
}

void UIComponent::textEditorTextChanged(TextEditor &tf)
{
}

UIComponent::~UIComponent()
{
}

void UIComponent::paint(Graphics &g)

{

	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillAll(Colours::slategrey); // clear the background

	createZenCircle(zenPath, g);
	g.strokePath(zenPath, juce::PathStrokeType(1.0));
	g.fillPath(zenPath);

	g.setColour(Colours::aliceblue);

	g.setFont(16.0f);
	g.setFont(12.0f);
	g.setColour(juce::Colours::silver);
	g.drawRoundedRectangle(plotFrame_.toFloat(), 5, 2);
	for (int i = 0; i < 10; ++i)
	{
		g.setColour(juce::Colours::silver.withAlpha(0.3f));
		auto x = plotFrame_.getX() + plotFrame_.getWidth() * i * 0.1f;
		if (i > 0)
			g.drawVerticalLine(juce::roundToInt(x), float(plotFrame_.getY()), float(plotFrame_.getBottom()));

		g.setColour(juce::Colours::silver);
		auto freq = getFrequencyForPosition_(i * 0.1f);
		g.drawFittedText((freq < 1000) ? juce::String(freq) + " Hz"
									   : juce::String(freq / 1000, 1) + " kHz",
						 juce::roundToInt(x + 3), plotFrame_.getBottom() + 0, 50, 15, juce::Justification::left, 1);
	}

	g.setColour(juce::Colours::silver.withAlpha(0.3f));
	g.drawHorizontalLine(juce::roundToInt(plotFrame_.getY() + 2 + ((maxDB) / (maxDB - minDB)) * plotFrame_.getHeight()), float(plotFrame_.getX()), float(plotFrame_.getRight()));
	g.drawHorizontalLine(juce::roundToInt(plotFrame_.getY() + 2 + ((maxDB - (1 * minDB / 4)) / (maxDB - minDB)) * plotFrame_.getHeight()), float(plotFrame_.getX()), float(plotFrame_.getRight()));
	g.drawHorizontalLine(juce::roundToInt(plotFrame_.getY() + 2 + ((maxDB - (2 * minDB / 4)) / (maxDB - minDB)) * plotFrame_.getHeight()), float(plotFrame_.getX()), float(plotFrame_.getRight()));
	g.drawHorizontalLine(juce::roundToInt(plotFrame_.getY() + 2 + ((maxDB - (3 * minDB / 4)) / (maxDB - minDB)) * plotFrame_.getHeight()), float(plotFrame_.getX()), float(plotFrame_.getRight()));

	g.setColour(juce::Colours::silver);
	g.drawFittedText(juce::String(maxDB) + " dB", plotFrame_.getX() + 3, plotFrame_.getY() + 2, 50, 14, juce::Justification::left, 1);
	g.drawFittedText(" 0 dB", plotFrame_.getX() + 3, juce::roundToInt(plotFrame_.getY() + 2 + ((maxDB) / (maxDB - minDB)) * plotFrame_.getHeight()), 50, 14, juce::Justification::left, 1);
	g.drawFittedText(juce::String(1 * minDB / 4) + " dB", plotFrame_.getX() + 3, juce::roundToInt(plotFrame_.getY() + 2 + ((maxDB - (1 * minDB / 4)) / (maxDB - minDB)) * plotFrame_.getHeight()), 50, 14, juce::Justification::left, 1);
	g.drawFittedText(juce::String(2 * minDB / 4) + " dB", plotFrame_.getX() + 3, juce::roundToInt(plotFrame_.getY() + 2 + ((maxDB - (2 * minDB / 4)) / (maxDB - minDB)) * plotFrame_.getHeight()), 50, 14, juce::Justification::left, 1);
	g.drawFittedText(juce::String(3 * minDB / 4) + " dB", plotFrame_.getX() + 3, juce::roundToInt(plotFrame_.getY() + 2 + ((maxDB - (3 * minDB / 4)) / (maxDB - minDB)) * plotFrame_.getHeight()), 50, 14, juce::Justification::left, 1);

	g.reduceClipRegion(plotFrame_);

	g.drawFittedText("Output", plotFrame_.reduced(8), juce::Justification::topRight, 1);
}

void UIComponent::resized()
{

	for (int i = 0; i < labels_.size(); i++)
	{
		int x = i < labels_.size() / 2 ? 320 : 420;
		int y = i >= labels_.size() / 2 ? 20 * (i - labels_.size() / 2) : 20 * i;

		labels_[i]->setBounds(x + 150, y + 200, getWidth() - 0, 30);
	}

	for (int i = 0; i < cSliderNames.size(); i++)
	{

		sliders_[i]->setBounds(cLeftOffset_ + ((cSliderWidth_ + cXSpacing_) * i),
							   cTopOffset_,
							   cSliderWidth_,
							   cSliderHeight_);
	}

	freqProcessor_.setSavedSize({getWidth(), getHeight()});
	plotFrame_.setBounds(cLeftOffset_,
						 400 - cTopOffset_,
						 400,
						 200);
	freqProcessor_.scope_.setBounds(420 + cLeftOffset_,
									400 - cTopOffset_,
									400,
									200);

	scopeSliderX_.setBounds(840 + cLeftOffset_,
							400 - cTopOffset_,
							cSliderWidth_,
							cSliderHeight_);
	scopeSliderY_.setBounds(840 + cLeftOffset_,
							500 - cTopOffset_,
							cSliderWidth_,
							cSliderHeight_);
}

void UIComponent::sliderValueChanged(Slider *s)
{
	setSliderValue(s->getName(), s->getValue());
}

void UIComponent::buttonStateChanged(Button *b)
{
	setButtonState(b->getName(), b->getState());
}

void UIComponent::comboBoxChanged(ComboBox *cb)
{
	setComboBoxState(cb->getName(), cb->getSelectedId() - 1);
}

void UIComponent::buttonClicked(Button *b)
{
	if (b->getName() == "load")
	{
		juce::FileChooser chooser("Select a Wave file to play...", {}, "*.wav");

		if (chooser.browseForFileToOpen())
		{
			auto file = chooser.getResult();
			auto *reader = formatManager_.createReaderFor(file);

			if (reader != nullptr)
			{
				AudioBuffer<float> buffer = AudioBuffer<float>(reader->numChannels, int(reader->lengthInSamples));

				reader->read(&buffer, 0, buffer.getNumSamples(), 0, true, true);

				loadedAudio.add(buffer);
			}
		}
	}
}

void UIComponent::labelTextChanged(Label *label)
{
	printf("lol");
}

float UIComponent::getFrequencyForPosition_(float pos)
{
	return 20.0f * std::pow(2.0f, pos * 10.0f);
}

float UIComponent::getPositionForGain_(float gain, float top, float bottom)
{
	return juce::jmap(juce::Decibels::gainToDecibels(gain, -maxDB), -maxDB, maxDB, bottom, top);
}

float UIComponent::getGainForPosition_(float pos, float top, float bottom)
{
	return juce::Decibels::decibelsToGain(juce::jmap(pos, bottom, top, -maxDB, maxDB), -maxDB);
}

float UIComponent::getPositionForFrequency_(float freq)
{
	return (std::log(freq / 20.0f) / std::log(2.0f)) / 10.0f;
}
