#include "NotesKeys.hpp"

#include <mutex>
#include <map>
#include <vector>

namespace jnickg::audio {

std::once_flag chords_initialized;
std::map<note_info, std::vector<chord_info>> chords;

void init_chords() {
    std::call_once(chords_initialized, []() {
        for (int octave = 0; octave < 8; ++octave) {
            for (int note = 0; note < 12; ++note) {
                note_info root { static_cast<jnickg::audio::note>(note), octave };
                chords[root] = std::vector<chord_info>();

                for (int c_idx = static_cast<int>(chord::__FIRST); c_idx < static_cast<int>(chord::__COUNT); ++c_idx) {
                    chord c = static_cast<chord>(c_idx);

                    for (int inv_idx = static_cast<int>(inversion::__FIRST); inv_idx < static_cast<int>(inversion::__COUNT); ++inv_idx) {
                        inversion inv = static_cast<inversion>(inv_idx);

                        auto& new_chord = chords.at(root).emplace_back();
                        new_chord.root = root;
                        new_chord.chord_type = c;
                        new_chord.inv = inv;
                    }
                }
            }
        }
    });
}

std::vector<chord_info> get_chords(note_info root, bool include_inversions) {
    init_chords();
    if (include_inversions) {
        return chords.at(root);
    }

    std::vector<chord_info> result;
    for (const auto& c : chords.at(root)) {
        if (c.inv == inversion::root) {
            result.push_back(c);
        }
    }
    return result;
}

std::vector<chord_info> get_chords(key_info key, bool include_inversions) {
    std::vector<chord_info> result;
    for (auto& [root, cs] : chords) {
        for (auto& c : cs) {
            if (!chord_fits_key(c, key)) {
                continue;
            }
            if (!include_inversions && c.inv != inversion::root) {
                continue;
            }
            result.push_back(c);
        }
    }
    return result;
}

} // namespace jnickg::audio
