#include "MixingEngineService.h"
#include <iostream>
#include <memory>
#include <cmath>

/**
 * Both decks start empty. active_deck starts at 1 so the first track
 * is loaded onto deck 0.
 */
MixingEngineService::MixingEngineService()
    : decks(), active_deck(1), auto_sync(false), bpm_tolerance(0)
{
    decks[0] = nullptr;
    decks[1] = nullptr;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl;
}

/**
 * The service owns the tracks on its decks and deletes them.
 */
MixingEngineService::~MixingEngineService()
{
    std::cout << "[MixingEngineService] Cleaning up decks..." << std::endl;
    for (int i = 0; i < 2; ++i)
    {
        delete decks[i];
        decks[i] = nullptr;
    }
}

// ========== RULE OF 5 IMPLEMENTATION ==========

// 1. Copy Constructor
MixingEngineService::MixingEngineService(const MixingEngineService &other)
    : decks(),
      active_deck(other.active_deck),
      auto_sync(other.auto_sync),
      bpm_tolerance(other.bpm_tolerance)
{
    // Deep copy of the decks using clone()
    for (int i = 0; i < 2; ++i)
    {
        if (other.decks[i] != nullptr)
        {
            // clone() returns a PointerWrapper, we release() to get the raw pointer
            decks[i] = other.decks[i]->clone().release();
        }
        else
        {
            decks[i] = nullptr;
        }
    }
}

// 2. Copy Assignment Operator
MixingEngineService &MixingEngineService::operator=(const MixingEngineService &other)
{
    if (this == &other)
    {
        return *this; // Self-assignment check
    }

    // Clone first, so a failure leaves *this unchanged
    AudioTrack *new_decks[2] = {nullptr, nullptr};
    for (int i = 0; i < 2; ++i)
    {
        if (other.decks[i] != nullptr)
        {
            new_decks[i] = other.decks[i]->clone().release();
        }
    }

    // Free the old decks and take the copies
    for (int i = 0; i < 2; ++i)
    {
        delete decks[i];
        decks[i] = new_decks[i];
    }

    active_deck = other.active_deck;
    auto_sync = other.auto_sync;
    bpm_tolerance = other.bpm_tolerance;

    return *this;
}

// 3. Move Constructor
MixingEngineService::MixingEngineService(MixingEngineService &&other) noexcept
    : decks(),
      active_deck(other.active_deck),
      auto_sync(other.auto_sync),
      bpm_tolerance(other.bpm_tolerance)
{
    // Steal the pointers
    decks[0] = other.decks[0];
    decks[1] = other.decks[1];

    // Nullify source pointers so destructor doesn't kill them
    other.decks[0] = nullptr;
    other.decks[1] = nullptr;
}

// 4. Move Assignment Operator
MixingEngineService &MixingEngineService::operator=(MixingEngineService &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    // Clean up our current resources
    delete decks[0];
    delete decks[1];

    // Steal resources from other
    decks[0] = other.decks[0];
    decks[1] = other.decks[1];
    active_deck = other.active_deck;
    auto_sync = other.auto_sync;
    bpm_tolerance = other.bpm_tolerance;

    // Nullify source to leave it in a valid state
    other.decks[0] = nullptr;
    other.decks[1] = nullptr;

    return *this;
}

/**
 * Load a clone of the track onto the inactive deck, then make it active.
 * @param track: Reference to the cached track (not modified; the deck gets a clone)
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack &track)
{
    std::cout << "\n=== Loading Track to Deck ===" << std::endl;

    // 1. The target deck is the one that is NOT currently active
    size_t target_deck = (active_deck == 0) ? 1 : 0;

    std::cout << "[Deck Switch] Target deck: " << target_deck << std::endl;

    // 2. Unload the target deck if it still holds a track
    if (decks[target_deck] != nullptr)
    {
        delete decks[target_deck];
        decks[target_deck] = nullptr;
    }

    // 3. The mixer always gets its own polymorphic clone
    decks[target_deck] = track.clone().release();

    // 4. Validate the clone
    if (decks[target_deck] == nullptr)
    {
        std::cout << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone" << std::endl;
        return -1;
    }

    // 5. Prepare the track on the deck
    decks[target_deck]->load();
    decks[target_deck]->analyze_beatgrid();

    // 6. Sync BPM against the active deck. This must happen before the previous
    //    deck is unloaded, because sync needs its BPM.
    if (auto_sync)
    {
        if (decks[active_deck] != nullptr)
        {
            // Temporary wrapper so the PointerWrapper-based API can be used;
            // release() below hands ownership back to decks[target_deck].
            PointerWrapper<AudioTrack> wrapper(decks[target_deck]);
            if (!can_mix_tracks(wrapper))
            {
                sync_bpm(wrapper);
            }
            wrapper.release();
        }
        else
        {
            std::cout << "[Sync BPM] Cannot sync - one of the decks is empty." << std::endl;
        }
    }

    std::cout << "[Load Complete] '" << decks[target_deck]->get_title()
              << "' is now loaded on deck " << target_deck << std::endl;

    // 7. Instant transition: the previously active deck is finished, so unload it
    if (decks[active_deck] != nullptr)
    {
        std::cout << "[Unload] Unloading previous deck " << active_deck
                  << " (" << decks[active_deck]->get_title() << ")" << std::endl;
        delete decks[active_deck];
        decks[active_deck] = nullptr;
    }

    // 8. Switch the active deck
    active_deck = target_deck;
    std::cout << "[Active Deck] Switched to deck " << active_deck << std::endl;

    return static_cast<int>(target_deck);
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const
{
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i)
    {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * Check if the given track can be mixed with the active deck based on BPM.
 *
 * @param track: Track to check for mixing compatibility
 * @return: true if |active BPM - track BPM| <= bpm_tolerance;
 *          false if the difference is larger or either track is missing
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack> &track) const
{
    if (decks[active_deck] == nullptr || !track)
    {
        return false;
    }

    int active_bpm = decks[active_deck]->get_bpm();
    int track_bpm = track->get_bpm();

    return std::abs(active_bpm - track_bpm) <= bpm_tolerance;
}

/**
 * Set the given track's BPM to the (integer) average of its BPM and the
 * active deck's BPM. Preconditions: the active deck and track are non-null;
 * otherwise nothing happens. Modifies only the given track (the method is
 * const because it does not change the service itself).
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack> &track) const
{
    if (decks[active_deck] == nullptr || !track)
    {
        return;
    }

    int active_bpm = decks[active_deck]->get_bpm();
    int original_bpm = track->get_bpm();
    int average_bpm = (active_bpm + original_bpm) / 2;

    // AudioTrack::set_bpm was added to AudioTrack.h for this purpose
    track->set_bpm(average_bpm);

    std::cout << "[Sync BPM] Syncing BPM from "
              << original_bpm << " to " << average_bpm << std::endl;
}

// ========== Setters (applied from the session configuration) ==========
void MixingEngineService::set_bpm_tolerance(int tolerance)
{
    bpm_tolerance = tolerance;
}

void MixingEngineService::set_auto_sync(bool enabled)
{
    auto_sync = enabled;
}