#ifndef MP3TRACK_H
#define MP3TRACK_H

#include "AudioTrack.h"

/**
 * MP3Track - Represents an MP3 audio file with lossy compression
 * MP3 uses perceptual coding to reduce file size while maintaining acceptable quality
 *
 * Phase 4 contracts:
 * - load(): simulate deck preparation (format-specific message); does not start playback.
 * - analyze_beatgrid(): run immediately after load() in this assignment for compatibility checks.
 * - get_quality_score(): derived from bitrate (normalized by 320kbps).
 * - clone(): return a deep polymorphic copy used by the mixer; source remains unchanged.
 */
class MP3Track : public AudioTrack {
private:
    int bitrate;        // Compression level: 128, 192, 320 kbps (higher = better quality)
    bool has_id3_tags;  // Whether file contains ID3 metadata (artist, album, etc.)

public:
    /**
     * Constructor for MP3Track
     */
    MP3Track(const std::string& title, const std::vector<std::string>& artists,
             int duration, int bpm, int bitrate, bool has_tags = true);

    // ========== VIRTUAL FUNCTIONS ==========

    /**
     * Prints MP3 loading steps: ID3 tag processing, frame decoding, completion
     */
    void load() override;

    /**
     * Prints estimated beats and a precision factor of bitrate / 320
     * (MP3 analysis is less precise than WAV at lower bitrates)
     */
    void analyze_beatgrid() override;

    /**
     * Quality score in [0, 100] from bitrate, +5 for ID3 tags, -10 below 128 kbps
     */
    double get_quality_score() const override;

    /**
     * Returns a PointerWrapper owning a new MP3Track copy of this track
     */
    PointerWrapper<AudioTrack> clone() const override;

    // Getters
    int get_bitrate() const { return bitrate; }
    bool has_tags() const { return has_id3_tags; }
};

#endif // MP3TRACK_H
