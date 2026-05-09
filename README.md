# ASCII Pacman

A Pacman clone rendered in an SFML window using box-drawing Unicode characters (║ ═ ╔), built with an Entity Component System (ECS) architecture using EnTT.

Built as a serious portfolio project and learning resource — intentionally structured to be readable, hackable, and extensible toward full multiplayer.

## Demo
Blue box-drawing walls, yellow directional Pacman, coloured ghosts with unique AI personalities, power pellets, score popups, level progression, sound effects, themes, leaderboard, and an in-game map editor.

## Architecture

This project uses ECS (Entity Component System):
- **Entities** are just IDs (Pacman, each ghost, each dot, each UI element)
- **Components** are plain data structs (Position, Velocity, GhostAI, etc.)
- **Systems** are stateless functions that operate on entities with specific components

~~~
src/
├── main.cpp                  # Entry point — wires systems together, runs the game loop
│
├── core/
│   ├── Constants.hpp         # All magic numbers (cell size, speeds, points, window size)
│   ├── Direction.hpp         # Direction enum + opposite() helper
│   ├── Components.hpp        # ALL component structs — start here to understand the data model
│   ├── Theme.hpp             # Theme struct — all colors for every visual element
│   └── ThemeLoader.hpp/cpp   # Loads themes from assets/themes/*.json
│
├── map/
│   ├── Map.hpp / Map.cpp     # Grid data + wall/cell queries
│   └── MapLoader.hpp/cpp     # Loads map + spawn points from assets/maps/*.txt, validates on load
│
├── editor/
│   ├── MapEditor.hpp/cpp         # In-game map editor state and logic
│   ├── MapValidator.hpp/cpp      # Validates map dimensions, spawns, dots, borders
│   └── EditorRenderSystem.hpp/cpp # Draws the editor grid, toolbar, help overlay
│
├── game/
│   ├── Game.hpp / Game.cpp   # Owns the registry. Spawns entities. Handles all game state
│   └── Leaderboard.hpp/cpp   # Top 10 scores, load/save to JSON, qualification check
│
└── systems/
    ├── InputSystem           # Reads player input → sets Velocity.nextDir
    ├── MovementSystem        # Moves entities, respects walls, tunnels, ghost collision
    ├── AISystem              # Ghost AI — Blinky chase, Pinky intercept, Inky flank, Clyde scatter
    ├── CollisionSystem       # Pacman vs dots, Pacman vs ghosts, combo scoring
    ├── AnimationSystem       # Pacman mouth/direction, ghost flash, power warning, popups
    ├── RenderSystem          # Draws everything — map, entities, HUD, overlays, all screens
    └── AudioSystem           # Loads and plays all sound effects and siren music

assets/
├── fonts/
│   └── DejaVuSansMono.ttf
├── sounds/
│   ├── waka.wav
│   ├── power.wav
│   ├── ghost_eat.wav
│   ├── death.wav
│   ├── extra_life.wav
│   ├── level_clear.wav
│   └── siren.wav
├── maps/
│   ├── classic.txt           # Standard Pacman layout
│   ├── maze.txt              # Tight corridors
│   ├── open.txt              # Open layout, faster paced
│   └── custom_*.txt          # Your maps from the editor
├── themes/
│   ├── classic.json          # Blue walls, yellow Pacman
│   ├── midnight.json         # Purple walls, cyan Pacman
│   └── retro.json            # Green walls, white Pacman
└── leaderboard.json          # Top 10 scores (auto-generated)
~~~

## Building

**Dependencies:**
```bash
sudo pacman -S sfml cmake     # Arch Linux
sudo apt install libsfml-dev cmake   # Ubuntu/Debian
```

EnTT and nlohmann/json are auto-fetched by CPM at configure time.

**Build:**
```bash
git clone git@github.com:Zerchen-StagnantWater/ascii-pacman.git
cd ascii-pacman
mkdir build && cd build
cmake ..
cmake --build .
./pacman
```

## Controls

**In game:**

| Key | Action |
|---|---|
| Arrow keys | Move Pacman |
| P | Pause / Resume |
| M | Mute / Unmute |
| ESC | Quit to start screen |

**Start screen:**

| Key | Action |
|---|---|
| ENTER | Start game |
| Left / Right | Cycle themes |
| L | View leaderboard |
| E | Open map editor |
| R | Retry (game over / win) |

**Map editor:**

| Key | Action |
|---|---|
| Left click | Place tile |
| Right click | Erase tile |
| Tab | Cycle tile types |
| W / D / O / E | Wall / Dot / Pellet / Erase |
| 1–5 | Pacman / Blinky / Pinky / Inky / Clyde spawn |
| S | Save map |
| H | Toggle help overlay |
| ESC | Quit editor |

## Map format

Maps are plain `.txt` files in `assets/maps/`. Special characters:

| Char | Meaning |
|---|---|
| `#` | Wall |
| `.` | Dot |
| `o` | Power pellet |
| ` ` | Empty |
| `P` | Pacman spawn |
| `B` | Blinky spawn |
| `p` | Pinky spawn |
| `i` | Inky spawn |
| `c` | Clyde spawn |

Add a new `.txt` file to `assets/maps/` and it will be picked up automatically on the next level. Maps are validated on load — invalid maps are skipped with a warning.

## Theme format

Themes are JSON files in `assets/themes/`. Each key is an RGB array `[r, g, b]`. Copy an existing theme and modify colors to create your own. Drop it in `assets/themes/` and it will appear in the theme selector on the start screen.

## Where to start

| I want to... | Look at... |
|---|---|
| Change how something looks | `RenderSystem.cpp` |
| Change ghost behaviour | `AISystem.cpp` + `GhostAI` in `Components.hpp` |
| Add a new entity type | `Components.hpp` → `Game.cpp` → relevant system |
| Change map layout | `assets/maps/classic.txt` or press E in-game |
| Add a new game rule | `Game.cpp` → `CollisionSystem.cpp` |
| Add/change sounds | `AudioSystem.cpp` + `assets/sounds/` |
| Change speeds / points | `core/Constants.hpp` |
| Add a new map | Press E in-game or create `.txt` in `assets/maps/` |
| Add a new theme | Copy a JSON from `assets/themes/`, modify colors |
| Change leaderboard size | `Leaderboard::MAX_ENTRIES` in `Leaderboard.hpp` |

## Roadmap

### ✅ Milestone 1 — Core Game (Complete)
- [x] ECS architecture with EnTT
- [x] Box-drawing Unicode walls
- [x] Directional Pacman glyphs with mouth animation
- [x] Ghost AI — Blinky chase, Pinky intercept, Inky flank, Clyde scatter/chase cycle
- [x] Power pellets + frightened ghosts + warning flash
- [x] Ghost death flash + score combo popups
- [x] Heart lives display
- [x] HUD panel with score, high score, level
- [x] Start screen, Game Over, Win screen
- [x] High score persistence (JSON)
- [x] Respawn countdown + READY! blink
- [x] Pause menu (P) + ESC to quit + mute (M)
- [x] Pacman death animation
- [x] Power pellet pulsing
- [x] Ghost eyes when dead
- [x] Ghost house exit logic (staggered delays)
- [x] Ghost respawn after being eaten
- [x] Ghost position reset on player death

### ✅ Milestone 2 — Polish & Content (Complete)
- [x] Sound effects — waka, power, ghost eat, death, extra life, level clear
- [x] Siren background music with pitch scaling per level
- [x] Level progression with speed scaling
- [x] Extra life at 10,000 points
- [x] Map loader from file with validation
- [x] Random map per level
- [x] Multiple maps (classic, maze, open)
- [x] Spawn points encoded in map files

### ✅ Milestone 3 — Themes, Editor & Leaderboard (Complete)
- [x] Theme system — Classic, Midnight, Retro loaded from JSON
- [x] Theme selector on start screen
- [x] In-game map editor with tile placement and spawn markers
- [x] Map validator — warns and skips invalid maps
- [x] Named leaderboard — initials entry, top 10 saved to JSON
- [x] Leaderboard screen accessible from start screen

### 👥 Milestone 4 — Local Multiplayer
- [ ] **Local co-op** — second Pacman controlled by WASD, shared lives pool
- [ ] **Vs mode** — one player controls a ghost, one controls Pacman
- [ ] **Individual scores** — separate score per player, winner on game over

### 🌐 Milestone 5 — Online Multiplayer
- [ ] **Network serialization** — serialize registry state to JSON each frame
- [ ] **Client/server split** — separate `server` executable owns game state
- [ ] **ENet integration** — reliable UDP, handle disconnection
- [ ] **Online co-op** — two Pacmans on different machines
- [ ] **Online vs mode** — one Pacman, one ghost, different machines

### 📦 Milestone 6 — Distribution
- [ ] **AppImage packaging** — single executable for any Linux distro
- [ ] **Windows/macOS port** — needs CI testing
- [ ] **GitHub Actions CI** — auto build on push for Linux/Windows/macOS
- [ ] **itch.io release** — publish as free download

## Contributing
PRs welcome. Pick any open milestone item, open an issue to claim it, and go. See `Where to start` above to find the right files.
