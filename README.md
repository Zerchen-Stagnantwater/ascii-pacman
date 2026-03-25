# ASCII Pacman

A Pacman clone rendered in an SFML window using box-drawing Unicode characters (║ ═ ╔), built with an Entity Component System (ECS) architecture using EnTT. 

Built as a learning project — intentionally structured to be readable and hackable. Every system is isolated, every component is plain data. If you want to learn ECS, game loops, or SFML, this is a good codebase to poke around in.

## Demo
Black SFML window, blue box-drawing walls, yellow `C` as Pacman, coloured `M` ghosts, and Unicode dots. Functional collision, ghost AI, and win/lose states.

## Architecture

This project uses ECS (Entity Component System):
- **Entities** are just IDs (Pacman, each ghost, each dot)
- **Components** are plain data structs (Position, Velocity, GhostAI, etc.)
- **Systems** are stateless functions that operate on entities with specific components
```
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
│   └── Game.hpp / Game.cpp   # Owns the registry. Spawns entities. Handles win/death/respawn
│
├── systems/
│   ├── InputSystem           # Reads player input → sets Velocity.nextDir
│   ├── MovementSystem        # Moves entities with Position+Velocity, respects walls, tunnels
│   ├── AISystem              # Ghost AI — Chase, Scatter, Frightened modes per personality
│   ├── CollisionSystem       # Pacman vs dots (collect), Pacman vs ghosts (death/eat)
│   ├── AnimationSystem       # Ticks Pacman mouth open/close
│   └── RenderSystem          # Draws everything — map walls, entities, HUD, messages
│
└── assets/
    ├── fonts/                # DejaVu Sans Mono (supports box-drawing chars)
    └── maps/
        └── classic.txt       # The map layout — edit this to make new maps
```

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

**Controls:** Arrow keys to move.

## Milestones

### Milestone 1 — Polish (Good first issues)
These are isolated, low-risk changes. Good for getting familiar with the codebase.

- [ ] **Ghost death animation** — when a ghost is eaten, flash it before removing. Touch: `AnimationSystem`, `Components.hpp`
- [ ] **Pacman direction glyph** — show `>` `<` `v` `^` based on movement direction instead of always `C`. Touch: `AnimationSystem`
- [ ] **Frightened ghost glyph** — change ghost glyph to `W` and color to blue when frightened. Touch: `RenderSystem`, `GhostAI` component
- [ ] **Score display improvement** — show lives as `♥ ♥ ♥` instead of a number. Touch: `RenderSystem::drawHUD`
- [ ] **Restart on Game Over** — press R to restart without closing the window. Touch: `main.cpp`, `Game::reset()`

### Milestone 2 — Features (Intermediate)
Requires understanding at least two systems and how they interact.

- [ ] **Sound effects** — waka waka on dot eat, siren background, death sound. Touch: add `AudioSystem`, link `SFML::Audio`
- [ ] **High score persistence** — save/load top score to JSON. Touch: add `ScoreManager` using nlohmann/json
- [ ] **Ghost scatter/chase timer** — ghosts should cycle between scatter and chase on a real timer (7s scatter, 20s chase). Touch: `AISystem`, `GhostAI` component
- [ ] **Map loader** — load map from `assets/maps/classic.txt` instead of the hardcoded `classic()` fallback. Touch: `MapLoader`, `Game::initEntities`
- [ ] **Multiple maps** — add a second map layout, cycle maps on win. Touch: `MapLoader`, `assets/maps/`

### Milestone 3 — Multiplayer foundations (Advanced)
Architectural changes. Read the full codebase before starting.

- [ ] **Local co-op** — second Pacman controlled by WASD. Touch: `Game::spawnPacman()`, `InputSystem`, `PlayerInput` component
- [ ] **Networked state serialization** — serialize registry state to JSON each frame. Touch: add `NetworkSerializer` using nlohmann/json, `Components.hpp`
- [ ] **Client/server split** — separate `server` executable that owns `Game`, clients just send input and render state. Touch: major refactor, add ENet

### Milestone 4 — Distribution
- [ ] **Bundle assets** — embed font path in config, fall back gracefully if missing
- [ ] **AppImage packaging** — single executable that runs on any Linux distro
- [ ] **Windows/macOS port** — CMake already supports it, needs testing and CI

## Where to start

| I want to... | Look at... |
|---|---|
| Change how something looks | `RenderSystem.cpp` |
| Change ghost behaviour | `AISystem.cpp` + `GhostAI` in `Components.hpp` |
| Add a new entity type | `Components.hpp` → `Game.cpp` → relevant system |
| Change map layout | `assets/maps/classic.txt` |
| Add a new game rule | `Game.cpp` → `CollisionSystem.cpp` |
| Add sound | Link `SFML::Audio`, add `AudioSystem` |

## Contributing
PRs welcome. Pick any open milestone item, open an issue to claim it, and go.
