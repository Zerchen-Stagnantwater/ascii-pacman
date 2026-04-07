#include "Game.hpp"
#include "../core/Constants.hpp"
#include "../map/MapLoader.hpp"
#include "core/Components.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Game::Game() {
  loadHighScore();
  map.load(MapLoader::classic());
  initEntities();
  spawnReadyText();
}

void Game::loadHighScore() {
  std::ifstream f(Config::SAVE_PATH);
  if (!f.is_open())
    return;
  try {
    json j;
    f >> j;
    highScore = j.value("highScore", 0);
  } catch (...) {
  }
}

void Game::saveHighScore() {
  json j;
  j["highScore"] = highScore;
  std::ofstream f(Config::SAVE_PATH);
  f << j.dump(2);
}

void Game::spawnReadyText() {
  readyEntity = registry.create();
  registry.emplace<BlinkingText>(readyEntity, "READY!", sf::Color::Yellow, 22u,
                                 0.4f, 0.f, true, -1.f);
}

void Game::clearUIEntities() {
  if (readyEntity != entt::null && registry.valid(readyEntity)) {
    registry.destroy(readyEntity);
    readyEntity = entt::null;
  }
  if (countdownEntity != entt::null && registry.valid(countdownEntity)) {
    registry.destroy(countdownEntity);
    countdownEntity = entt::null;
  }
}

void Game::reset() {
  clearUIEntities();
  registry.clear();
  map.load(MapLoader::classic());
  score = 0;
  lives = 3;
  ghostCombo = 1;
  status = GameStatus::Playing;
  respawnTimer = 0.f;
  countdownTimer = 0.f;
  countdownVal = 2;
  initEntities();
  spawnReadyText();
  status = GameStatus::Respawn;
  respawnTimer = 3.f;
}

void Game::initEntities() {
  spawnPacman();
  spawnGhosts();
  spawnDots();
}

void Game::spawnPacman() {
  auto e = registry.create();
  registry.emplace<Position>(e, 23, 14);
  registry.emplace<Velocity>(e, Direction::None, Direction::None,
                             Config::PACMAN_SPEED, 0.f);
  registry.emplace<Renderable>(e, L'C', sf::Color::Yellow);
  registry.emplace<PlayerInput>(e, PlayerInput::Scheme::Arrows);
  registry.emplace<Animated>(e, 0.f, true);
  registry.emplace<TagPacman>(e);
}

void Game::spawnGhosts() {
  struct GhostDef {
    int row, col;
    wchar_t glyph;
    sf::Color color;
    GhostPersonality personality;
    int scatterRow, scatterCol;
  };

  std::vector<GhostDef> defs = {
      {14, 14, L'M', sf::Color(255, 0, 0), GhostPersonality::Blinky, 0, 25},
      {14, 13, L'M', sf::Color(255, 184, 255), GhostPersonality::Pinky, 0, 2},
      {14, 15, L'M', sf::Color(0, 255, 255), GhostPersonality::Inky, 29, 25},
      {14, 12, L'M', sf::Color(255, 184, 82), GhostPersonality::Clyde, 29, 2},
  };

  for (auto &d : defs) {
    auto e = registry.create();
    registry.emplace<Position>(e, d.row, d.col);
    registry.emplace<Velocity>(e, Direction::Left, Direction::None,
                               Config::GHOST_SPEED, 0.f);
    registry.emplace<Renderable>(e, d.glyph, d.color);
    registry.emplace<GhostAI>(e, d.personality, GhostMode::Scatter, 0.f,
                              d.scatterRow, d.scatterCol);
    registry.emplace<TagGhost>(e);
  }
}

void Game::spawnDots() {
  for (int r = 0; r < Map::ROWS; r++) {
    for (int c = 0; c < Map::COLS; c++) {
      Cell cell = map.getCell(r, c);
      if (cell == Cell::Dot || cell == Cell::Pellet) {
        auto e = registry.create();
        registry.emplace<Position>(e, r, c);
        registry.emplace<Collectible>(
            e,
            cell == Cell::Pellet ? Config::PELLET_POINTS : Config::DOT_POINTS,
            cell == Cell::Pellet);
        registry.emplace<Renderable>(
            e, cell == Cell::Pellet ? L'\u25CF' : L'\u00B7',
            sf::Color(200, 200, 200));
        registry.emplace<TagDot>(e);
      }
    }
  }
}

void Game::handleInput(sf::Keyboard::Key key, bool pressed) {
  if (!pressed)
    return;
  if (status == GameStatus::StartScreen) {
    if (key == sf::Keyboard::Key::Enter)
      reset();
    return;
  }

  if (status == GameStatus::GameOver || status == GameStatus::Win) {
    if (key == sf::Keyboard::Key::R)
      reset();
    return;
  }

  if (status != GameStatus::Playing)
    return;

  auto view = registry.view<Velocity, PlayerInput>();
  for (auto [entity, vel, input] : view.each()) {
    if (input.scheme == PlayerInput::Scheme::Arrows) {
      if (key == sf::Keyboard::Key::Up)
        vel.nextDir = Direction::Up;
      if (key == sf::Keyboard::Key::Down)
        vel.nextDir = Direction::Down;
      if (key == sf::Keyboard::Key::Left)
        vel.nextDir = Direction::Left;
      if (key == sf::Keyboard::Key::Right)
        vel.nextDir = Direction::Right;
    }
  }
}

void Game::update(float dt) {
  if (status == GameStatus::StartScreen)
    return;
  if (status == GameStatus::GameOver || status == GameStatus::Win)
    return;
  if (status == GameStatus::Respawn) {
    respawnTimer -= dt;
    countdownTimer += dt;

    int newVal = (int)(respawnTimer);
    if (newVal != countdownVal && newVal > 0) {
      countdownVal = newVal;
    }
    if (respawnTimer <= 0.f) {
      clearUIEntities();
      status = GameStatus::Playing;
      ghostCombo = 1;
    }
    return;
  }

  // tick powered timers
  registry.view<Powered>().each([&](auto entity, auto &powered) {
    powered.timer -= dt;
    if (powered.timer <= 0.f)
      registry.remove<Powered>(entity);
  });

  // sync ghost modes with powered state
  bool pacPowered = false;
  float powerTimeLeft = 0.f;
  registry.view<TagPacman, Powered>().each([&](auto, auto &p) {
    pacPowered = true;
    powerTimeLeft = p.timer;
  });

  registry.view<GhostAI>().each([&](auto, auto &ai) {
    if (ai.mode == GhostMode::Dead)
      return;
    ai.mode = pacPowered ? GhostMode::Frightened : GhostMode::Chase;
  });

  // tick score popups
  registry.view<ScorePopup, Position>().each(
      [&](auto entity, auto &popup, auto &) {
        popup.lifetime -= dt;
        if (popup.lifetime <= 0.f) {
          registry.destroy(entity);
        }
      });

  // check win — count remaining dot entities
  int dotsLeft = 0;
  registry.view<TagDot>().each([&](auto) { dotsLeft++; });
  if (dotsLeft == 0) {
    if (score > highScore) {
      highScore = score;
      saveHighScore();
    }
    status = GameStatus::Win;
    return;
  }

  // check death — pacman touching a non-frightened ghost
  registry.view<Position, TagPacman>().each([&](auto, auto &pacPos) {
    registry.view<Position, GhostAI>().each(
        [&](auto, auto &ghostPos, auto &ai) {
          if (ai.mode == GhostMode::Frightened || ai.mode == GhostMode::Dead)
            return;
          if (pacPos.row == ghostPos.row && pacPos.col == ghostPos.col) {
            lives--;
            if (score > highScore) {
              highScore = score;
              saveHighScore();
            }
            if (lives <= 0) {
              status = GameStatus::GameOver;
            } else {
              status = GameStatus::Respawn;
              respawnTimer = 3.f;
              countdownVal = 3;
              countdownTimer = 0.f;
              registry.view<Position, TagPacman>().each([&](auto, auto &pos) {
                pos.row = 23;
                pos.col = 14;
              });
              registry.view<Velocity, TagPacman>().each([&](auto, auto &vel) {
                vel.dir = Direction::None;
                vel.nextDir = Direction::None;
              });
              spawnReadyText();
            }
          }
        });
  });
}

float Game::getPowerTimeLeft() {
  float t = 0.f;
  registry.view<TagPacman, Powered>().each([&](auto, auto &p) { t = p.timer; });
  return t;
}
