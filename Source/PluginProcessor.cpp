/*
==============================================================================
  This file contains the basic framework code for a JUCE plugin processor.
==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>
#include <juce_audio_formats/codecs/flac/compat.h>

using namespace juce::dsp;

//==============================================================================
PhaserAudioProcessor::PhaserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{



    numBands = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("numBands"));
    jassert(numBands != nullptr);
    int nBands = numBands->get();


    freqs.resize(nBands - 1);
    phasers.resize(nBands);
    filterBuffers.resize(nBands);


    int size = phasers.size();

    for (int i = 0; i < size; ++i) {
        const auto bandPrefix = juce::String("Band") + juce::String(i + 1) + juce::String(".");
        phasers[i].rate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(bandPrefix + "Rate"));
        jassert(phasers[i].rate != nullptr);
        phasers[i].depth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(bandPrefix + "Depth"));
        jassert(phasers[i].depth != nullptr);
        phasers[i].feedback = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(bandPrefix + "Feedback"));
        jassert(phasers[i].feedback != nullptr);
        phasers[i].centreFrequency = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(bandPrefix + "CentreFrequency"));
        jassert(phasers[i].centreFrequency != nullptr);
        phasers[i].mix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(bandPrefix + "Mix"));
        jassert(phasers[i].mix != nullptr);

    }
    pan = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("pan"));
    jassert(pan != nullptr);
    /*
    phaser.bypass = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Bypass"));
    jassert(phaser.bypass != nullptr);
    */
    //CROSSOVERS
    for (int i = 0; i < size - 1; ++i) {
        const auto bandPrefix = juce::String("Crossover") + juce::String(i + 1) + juce::String(".");
        freqs[i] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(bandPrefix + "freq"));
        jassert(freqs[i] != nullptr);
    }


    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);


    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    AP3.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    AP4.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    LP3.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);

    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    HP3.setType(juce::dsp::LinkwitzRileyFilterType::highpass);


}

PhaserAudioProcessor::~PhaserAudioProcessor()
{
}

//==============================================================================
const juce::String PhaserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PhaserAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PhaserAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PhaserAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PhaserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PhaserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int PhaserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PhaserAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String PhaserAudioProcessor::getProgramName(int index)
{
    return {};
}

void PhaserAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void PhaserAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    for (auto& phaser : phasers)
        phaser.prepare(spec);



    LP1.prepare(spec);
    HP1.prepare(spec);

    AP2.prepare(spec);
    AP3.prepare(spec);
    AP4.prepare(spec);

    LP2.prepare(spec);
    HP2.prepare(spec);

    LP3.prepare(spec);
    HP3.prepare(spec);



    for (auto& buffer : filterBuffers)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }


}

void PhaserAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PhaserAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void PhaserAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());



    std::cout << "Current numBands value: " << numBands->get() << std::endl;
    std::cout << "Current freqs size: " << freqs.size() << std::endl;



    for (auto& fb : filterBuffers) {
        fb = buffer;
    }

    if (numBands->get() == 4) {
        std::cout << "4 bandas " << std::endl;
        auto lowMidCutoffFreq = freqs[0]->get();
        LP1.setCutoffFrequency(lowMidCutoffFreq);
        HP1.setCutoffFrequency(lowMidCutoffFreq);


        auto midHighCutoffFreq = freqs[1]->get();
        AP2.setCutoffFrequency(midHighCutoffFreq);
        AP3.setCutoffFrequency(midHighCutoffFreq);
        LP2.setCutoffFrequency(midHighCutoffFreq);
        HP2.setCutoffFrequency(midHighCutoffFreq);

        auto mid2Freq = freqs[2]->get();
        AP4.setCutoffFrequency(mid2Freq);
        LP3.setCutoffFrequency(mid2Freq);
        HP3.setCutoffFrequency(mid2Freq);

        auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
        auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
        auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);
        auto fb3Block = juce::dsp::AudioBlock<float>(filterBuffers[3]);



        auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
        auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
        auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);
        auto fb3Ctx = juce::dsp::ProcessContextReplacing<float>(fb3Block);

        LP1.process(fb0Ctx);//1
        AP2.process(fb0Ctx);//2
        AP3.process(fb0Ctx);//3
        HP1.process(fb1Ctx);//1
        filterBuffers[2] = filterBuffers[1];

        AP4.process(fb1Ctx);//2
        LP2.process(fb1Ctx);//3
        HP2.process(fb2Ctx);//2
        filterBuffers[3] = filterBuffers[2];
        LP3.process(fb2Ctx);//3
        HP3.process(fb3Ctx);//3
    }
    if (numBands->get() == 3) {
        std::cout << "3 bandas" << std::endl;
        auto lowMidCutoffFreq = freqs[0]->get();
        LP1.setCutoffFrequency(lowMidCutoffFreq);
        HP1.setCutoffFrequency(lowMidCutoffFreq);


        auto midHighCutoffFreq = freqs[1]->get();
        AP2.setCutoffFrequency(midHighCutoffFreq);
        LP2.setCutoffFrequency(midHighCutoffFreq);
        HP2.setCutoffFrequency(midHighCutoffFreq);

        auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
        auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
        auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);
        auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
        auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
        auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);


        LP1.process(fb0Ctx);
        AP2.process(fb0Ctx);

        HP1.process(fb1Ctx);
        filterBuffers[2] = filterBuffers[1];
        LP2.process(fb1Ctx);

        HP2.process(fb2Ctx);

    }
    if (numBands->get() == 2) {
        std::cout << "2 bandas " << std::endl;
        auto lowMidCutoffFreq = freqs[0]->get();
        LP1.setCutoffFrequency(lowMidCutoffFreq);
        HP1.setCutoffFrequency(lowMidCutoffFreq);
        auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
        auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
        auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
        auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
        LP1.process(fb0Ctx);
        HP1.process(fb1Ctx);
    }

    for (int i = 0; i < numBands->get(); ++i)
        phasers[i].updatePhaserSettings();


    for (size_t i = 0; i < numBands->get(); ++i)

        phasers[i].process(filterBuffers[i]);


    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();



    buffer.clear();
    //helper

    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
    {
        for (auto i = 0; i < nc; ++i)
        {
            inputBuffer.addFrom(i, 0, source, i, 0, ns);
        }
    };

    for (size_t i = 0; i < numBands->get(); ++i) {
        addFilterBand(buffer, filterBuffers[i]);
    }
    //PANEO
    if (numChannels == 2) {

        float panParameter = M_PI * (pan->get() + 1) / 4;
        //TIENE 2 CANALES(stereo), la información que no le mandas a uno se la mandas al otro
        auto* writePointer = buffer.getWritePointer(0);
        for (int sample = 0; sample < numSamples; ++sample)
            writePointer[sample] = writePointer[sample] * cos(panParameter);//CANAL IZQUIERDO
        writePointer = buffer.getWritePointer(1);
        for (int sample = 0; sample < numSamples; ++sample)
            writePointer[sample] = writePointer[sample] * sin(panParameter);//CANAL DERECHO
    }

}

//==============================================================================

bool PhaserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PhaserAudioProcessor::createEditor()
{
    return new PhaserAudioProcessorEditor(*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void PhaserAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);

}

void PhaserAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}
juce::AudioProcessorValueTreeState::ParameterLayout PhaserAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    using namespace juce;


    layout.add(std::make_unique<AudioParameterInt>("numBands", "Num bandas", 2, 4, 4));
    auto num = std::make_unique<AudioParameterInt>("numBands", "Num bandas", 2, 4, 4);
    num->get();

    layout.add(std::make_unique<AudioParameterFloat>("pan", "pan", -1, 1, 0));


    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    for (int i = 0; i < num->get(); ++i) {
        // Prefix para cada parámetro de banda
        const auto bandPrefix = juce::String("Band") + juce::String(i + 1) + juce::String(".");
        // Creamos los parámetros de la banda
        auto rate = std::make_unique<juce::AudioParameterFloat>(bandPrefix + "Rate", bandPrefix + "Rate", juce::NormalisableRange<float>(0, 5, 0.01f, 1), 1.0f, String(), AudioProcessorParameter::genericParameter, [](float value, int) { return String(value, 0) + "Hz"; }, nullptr);
        params.push_back(std::move(rate));
        auto depth = std::make_unique<AudioParameterFloat>(bandPrefix + "Depth", bandPrefix + "Depth", juce::NormalisableRange<float>(0, 1, 0.01f, 1), 0.5f);
        params.push_back(std::move(depth));
        // Calcula de forma proporcional los valores de frecuencia de corte para cada banda
        float minFrequency = 20.0f * std::pow(10, i);
        float maxFrequency = 20.0f * std::pow(10, i + 1);
        float defaultFrequency = (minFrequency + maxFrequency) * 0.5f;
        auto feedback = std::make_unique<AudioParameterFloat>(bandPrefix + "Feedback", bandPrefix + "Feedback", juce::NormalisableRange<float>(0, 1, 0.01f, 1), 0.5f);
        params.push_back(std::move(feedback));
        auto centreFrequency = std::make_unique<AudioParameterFloat>(bandPrefix + "CentreFrequency", bandPrefix + "CentreFrequency", juce::NormalisableRange<float>(20, 20000, 1, 1), 1000, String(), AudioProcessorParameter::genericParameter, [](float value, int) { return String(value, 0) + "Hz"; }, nullptr);
        params.push_back(std::move(centreFrequency));
        auto mix = std::make_unique<AudioParameterFloat>(bandPrefix + "Mix", bandPrefix + "Mix", juce::NormalisableRange<float>(0, 1, 0.01f, 1), 0.0f);
        params.push_back(std::move(mix));

    }
    layout.add(params.begin(), params.end());




    //const int numBands = 4; // Número de bandas del phaser
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> crossoverParameters; // Vector para almacenar los parámetros de frecuencia de corte en el metodo

    for (int i = 0; i < num->get() - 1; ++i)
    {
        // Genera el nombre único del parámetro para cada banda
        std::string parameterName = "Crossover" + std::to_string(i + 1) + ".freq";

        // Calcula de forma proporcional los valores de frecuencia de corte para cada banda
        float minFrequency = 20.0f * std::pow(10, i);
        float maxFrequency = 20.0f * std::pow(10, i + 1);
        float defaultFrequency = (minFrequency + maxFrequency) * 0.5f;


        // Crea el parámetro de frecuencia de corte y agrégalo al layout

        auto freq = std::make_unique<juce::AudioParameterFloat>(parameterName, parameterName, juce::NormalisableRange<float>(minFrequency, maxFrequency, 1.0f), defaultFrequency, String(), AudioProcessorParameter::genericParameter, [](float value, int) {return(value < 1000.0f) ? String(value, 0) + "Hz" : String(value / 1000.0f, 1) + "kHz"; }, nullptr);
        crossoverParameters.push_back(std::move(freq));


    }
    layout.add(crossoverParameters.begin(), crossoverParameters.end());



    return layout;
}





//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PhaserAudioProcessor();
}
