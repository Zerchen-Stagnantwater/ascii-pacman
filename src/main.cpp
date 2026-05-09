#include "core/Constants.hpp"
#include "editor/EditorRenderSystem.hpp"
#include "editor/MapEditor.hpp"
#include "game/Game.hpp"
#include "systems/AISystem.hpp"
#include "systems/AnimationSystem.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "systems/RenderSystem.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

int main() {
  sf::RenderWindow window(
      sf::VideoMode({(unsigned)Config::WINDOW_W, (unsigned)Config::WINDOW_H}),
      "ASCII Pacman");
  window.setFramerateLimit(60);

  RenderSystem renderer(window);
  if (!renderer.loadFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf"))
    return -1;

  MapEditor mapEditor;
  EditorRenderSystem editorRenderer(window);
  editorRenderer.loadFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf");

  Game game;
  sf::Clock clock;

  while (window.isOpen()) {
    float dt = clock.restart().asSeconds();

    if (game.getStatus() == GameStatus::MapEditor) {
      // handle editor input
      while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
          window.close();
        if (const auto *key = event->getIf<sf::Event::KeyPressed>()) {
          mapEditor.handleKey(key->code);
        }
        if (const auto *click = event->getIf<sf::Event::MouseButtonPressed>()) {
          bool right = click->button == sf::Mouse::Button::Right;
          mapEditor.handleMouseClick(click->position.x, click->position.y,
                                     right);
        }
      }

      mapEditor.update(dt);

      if (mapEditor.wantsToQuit()) {
        mapEditor = MapEditor(); // reset editor
        game.exitEditor();
      }

      window.clear(sf::Color(20, 20, 20));
      editorRenderer.draw(mapEditor, game.getTheme());
      window.display();
      continue; // skip normal game loop
    }
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();
      if (const auto *key = event->getIf<sf::Event::KeyPressed>()) {
        game.handleInput(key->code, true);
        if (key->code == sf::Keyboard::Key::M)
          game.getAudio().setMuted(!game.getAudio().isMuted());
      }
    }

    game.update(dt);

    // sync theme every frame
    renderer.setTheme(game.getTheme());
    GameStatus status = game.getStatus();

    window.clear(game.getTheme().backgroundColor);

    if (status == GameStatus::StartScreen) {
      renderer.drawStartScreen(game.getHighScore(), game.getThemes(),
                               game.getSelectedThemeIndex());
    } else {
      auto &registry = game.getRegistry();

      if (status == GameStatus::Playing) {
        MovementSystem::update(registry, game.getMap(), dt);
        AISystem::update(registry, game.getMap(), dt);
        int earned = CollisionSystem::update(
            registry, game.getMapMut(), game.getGhostCombo(), game.getAudio());
        game.addScore(earned);
      }

      if (status == GameStatus::Playing || status == GameStatus::Respawn)
        AnimationSystem::update(registry, dt, game.getPowerTimeLeft());

      renderer.drawMap(game.getMap());
      renderer.drawEntities(registry, dt);

      bool powered = false;
      registry.view<TagPacman, Powered>().each(
          [&](auto, auto) { powered = true; });
      renderer.drawHUD(game.getScore(), game.getHighScore(), game.getLives(),
                       powered, game.getCountdown(), game.isRespawning(),
                       game.getLevel());

      if (status == GameStatus::Paused)
        renderer.drawPauseScreen();
      if (status == GameStatus::GameOver)
        renderer.drawGameOver(game.getScore(), game.getHighScore());
      if (status == GameStatus::Win)
        renderer.drawWinScreen(game.getScore(), game.getHighScore(),
                               game.getLevel());
    }

    window.display();
  }
  return 0;
}
