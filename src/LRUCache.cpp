#include "LRUCache.h"
#include <iostream>
#include <cstdint>

LRUCache::LRUCache(size_t capacity)
    : slots(capacity), max_size(capacity), access_counter(0) {}

bool LRUCache::contains(const std::string& track_id) const {
    return findSlot(track_id) != max_size;
}

AudioTrack* LRUCache::get(const std::string& track_id) {
    size_t idx = findSlot(track_id);
    if (idx == max_size) return nullptr;
    return slots[idx].access(++access_counter);
}

/**
 * Insert a track into the cache (takes ownership).
 * If a track with the same title is already cached, it is only marked as
 * most recently used and the new copy is discarded.
 * @return true if an LRU track was evicted to make room, false otherwise
 */
bool LRUCache::put(PointerWrapper<AudioTrack> track) {
    bool evicted = false;

    // (a) Handle nullptr track by returning immediately
    if (!track) {
        return false;
    }

    // Get track title for lookup
    std::string title = track -> get_title();

    // (b) Check if a track with the same title already exists
    size_t exists = findSlot(title);
    if (exists != max_size) {

        // If exists: update its access time to make it most recently used.
        // No eviction occurred; the incoming duplicate is freed when `track` goes out of scope.
        slots[exists].access(++access_counter);
        return false;
    }

    // (c) If the cache is full, evict the least recently used track
    if (isFull()) {
        evicted = evictLRU();
    }

    // (d) Find an empty slot
    size_t empty = findEmptySlot();

    // Only happens when the capacity is 0
    if (empty == max_size) {
        return false;
    }

    // (e) Store the new track; the slot takes ownership of the PointerWrapper
    slots[empty].store(std::move(track), ++access_counter);

    // (f) Return true if an eviction occurred, false otherwise
    return evicted;
}

bool LRUCache::evictLRU() {
    size_t lru = findLRUSlot();
    if (lru == max_size || !slots[lru].isOccupied()) return false;
    slots[lru].clear();
    return true;
}

size_t LRUCache::size() const {
    size_t count = 0;
    for (const auto& slot : slots) if (slot.isOccupied()) ++count;
    return count;
}

void LRUCache::clear() {
    for (auto& slot : slots) {
        slot.clear();
    }
}

void LRUCache::displayStatus() const {
    std::cout << "[LRUCache] Status: " << size() << "/" << max_size << " slots used\n";
    for (size_t i = 0; i < max_size; ++i) {
        if(slots[i].isOccupied()){
            std::cout << "  Slot " << i << ": " << slots[i].getTrack()->get_title()
                      << " (last access: " << slots[i].getLastAccessTime() << ")\n";
        } else {
            std::cout << "  Slot " << i << ": [EMPTY]\n";
        }
    }
}

size_t LRUCache::findSlot(const std::string& track_id) const {
    for (size_t i = 0; i < max_size; ++i) {
        if (slots[i].isOccupied() && slots[i].getTrack()->get_title() == track_id) return i;
    }
    return max_size;

}

/**
 * Find the occupied slot with the smallest access time (least recently used).
 * @return index of that slot, or max_size if the cache is empty
 */
size_t LRUCache::findLRUSlot() const {

    // LRU_index = Least Recently Used slot; max_size means "not found"
    size_t LRU_index = max_size;
    // Start from the largest possible value so any real access time is smaller
    uint64_t min_access_time = UINT64_MAX;

    // Iterate over and check every index
    for (size_t i = 0; i < max_size; ++i) {
        if (slots[i].isOccupied()) {
            // Check if this slot has an older access time than the current minimum
            if (LRU_index == max_size || slots[i].getLastAccessTime() < min_access_time) {
                min_access_time = slots[i].getLastAccessTime();
                LRU_index = i;
            }
        }
    }

    return LRU_index;
}

size_t LRUCache::findEmptySlot() const {
    for (size_t i = 0; i < max_size; ++i) {
        if (!slots[i].isOccupied()) return i;
    }
    return max_size;
}

void LRUCache::set_capacity(size_t capacity){
    if (max_size == capacity)
        return;
    // Update max size
    max_size = capacity;
    // Resize the slots vector (shrinking destroys, and frees, the tracks in the removed slots)
    slots.resize(capacity);
}

size_t LRUCache::get_capacity() const {
    return max_size;
}