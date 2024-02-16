# Audio Synthesizer: `wabisonorance`
|||
| - | - |
| Name | Nick Giampietro |
| Handle | `jnickg` |
| PSU Handle| `giampiet` |
|||

## Introduction
<img style="float: right; width:35%; margin: 10px 5%; border:solid white 10px" src="./docs/wabi-sabi-tea-bowl.jpg" alt="A handmade tea bowl"/>

This new synth, made using JUCE and intended to connect with a MIDI controller in a user's DAW, is designed to invoke the traditional Japanese aesthetics commonly bundled together in the West, and called 侘び寂び (_wabi-sabi_, literally "dreariness and loneliness," but often translated when discussing art closer to "austere, simple, imperfect, and impermanent."). This aesthetic often involves obviously hand-made items, flaws that are allowed to persist, signs of aging such as patina or wear-and-tear, and so forth.

While it's sometimes difficult to articulate, the family of aesthetics comprising _wabi-sabi_ can easily be recognized, as shown in the images in this document (including the album cover for my band's EP, below). [This video](https://youtu.be/5xaEaTRtiBo) (which has a decent automatic translation to English) explains it well without orientalist undertones—for example, around [6:00](https://www.youtube.com/watch?v=5xaEaTRtiBo#t=6m) the speaker discusses intentionally allowing objects to remain apparently worn by nature to maintain the _sabi_ aesthetic.

<img style="align: center; width: 40%; margin: 25px 30%; border:solid white 10px" src="./docs/TnT-Old-Walls.png" alt="Tens n Twenties - Old Walls We Built EP"/>

## _Wabi-Sabi_ in Sound

The _wabi-sabi_ aesthetic is often falsely equated with "old Japanese stuff," and thus is often conflated with music from feudal Japan such as the _koto_ or _shamisen_. While sometimes accurate (such as when the traditional music happens to be played slowly), this comparison fails to capture the essence of the aesthetic because traditional Japanese music is often busy and noisy. To better understand the aesthetic through sound, a better idea would be to find a _non_-Japanese example that captures its essential qualities.

<img style="float: left; width:35%; margin: 10px 5%; border: solid white 10px" src="./docs/ryoan-ji.jpg" alt="Ryōan-ji" />

In a non-Japanese context, _wabi-sabi_ can be found in projects such as David Whitling's ["I Need To Grow Away From These Roots"](https://www.vitling.xyz/i-need-to-grow-away-from-these-roots/). The isolated, slow-moving nature of each sound, ample use of negative space, along with the semi-random progression of notes and its visualization as a leafless tree, all build into an overall feel closely reminiscent of _wabi-sabi_.

Similarly, the some of the tracks in the [_Horizon Zero Dawn_](https://youtu.be/3wa3YtelPO4) OST, with their slow musical progressions, use of minor keys, atmosphereic feel, and integration with sounds from nature (which happens to be by audio production company [Wabi Sabi Sound](https://wabisabisound.com/our-work/horizon-zero-dawn/)) evokes a feeling quite similar to _wabi-sabi_. Chalk it up to the game's overall aesthetic of a crumpling world, where nature has overtaken society. Other music examples include sparse music like [_Het Holle Weg_](https://youtu.be/n_omGXOYpW4) by Jan Swerts (pretty much [anything](https://youtu.be/B7UXYYnHv6o) by Jan Swerts, really), [After End](https://youtu.be/sQ3e0vjZBV0) by Adam Hurst, music by [MEITEI](https://youtu.be/fi0XZnCQM_o), and anything by [Masakatsu Tagaki](https://youtu.be/i1zhDusTXzY).

## How `wabisonorance` Does It

To invoke the feel through sound, `wabisonorance` takes inspiration from Whitling's project, and produces a synth that, when a note is played, an echo is produced that moves through notes in [_Yona-nuki Onkai_](https://en.wikipedia.org/wiki/Japanese_mode) and similar modes/scales.

This synth is intended to be played with long, sustained notes, rather than an arpeggio or chord. It is slightly dissonant to invoke the feeling of imperfection, but musical enough to remain beautiful.

## Implementation

To make a synth I will make a plugin using JUCE, and support a few popular output formats for the plugin. The plugin will be a hand-tuned series of oscillators and filters, with some parameters exposed to change its sound, as well as adjustable ADSR, and one or more LFOs used to adjust the sound over time, as a kind of "breath" for each note played. Discrete plan for what oscillators & filters will be crafted during implementation.

It will be tesetd using Ardour as the primary DAW, and connected to an entry-level MIDI keyboard (Donner N-32).

As of the time of writing this proposal, this repository already has a basic sine-wave synthesizer with (hard-coded) ADSR support. This synth supports single-key input at this time.

## Considerations

* Parameters exposed to users should be thematic and intuitive, but not require extensive domain knowledge. More like a "Melancholy" slider, and less like a "Phase shift" slider.
* It is worth investigating tools like [Synplant2](https://soniccharge.com/synplant) to experiment with different synthesizers, and identify a general sound as the basis for WabiSonorance, and then figure out how to expose parameters.

## Open Questions

* What does _Yona-nuke Onkai_ look like on the circle of fifths?
* Generally, how do those concepts overlap and impact each other?
* What tools are there for crafting synthesized sounds using many oscillators, especially to reproduce certain real-world sounds?
* What tooling exists for, given some user-defined musical key, identifying how to make a chord using that note which fits into the key?
