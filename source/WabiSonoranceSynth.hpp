#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include <algorithm>
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

    OscillatorType selected_osc { OscillatorType::Triangle };

    float clip { 0.6f }; ///< Pre-gain clipping value for the waveform.

    float sine_wave(float t) {
        auto val = std::sin(t);
        return std::clamp(val, -clip, clip);
    }

    float saw_wave(float t) {
        auto val = t / juce::MathConstants<float>::pi;
        return std::clamp(val, -clip, clip);
    }

    float square_wave(float t) {
        auto val = t < 0.0f ? -1.0f : 1.0f;
        return std::clamp(val, -clip, clip);
    }

    float triangle_wave(float t) {
        auto val = 2.0f * std::abs(t - std::round(t)) - 1.0f;
        return std::clamp(val, -clip, clip);
    }

    const size_t NUM_LUT_SAMPLES = 1200;

    std::unordered_map<OscillatorType, std::shared_ptr<osc_t>> oscillators = {
        { OscillatorType::Sine, std::make_shared<osc_t>([this](float t){ return this->sine_wave(t); }, NUM_LUT_SAMPLES)},
        { OscillatorType::Saw, std::make_shared<osc_t>([this](float t){ return this->saw_wave(t); }, NUM_LUT_SAMPLES)},
        { OscillatorType::Square, std::make_shared<osc_t>([this](float t){ return this->square_wave(t); }, NUM_LUT_SAMPLES)},
        { OscillatorType::Triangle, std::make_shared<osc_t>( [this](float t){ return this->triangle_wave(t); }, NUM_LUT_SAMPLES)}
    };
    juce::dsp::Gain<float> gain;
    juce::ADSR adsr;

    bool isPrepared { false };
};

} // namespace jnickg::audio::ws
