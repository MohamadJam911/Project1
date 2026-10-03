# DJ Track Session Manager

A C++11 console application that simulates a DJ controller: it builds a music library, loads playlists, keeps recently used tracks in an LRU cache, and mixes tracks across two decks with automatic BPM sync.

It was built for **Assignment 1 of the Systems Programming (SPL) course at Ben-Gurion University**, which is about manual memory management in C++. The program runs leak-free under Valgrind.

---

## Features

- **Polymorphic tracks**: `MP3Track` and `WAVTrack` derive from an abstract `AudioTrack`. Each has its own loading, beat-grid analysis and quality scoring.
- **Rule of 5**: `AudioTrack` owns a heap-allocated waveform buffer, with a correct destructor, copy and move operations.
- **Custom smart pointer**: `PointerWrapper<T>` is a move-only owning pointer, similar to `std::unique_ptr`.
- **LRU cache**: the controller holds a fixed number of tracks and evicts the least recently used one when full.
- **Two-deck mixer**: tracks alternate between deck 0 and deck 1 with instant transitions. If BPM auto-sync is on, a new track whose BPM is too far from the active deck is synced to the average of the two.
- **Config-driven sessions**: the library, cache size, BPM tolerance and playlists are all read from `bin/dj_config.txt`.

## Requirements

- `g++` with C++11 support
- GNU Make
- Linux, macOS, or Windows via **WSL**. The included Dev Container also works.
- Optional: Valgrind, for leak checking

On Ubuntu/Debian, install everything with:

```bash
make install-deps
```

## Build

From the repository root:

```bash
make          # build bin/dj_manager
make debug    # clean rebuild with -DDEBUG (extra constructor/destructor logging)
make release  # clean optimized rebuild (-O2)
make clean    # remove object files and the executable
make help     # list all targets
```

## Run

The program must be started from the repository root, because it reads `bin/dj_config.txt` by relative path.

| Command | What it does |
|---|---|
| `./bin/dj_manager` (or `make test`) | Runs the built-in tests for each phase: memory leaks, Rule of 5, smart pointer, polymorphism |
| `./bin/dj_manager -I` | Starts an interactive session: pick playlists from a menu, `0` to quit |
| `./bin/dj_manager -I -A` | Plays every configured playlist in alphabetical order, without prompting |

Check for memory leaks (runs `-I -A` under Valgrind):

```bash
make test-leaks
```

## Configuration

`bin/dj_config.txt` is a `key=value` file. Lines starting with `#` are comments.

```ini
# Track library: TYPE,title,{artist1;artist2;},duration_seconds,bpm,extra1,extra2
#   MP3: extra1 = bitrate (kbps),     extra2 = has ID3 tags (1/0)
#   WAV: extra1 = sample rate (Hz),   extra2 = bit depth
library_track_1=MP3,Right Back,{Yuri Kane;},374,125,256,1
library_track_2=WAV,Concrete Angel,{Gareth Emery;},306,122,44100,24

# Number of LRU cache slots
controller_cache_size=4
# Max BPM difference that still counts as mixable
bpm_tolerance=14
# Sync BPM when tracks are outside the tolerance
auto_sync=true

# Playlists: name=comma-separated, 1-based library track numbers
my_playlist=1,2
```

> Put comments on their own lines. Text after a value is not stripped, so `auto_sync=true # note` would be read as `false`.

## How It Works

For each track in a playlist, the session:

1. **Loads it into the controller cache.** On a hit, the track becomes the most recently used. On a miss, a clone is stored, evicting the least recently used track if the cache is full.
2. **Loads it onto the inactive deck.** The mixer gets its own clone, which is loaded and analyzed and then synced if needed. The previously active deck is then unloaded, and the new deck becomes active.
3. **Updates statistics.** After each playlist, a summary prints cache hits, misses and evictions, deck loads, and errors.

### Memory ownership

Every object has exactly one owner, so nothing is freed twice or leaked:

| Owner | Owns |
|---|---|
| `DJLibraryService::library` | The original tracks built from the config |
| `Playlist` | Clones of library tracks (freed on removal, reassignment or destruction) |
| `LRUCache` slots (`PointerWrapper`) | The controller's cached clones |
| `MixingEngineService::decks` | The clones currently loaded on each deck |

## Project Structure

```
.
├── Makefile                # build, test, and valgrind targets
├── bin/
│   └── dj_config.txt       # session configuration (library, settings, playlists)
├── include/                # class declarations
│   ├── AudioTrack.h        # abstract base track (Rule of 5)
│   ├── MP3Track.h / WAVTrack.h
│   ├── PointerWrapper.h    # move-only owning smart pointer (header-only)
│   ├── Playlist.h          # owning linked list of tracks
│   ├── CacheSlot.h / LRUCache.h
│   ├── DJLibraryService.h  # library + current playlist
│   ├── DJControllerService.h  # LRU cache front-end
│   ├── MixingEngineService.h  # two-deck mixer
│   ├── DJSession.h         # orchestrates a session
│   ├── SessionFileParser.h # config file parser
│   └── ConfigurationManager.h
└── src/                    # implementations + main.cpp
```

## Getting the Code

```bash
git clone https://github.com/MohamadJam911/Project1.git
cd Project1
make
./bin/dj_manager -I
```
