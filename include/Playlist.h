#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "AudioTrack.h"
#include <string>
#include <vector>

/**
 * Playlist - a singly linked list of tracks (Phase 1, memory leaks fixed).
 *
 * Ownership: the playlist OWNS every track added to it. remove_track(), the
 * destructor and copy assignment delete the tracks they drop. Copying a
 * playlist deep-copies its tracks with clone(), so two playlists never share a track.
 * In Phase 4, DJLibraryService fills its playlist with clones of library tracks,
 * so the library and the playlist each free only their own copies.
 */

struct PlaylistNode {
    AudioTrack* track;   // Owned by the Playlist (not by the node)
    PlaylistNode* next;

    PlaylistNode(AudioTrack* t) : track(t), next(nullptr) {}
    ~PlaylistNode() = default;
};

class Playlist {
private:
    PlaylistNode* head;
    std::string playlist_name;
    int track_count;

    // Delete all nodes and their tracks, leaving an empty list
    void clear();
    // Append clones of other's tracks (same order) to this (empty) list
    void copy_tracks_from(const Playlist& other);

public:
    /**
     * Constructor - creates an empty playlist
     */
    Playlist(const std::string& name="");
    /**
     * Deep copy (tracks are cloned)
     */
    Playlist(const Playlist& other);
    Playlist& operator=(const Playlist& other);
    /**
     * Destructor - frees all nodes and the tracks they own
     */
    ~Playlist();

    /**
     * Add a track to the front of the playlist
     * @param track Pointer to AudioTrack to add (the playlist takes ownership)
     */
    void add_track(AudioTrack* track);

    /**
     * Remove the first track with the given title and delete it
     * @param title Title of the track to remove
     */
    void remove_track(const std::string& title);

    /**
     * Display all tracks in the playlist
     */
    void display() const;

    /**
     * Get playlist statistics
     * @return Number of tracks in the playlist
     */
    int get_track_count() const { return track_count; }
    const std::string& get_name() const { return playlist_name; }

    /**
     * @param title Title of the track to find
     * @brief Find a track by title
     * @return Pointer to the found track (still owned by the playlist), or nullptr if not found
     */
    AudioTrack* find_track(const std::string& title) const;

    /**
     * Check if playlist is empty
     */
    bool is_empty() const { return head == nullptr; }

    /**
     * Calculate total duration of all tracks
     */
    int get_total_duration() const;

    /**
     * Get all tracks as a vector, in list order (most recently added first).
     * The playlist keeps ownership of the returned pointers.
     */
    std::vector<AudioTrack*> getTracks() const;

};



#endif // PLAYLIST_H