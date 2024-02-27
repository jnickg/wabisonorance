#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <unordered_map>

#include <NotesKeys.hpp>

#include "helpers/test_helpers.h"

using jnickg::audio::chord;
using jnickg::audio::chord_info;
using jnickg::audio::note;
using jnickg::audio::note_info;
using jnickg::audio::get_notes;
using jnickg::audio::get_chords;
using jnickg::audio::get_intervals;
using jnickg::audio::get_inversions;
using jnickg::audio::inversion;


TEST_CASE("jnickg::audio::note / jnickg::audio::note_info") {
    auto notes = get_notes();
    REQUIRE(notes.size() == 12);

    SECTION("note_info can be constructed using note") {
        for (auto note : notes) {
            note_info i {
                .n = note
            };
            REQUIRE(i.n == note);
        }
    }

    SECTION("note_info::to_string() returns a non-empty string") {
        for (auto note : notes) {
            note_info i {
                .n = note
            };
            auto s = i.to_string();
            REQUIRE(!s.empty());
        }
    }

    SECTION("note_info::to_string() contains ::jnickg::audio::to_string(note)") {
        for (auto note : notes) {
            note_info i {
                .n = note
            };
            auto s = i.to_string();
            REQUIRE(s.find(::jnickg::audio::to_string(note)) != std::string::npos);
        }
    }

    SECTION("note_info::to_midi() returns a valid MIDI note number") {
        for (auto note : notes) {
            note_info i {
                .n = note
            };
            auto midi = i.to_midi();
            REQUIRE(midi >= 0);
            REQUIRE(midi <= 127);
        }
    }
}

TEST_CASE("jnickg::audio::inversion") {
    auto inversions = get_inversions();
    REQUIRE(inversions.size() == 5);

    SECTION("::jnickg::audio::to_string(inversion) returns a non-empty string") {
        for (auto inv : inversions) {
            auto s = ::jnickg::audio::to_string(inv);
            REQUIRE((!s.empty() || inv == inversion::root));
        }
    }

    SECTION("invert(chord, inversion) succeeds on expected inversions") {
        for (auto inv : inversions) {
            for (size_t i = 1; i < 6; ++i) {
                // The notes we use don't really matter here.
                std::vector<int> midi_notes;
                for (int note = 0; note < static_cast<int>(i); ++note) {
                    midi_notes.push_back(note);
                }

                auto inverted = ::jnickg::audio::invert(midi_notes, inv);
                REQUIRE(inverted.size() == i);

                // We should add some assertions about the contents of it, like checking whether
                // the notes are the same (even if they changed octave)
            }
        }
    }
}

TEST_CASE("jnickg::audio::chord / jnickg::audio::chord_info") {
    auto notes = jnickg::audio::get_notes();
    REQUIRE(notes.size() == 12);
 
    auto chords = jnickg::audio::get_chords();
    REQUIRE(chords.size() == 93);

    auto inversions = jnickg::audio::get_inversions();
    REQUIRE(inversions.size() == 5);

    SECTION("get_intervals(chord) returns a non-empty vector") {
        for (auto chord : chords) {
            auto intervals = jnickg::audio::get_intervals(chord);
            REQUIRE(!intervals.empty());
        }
    }

    SECTION("chord_info") {
        std::vector<chord_info> all_chords;
        for (auto note : notes) {
            for (auto chord : chords) {
                for (auto inv : inversions) {
                    auto& c = all_chords.emplace_back();
                    c.root = {
                        .n = note
                    };
                    c.chord_type = chord;
                    c.inv = inv;
                    REQUIRE(c.root.n == note);
                    REQUIRE(c.chord_type == chord);
                    REQUIRE(c.inv == inv);
                }
            }
        }
        REQUIRE(all_chords.size() == 12 * 93 * 5);

        SECTION("get_notes()") {
            std::vector<std::vector<note_info>> all_chord_notes;
            for (auto& c : all_chords) {
                auto ns = c.get_notes();
                REQUIRE(!ns.empty());
                all_chord_notes.push_back(ns);
            }
            REQUIRE(all_chord_notes.size() == 12 * 93 * 5);

            SECTION("always includes root note") {
                for (size_t i = 0; i < all_chord_notes.size(); ++i) {
                    auto& nis = all_chord_notes[i];
                    auto& chord_root_ni = all_chords[i].root;
                    auto found = std::find_if(nis.begin(), nis.end(), [&chord_root_ni](const note_info& ni) {
                        return ni.n == chord_root_ni.n;
                    });
                    REQUIRE(found != nis.end());
                }
            }
        }

        SECTION("get_midi_notes()") {
            for (auto& c : all_chords) {
                auto midi_notes = c.get_midi_notes();
                REQUIRE(!midi_notes.empty());
            }
        }

        SECTION("get_midi_notes() returns a vector of valid MIDI note numbers") {
            for (auto& c : all_chords) {
                auto midi_notes = c.get_midi_notes();
                for (auto note : midi_notes) {
                    REQUIRE(note >= 0);
                    REQUIRE(note <= 127);
                }
            }
        }

        SECTION("get_notes()") {
            for (auto& c : all_chords) {
                auto ns = c.get_notes();
                REQUIRE(!ns.empty());
            }
        }
    }
}

TEST_CASE("jnickg::audio::get_chords(root, include_inversions") {
    auto octaves = std::vector<int> { 0, 1, 2, 3, 4, 5, 6, 7 };
    auto notes = jnickg::audio::get_notes();

    std::vector<note_info> all_notes;
    for (auto octave : octaves) {
        for (auto note : notes) {
            auto& n = all_notes.emplace_back();
            n.n = note;
            n.octave = octave;
        }
    }
    REQUIRE(all_notes.size() == 12 * 8);

    SECTION("get_chords(root, include_inversions = false) always includes root note") {
        for (auto n : all_notes) {
            auto n_midi = n.to_midi();
            auto cs = jnickg::audio::get_chords(n, false);
            REQUIRE(!cs.empty());
            for (auto& c : cs) {
                auto ns = c.get_midi_notes();
                REQUIRE(std::find(ns.begin(), ns.end(), n_midi) != ns.end());
            }
        }
    }

    SECTION("get_chords(root, include_inversions = false) returns only root inversions") {
        for (auto n : all_notes) {
            auto cs = jnickg::audio::get_chords(n, false);
            REQUIRE(!cs.empty());
            for (auto& c : cs) {
                REQUIRE(c.inv == jnickg::audio::inversion::root);
            }
        }
    }

    SECTION("get_chords(root, include_inversions = true) returns all inversions") {
        for (auto n : all_notes) {
            auto cs = jnickg::audio::get_chords(n, true);
            REQUIRE(!cs.empty());

            std::unordered_map<inversion, bool> inversions_found;
            // Kind of redundant because operator[] defaults to false, but this makes me feel
            // better.
            for (auto inv : jnickg::audio::get_inversions()) {
                inversions_found[inv] = false;
            }           

            for (auto& c : cs) {
                inversions_found[c.inv] = true;
            }

            for (auto inv : jnickg::audio::get_inversions()) {
                REQUIRE(inversions_found[inv]);
            }           
        }
    }
}