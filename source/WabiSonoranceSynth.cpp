#include "WabiSonoranceSynth.hpp"

#include "NotesKeys.hpp"

#include <algorithm>

namespace jnickg::audio::ws {

void Voice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) {
    auto* s = dynamic_cast<Sound*>(sound);
    if (s == nullptr) {
        return;
    }

    auto base = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    auto bend = this->pitch_wheel_pos_to_bend_factor(currentPitchWheelPosition);
    this->update_pitch(base, bend);

    auto params = this->adsr.getParameters();
    params.attack = velocity_to_attack(velocity);
    params.release = params.attack;
    this->adsr.setParameters(params);

    this->adsr.noteOn();
    this->isActive = true;
}

void Voice::stopNote (float velocity, bool allowTailOff) {
    juce::ignoreUnused(velocity);
    juce::ignoreUnused(allowTailOff);

    this->adsr.noteOff();
    this->isActive = false;
}

void Voice::pitchWheelMoved (int newPitchWheelValue) {
    auto bend = this->pitch_wheel_pos_to_bend_factor(newPitchWheelValue);
    this->update_pitch(std::nullopt, bend);
}

void Voice::controllerMoved (int controllerNumber, int newControllerValue) {
    juce::ignoreUnused(controllerNumber);
    juce::ignoreUnused(newControllerValue);
}

void Voice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
    jassert(this->isPrepared);

    if (numSamples == 0) {
        return;
    }

    if (!this->is_active()) {
        return;
    }

    using replace_t = juce::dsp::ProcessContextReplacing<float>;
    using block_t = juce::dsp::AudioBlock<float>;
    block_t block { outputBuffer };
    const auto osc = this->oscillators[selected_osc];
    osc->process(replace_t(block));
    this->adsr.applyEnvelopeToBuffer(outputBuffer, startSample, numSamples);
    this->gain.process(replace_t(block));
}

void Voice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(outputChannels);

    for (auto& osc : this->oscillators) {
        osc.second->prepare(spec);
    }

    this->gain.prepare(spec);
    this->gain.setGainLinear(0.1f); // TODO parameterize

    this->adsr.setSampleRate(sampleRate);
    juce::ADSR::Parameters params;
    params.attack = DEFAULT_ATTACK;
    params.decay = DEFAULT_DECAY;
    params.sustain = DEFAULT_SUSTAIN;
    params.release = DEFAULT_RELEASE;
    this->adsr.setParameters(params);

    //
    //
    //

    this->isPrepared = true;
}

} // namespace jnickg::audio::ws
