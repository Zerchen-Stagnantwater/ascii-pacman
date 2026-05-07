#include "core/Constants.hpp"
#include "game/Game.hpp"
#include "systems/AISystem.hpp"
#include "systems/AnimationSystem.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "systems/RenderSystem.hpp"
#include <SFML/Graphics.hpp>

int main() {
  sf::RenderWindow window(
      sf::VideoMode({(unsigned)Config::WINDOW_W, (unsigned)Config::WINDOW_H}),
      "ASCII Pacman");
  window.setFramerateLimit(60);

  RenderSystem renderer(window);
  if (!renderer.loadFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf"))
    return -1;

  Game game;
  sf::Clock clock;

  while (window.isOpen()) {
    float dt = clock.restart().asSeconds();

    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();
      if (const auto *key = event->getIf<sf::Event::KeyPressed>())
        game.handleInput(key->code, true);
    }

    game.update(dt);

    window.clear(sf::Color::Black);

    GameStatus status = game.getStatus();

    if (status == GameStatus::StartScreen) {
      renderer.drawStartScreen(game.getHighScore());
    } else {
      auto &registry = game.getRegistry();
      if (status == GameStatus::Playing || status == GameStatus::Respawn) {
        if (status == GameStatus::Playing) {

          MovementSystem::update(registry, game.getMap(), dt);
          AISystem::update(registry, game.getMap(), dt);
          int earned = CollisionSystem::update(
              registry, const_cast<Map &>(game.getMap()), game.getGhostCombo());
          game.addScore(earned);
        }
        AnimationSystem::update(registry, dt, game.getPowerTimeLeft());
      }

      renderer.drawMap(game.getMap());
      renderer.drawEntities(registry);

      bool powered = false;
      registry.view<TagPacman, Powered>().each(
          [&](auto, auto) { powered = true; });
      renderer.drawHUD(game.getScore(), game.getHighScore(), game.getLives(),
                       powered, game.getCountdown(), game.isRespawning(),
                       game.getLevel());
      if (status == GameStatus::Paused)
        renderer.drawPauseScreen();
      if (status == GameStatus::Win)
        renderer.drawWinScreen(game.getScore(), game.getHighScore(),
                               game.getLevel());
      if (status == GameStatus::GameOver)
        renderer.drawGameOver(game.getScore(), game.getHighScore());
    }
    window.display();
  }

  return 0;
}
