#include "WAVTrack.h"
#include <iostream>
#include <algorithm>

WAVTrack::WAVTrack(const std::string& title, const std::vector<std::string>& artists,
                   int duration, int bpm, int sample_rate, int bit_depth)
    : AudioTrack(title, artists, duration, bpm), sample_rate(sample_rate), bit_depth(bit_depth) {

    std::cout << "WAVTrack created: " << sample_rate << "Hz/" << bit_depth << "bit" << std::endl;
}

// ========== VIRTUAL FUNCTION IMPLEMENTATIONS ==========

// Simulates preparing a WAV for a deck. Output lines use exactly 2 spaces before the arrow.
void WAVTrack::load() {
    std::cout << "[WAVTrack::load] Loading WAV: \"" << title
              << "\" at " << sample_rate << "Hz/" << bit_depth << "bit (uncompressed)...\n";

    // Estimated file size in bytes: duration * sample_rate * bytes per sample * 2 channels (stereo)
    long long size = static_cast<long long>(duration_seconds) * sample_rate * (bit_depth / 8) * 2;

    std::cout << "  → Estimated file size: " << size << " bytes\n";
    std::cout << "  → Fast loading due to uncompressed format.\n";
}

// Prints the estimated beat count. Uncompressed audio gives full precision (factor 1).
void WAVTrack::analyze_beatgrid() {
    std::cout << "[WAVTrack::analyze_beatgrid] Analyzing beat grid for: \"" << title << "\"\n";

    // Estimated beats = minutes * BPM, printed as an integer
    int beats = static_cast<int>(static_cast<long>(duration_seconds) * bpm / 60);

    std::cout << "  → Estimated beats: " << beats
              << "  → Precision factor: 1 (uncompressed audio)\n";
}

// Quality score: base 70, +10 for sample_rate >= 44100 (CD), +5 more for >= 96000 (hi-res),
// +10 for bit_depth >= 16 (CD), +5 more for >= 24 (pro). Capped at 100. Does not print.
double WAVTrack::get_quality_score() const {
    double score = 70.0;

    if (sample_rate >= 44100) {
        score += 10.0;
    }
    if (sample_rate >= 96000) {
        score += 5.0;
    }

    if (bit_depth >= 16) {
        score += 10.0;
    }
    if (bit_depth >= 24) {
        score += 5.0;
    }

    return std::min(100.0, score);
}

// Polymorphic deep copy: a new WAVTrack created by the copy constructor
PointerWrapper<AudioTrack> WAVTrack::clone() const {
    return PointerWrapper<AudioTrack>(new WAVTrack(*this));
}
