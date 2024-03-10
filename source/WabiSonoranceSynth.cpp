#include "WabiSonoranceSynth.hpp"

#include "NotesKeys.hpp"

#include <algorithm>

namespace jnickg::audio::ws {

void Voice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) {
    auto* s = dynamic_cast<Sound*>(sound);
    if (s == nullptr) {
        return;
    }

    note_info n(midiNoteNumber);

    auto chords_in_key = get_chords(n, key, true);
    auto found_chord = true;
    if (chords_in_key.empty()) {
        auto& unison = chords_in_key.emplace_back();
        unison.root = n;
        unison.chord_type = chord::_unison;
        unison.inv = inversion::root;
        found_chord = false;
    }
    auto idx = static_cast<size_t>(std::rand() % static_cast<int>(chords_in_key.size()));
    jnickg::audio::chord_info& current_chord = chords_in_key[idx];
    auto chord_str = current_chord.to_string(true);
    printf("Note %s -> Playing chord: %s%s\n", n.to_string().c_str(), chord_str.c_str(), !found_chord ? " (fallback)" : "");
    auto midi_notes = current_chord.get_midi_notes();

    std::vector<double> new_bases;
    for (auto note : midi_notes) {
        new_bases.push_back(juce::MidiMessage::getMidiNoteInHertz(note));
    }
    auto bend = this->pitch_wheel_pos_to_bend_factor(currentPitchWheelPosition);
    this->update_pitches(new_bases, bend);

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
    this->update_pitches(std::nullopt, bend);
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

    for (int sample = startSample; sample < startSample + numSamples; ++sample) {
        for (size_t i = 0; i < this->chord_bases.size(); ++i) {
            auto osc_sample = this->chord_oscillators[i]->processSample(0.0f);
            osc_sample *= this->adsr.getNextSample();
            osc_sample *= this->gain.getGainLinear();
            osc_sample = this->filter.processSample(osc_sample);
            outputBuffer.addSample(0, sample, osc_sample);
        }
    }
}

void Voice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels, float bpm) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(outputChannels);

    for (auto& osc : this->chord_oscillators) {
        osc->prepare(spec);
    }

    this->gain.prepare(spec);
    this->gain.setGainLinear(0.1f); // TODO parameterize

    this->filter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 500.0f); // TODO parameterize
    this->filter.reset();
    this->filter.prepare(spec);

    this->adsr.setSampleRate(sampleRate);
    juce::ADSR::Parameters params;
    params.attack = DEFAULT_ATTACK;
    params.decay = DEFAULT_DECAY;
    params.sustain = DEFAULT_SUSTAIN;
    params.release = DEFAULT_RELEASE;
    this->adsr.setParameters(params);

    this->_bpm = bpm;

    //
    //
    //

    this->isPrepared = true;
}

} // namespace jnickg::audio::ws
