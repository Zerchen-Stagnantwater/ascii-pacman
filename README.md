# ASCII Pacman

A Pacman clone rendered in an SFML window using box-drawing Unicode characters, built with an Entity Component System architecture using EnTT.

Built as a learning project — intentionally structured to be readable and hackable.

## What this project covers
- ECS architecture with EnTT
- SFML 3 window, rendering, and input
- Grid-based movement with delta time accumulation
- Ghost AI (Chase, Scatter, Frightened modes)
- CMake build system with CPM for dependency management

## Dependencies
- SFML 3
- EnTT (auto-fetched via CPM)
- nlohmann/json (auto-fetched via CPM)

## Building
```bash
sudo pacman -S sfml cmake
git clone git@github.com:Zerchen-Stagnantwater/ascii-pacman.git
cd ascii-pacman
mkdir build && cd build
cmake ..
cmake --build .
./pacman
```

## Good first issues for contributors
- [ ] Add ghost death animation
- [ ] Add sound effects via SFML::Audio
- [ ] Add high score persistence with nlohmann/json
- [ ] Add a second player (WASD controls)
- [ ] Add a map editor that reads from assets/maps/
- [ ] Port to Windows/macOS
