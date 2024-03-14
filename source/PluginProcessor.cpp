#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "NotesKeys.hpp"

#include <juce_dsp/juce_dsp.h>

#include <cmath>

//==============================================================================
PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    printf("Synth initialized in key: %s\n", key.to_string(true).c_str());
    for (size_t i = 0; i < NUM_VOICES; i++) {
        auto* v = synth.addVoice(new jnickg::audio::ws::Voice(key));
        if (v == nullptr) {
            throw std::runtime_error("Failed to add voice to synth");
        }
    }
    auto* s = synth.addSound(new jnickg::audio::ws::Sound());
    if (s == nullptr) {
        throw std::runtime_error("Failed to add sound to synth");
    }

    jnickg::audio::init_chords();
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto outputChannels = this->getTotalNumOutputChannels();
    this->spec.sampleRate = sampleRate;
    this->spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    this->spec.numChannels = static_cast<juce::uint32>(outputChannels);

    this->synth.setCurrentPlaybackSampleRate(sampleRate);

    float bpm = 120.0f; // TODO parameterize... and actually use this

    for (auto i = 0; i < this->synth.getNumVoices(); ++i) {
        auto* voice = dynamic_cast<jnickg::audio::ws::Voice*>(this->synth.getVoice(i));
        if (voice != nullptr) {
            voice->prepareToPlay(sampleRate, samplesPerBlock, outputChannels, bpm);
        }
    }

    auto sample_rate = this->spec.sampleRate;
    auto lfo_frequency = this->amplitude_modulation_lfo_frequency;
    auto lfo_period = 1.0 / lfo_frequency;
    auto lfo_samples = static_cast<size_t>(lfo_period * sample_rate);
    amplitude_modulation_lfo.clear();
    amplitude_modulation_lfo.resize(lfo_samples);
    // printf("lfo_samples: %zu\n\tvalues: ", lfo_samples);
    for (size_t i = 0; i < lfo_samples; i++) {
        amplitude_modulation_lfo[i] = static_cast<float>(std::sin(2.0 * M_PI * lfo_frequency * static_cast<double>(i) / sample_rate));
        // printf("%f ", amplitude_modulation_lfo[i]);
    }
    // printf("\n");

    this->phaser.setRate(0.5f); // TODO parameterize
    this->phaser.setDepth(0.7f); // TODO parameterize
    this->phaser.setCentreFrequency(0.56f); // TODO parameterize
    this->phaser.setFeedback(1.0f); // TODO parameterize
    this->phaser.setMix(0.7f); // TODO parameterize
    this->phaser.prepare(this->spec);

    this->reverb_params.roomSize = 0.9f; // TODO parameterize
    this->reverb_params.damping = 0.35f; // TODO parameterize
    this->reverb_params.wetLevel = 0.7f; // TODO parameterize
    this->reverb_params.dryLevel = 0.5f; // TODO parameterize
    this->reverb_params.width = 0.5f; // TODO parameterize
    this->reverb_params.freezeMode = false;
    this->reverb.setParameters(this->reverb_params);
    this->reverb.prepare(this->spec);
    this->reverb.reset();
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages); // TODO: use midiMessages to get notes

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

    // Update all voices with the current parameters
    for (auto i = 0; i < this->synth.getNumVoices(); ++i) {
        auto* voice = dynamic_cast<jnickg::audio::ws::Voice*>(this->synth.getVoice(i));
        if (voice != nullptr) {
            // Oscillator controlls
            // ADSR
            // LFO
            // Filter
            // Gain
        }
    }

    this->synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // Apple post FX
    juce::dsp::AudioBlock<float> block(buffer);
    // Apply phaser
    this->phaser.process(juce::dsp::ProcessContextReplacing<float>(block));

    // Apply amplitude modulation. Using amplitude_modulation_lfo_frequency and
    // spec.sampleRate, calculate the amplitude modulation signal and apply it
    // to the buffer.
    // This sounds like crap so let's leave it out :-)
    // auto num_samples = buffer.getNumSamples();
    // auto num_channels = buffer.getNumChannels();
    // auto lfo_samples = this->amplitude_modulation_lfo.size();
    // for (int i = 0; i < num_samples; i++) {
    //     auto lfo_i = static_cast<size_t>(i) % lfo_samples;
    //     for (int j = 0; j < num_channels; j++) {
    //         buffer.setSample(j, i, buffer.getSample(j, i) * this->amplitude_modulation_lfo[lfo_i]);
    //     }
    // }

    // Apply reverb
    this->reverb.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
