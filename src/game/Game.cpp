#include "Game.hpp"
#include "../core/Constants.hpp"
#include "../map/MapLoader.hpp"

Game::Game() {
  map.load(MapLoader::classic());
  initEntities();
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
  if (status == GameStatus::GameOver || status == GameStatus::Win)
    return;

  if (status == GameStatus::Respawn) {
    respawnTimer -= dt;
    if (respawnTimer <= 0.f)
      status = GameStatus::Playing;
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
  registry.view<TagPacman, Powered>().each(
      [&](auto, auto) { pacPowered = true; });

  registry.view<GhostAI>().each([&](auto, auto &ai) {
    if (ai.mode == GhostMode::Dead)
      return;
    ai.mode = pacPowered ? GhostMode::Frightened : GhostMode::Chase;
  });

  // check win — count remaining dot entities
  int dotsLeft = 0;
  registry.view<TagDot>().each([&](auto) { dotsLeft++; });
  if (dotsLeft == 0) {
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
            if (lives <= 0) {
              status = GameStatus::GameOver;
            } else {
              status = GameStatus::Respawn;
              respawnTimer = 2.f;
              // reset pacman position
              registry.view<Position, TagPacman>().each([&](auto, auto &pos) {
                pos.row = 23;
                pos.col = 14;
              });
              registry.view<Velocity, TagPacman>().each([&](auto, auto &vel) {
                vel.dir = Direction::None;
                vel.nextDir = Direction::None;
              });
            }
          }
        });
  });
}
