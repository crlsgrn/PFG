/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>

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
    //hay que añadir esto si no no hay cambios audibles
    //Esta línea se usa para obtener el objeto AudioParameterFloat con 
    //nombre "Rate" desde el objeto AudioProcessorValueTreeState (apvts)

    numBands = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("numBands"));
    jassert(numBands != nullptr);
    int nBands = numBands->get();

    //this->numBands= numBands;

    //for (int i = 0; i < numBands->get(); ++i) {

        //auto phaser = std::make_unique<PhaserBand>();

      //  PhaserBand& bandComp = phasers[i];
    //acceso a cada banda en un bucle for

       // std::string rateParamName = "Rate_" + std::to_string(i+1);
        //bandComp.rate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(rateParamName));
        //jassert(bandComp.rate != nullptr);
        /*
        phasers[i].rate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Rate"));
        jassert(phasers[i].rate != nullptr);

        rateParams.emplace_back(phasers[i].rate);
        phasers[i].rate = rateParams.back().get();
        */
        //phasers[i].rate = rateParam;
    //}


    //phasers[i].rate[i] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Rate"));
    //jassert(phaser.rate != nullptr);
    /*

    for (int i = 0; i < 3; ++i) {

        phasers[i].rateParam[i] = std::make_unique<juce::AudioParameterFloat>("Rate" + std::to_string(i), "Rate" + std::to_string(i), juce::NormalisableRange<float>(0.f, 40.f, 0.01f), 30.f);
        //phasers[i].rate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Rate" + std::to_string(i)));
        jassert(phasers[i].rateParam[i] != nullptr);
        phasers[1].rate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Rate"));
        jassert(phasers[1].rate != nullptr);
        phasers[2].rate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Rate"));
        jassert(phasers[2].rate != nullptr);

    }*/
    //inicialización de tamaños de los vectores
    
    filters.resize(nBands);//para la estructura de IIR
    freqs.resize(nBands - 1);
    phasers.resize(nBands);
    filterBuffers.resize(nBands);
    //polifasicos.resize(nBands);

    int size = phasers.size();

    for (int i = 0; i < size; ++i) {
        const auto bandPrefix = juce::String("Band") + juce::String(i + 1) + juce::String(".");
        phasers[i].rate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(bandPrefix + "Rate"));
        jassert(phasers[i].rate != nullptr);
        phasers[i].depth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(bandPrefix + "Depth"));
        jassert(phasers[i].depth != nullptr);
    }

    /*
    phaser.centreFrequency = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("CentreFrequency"));
    jassert(phaser.centreFrequency != nullptr);
    phaser.depth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Depth"));
    jassert(phaser.depth != nullptr);
    phaser.feedback = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Feedback"));
    jassert(phaser.feedback != nullptr);
    phaser.mix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Mix"));
    jassert(phaser.mix != nullptr);
    phaser.bypass = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Bypass"));
    jassert(phaser.bypass != nullptr);
    */

    for (int i = 0; i < size - 1; ++i) {
        const auto bandPrefix = juce::String("Band") + juce::String(i + 1) + juce::String(".");
        freqs[i].freqCrossover = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(bandPrefix + "freq"));
        jassert(freqs[i].freqCrossover != nullptr);
        //midHighCrossover = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("midHighCrossover"));
        //jassert(midHighCrossover != nullptr);
    }
    //LR
    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    //estructura de filtros IIR 


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

    LP2.prepare(spec);
    HP2.prepare(spec);


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


    //updatePhaserSettings
    //phaser.updatePhaserSettings();


    //si no se hace esto no se escuchan los cambios ya que no se reemplaza el buffer
    //for (auto& phaser : phasers) {
      //  phasers[0].updatePhaserSettings();
    //}
    for (int i = 0; i < numBands->get(); ++i) {
        phasers[i].updatePhaserSettings();
        phasers.resize(numBands->get());
    }

    for (auto& fb : filterBuffers) {
        fb = buffer;
    }
    //hacer for aqui

    //frecuencias de corte de los filtros
    auto lowMidCutoffFreq = freqs[0].freqCrossover->get();
    LP1.setCutoffFrequency(lowMidCutoffFreq);
    HP1.setCutoffFrequency(lowMidCutoffFreq);
    //    invAP1.setCutoffFrequency(lowMidCutoffFreq);

    auto midHighCutoffFreq = freqs[1].freqCrossover->get();
    AP2.setCutoffFrequency(midHighCutoffFreq);
    LP2.setCutoffFrequency(midHighCutoffFreq);
    HP2.setCutoffFrequency(midHighCutoffFreq);
    //    invAP2.setCutoffFrequency(midHighCutoffFreq);

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

    for (size_t i = 0; i < filterBuffers.size(); ++i)
    {
        phasers[i].process(filterBuffers[i]);
    }

    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    buffer.clear();

    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
    {
        for (auto i = 0; i < nc; ++i)
        {
            inputBuffer.addFrom(i, 0, source, i, 0, ns);
        }
    };

    addFilterBand(buffer, filterBuffers[0]);
    addFilterBand(buffer, filterBuffers[1]);
    addFilterBand(buffer, filterBuffers[2]);

}

//==============================================================================
bool PhaserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PhaserAudioProcessor::createEditor()
{
    //return new PhaserAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}



//==============================================================================
void PhaserAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PhaserAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}
void resizeVector(std::vector<PhaserBand> ph, int n) {
    ph.resize(n);
}
juce::AudioProcessorValueTreeState::ParameterLayout PhaserAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    using namespace juce;

    //std::vector < AudioParameterFloat* >bandParams;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;



    //layout.add(std::make_unique<AudioParameterFloat>("lowRate", "Rate", NormalisableRange<float>(0, 40, 0.01f, 1), 30));

    for (int i = 0; i < 3; ++i) {
        // Prefix para cada parámetro de banda
        const auto bandPrefix = juce::String("Band") + juce::String(i + 1) + juce::String(".");
        // Creamos los parámetros de la banda
        auto rate = std::make_unique<juce::AudioParameterFloat>(bandPrefix + "Rate", bandPrefix + "Rate", juce::NormalisableRange<float>(0, 5, 0.01f, 1), 1.0f);
        params.push_back(std::move(rate));
        auto depth = std::make_unique<AudioParameterFloat>(bandPrefix + "Depth", bandPrefix + "Depth", juce::NormalisableRange<float>(0, 1, 0.01f, 1), 0.5f);
        params.push_back(std::move(depth));

    }
    layout.add(params.begin(), params.end());

    /*
    layout.add(std::make_unique<AudioParameterFloat>("CentreFrequency", "CentreFrequency", NormalisableRange<float>(20, 20000, 1, 1), 1000));

    layout.add(std::make_unique<AudioParameterFloat>("Feedback", "Feedback", NormalisableRange<float>(0, 1, 0.01f, 1), 0.5f));
    layout.add(std::make_unique<AudioParameterFloat>("Mix", "Mix", NormalisableRange<float>(0, 1, 0.01f, 1), 0.5f));//dry/wet
    layout.add(std::make_unique<AudioParameterBool>("Bypass", "Bypass", false));
    */

    layout.add(std::make_unique<AudioParameterInt>("numBands", "Num bandas", 1, 10, 3));
    //actualizar aqui el tamaño
    //resizeVector(phasers, 0);

    const int numBands = 3; // Número de bandas del phaser
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> crossoverParameters; // Vector para almacenar los parámetros de frecuencia de corte

    for (int i = 0; i < numBands - 1; ++i)
    {
        // Genera el nombre único del parámetro para cada banda
        std::string parameterName = "Band" + std::to_string(i + 1) + ".freq";

        // Calcula de forma proporcional los valores de frecuencia de corte para cada banda
        float minFrequency = 20.0f * std::pow(10, i);
        float maxFrequency = 20.0f * std::pow(10, i + 1);
        float defaultFrequency = (minFrequency + maxFrequency) * 0.5f;


        // Crea el parámetro de frecuencia de corte y agrégalo al layout
        //juce::AudioParameterFloat* crossoverParameter = new juce::AudioParameterFloat(parameterName, parameterName, juce::NormalisableRange<float>(minFrequency, maxFrequency, 1.0f), defaultFrequency);
        auto freq = std::make_unique<juce::AudioParameterFloat>(parameterName, parameterName, juce::NormalisableRange<float>(minFrequency, maxFrequency, 1.0f), defaultFrequency);
        crossoverParameters.push_back(std::move(freq));
        // Agrega el parámetro al vector para su posterior uso

    }
    layout.add(crossoverParameters.begin(), crossoverParameters.end());


    //layout.add(std::make_unique<AudioParameterFloat>("Band1.freq", "Band1.freq", NormalisableRange<float>(20, 999, 1, 1), 400));
    //layout.add(std::make_unique<AudioParameterFloat>("Band2.freq", "Band2.freq", NormalisableRange<float>(1000, 20000, 1, 1), 2000));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PhaserAudioProcessor();
}
