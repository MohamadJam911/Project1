#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <algorithm>

DJLibraryService::DJLibraryService(const Playlist &playlist)
    : playlist(playlist), library() {}

// The library owns its tracks, so the destructor deletes them.
// (The playlist member frees its own clones in Playlist::~Playlist.)
DJLibraryService::~DJLibraryService()
{
    for (AudioTrack *track : library)
    {
        delete track;
    }
    library.clear();
}

void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo> &library_tracks)
{
    size_t tracks_loaded = 0;

    // Optimization: reserve space
    library.reserve(library_tracks.size());

    for (const auto &meta : library_tracks)
    {
        AudioTrack *new_track = nullptr;

        // Create appropriate track type (MP3 or WAV)
        if (meta.type == "MP3")
        {
            // extra_param1 = bitrate, extra_param2 = has_tags
            new_track = new MP3Track(
                meta.title,
                meta.artists,
                meta.duration_seconds,
                meta.bpm,
                meta.extra_param1, // bitrate
                meta.extra_param2  // has_tags
            );
        }
        else if (meta.type == "WAV")
        {
            // extra_param1 = sample_rate, extra_param2 = bit_depth
            new_track = new WAVTrack(
                meta.title,
                meta.artists,
                meta.duration_seconds,
                meta.bpm,
                meta.extra_param1, // sample_rate
                meta.extra_param2  // bit_depth
            );
        }

        // Store the raw pointer in the library vector (the library owns it).
        // Unknown types are skipped; the parser only accepts MP3 and WAV.
        if (new_track)
        {
            library.push_back(new_track);
            tracks_loaded++;
        }
    }

    // Log summary: [INFO] Track library built: <count> tracks loaded
    std::cout << "[INFO] Track library built: "
              << tracks_loaded << " tracks loaded\n";
}

/**
 * @brief Display the current state of the DJ library playlist
 *
 */
void DJLibraryService::displayLibrary() const
{
    std::cout << "=== DJ Library Playlist: "
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty())
    {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();
    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 *
 * @return Playlist&
 */
Playlist &DJLibraryService::getPlaylist()
{
    return playlist;
}

/**
 * Find a track by title in the currently loaded playlist.
 * The playlist keeps ownership of the returned track.
 */
AudioTrack *DJLibraryService::findTrack(const std::string &track_title)
{
    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string &playlist_name,
                                               const std::vector<int> &track_indices)
{
    // (a) Log: [INFO] Loading playlist: <name>
    std::cout << "[INFO] Loading playlist: " << playlist_name << "\n";

    // (b) Replace the current playlist with an empty one (frees the old tracks)
    playlist = Playlist(playlist_name);

    //(c) For each index in the indices vector (1-based):
    for (int index : track_indices)
    {
        // Validate index is within library bounds.
        if (index < 1 || static_cast<size_t>(index) > library.size())
        {
            // If invalid
            std::cout << "[WARNING] Invalid track index: " << index << "\n";
            continue;
        }
        else
        {
            // If valid: the playlist gets its own clone, the library keeps the original
            AudioTrack *original = library[index - 1];
            AudioTrack *cloned_track = original->clone().release();

            if (cloned_track == nullptr)
            {
                std::cout << "[ERROR] Track creation failed at index: " << index << "\n";
            }
            else
            {
                // Call load() and analyze_beatgrid() on cloned track
                cloned_track->load();
                cloned_track->analyze_beatgrid();
                // Playlist takes ownership (and logs "Added '<title>' to playlist '<name>'")
                playlist.add_track(cloned_track);
            }
        }
    }
    // Log summary
    std::cout << "[INFO] Playlist loaded: " << playlist_name
              << " (" << playlist.get_track_count() << " tracks)\n";
}

/**
 * @return Vector of track titles in playlist order (most recently added first)
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const
{
    std::vector<std::string> titles;
    for (AudioTrack *track : playlist.getTracks())
    {
        if (track)
        {
            titles.push_back(track->get_title());
        }
    }

    return titles;
}