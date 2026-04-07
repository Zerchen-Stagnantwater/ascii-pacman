#pragma once
#include "Direction.hpp"
#include <SFML/Graphics.hpp>

// --- Position on the grid ---
struct Position {
  int row, col;
};

// --- Movement ---
struct Velocity {
  Direction dir = Direction::None;
  Direction nextDir = Direction::None;
  float speed = 0.15f;
  float moveTimer = 0.f;
};

// --- How to draw this entity ---
struct Renderable {
  wchar_t glyph;
  sf::Color color;
};

// --- Marks entity as player controlled ---
struct PlayerInput {
  enum class Scheme { Arrows, WASD };
  Scheme scheme;
};

// --- Ghost AI ---
enum class GhostPersonality { Blinky, Pinky, Inky, Clyde };
enum class GhostMode { Chase, Scatter, Frightened, Dead };

struct GhostAI {
  GhostPersonality personality;
  GhostMode mode = GhostMode::Scatter;
  float modeTimer = 0.f;
  int scatterRow = 0;
  int scatterCol = 0;
};

// --- Power pellet state ---
struct Powered {
  float timer = 0.f;
};

// --- Mouth animation ---
struct Animated {
  float timer = 0.f;
  bool mouthOpen = true;
};

// --- Collectible dots/pellets ---
struct Collectible {
  int points;
  bool isPellet = false;
};

// Ghost death flash state
struct Flashing {
  float timer = 1.5f;      // total flash duration
  float flashRate = 0.15f; // how fast it alternates
  float flashTimer = 0.f;
  bool visible = true;
};

// --- Tags (no data, just marks an entity) ---
struct TagPacman {};
struct TagGhost {};
struct TagDot {};
