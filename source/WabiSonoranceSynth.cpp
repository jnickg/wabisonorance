#include "WabiSonoranceSynth.hpp"

namespace jnickg::audio::ws {

void Voice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) {
    juce::ignoreUnused(midiNoteNumber);
    juce::ignoreUnused(velocity);
    juce::ignoreUnused(sound);
    juce::ignoreUnused(currentPitchWheelPosition);

    auto freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    for (auto& osc : this->oscillators) {
        osc.second->setFrequency(static_cast<float>(freq));
    }
    this->adsr.noteOn();
}

void Voice::stopNote (float velocity, bool allowTailOff) {
    juce::ignoreUnused(velocity);
    juce::ignoreUnused(allowTailOff);

    this->adsr.noteOff();
}

void Voice::pitchWheelMoved (int newPitchWheelValue) {
    juce::ignoreUnused(newPitchWheelValue);
}

void Voice::controllerMoved (int controllerNumber, int newControllerValue) {
    juce::ignoreUnused(controllerNumber);
    juce::ignoreUnused(newControllerValue);
}

void Voice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
    jassert(this->isPrepared);
    juce::ignoreUnused(startSample);
    juce::ignoreUnused(numSamples);

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
    params.attack = 0.1f;
    params.decay = 0.1f;
    params.sustain = 0.8f;
    params.release = 0.1f;
    this->adsr.setParameters(params);

    //
    //
    //

    this->isPrepared = true;
}

} // namespace jnickg::audio::ws
