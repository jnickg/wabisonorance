#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include <algorithm>
#include <cmath>
#include <optional>
#include <unordered_map>
#include <memory>

#include "NotesKeys.hpp"

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
    jnickg::audio::key_info& key;
public:
    inline static const float DEFAULT_ATTACK { 2.0f };
    inline static const float DEFAULT_DECAY { 1.0f };
    inline static const float DEFAULT_SUSTAIN { 0.8f };
    inline static const float DEFAULT_RELEASE { 4.0f };

    Voice(jnickg::audio::key_info& k)
        : key(k)
        , filter()
    {
        // no-op
    }
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

    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels, float bpm);

private:
    using osc_t = juce::dsp::Oscillator<float>;
    enum class OscillatorType
    {
        Sine,
        Saw,
        Square,
        Triangle,
        SineWithHarmonics
    };

    OscillatorType selected_osc { OscillatorType::SineWithHarmonics };

    float clip { 0.6f }; ///< Pre-gain clipping value for the waveform.

    float sine_wave_with_harmonics(float t, std::vector<float> harmonic_weights) const {
        auto val = 0.0f;
        // We assume harmonic weights add to one and DO NOT CHECK THAT HERE
        for (size_t i = 0; i < harmonic_weights.size(); ++i) {
            float factor = std::pow(2.0f, static_cast<float>(i));
            val += harmonic_weights[i] * std::sin(factor * t);
        }
        return std::clamp(val, -clip, clip);
    }

    float sine_wave(float t) const{
        auto val = std::sin(t);
        return std::clamp(val, -clip, clip);
    }

    float saw_wave(float t) const{
        auto val = t / juce::MathConstants<float>::pi;
        return std::clamp(val, -clip, clip);
    }

    float square_wave(float t) const{
        auto val = t < 0.0f ? -1.0f : 1.0f;
        return std::clamp(val, -clip, clip);
    }

    float triangle_wave(float t) const{
        auto val = 2.0f * std::abs(t - std::round(t)) - 1.0f;
        return std::clamp(val, -clip, clip);
    }

    float custom_wave(float t) const {
        switch(selected_osc) {
            case OscillatorType::Sine:
                return this->sine_wave(t);
            case OscillatorType::SineWithHarmonics:
                return this->sine_wave_with_harmonics(t, {0.8f, 0.16f, 0.032f, 0.008f});
            case OscillatorType::Saw:
                return this->saw_wave(t);
            case OscillatorType::Square:
                return this->square_wave(t);
            case OscillatorType::Triangle:
                return this->triangle_wave(t);
            default:
                return 0.0f;
        }
    }

    std::vector<std::shared_ptr<osc_t>> chord_oscillators = {
        std::make_shared<osc_t>([this](float t){ return this->custom_wave(t); }),
        std::make_shared<osc_t>([this](float t){ return this->custom_wave(t); }),
        std::make_shared<osc_t>([this](float t){ return this->custom_wave(t); }),
        std::make_shared<osc_t>([this](float t){ return this->custom_wave(t); }),
        std::make_shared<osc_t>([this](float t){ return this->custom_wave(t); }),
        std::make_shared<osc_t>([this](float t){ return this->custom_wave(t); }),
        std::make_shared<osc_t>([this](float t){ return this->custom_wave(t); }),
        std::make_shared<osc_t>([this](float t){ return this->custom_wave(t); }),
        std::make_shared<osc_t>([this](float t){ return this->custom_wave(t); }),
        std::make_shared<osc_t>([this](float t){ return this->custom_wave(t); }),
        std::make_shared<osc_t>([this](float t){ return this->custom_wave(t); })
    };

    std::vector<double> chord_bases;

    juce::dsp::IIR::Filter<float> filter;
    juce::dsp::Gain<float> gain;
    juce::ADSR adsr;

    double pitch_bend { 1.0 }; ///< Factor by which to bend the pitch.
    float _bpm { 120.0f };

    bool isPrepared { false };
    bool isActive { false };

    inline bool is_active() const {
        return this->isActive || this->adsr.isActive();
    }

    void update_pitches(std::optional<std::vector<double>> bases = std::nullopt, std::optional<double> bend = std::nullopt) {
        if (bases) {
            this->chord_bases = *bases;
        }
        if (bend) {
            this->pitch_bend = *bend;
        }
        for (size_t i = 0; i < this->chord_oscillators.size(); ++i) {
            auto freq = this->chord_bases[i] * this->pitch_bend;
            this->chord_oscillators[i]->setFrequency(static_cast<float>(freq));
        }
    }

    inline double pitch_wheel_pos_to_bend_factor(int pos) const {
        // pitch wheel value is a 14-bit value, allowing for 16,384 possible values
        // We want to map this to a range of +- 2 semitones
        auto semitones = 2.0 *  std::clamp(static_cast<double>(pos) / 16384.0, -1.0, 1.0);
        auto bend = std::pow(2.0, semitones / 12.0);
        return bend;
    }

    inline float velocity_to_attack(float velocity) const {
        // velocity is a 0.0-1.0 value, where 1.0 means "fast". Most in reality, most velocity
        // values are between .3 and .6. Attack is a value where lower values mean quicker attack.
        // We want to spread out the central range of velocities, and especially exaggerate the
        // extremes, and map that to attack values ranging from 0.01 to 0.99.
        velocity += 0.001f; // avoid divide by zero
        float attack = 1.0f / std::sqrt(velocity);
        return std::clamp(attack, 0.1f, DEFAULT_ATTACK);
    }

    inline float velocity_to_release(float velocity) const {
        velocity += 0.001f; // avoid divide by zero
        float release = 1.0f / std::sqrt(velocity);
        return std::clamp(release, 0.1f, DEFAULT_RELEASE);
    }
};

} // namespace jnickg::audio::ws
