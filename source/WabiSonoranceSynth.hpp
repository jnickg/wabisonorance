#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <cmath>
#include <unordered_map>
#include <memory>

namespace jnickg::audio::ws {

/**
 * @brief A sound for the WabiSonoranceSynth.
 */
class Sound : public juce::SynthesiserSound
{
public:
    Sound() {}
    ~Sound() override {}

    virtual bool appliesToNote (int midiNoteNumber) override {
        juce::ignoreUnused(midiNoteNumber);
        return true;
    }
    virtual bool appliesToChannel (int midiChannel) override {
        juce::ignoreUnused(midiChannel);
        return true;
    }
};

/**
 * @brief A voice for the WabiSonoranceSynth.
 */
class Voice : public juce::SynthesiserVoice
{
public:
    Voice() {}
    ~Voice() override {}

    virtual bool canPlaySound (juce::SynthesiserSound* s) override {
        auto* sound = dynamic_cast<Sound*>(s);
        return sound != nullptr;
    }

    virtual void startNote (
        int midiNoteNumber,
        float velocity,
        juce::SynthesiserSound* sound,
        int currentPitchWheelPosition) override;

    virtual void stopNote (float velocity, bool allowTailOff) override;

    virtual void pitchWheelMoved (int newPitchWheelValue) override;

    virtual void controllerMoved (int controllerNumber, int newControllerValue) override;

    virtual void renderNextBlock (
        juce::AudioBuffer<float>& outputBuffer,
        int startSample,
        int numSamples) override;

    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);

private:
    using osc_t = juce::dsp::Oscillator<float>;
    enum class OscillatorType
    {
        Sine,
        Saw,
        Square,
        Triangle
    };

    OscillatorType selected_osc { OscillatorType::Sine };

    std::unordered_map<OscillatorType, std::shared_ptr<osc_t>> oscillators = {
        { OscillatorType::Sine, std::make_shared<osc_t>([](float t){ return std::sin(t); })},
        { OscillatorType::Saw, std::make_shared<osc_t>([](float t){ return t / juce::MathConstants<float>::pi; })},
        { OscillatorType::Square, std::make_shared<osc_t>([](float t){ return t < 0.0f ? -1.0f : 1.0f; })},
        { OscillatorType::Triangle, std::make_shared<osc_t>( [](float t){ return 2.0f * std::abs(t - std::round(t)) - 1.0f; }) }
    };
    juce::dsp::Gain<float> gain;
    juce::ADSR adsr;

    bool isPrepared { false };
};

} // namespace jnickg::audio::ws
