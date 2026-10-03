#include "Playlist.h"
#include "AudioTrack.h"
#include <iostream>
#include <algorithm>
Playlist::Playlist(const std::string& name) 
    : head(nullptr), playlist_name(name), track_count(0) {
    std::cout << "Created playlist: " << name << std::endl;
}
/**
 * The playlist owns its tracks: the destructor frees every node and track.
 */
Playlist::~Playlist() {
    #ifdef DEBUG
    std::cout << "Destroying playlist: " << playlist_name << std::endl;
    #endif

    clear();
}

void Playlist::add_track(AudioTrack* track) {
    if (!track) {
        std::cout << "[Error] Cannot add null track to playlist" << std::endl;
        return;
    }

    // Create new node - this allocates memory!
    PlaylistNode* new_node = new PlaylistNode(track);

    // Add to front of list (so the list is in reverse insertion order)
    new_node -> next = head;
    head = new_node;
    // Update the number of tracks
    track_count++;

    std::cout << "Added '" << track -> get_title() << "' to playlist '" 
              << playlist_name << "'" << std::endl;
}

void Playlist::remove_track(const std::string& title) {
    PlaylistNode* current = head;
    PlaylistNode* prev = nullptr;

    // Find the track to remove
    while (current && current -> track -> get_title() != title) {
        prev = current;
        current = current -> next;
    }

    if (current) {
        // Remove from linked list
        if (prev) {
            prev -> next = current -> next;
        } else {
            head = current -> next;
        }

        // The playlist owns the track, so free it together with its node
        delete current -> track;
        delete current;

        track_count--;
        std::cout << "Removed '" << title << "' from playlist" << std::endl;

    } else {
        std::cout << "Track '" << title << "' not found in playlist" << std::endl;
    }
}

void Playlist::display() const {
    std::cout << "\n=== Playlist: " << playlist_name << " ===" << std::endl;
    std::cout << "Track count: " << track_count << std::endl;

    PlaylistNode* current = head;
    int index = 1;

    while (current) {
        std::vector<std::string> artists = current -> track -> get_artists();
        std::string artist_list;

        std::for_each(artists.begin(), artists.end(), [&](const std::string& artist) {
            if (!artist_list.empty()) {
                artist_list += ", ";
            }
            artist_list += artist;
        });

        AudioTrack* track = current->track;
        std::cout << index << ". " << track->get_title() 
                  << " by " << artist_list
                  << " (" << track->get_duration() << "s, " 
                  << track -> get_bpm() << " BPM)" << std::endl;
        current = current -> next;
        index++;
    }

    if (track_count == 0) {
        std::cout << "(Empty playlist)" << std::endl;
    }
    std::cout << "========================\n" << std::endl;
}

AudioTrack* Playlist::find_track(const std::string& title) const {
    PlaylistNode* current = head;

    while (current) {
        if (current -> track -> get_title() == title) {
            return current -> track;
        }
        current = current -> next;
    }

    return nullptr;
}

int Playlist::get_total_duration() const {
    int total = 0;
    PlaylistNode* current = head;

    while (current) {
        total += current -> track -> get_duration();
        current = current -> next;
    }

    return total;
}

std::vector<AudioTrack*> Playlist::getTracks() const {
    std::vector<AudioTrack*> tracks;
    PlaylistNode* current = head;
    while (current) {
        if (current -> track)
            tracks.push_back(current -> track);
        current = current -> next;
    }
    return tracks;
}
/**
 * Deep-copy constructor: the new playlist owns clones of other's tracks,
 * kept in the same order.
 */
Playlist::Playlist(const Playlist& other)
    : head(nullptr),
      playlist_name(other.playlist_name),
      track_count(0)
{
    copy_tracks_from(other);
}

/**
 * Deep-copy assignment: frees this playlist's nodes and tracks, then clones other's.
 */
Playlist& Playlist::operator=(const Playlist& other) {
    if (this == &other) {
        return *this;
    }

    clear();
    playlist_name = other.playlist_name;
    copy_tracks_from(other);

    return *this;
}

void Playlist::clear() {
    PlaylistNode* current = head;
    while (current != nullptr) {
        PlaylistNode* next = current->next;
        delete current->track;
        delete current;
        current = next;
    }
    head = nullptr;
    track_count = 0;
}

void Playlist::copy_tracks_from(const Playlist& other) {
    // Append at the tail so the copy keeps other's order
    PlaylistNode* tail = nullptr;
    for (PlaylistNode* current = other.head; current != nullptr; current = current->next) {
        PlaylistNode* node = new PlaylistNode(current->track->clone().release());
        if (tail == nullptr) {
            head = node;
        } else {
            tail->next = node;
        }
        tail = node;
        track_count++;
    }
}
