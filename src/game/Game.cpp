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

float Game::pacmanSpeed() const {
  return std::max(Config::PACMAN_MAX_SPEED,
                  Config::PACMAN_SPEED - (level - 1) * Config::SPEED_SCALE);
}

float Game::ghostSpeed() const {
  return std::max(Config::GHOST_MAX_SPEED,
                  Config::GHOST_SPEED - (level - 1) * Config::SPEED_SCALE);
}

void Game::nextLevel() {
  level++;
  levelTransition = false;
  levelTimer = 0.f;

  // clear entities but keep score and lives
  clearUIEntities();
  registry.clear();
  map.load(MapLoader::classic());
  ghostCombo = 1;
  respawnTimer = 3.f;
  countdownVal = 3;
  status = GameStatus::Respawn;

  initEntities();
  spawnReadyText();
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
  level = 1;
  levelTimer = 0.f;
  levelTransition = false;
  status = GameStatus::Respawn;
  respawnTimer = 3.f;
  countdownTimer = 0.f;
  countdownVal = 0;
  initEntities();
  spawnReadyText();
}

void Game::initEntities() {
  spawnPacman();
  spawnGhosts();
  spawnDots();
}

void Game::spawnPacman() {
  auto e = registry.create();
  registry.emplace<Position>(e, 23, 14);
  registry.emplace<Velocity>(e, Direction::None, Direction::None, pacmanSpeed(),
                             0.f);
  registry.emplace<Renderable>(e, L'C', sf::Color::Yellow);
  registry.emplace<PlayerInput>(e, PlayerInput::Scheme::Arrows);
  registry.emplace<Animated>(e, 0.f, true);
  registry.emplace<TagPacman>(e);
}

void Game::spawnGhosts() {
  struct GhostDef {
    int row, col;
    int homeRow, homeCol;
    wchar_t glyph;
    sf::Color color;
    GhostPersonality personality;
    int scatterRow, scatterCol;
    float exitDelay;
  };

  std::vector<GhostDef> defs = {
      {11, 14, 14, 14, L'M', sf::Color(255, 0, 0), GhostPersonality::Blinky, 0,
       25, 0.f},
      {14, 14, 14, 14, L'M', sf::Color(255, 184, 255), GhostPersonality::Pinky,
       0, 2, 3.f},
      {14, 13, 14, 13, L'M', sf::Color(0, 255, 255), GhostPersonality::Inky, 29,
       25, 6.f},
      {14, 15, 14, 15, L'M', sf::Color(255, 184, 82), GhostPersonality::Clyde,
       29, 2, 9.f},
  };

  for (auto &d : defs) {
    auto e = registry.create();
    registry.emplace<Position>(e, d.row, d.col);
    registry.emplace<Velocity>(e, Direction::Left, Direction::None,
                               ghostSpeed(), 0.f);
    registry.emplace<Renderable>(e, d.glyph, d.color);
    registry.emplace<GhostAI>(e, d.personality, GhostMode::Scatter, 0.f,
                              d.scatterRow, d.scatterCol);
    registry.emplace<GhostHouse>(e, d.exitDelay, 0.f, d.exitDelay == 0.f,
                                 d.homeRow, d.homeCol);
    registry.emplace<OriginalColor>(e, d.color);
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
        if (cell == Cell::Pellet) {
          registry.emplace<Pulsing>(e);
        }
      }
    }
  }
}

void Game::handleInput(sf::Keyboard::Key key, bool pressed) {
  // ESC — quit to start screen
  if (key == sf::Keyboard::Key::Escape) {
    clearUIEntities();
    registry.clear();
    map.load(MapLoader::classic());
    score = 0;
    lives = 3;
    level = 1;
    ghostCombo = 1;
    levelTransition = false;
    status = GameStatus::StartScreen;
    initEntities();
    return;
  }

  // P — pause toggle
  if (key == sf::Keyboard::Key::P) {
    if (status == GameStatus::Playing) {
      prePauseStatus = GameStatus::Playing;
      status = GameStatus::Paused;
    } else if (status == GameStatus::Paused) {
      status = prePauseStatus;
    }
    return;
  }
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
  if (status == GameStatus::Paused)
    return;
  if (status == GameStatus::StartScreen)
    return;
  if (status == GameStatus::GameOver)
    return;
  // clear READY! once player starts moving
  if (status == GameStatus::Playing) {
    registry.view<Velocity, TagPacman>().each([&](auto, auto &vel) {
      if (vel.dir != Direction::None)
        clearUIEntities();
    });
  }
  if (status == GameStatus::Respawn) {
    respawnTimer -= dt;

    // check if dying animation is done
    bool stillDying = false;
    registry.view<Dying>().each([&](auto, auto &dying) {
      if (!dying.done)
        stillDying = true;
    });

    if (respawnTimer <= 0.f && !stillDying) {
      // remove Dying component
      registry.view<Dying, TagPacman>().each(
          [&](auto entity, auto &) { registry.remove<Dying>(entity); });

      // reset pacman position
      registry.view<Position, TagPacman>().each([&](auto, auto &pos) {
        pos.row = 23;
        pos.col = 14;
      });
      registry.view<Velocity, TagPacman>().each([&](auto, auto &vel) {
        vel.dir = Direction::None;
        vel.nextDir = Direction::None;
      });

      // reset ghosts
      registry.view<GhostHouse, Position, Velocity, GhostAI>().each(
          [&](auto entity, auto &house, auto &pos, auto &vel, auto &ai) {
            pos.row = house.homeRow;
            pos.col = house.homeCol;
            vel.dir = Direction::Left;
            vel.nextDir = Direction::None;
            ai.mode = GhostMode::Scatter;
            ai.modeTimer = 0.f;
            house.exited = false;
            house.timer = 0.f;
            if (registry.all_of<Flashing>(entity))
              registry.remove<Flashing>(entity);
          });
      // clear UI and show READY!
      clearUIEntities();
      countdownVal = 3;
      countdownTimer = 0.f;
      respawnTimer = 3.f; // countdown duration

      if (lives <= 0) {
        status = GameStatus::GameOver;
      } else {
        spawnReadyText();
        status = GameStatus::Playing;
      }
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
  if (dotsLeft == 0 && !levelTransition) {
    if (score > highScore) {
      highScore = score;
      saveHighScore();
    }
    levelTransition = true;
    levelTimer = 0.f;
    status = GameStatus::Win;
  }

  if (levelTransition) {
    levelTimer += dt;
    if (levelTimer >= 3.f) {
      nextLevel();
    }
    return;
  }
  // check death — pacman touching a non-frightened ghost
  registry.view<Position, TagPacman>().each([&](auto pacEntity, auto &pacPos) {
    if (registry.all_of<Dying>(pacEntity))
      return;
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

            registry.emplace<Dying>(pacEntity);
            status = GameStatus::Respawn;
            respawnTimer = 2.5f;
          }
        });
  });
}

float Game::getPowerTimeLeft() {
  float t = 0.f;
  registry.view<TagPacman, Powered>().each([&](auto, auto &p) { t = p.timer; });
  return t;
}
