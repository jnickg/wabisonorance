#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace jnickg::audio {

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

struct note_info {
    note n { note::C };
    int octave { 4 };

    int to_midi() const {
        return static_cast<int>(this->n) + (12 * this->octave);
    }

    static note_info from_midi(int midi) {
        note_info info;
        info.n = static_cast<note>(midi % 12);
        info.octave = midi / 12;
        return info;
    }

    inline bool operator==(const note_info& other) const {
        return to_midi() == other.to_midi();
    }

    inline bool operator<(const note_info& other) const {
        return to_midi() < other.to_midi();
    }

    std::string to_string() const {
        std::string note_str;
        switch (this->n) {
            case note::C: note_str = "C"; break;
            case note::Csharp: note_str = "C#"; break;
            case note::D: note_str = "D"; break;
            case note::Dsharp: note_str = "D#"; break;
            case note::E: note_str = "E"; break;
            case note::F: note_str = "F"; break;
            case note::Fsharp: note_str = "F#"; break;
            case note::G: note_str = "G"; break;
            case note::Gsharp: note_str = "G#"; break;
            case note::A: note_str = "A"; break;
            case note::Asharp: note_str = "A#"; break;
            case note::B: note_str = "B"; break;
        }
        return note_str + std::to_string(this->octave);
    }
};

enum class chord {
    __FIRST = 0,
    _unison = __FIRST,
    _b2,
    _2,
    _b3,
    _3,
    _4,
    _b5,
    _5,
    _b6,
    _6,
    _b7,
    _7,
    _8va,
    _b9,
    _9,
    _b10,
    _10,
    _11,
    _b12,
    _12,
    _b13,
    _13,
    _b14,
    _14,
    _15ma,
    _maj,
    _min,
    _sus2,
    _sus4,
    _dim,
    _aug,
    _maj7,
    _min7,
    _dom7,
    _min7_b5,
    _half_dim7,
    _dim7,
    _full_dim7,
    _min_maj7,
    _maj7_sus2,
    _maj7_sus4,
    _min7_sus2,
    _min7_sus4,
    _maj6,
    _min6,
    _maj_add_b9,
    _maj_add_9,
    _maj_add_sharp9,
    _maj_add_b10,
    _maj_add_11,
    _maj_add_sharp11,
    _maj_add_b12,
    _maj_add_13,
    _maj_add_sharp13,
    _maj_add_b14,
    _min_add_b9,
    _min_add_9,
    _min_add_sharp9,
    _min_add_b10,
    _min_add_11,
    _min_add_sharp11,
    _min_add_b12,
    _min_add_13,
    _min_add_sharp13,
    _min_add_b14,
    _sus2_add_9,
    _sus2_add_sharp9,
    _sus2_add_b10,
    _sus2_add_11,
    _sus2_add_sharp11,
    _sus2_add_b12,
    _sus2_add_13,
    _sus2_add_sharp13,
    _sus2_add_b14,
    _sus4_add_9,
    _sus4_add_sharp9,
    _sus4_add_b10,
    _sus4_add_11,
    _sus4_add_sharp11,
    _sus4_add_b12,
    _sus4_add_13,
    _sus4_add_sharp13,
    _sus4_add_b14,
    _maj9,
    _min9,
    _dom9,
    _min9_b5,
    _maj11,
    _min11,
    _maj13,
    _min13,
    _dom11,
    _dom13,
    __COUNT
};

inline std::string to_string(chord c) {
    switch (c) {
        case chord::_unison               : return "Unison";
        case chord::_b2                   : return "b2";
        case chord::_2                    : return "2";
        case chord::_b3                   : return "b3";
        case chord::_3                    : return "3";
        case chord::_4                    : return "4";
        case chord::_b5                   : return "b5";
        case chord::_5                    : return "5";
        case chord::_b6                   : return "b6";
        case chord::_6                    : return "6";
        case chord::_b7                   : return "b7";
        case chord::_7                    : return "7";
        case chord::_8va                  : return "8va";
        case chord::_b9                   : return "b9";
        case chord::_9                    : return "9";
        case chord::_b10                  : return "b10";
        case chord::_10                   : return "10";
        case chord::_11                   : return "11";
        case chord::_b12                  : return "b12";
        case chord::_12                   : return "12";
        case chord::_b13                  : return "b13";
        case chord::_13                   : return "13";
        case chord::_b14                  : return "b14";
        case chord::_14                   : return "14";
        case chord::_15ma                 : return "15ma";
        case chord::_maj                  : return "maj";
        case chord::_min                  : return "min";
        case chord::_sus2                 : return "sus2";
        case chord::_sus4                 : return "sus4";
        case chord::_dim                  : return "dim";
        case chord::_aug                  : return "aug";
        case chord::_maj7                 : return "maj7";
        case chord::_min7                 : return "min7";
        case chord::_dom7                 : return "dom7";
        case chord::_min7_b5              : return "min7_b5";
        case chord::_half_dim7            : return "half_dim7";
        case chord::_dim7                 : return "dim7";
        case chord::_full_dim7            : return "full_dim7";
        case chord::_min_maj7             : return "min_maj7";
        case chord::_maj7_sus2            : return "maj7_sus2";
        case chord::_maj7_sus4            : return "maj7_sus4";
        case chord::_min7_sus2            : return "min7_sus2";
        case chord::_min7_sus4            : return "min7_sus4";
        case chord::_maj6                 : return "maj6";
        case chord::_min6                 : return "min6";
        case chord::_maj_add_b9           : return "maj_add_b9";
        case chord::_maj_add_9            : return "maj_add_9";
        case chord::_maj_add_sharp9       : return "maj_add_sharp9";
        case chord::_maj_add_b10          : return "maj_add_b10";
        case chord::_maj_add_11           : return "maj_add_11";
        case chord::_maj_add_sharp11      : return "maj_add_sharp11";
        case chord::_maj_add_b12          : return "maj_add_b12";
        case chord::_maj_add_13           : return "maj_add_13";
        case chord::_maj_add_sharp13      : return "maj_add_sharp13";
        case chord::_maj_add_b14          : return "maj_add_b14";
        case chord::_min_add_b9           : return "min_add_b9";
        case chord::_min_add_9            : return "min_add_9";
        case chord::_min_add_sharp9       : return "min_add_sharp9";
        case chord::_min_add_b10          : return "min_add_b10";
        case chord::_min_add_11           : return "min_add_11";
        case chord::_min_add_sharp11      : return "min_add_sharp11";
        case chord::_min_add_b12          : return "min_add_b12";
        case chord::_min_add_13           : return "min_add_13";
        case chord::_min_add_sharp13      : return "min_add_sharp13";
        case chord::_min_add_b14          : return "min_add_b14";
        case chord::_sus2_add_9           : return "sus2_add_9";
        case chord::_sus2_add_sharp9      : return "sus2_add_sharp9";
        case chord::_sus2_add_b10         : return "sus2_add_b10";
        case chord::_sus2_add_11          : return "sus2_add_11";
        case chord::_sus2_add_sharp11     : return "sus2_add_sharp11";
        case chord::_sus2_add_b12         : return "sus2_add_b12";
        case chord::_sus2_add_13          : return "sus2_add_13";
        case chord::_sus2_add_sharp13     : return "sus2_add_sharp13";
        case chord::_sus2_add_b14         : return "sus2_add_b14";
        case chord::_sus4_add_9           : return "sus4_add_9";
        case chord::_sus4_add_sharp9      : return "sus4_add_sharp9";
        case chord::_sus4_add_b10         : return "sus4_add_b10";
        case chord::_sus4_add_11          : return "sus4_add_11";
        case chord::_sus4_add_sharp11     : return "sus4_add_sharp11";
        case chord::_sus4_add_b12         : return "sus4_add_b12";
        case chord::_sus4_add_13          : return "sus4_add_13";
        case chord::_sus4_add_sharp13     : return "sus4_add_sharp13";
        case chord::_sus4_add_b14         : return "sus4_add_b14";
        case chord::_maj9                 : return "maj9";
        case chord::_min9                 : return "min9";
        case chord::_dom9                 : return "dom9";
        case chord::_min9_b5              : return "min9_b5";
        case chord::_maj11                : return "maj11";
        case chord::_min11                : return "min11";
        case chord::_maj13                : return "maj13";
        case chord::_min13                : return "min13";
        case chord::_dom11                : return "dom11";
        case chord::_dom13                : return "dom13";
        case chord::__COUNT:
        default: throw std::runtime_error("Invalid chord");
    }
}

inline std::vector<int> get_intervals(chord c) {
    switch (c) {
        case chord::_unison               : return { 0,0 };
        case chord::_b2                   : return { 0,1 };
        case chord::_2                    : return { 0,2 };
        case chord::_b3                   : return { 0,3 };
        case chord::_3                    : return { 0,4 };
        case chord::_4                    : return { 0,5 };
        case chord::_b5                   : return { 0,6 };
        case chord::_5                    : return { 0,7 };
        case chord::_b6                   : return { 0,8 };
        case chord::_6                    : return { 0,9 };
        case chord::_b7                   : return { 0,10 };
        case chord::_7                    : return { 0,11 };
        case chord::_8va                  : return { 0,12 };
        case chord::_b9                   : return { 0,13 };
        case chord::_9                    : return { 0,14 };
        case chord::_b10                  : return { 0,15 };
        case chord::_10                   : return { 0,16 };
        case chord::_11                   : return { 0,17 };
        case chord::_b12                  : return { 0,18 };
        case chord::_12                   : return { 0,19 };
        case chord::_b13                  : return { 0,20 };
        case chord::_13                   : return { 0,21 };
        case chord::_b14                  : return { 0,22 };
        case chord::_14                   : return { 0,23 };
        case chord::_15ma                 : return { 0,24 };
        case chord::_maj                  : return { 0,4,7 };
        case chord::_min                  : return { 0,3,7 };
        case chord::_sus2                 : return { 0,2,7 };
        case chord::_sus4                 : return { 0,5,7 };
        case chord::_dim                  : return { 0,3,6 };
        case chord::_aug                  : return { 0,4,8 };
        case chord::_maj7                 : return { 0,4,7,11 };
        case chord::_min7                 : return { 0,3,7,10 };
        case chord::_dom7                 : return { 0,4,7,10 };
        case chord::_min7_b5              : return { 0,3,6,10 };
        case chord::_half_dim7            : return { 0,3,6,10 };
        case chord::_dim7                 : return { 0,3,6,9 };
        case chord::_full_dim7            : return { 0,3,6,9 };
        case chord::_min_maj7             : return { 0,3,7,11 };
        case chord::_maj7_sus2            : return { 0,2,7,11 };
        case chord::_maj7_sus4            : return { 0,5,7,11 };
        case chord::_min7_sus2            : return { 0,2,7,10 };
        case chord::_min7_sus4            : return { 0,5,7,10 };
        case chord::_maj6                 : return { 0,4,7,9 };
        case chord::_min6                 : return { 0,3,7,9 };
        case chord::_maj_add_b9           : return { 0,4,7,13 };
        case chord::_maj_add_9            : return { 0,4,7,14 };
        case chord::_maj_add_sharp9       : return { 0,4,7,15 };
        case chord::_maj_add_b10          : return { 0,4,7,15 };
        case chord::_maj_add_11           : return { 0,4,7,17 };
        case chord::_maj_add_sharp11      : return { 0,4,7,18 };
        case chord::_maj_add_b12          : return { 0,4,7,18 };
        case chord::_maj_add_13           : return { 0,4,7,21 };
        case chord::_maj_add_sharp13      : return { 0,4,7,22 };
        case chord::_maj_add_b14          : return { 0,4,7,22 };
        case chord::_min_add_b9           : return { 0,3,7,13 };
        case chord::_min_add_9            : return { 0,3,7,14 };
        case chord::_min_add_sharp9       : return { 0,3,7,15 };
        case chord::_min_add_b10          : return { 0,3,7,15 };
        case chord::_min_add_11           : return { 0,3,7,17 };
        case chord::_min_add_sharp11      : return { 0,3,7,18 };
        case chord::_min_add_b12          : return { 0,3,7,18 };
        case chord::_min_add_13           : return { 0,3,7,21 };
        case chord::_min_add_sharp13      : return { 0,3,7,22 };
        case chord::_min_add_b14          : return { 0,3,7,22 };
        case chord::_sus2_add_9           : return { 0,2,7,14 };
        case chord::_sus2_add_sharp9      : return { 0,2,7,15 };
        case chord::_sus2_add_b10         : return { 0,2,7,15 };
        case chord::_sus2_add_11          : return { 0,2,7,17 };
        case chord::_sus2_add_sharp11     : return { 0,2,7,18 };
        case chord::_sus2_add_b12         : return { 0,2,7,18 };
        case chord::_sus2_add_13          : return { 0,2,7,21 };
        case chord::_sus2_add_sharp13     : return { 0,2,7,22 };
        case chord::_sus2_add_b14         : return { 0,2,7,22 };
        case chord::_sus4_add_9           : return { 0,5,7,14 };
        case chord::_sus4_add_sharp9      : return { 0,5,7,15 };
        case chord::_sus4_add_b10         : return { 0,5,7,15 };
        case chord::_sus4_add_11          : return { 0,5,7,17 };
        case chord::_sus4_add_sharp11     : return { 0,5,7,18 };
        case chord::_sus4_add_b12         : return { 0,5,7,18 };
        case chord::_sus4_add_13          : return { 0,5,7,21 };
        case chord::_sus4_add_sharp13     : return { 0,5,7,22 };
        case chord::_sus4_add_b14         : return { 0,5,7,22 };
        case chord::_maj9                 : return { 0,4,7,11,14 };
        case chord::_min9                 : return { 0,3,7,10,14 };
        case chord::_dom9                 : return { 0,4,7,10,14 };
        case chord::_min9_b5              : return { 0,3,6,10,14 };
        case chord::_maj11                : return { 0,4,11,14,18 };
        case chord::_min11                : return { 0,3,10,14,17 };
        case chord::_maj13                : return { 0,4,11,14,21 };
        case chord::_min13                : return { 0,3,10,14,21 };
        case chord::_dom11                : return { 0,4,10,14,18 };
        case chord::_dom13                : return { 0,4,10,14,21 };
        case chord::__COUNT:
        default: throw std::runtime_error("Invalid chord");
    }
}

enum class inversion {
    __FIRST = 0,
    root = __FIRST,
    first = 1,
    second = 2,
    third = 3,
    fourth = 4,
    __COUNT
};

inline std::string to_string(inversion inv) {
    switch (inv) {
        case inversion::root: return "";
        case inversion::first: return "inv";
        case inversion::second: return "2nd inv";
        case inversion::third: return "3rd inv";
        case inversion::fourth: return "4th inv";
        case inversion::__COUNT:
        default: throw std::runtime_error("Invalid inversion");
    }
}

inline std::vector<int> invert_dyad(std::vector<int> chord, inversion inv) {
    int inversion_amt = static_cast<int>(inv) % 2;
    switch (inversion_amt) {
        case 0: return chord;
        case 1: return { chord[1], chord[0] + 12 };
        default: throw std::runtime_error("Invalid inversion");
    }
}

inline std::vector<int> invert_triad(std::vector<int> chord, inversion inv) {
    int inversion_amt = static_cast<int>(inv) % 3;
    switch (inversion_amt) {
        case 0: return chord;
        case 1: return { chord[1], chord[2], chord[0] + 12 };
        case 2: return { chord[2], chord[0] + 12, chord[1] + 12 };
        default: throw std::runtime_error("Invalid inversion");
    }
}

inline std::vector<int> invert_quad(std::vector<int> chord, inversion inv) {
    int inversion_amt = static_cast<int>(inv) % 4;
    switch (inversion_amt) {
        case 0: return chord;
        case 1: return { chord[1], chord[2], chord[3], chord[0] + 12 };
        case 2: return { chord[2], chord[3], chord[0] + 12, chord[1] + 12 };
        case 3: return { chord[3], chord[0] + 12, chord[1] + 12, chord[2] + 12 };
        default: throw std::runtime_error("Invalid inversion");
    }
}

inline std::vector<int> invert_quint(std::vector<int> chord, inversion inv) {
    int inversion_amt = static_cast<int>(inv) % 5;
    switch (inversion_amt) {
        case 0: return chord;
        case 1: return { chord[1], chord[2], chord[3], chord[4], chord[0] + 12 };
        case 2: return { chord[2], chord[3], chord[4], chord[0] + 12, chord[1] + 12 };
        case 3: return { chord[3], chord[4], chord[0] + 12, chord[1] + 12, chord[2] + 12 };
        case 4: return { chord[4], chord[0] + 12, chord[1] + 12, chord[2] + 12, chord[3] + 12 };
        default: throw std::runtime_error("Invalid inversion");
    }
}

inline std::vector<int> invert(std::vector<int> chord, inversion inv) {
    switch (chord.size()) {
        case 2: return invert_dyad(chord, inv);
        case 3: return invert_triad(chord, inv);
        case 4: return invert_quad(chord, inv);
        case 5: return invert_quint(chord, inv);
        default: throw std::runtime_error("Invalid chord size");
    }
}

struct chord_info {
    note_info root { note::C, 4 };
    chord chord_type { chord::_maj };
    inversion inv { inversion::root };

    std::vector<int> get_midi_notes() const {
        std::vector<int> notes;
        auto intervals = get_intervals(this->chord_type);
        intervals = invert(intervals, this->inv);
        for (auto interval : intervals) {
            notes.push_back(static_cast<int>(this->root.n) + interval + (12 * this->root.octave));
        }
        return notes;
    }

    std::vector<note_info> get_notes() const {
        auto midi_notes = this->get_midi_notes();
        std::vector<note_info> notes;
        std::transform(midi_notes.begin(), midi_notes.end(), std::back_inserter(notes), [](int midi) {
            return note_info::from_midi(midi);
        });
        return notes;
    }

    inline bool has(note n) const {
        auto notes = this->get_notes();
        return std::find_if(notes.begin(), notes.end(), [n](note_info note) {
            return note.n == n;
        }) != notes.end();
    }

    inline bool has(note_info n) const {
        auto notes = this->get_notes();
        return std::find(notes.begin(), notes.end(), n) != notes.end();
    }

    std::string to_string() const {
        return std::string(); // TODO
    }

    inline bool has_same_notes(const chord_info& other) const {
        auto notes = this->get_midi_notes();
        auto other_notes = other.get_midi_notes();
        return std::is_permutation(notes.begin(), notes.end(), other_notes.begin(), other_notes.end());
    }

    inline bool is_equal(const chord_info& other) const {
        return this->root == other.root && this->chord_type == other.chord_type && this->inv == other.inv;
    }

    inline bool operator==(const chord_info& other) const {
        return this->is_equal(other);
    }

    inline bool operator<(const chord_info& other) const {
        if (this->root != other.root) {
            return this->root < other.root;
        }
        if (!this->has_same_notes(other)) {
            auto this_notes = this->get_midi_notes();
            auto other_notes = other.get_midi_notes();
            return std::lexicographical_compare(this_notes.begin(), this_notes.end(), other_notes.begin(), other_notes.end());
        }
        if (this->chord_type != other.chord_type) {
            return this->chord_type < other.chord_type;
        }
        return this->inv < other.inv;
    }
};

void init_chords();
std::vector<chord_info> get_chords(note_info root, bool include_inversions = false);

} // namespace jnickg::audio