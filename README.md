# ASCII Pacman

A Pacman clone rendered in an SFML window using box-drawing Unicode characters (║ ═ ╔), built with an Entity Component System (ECS) architecture using EnTT.

Built as a serious portfolio project and learning resource — intentionally structured to be readable, hackable, and extensible toward full multiplayer.

## Demo
Blue box-drawing walls, yellow directional Pacman, coloured ghosts with unique AI personalities, power pellets, score popups, level progression, and full game state management.

## Architecture

This project uses ECS (Entity Component System):
- **Entities** are just IDs (Pacman, each ghost, each dot, each UI element)
- **Components** are plain data structs (Position, Velocity, GhostAI, etc.)
- **Systems** are stateless functions that operate on entities with specific components
~~~
~~~
~~~
src/
├── main.cpp                  # Entry point — wires systems together, runs the game loop
│
├── core/
│   ├── Constants.hpp         # All magic numbers (cell size, speeds, points, window size)
│   ├── Direction.hpp         # Direction enum + opposite() helper
│   └── Components.hpp        # ALL component structs — start here to understand the data model
│
├── map/
│   ├── Map.hpp / Map.cpp     # Grid data + wall/cell queries. Knows nothing about entities
│   └── MapLoader.hpp/cpp     # Loads map from assets/maps/*.txt or falls back to classic()
│
├── game/
│   └── Game.hpp / Game.cpp   # Owns the registry. Spawns entities. Handles win/death/respawn/pause/level
│
├── systems/
│   ├── InputSystem           # Reads player input → sets Velocity.nextDir
│   ├── MovementSystem        # Moves entities with Position+Velocity, respects walls, tunnels
│   ├── AISystem              # Ghost AI — Blinky chase, Pinky intercept, Inky flank, Clyde scatter
│   ├── CollisionSystem       # Pacman vs dots (collect), Pacman vs ghosts (death/eat), combo scoring
│   ├── AnimationSystem       # Pacman mouth/direction glyph, ghost flash, score popup lifetime, blink
│   └── RenderSystem          # Draws everything — map, entities, HUD panel, overlays, screens
│
└── assets/
├── fonts/                # DejaVu Sans Mono (supports box-drawing chars)
└── maps/
└── classic.txt       # The map layout — edit this to make new maps
~~~
## Building

**Dependencies:**
```bash
sudo pacman -S sfml cmake     # Arch Linux
sudo apt install libsfml-dev cmake   # Ubuntu/Debian
```

EnTT and nlohmann/json are auto-fetched by CPM at configure time — no manual install needed.

**Build:**
```bash
git clone git@github.com:Zerchen-StagnantWater/ascii-pacman.git
cd ascii-pacman
mkdir build && cd build
cmake ..
cmake --build .
./pacman
```

**Controls:**

| Key | Action |
|---|---|
| Arrow keys | Move Pacman |
| P | Pause / Resume |
| ESC | Quit to start screen |
| ENTER | Start game |
| R | Retry after game over / win |

## Where to start

| I want to... | Look at... |
|---|---|
| Change how something looks | `RenderSystem.cpp` |
| Change ghost behaviour | `AISystem.cpp` + `GhostAI` in `Components.hpp` |
| Add a new entity type | `Components.hpp` → `Game.cpp` → relevant system |
| Change map layout | `assets/maps/classic.txt` |
| Add a new game rule | `Game.cpp` → `CollisionSystem.cpp` |
| Add sound | Link `SFML::Audio`, add `AudioSystem` |
| Change speeds / points | `core/Constants.hpp` |

## Roadmap

### ✅ Milestone 1 — Core Game (Complete)
- [x] ECS architecture with EnTT
- [x] Box-drawing Unicode walls
- [x] Directional Pacman glyphs
- [x] Ghost AI — Blinky chase, Pinky intercept, Inky flank, Clyde scatter/chase cycle
- [x] Power pellets + frightened ghosts
- [x] Ghost death flash + score combo popups
- [x] Heart lives display
- [x] HUD panel with score, high score, level
- [x] Start screen, Game Over, Win screen
- [x] High score persistence (JSON)
- [x] Respawn countdown + READY! blink
- [x] Level progression with speed scaling
- [x] Pause menu (P) + ESC to quit

### 🔧 Milestone 2 — Polish & Content
- [ ] **Sound effects** — waka waka, ghost death, siren, power up (SFML::Audio)
- [ ] **Pacman death animation** — flash before respawn
- [ ] **Power pellet pulsing** — slow blink on `●`
- [ ] **Extra life at 10,000 points**
- [ ] **Ghost house exit logic** — ghosts exit one by one with delay
- [ ] **Ghost eyes when dead** — show `oo` returning to house
- [ ] **Map loader from file** — load from `assets/maps/classic.txt`
- [ ] **Multiple maps** — cycle maps on level up

### 🎨 Milestone 3 — Themes & Editor
- [ ] **Custom themes/skins** — swap wall color, glyph sets, HUD colors via config file
- [ ] **Theme selector** on start screen (Classic, Midnight, Retro)
- [ ] **Map editor** — in-game editor to place walls/dots, save to `assets/maps/`
- [ ] **Map validation** — check map is solvable before loading

### 🏆 Milestone 4 — Leaderboard
- [ ] **Named leaderboard** — enter initials on game over, save top 10 to JSON
- [ ] **Leaderboard screen** — view top scores from start screen
- [ ] **Per-level high scores** — track best score per level

### 👥 Milestone 5 — Local Multiplayer
- [ ] **Local co-op** — second Pacman controlled by WASD, shared lives pool
- [ ] **Vs mode** — one player is a ghost, one is Pacman
- [ ] **Split score** — individual scores per player

### 🌐 Milestone 6 — Online Multiplayer
- [ ] **Network serialization** — serialize registry state to JSON each frame
- [ ] **Client/server split** — separate `server` executable owns `Game`, clients send input and render state
- [ ] **ENet integration** — reliable UDP, handle disconnection and reconnection
- [ ] **Online vs mode** — two players on different machines, one Pacman one ghost
- [ ] **Online co-op** — two Pacmans on different machines

### 📦 Milestone 7 — Distribution
- [ ] **AppImage packaging** — single executable for any Linux distro
- [ ] **Windows/macOS port** — CMake already supports it, needs CI
- [ ] **GitHub Actions CI** — auto build on push for Linux/Windows/macOS
- [ ] **itch.io release** — publish as a free download

## Contributing
PRs welcome. Pick any open milestone item, open an issue to claim it, and go. See `Where to start` above to find the right files.
