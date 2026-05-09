#pragma once
#include "Direction.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>

// --- Position on the grid ---
struct Position {
  int row, col;
};

struct OriginalColor {
  sf::Color color;
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

struct FrightenedWarning {
}; // tag — set by AnimationSystem when power is about to expire

// Controls ghost house exit timing
struct GhostHouse {
  float exitDelay; // seconds before leaving house
  float timer = 0.f;
  bool exited = false;
  int homeRow, homeCol; // position inside house to return to when dead
};

// --- Power pellet state ---
struct Powered {
  float timer = 0.f;
};

// Power pellet pulse
struct Pulsing {
  float timer = 0.f;
  float rate = 0.5f;
  bool bright = true;
};

// Pacman death animation
struct Dying {
  float timer = 0.f;
  float duration = 1.5f;
  int frame = 0;
  bool done = false;
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

// Score popup tht floats and fades
struct ScorePopup {
  std::string text;
  float lifetime = 1.2f;
  float maxLife = 1.2f;
};

// Blinking text (Ready!, countdown)
struct BlinkingText {
  std::string text;
  sf::Color color;
  unsigned int size;
  float blinkRate = 0.4f;
  float blinkTimer = 0.f;
  bool visible = true;
  float lifetime = -1.f; // -1 = infinite
};

// --- Tags (no data, just marks an entity) ---
struct TagPacman {};
struct TagGhost {};
struct TagDot {};
