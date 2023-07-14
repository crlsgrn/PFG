/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct LookAndFeel1 : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;
};

struct RotarySliderWithLabels : juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::TextBoxBelow),
        param(&rap),
        suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
private:
    LookAndFeel1 lnf;

    juce::RangedAudioParameter* param;
    juce::String suffix;
};

//=========================================================
struct CustomRotarySlider2 : juce::Slider
{
    CustomRotarySlider2() : juce::Slider(juce::Slider::SliderStyle::LinearBar,
        juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    {

    }
};

struct CustomRotarySlider3 : juce::Slider
{
    CustomRotarySlider3() : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag,
        juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    {

    }
};

struct CustomRotarySlider4 : juce::Slider
{
    CustomRotarySlider4() : juce::Slider(juce::Slider::SliderStyle::LinearHorizontal,
        juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    {

    }
};



//==============================================================================
/**
*/
class PhaserAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    PhaserAudioProcessorEditor(PhaserAudioProcessor&);
    ~PhaserAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    PhaserAudioProcessor& audioProcessor;


    RotarySliderWithLabels
        depthSlider1,
        rateSlider1,
        feedbackSlider1,
        centreFrequencySlider1,
        mixSlider1,
        depthSlider2,
        rateSlider2,
        feedbackSlider2,
        centreFrequencySlider2,
        mixSlider2,
        depthSlider3,
        rateSlider3,
        feedbackSlider3,
        centreFrequencySlider3,
        mixSlider3,
        depthSlider4,
        rateSlider4,
        feedbackSlider4,
        centreFrequencySlider4,
        mixSlider4;

    CustomRotarySlider2 freqSlider1,
        freqSlider2,
        freqSlider3;

    CustomRotarySlider3 panSlider;
    CustomRotarySlider4 numBandsSlider;


    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment  depthSliderAttachment1,
        rateSliderAttachment1,
        feedbackSliderAttachment1,
        centreFrequencySliderAttachment1,
        mixSliderAttachment1,
        feedbackSliderAttachment2,
        centreFrequencySliderAttachment2,
        depthSliderAttachment2,
        rateSliderAttachment2,
        mixSliderAttachment2,
        depthSliderAttachment3,
        rateSliderAttachment3,
        feedbackSliderAttachment3,
        centreFrequencySliderAttachment3,
        mixSliderAttachment3,
        depthSliderAttachment4,
        rateSliderAttachment4,
        feedbackSliderAttachment4,
        centreFrequencySliderAttachment4,
        mixSliderAttachment4,
        freqSliderAttachment1,
        freqSliderAttachment2,
        freqSliderAttachment3,
        panSliderAttachment,
        numBandsAttachment;

    std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaserAudioProcessorEditor)
};
