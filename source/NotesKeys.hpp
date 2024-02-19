#pragma once

#include <cstdint>
#include <array>
#include <tuple>
#include <vector>

namespace jnickg::audio
{
    enum class note : uint8_t {
        C = 0,

        Csharp = 1,
        Db = 1,

        D = 2,

        Dsharp = 3,
        Eb = 3,

        E = 4,

        F = 5,

        Fsharp = 6,
        Gb = 6,

        G = 7,

        Gsharp = 8,
        Ab = 8,

        A = 9,

        Asharp = 10,
        Bb = 10,

        B = 11
    };

    enum class chord {
        _maj,
        _min,
        _7,
        _m7,
        _maj7,
        _dim,
        _dim7,
        _7_flat_5,
        _7_sharp_5,
        _m7_flat_5,
        _7_flat_9,
        _6,
        _m6,
        _6add9,
        _9,
        _m9,
        _maj9,
        _add9,
        _11,
        _m11,
        _13,
        _sus2,
        _sus4,
        _aug
    };

    enum class inversion {
        none,
        first,
        second,
        third
    };

    /**
     * @brief Generates a chord for the given root note, with the given parameters
     * 
     * @param root The root note of the chord. If inverted, this may not be the first note.
     * @param chord_type The type of chord to generate
     * @param inv The inversion of the chord
     * @param octave The octave of the root note, which defaults to "middle" or 4.
     */
    inline constexpr auto generate_chord(note root, chord chord_type, inversion inv = inversion::none, int octave = 4) {
        

    }

} // namespace jnickg::audio