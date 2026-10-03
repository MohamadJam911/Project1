#ifndef LRUCACHE_H
#define LRUCACHE_H

#include "AudioTrack.h"
#include "CacheSlot.h" 
#include <string>
#include <vector>

/**
 * Fixed-capacity cache of tracks with Least-Recently-Used eviction.
 * Tracks are identified by title. Each access stamps the slot with an
 * increasing counter; the occupied slot with the smallest stamp is evicted
 * when a new track is inserted into a full cache.
 */
class LRUCache {
private:
    std::vector<CacheSlot> slots;
    size_t max_size;
    size_t access_counter;

    // Helper functions. Index-returning helpers return max_size for "not found".
    size_t findSlot(const std::string& track_id) const;
    size_t findLRUSlot() const;
    size_t findEmptySlot() const;
    bool evictLRU();
    bool isFull() const { return size() >= max_size; }

public:
    LRUCache(size_t capacity);

    bool contains(const std::string& track_id) const;
    // Returns the cached track (cache keeps ownership) and marks it most recently used, or nullptr
    AudioTrack* get(const std::string& track_id);
    // Takes ownership of track; returns true if an eviction happened
    bool put(PointerWrapper<AudioTrack> track);

    size_t size() const;
    void clear();
    void displayStatus() const;
    void set_capacity(size_t capacity);
    size_t get_capacity() const;
};

#endif 