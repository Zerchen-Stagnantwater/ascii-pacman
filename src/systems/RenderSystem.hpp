#pragma once
#include "../core/Components.hpp"
#include "../map/Map.hpp"
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

class RenderSystem {
public:
  RenderSystem(sf::RenderWindow &window);
  bool loadFont(const std::string &path);

  void drawMap(const Map &map);
  void drawEntities(entt::registry &registry);
  void drawHUD(int score, int highScore, int lives, bool powered, int countdown,
               bool respawning, int level);
  void drawStartScreen(int HighScore);
  void drawGameOver(int score, int HighScore);
  void drawWinScreen(int score, int HighScore, int level);

private:
  sf::RenderWindow &window;
  sf::Font font;

  sf::String getWallChar(const Map &map, int row, int col) const;
  void drawCenteredText(const sf::String &str, unsigned int size,
                        sf::Color color, float yOffset);
};
