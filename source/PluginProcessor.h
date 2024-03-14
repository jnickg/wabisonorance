#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include <cmath>
#include <unordered_map>
#include <memory>

#include "WabiSonoranceSynth.hpp"
#include "NotesKeys.hpp"

#if (MSVC)
#include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    juce::dsp::ProcessSpec spec;
    juce::dsp::Phaser<float> phaser;
    juce::dsp::Reverb::Parameters reverb_params;
    juce::dsp::Reverb reverb;
    juce::Synthesiser synth;

    jnickg::audio::key_info key {
        .root = jnickg::audio::note::A,
        .scale_type = jnickg::audio::scale::yonanuki,
    };
    double amplitude_modulation_lfo_frequency = 3.0;
    std::vector<float> amplitude_modulation_lfo;
    static inline constexpr size_t NUM_VOICES = 16;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
