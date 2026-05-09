#pragma once
#include "../core/Components.hpp"
#include "../core/Theme.hpp"
#include "../map/Map.hpp"
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

class RenderSystem {
public:
  RenderSystem(sf::RenderWindow &window);
  bool loadFont(const std::string &path);

  void setTheme(const Theme &t) { theme = t; }

  void drawMap(const Map &map);
  void drawEntities(entt::registry &registry, float dt);
  void drawHUD(int score, int highScore, int lives, bool powered, int countdown,
               bool respawning, int level);
  void drawStartScreen(int highScore, const std::vector<Theme> &themes,
                       int selectedIndex);
  void drawGameOver(int score, int highScore);
  void drawWinScreen(int score, int highScore, int level);
  void drawPauseScreen();

private:
  float warnFlashTimer = 0.f;
  bool warnFlashBright = true;
  sf::RenderWindow &window;
  sf::Font font;
  Theme theme;

  sf::String getWallChar(const Map &map, int row, int col) const;
  void drawCenteredText(const sf::String &str, unsigned int size,
                        sf::Color color, float yOffset);
};
