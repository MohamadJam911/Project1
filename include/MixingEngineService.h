#ifndef MIXINGENGINESERVICE_H
#define MIXINGENGINESERVICE_H

#include "AudioTrack.h"
#include <string>

// Service responsible for deck operations and track analysis
// Phase 4 binding:
// - Enforces instant transitions and deck alternation policy.
// - After loading to a deck: call track.load(); then analyze_beatgrid(); then switch active deck.
// - The previously active deck becomes finished and is unloaded immediately.
// - The service owns the tracks on its decks (each one is a clone).
class MixingEngineService {
private:
    AudioTrack* decks[2];   // Owned; nullptr means the deck is empty
    size_t active_deck;
    bool auto_sync;
    int bpm_tolerance;
public:
    MixingEngineService();
    ~MixingEngineService();

    /** Contract: Load a track to the next deck per instant-transition policy
     * - @param track: reference to a cached track to be cloned for the mixer
     * - @return: index of the deck the track was loaded to (0 or 1), or -1 on failure.
     * - @brief: Clones the track, unloads the target deck if needed, loads the clone,
     *   analyzes the beatgrid, syncs BPM if auto_sync is on and the BPMs are outside
     *   the tolerance, unloads the previous deck, and switches the active deck.
     * - @attention: on clone failure, logs an error and returns -1
     */
    int loadTrackToDeck(const AudioTrack& track);

    // Display deck status
    void displayDeckStatus() const;

    /**
     * Contract: Determine if the active deck and the given track can be mixed
     * @return true if the BPM difference is within bpm_tolerance; false otherwise
     *         (also false if the active deck or the track is empty)
     */
    bool can_mix_tracks(const PointerWrapper<AudioTrack>& track) const;

    /**
     * Contract: Synchronize BPM between active deck and given track.
     * - @param track: the track to sync with the currently active deck
     * - @brief Sets the given track's BPM to the integer average of its BPM and the active deck's BPM.
     * - Preconditions: the active deck and the track are non-null (otherwise it does nothing).
     * - Modifies: only the given track's BPM; the decks are unchanged.
     */
    void sync_bpm(const PointerWrapper<AudioTrack>& track) const;

    /**
     * @brief set auto sync mode
     * 
     */
    void set_auto_sync(bool enabled);

    /**
     * @brief Set BPM tolerance for mixing compatibility
     * 
     */
    void set_bpm_tolerance(int tolerance);

    // ========== RULE OF 5 ADDITIONS ==========
    
    // Copy Constructor
    MixingEngineService(const MixingEngineService& other);

    // Copy Assignment Operator
    MixingEngineService& operator=(const MixingEngineService& other);

    // Move Constructor
    MixingEngineService(MixingEngineService&& other) noexcept;

    // Move Assignment Operator
    MixingEngineService& operator=(MixingEngineService&& other) noexcept;

    // =========================================

};

#endif // MIXINGENGINESERVICE_H
