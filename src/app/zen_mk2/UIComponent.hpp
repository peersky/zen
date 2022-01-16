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

#ifndef UICOMPONENT_H_INCLUDED
#define UICOMPONENT_H_INCLUDED

#include "../../target/plugin/PluginProcessor.h"

//==============================================================================
/*
*/

class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
	OtherLookAndFeel()
	{
		setColour(juce::Slider::thumbColourId, juce::Colours::red);
	}

	void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
						  const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override
	{
		auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
		auto centreX = (float)x + (float)width * 0.5f;
		auto centreY = (float)y + (float)height * 0.5f;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0f;
		auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

		// fill
		g.setColour(juce::Colours::orange);
		g.fillEllipse(rx, ry, rw, rw);

		// outline
		g.setColour(juce::Colours::red);
		g.drawEllipse(rx, ry, rw, rw, 1.0f);

		juce::Path p;
		auto pointerLength = radius * 0.33f;
		auto pointerThickness = 2.0f;
		p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
		p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

		// pointer
		g.setColour(juce::Colours::yellow);
		g.fillPath(p);
	}
};

class UIComponent : public Component,
					public Slider::Listener,
					public TextButton::Listener,
					public ComboBox::Listener,
					public TextEditor::Listener,
					public Label::Listener,
					private Timer
{
public:
	UIComponent(ZenAudioProcessor &);

	~UIComponent();

	void paint(Graphics &g) override;

	void resized() override;

	void sliderValueChanged(Slider *s) override;

	void buttonClicked(Button *b) override;
	void buttonStateChanged(Button *b) override;

	void textEditorTextChanged(TextEditor &) override;

	void comboBoxChanged(ComboBox *cb) override;

	void timerCallback() override;

	void labelTextChanged(Label *label) override;

private:
	static float getPositionForFrequency_(float freq);

	static float getFrequencyForPosition_(float pos);

	static float getPositionForGain_(float gain, float top, float bottom);

	static float getGainForPosition_(float pos, float top, float bottom);

	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	ZenAudioProcessor &freqProcessor_;

	static const int cLeftOffset_ = 30;
	static const int cTopOffset_ = 30;

	static const int cXSpacing_ = 10;
	static const int cYSpacing_ = 5;

	static const int cSliderHeight_ = 75;
	static const int cSliderWidth_ = 75;

	static const int cLabelHeight_ = 20;
	static const int cLabelWidth_ = cSliderWidth_ + cXSpacing_;

	static const int cButtonHeight_ = 30;
	static const int cButtonWidth_ = 75;

	static const int cBoxHeight_ = 20;
	static const int cBoxWidth_ = 100;

	OwnedArray<Slider> sliders_;
	OwnedArray<Label> sliderLabels_;
	OwnedArray<TextButton> buttons_;
	OwnedArray<TextEditor> textFields_;
	OwnedArray<ComboBox> comboBoxes_;
	OwnedArray<Label> labels_;

	juce::Rectangle<int> plotFrame_;
	juce::Rectangle<int> brandingFrame_;

	juce::DrawablePath analyserPath_;

	juce::AudioFormatManager formatManager_;

	OtherLookAndFeel otherLookAndFeel_;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UIComponent)
};

#endif // UICOMPONENT_H_INCLUDED
