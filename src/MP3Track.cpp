#include "MP3Track.h"
#include <iostream>
#include <cmath>
#include <algorithm>

MP3Track::MP3Track(const std::string& title, const std::vector<std::string>& artists,
                   int duration, int bpm, int bitrate, bool has_tags)
    : AudioTrack(title, artists, duration, bpm), bitrate(bitrate), has_id3_tags(has_tags) {

    std::cout << "MP3Track created: " << bitrate << " kbps" << std::endl;
}

// ========== VIRTUAL FUNCTION IMPLEMENTATIONS ==========

// Simulates preparing an MP3 for a deck. Output lines use exactly 2 spaces before the arrow.
void MP3Track::load() {
    std::cout << "[MP3Track::load] Loading MP3: \"" << title
              << "\" at " << bitrate << " kbps...\n";

    if (has_id3_tags) {
        std::cout << "  → Processing ID3 metadata (artist info, album art, etc.)...\n";
    } else {
        std::cout << "  → No ID3 tags found.\n";
    }
    std::cout << "  → Decoding MP3 frames...\n";
    std::cout << "  → Load complete.\n";
}

// Prints the estimated beat count and a precision factor of bitrate / 320
// (lower bitrate = less precise analysis).
void MP3Track::analyze_beatgrid() {
    std::cout << "[MP3Track::analyze_beatgrid] Analyzing beat grid for: \"" << title << "\"\n";

    // Estimated beats = minutes * BPM (integer result)
    int beats = (duration_seconds * bpm) / 60;

    // Compression precision factor: 1.0 at 320 kbps
    double precision_factor = static_cast<double>(bitrate) / 320.0;

    std::cout << "  → Estimated beats: " << beats
              << "  → Compression precision factor: " << precision_factor << "\n";
}

// Quality score in [0, 100]: (bitrate / 320) * 100, +5 if ID3 tags are present,
// -10 if bitrate is below 128 kbps. This method does not print anything.
double MP3Track::get_quality_score() const {
    double score = (static_cast<double>(bitrate) / 320.0) * 100.0;

    if (has_id3_tags) {
        score += 5.0;
    }

    if (bitrate < 128) {
        score -= 10.0;
    }

    // Clamp to [0, 100] (std::clamp is C++17, so use max/min)
    return std::max(0.0, std::min(100.0, score));
}

// Polymorphic deep copy: a new MP3Track created by the copy constructor
PointerWrapper<AudioTrack> MP3Track::clone() const {
    return PointerWrapper<AudioTrack>(new MP3Track(*this));
}
