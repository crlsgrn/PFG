/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin processor.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

struct PhaserBand
{

    juce::AudioParameterFloat* rate{ nullptr };
    juce::AudioParameterFloat* depth{ nullptr };
    juce::AudioParameterFloat* centreFrequency{ nullptr };
    juce::AudioParameterFloat* feedback{ nullptr };
    juce::AudioParameterFloat* mix{ nullptr };
    /*
    juce::AudioParameterBool* bypass{ nullptr };
    */



    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        phaser.prepare(spec);
    }

    void updatePhaserSettings()
    {

        phaser.setRate(rate->get());
        phaser.setDepth(depth->get());
        phaser.setFeedback(feedback->get());
        phaser.setCentreFrequency(centreFrequency->get());
        phaser.setMix(mix->get());
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);

        //context.isBypassed = bypass->get();

        phaser.process(context);
    }



private:
    juce::dsp::Phaser<float> phaser;

};


//==============================================================================
/**
*/
class PhaserAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    PhaserAudioProcessor();
    ~PhaserAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();

    APVTS apvts{ *this, nullptr, "Parameters", createParameterLayout() };
    std::vector<PhaserBand> phasers;

private:
    //PhaserBand phasers;
    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    Filter LP1, AP2, AP3,
        HP1, LP2, LP3, AP4,
        HP2, HP3;



    std::vector<juce::AudioBuffer<float>> filterBuffers;

    std::vector<juce::AudioParameterFloat*> freqs;//asociado generalmene no x  banda
    juce::AudioParameterFloat* pan{ nullptr };
    juce::AudioParameterInt* numBands{ nullptr };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaserAudioProcessor)
};


