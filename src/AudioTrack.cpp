#include "AudioTrack.h"
#include <iostream>
#include <cstring>
#include <random>

AudioTrack::AudioTrack(const std::string &title, const std::vector<std::string> &artists,
                       int duration, int bpm, size_t waveform_samples)
    : title(title), artists(artists), duration_seconds(duration), bpm(bpm),
      waveform_data(nullptr), waveform_size(waveform_samples)
{

    // Allocate memory for waveform analysis
    waveform_data = new double[waveform_size];
    // Generate some dummy waveform data for testing
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-1.0, 1.0);

    for (size_t i = 0; i < waveform_size; ++i)
    {
        waveform_data[i] = dis(gen);
    }
#ifdef DEBUG
    std::cout << "AudioTrack created: " << title << " by " << std::endl;
    for (const auto &artist : artists)
    {
        std::cout << artist << " ";
    }
    std::cout << std::endl;
#endif
}

// ========== RULE OF 5 ==========

// Destructor: free the waveform array (delete[] on nullptr is a no-op)
AudioTrack::~AudioTrack()
{
#ifdef DEBUG
    std::cout << "AudioTrack destructor called for: " << title << std::endl;
#endif
    delete[] waveform_data;
}

// Copy constructor: deep copy of the waveform array
AudioTrack::AudioTrack(const AudioTrack &other)
    : title(other.title),
      artists(other.artists),
      duration_seconds(other.duration_seconds),
      bpm(other.bpm),
      waveform_data(nullptr),
      waveform_size(other.waveform_size)
{
#ifdef DEBUG
    std::cout << "AudioTrack copy constructor called for: " << other.title << std::endl;
#endif
    if (other.waveform_data && waveform_size > 0)
    {
        waveform_data = new double[waveform_size];
        std::memcpy(waveform_data, other.waveform_data, waveform_size * sizeof(double));
    }
}

// Copy assignment: deep copy. The new array is allocated before the old one is
// freed, so if allocation throws, *this is left unchanged.
AudioTrack &AudioTrack::operator=(const AudioTrack &other)
{
#ifdef DEBUG
    std::cout << "AudioTrack copy assignment called for: " << other.title << std::endl;
#endif
    // 1. Handle self-assignment
    if (this == &other)
    {
        return *this;
    }

    // 2. Deep copy the other object's waveform into a new buffer
    double *new_data = nullptr;
    if (other.waveform_data && other.waveform_size > 0)
    {
        new_data = new double[other.waveform_size];
        std::memcpy(new_data, other.waveform_data, other.waveform_size * sizeof(double));
    }

    // 3. Free the old waveform and take the new one
    delete[] waveform_data;
    waveform_data = new_data;
    waveform_size = other.waveform_size;

    // 4. Copy the remaining members
    title = other.title;
    artists = other.artists;
    duration_seconds = other.duration_seconds;
    bpm = other.bpm;

    return *this;
}

// Move constructor: steal the waveform array instead of copying it
AudioTrack::AudioTrack(AudioTrack &&other) noexcept
    : title(std::move(other.title)),
      artists(std::move(other.artists)),
      duration_seconds(other.duration_seconds),
      bpm(other.bpm),
      waveform_data(other.waveform_data),
      waveform_size(other.waveform_size)
{
#ifdef DEBUG
    // other.title has already been moved from, so print our own title
    std::cout << "AudioTrack move constructor called for: " << title << std::endl;
#endif
    // Leave the source object in a valid but empty state
    other.waveform_data = nullptr;
    other.waveform_size = 0;
}

// Move assignment: free our array, then steal other's
AudioTrack &AudioTrack::operator=(AudioTrack &&other) noexcept
{
#ifdef DEBUG
    std::cout << "AudioTrack move assignment called for: " << other.title << std::endl;
#endif
    // 1. Handle self-assignment
    if (this == &other)
    {
        return *this;
    }

    // 2. Free this object's existing waveform
    delete[] waveform_data;

    // 3. Copy simple members and move the rest
    duration_seconds = other.duration_seconds;
    bpm = other.bpm;
    title = std::move(other.title);
    artists = std::move(other.artists);

    // Steal the raw pointer and its size
    waveform_data = other.waveform_data;
    waveform_size = other.waveform_size;

    // 4. Leave the source object in a valid, empty state
    other.waveform_data = nullptr;
    other.waveform_size = 0;

    return *this;
}

void AudioTrack::get_waveform_copy(double *buffer, size_t buffer_size) const
{
    if (buffer && waveform_data && buffer_size <= waveform_size)
    {
        std::memcpy(buffer, waveform_data, buffer_size * sizeof(double));
    }
}
