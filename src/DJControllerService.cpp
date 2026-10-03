#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * Ensure a track is in the controller cache.
 * HIT: mark it most recently used and return 1.
 * MISS: clone the track (the cache owns its own copy), load and analyze the
 * clone, and insert it. Return -1 if an LRU track was evicted, otherwise 0.
 */
int DJControllerService::loadTrackToCache(AudioTrack &track)
{
    const std::string title = track.get_title();

    // HIT: get() refreshes the track's access time (MRU)
    if (cache.get(title) != nullptr)
    {
        return 1;
    }

    // MISS: the cache stores its own polymorphic copy
    PointerWrapper<AudioTrack> cloned_track = track.clone();
    if (!cloned_track)
    {
        std::cerr << "[ERROR] Track: \"" << title << "\" failed to clone" << std::endl;
        return 0;
    }

    cloned_track->load();
    cloned_track->analyze_beatgrid();

    bool evicted = cache.put(std::move(cloned_track));
    return evicted ? -1 : 0;
}

void DJControllerService::set_cache_size(size_t new_size)
{
    cache.set_capacity(new_size);
}

void DJControllerService::displayCacheStatus() const
{
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * Look up a cached track by title. Counts as an access (refreshes LRU order).
 * The cache keeps ownership of the returned track.
 */
AudioTrack *DJControllerService::getTrackFromCache(const std::string &track_title)
{
    return cache.get(track_title);
}

