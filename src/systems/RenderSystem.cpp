#include "RenderSystem.hpp"
#include "../core/Constants.hpp"

RenderSystem::RenderSystem(sf::RenderWindow &win) : window(win) {}

bool RenderSystem::loadFont(const std::string &path) {
  return font.openFromFile(path);
}

sf::String RenderSystem::getWallChar(const Map &map, int r, int c) const {
  bool up = (r > 0) && map.isWall(r - 1, c);
  bool down = (r < Map::ROWS - 1) && map.isWall(r + 1, c);
  bool left = (c > 0) && map.isWall(r, c - 1);
  bool right = (c < Map::COLS - 1) && map.isWall(r, c + 1);

  int mask = (up ? 8 : 0) | (down ? 4 : 0) | (left ? 2 : 0) | (right ? 1 : 0);
  switch (mask) {
  case 0b1111:
    return sf::String(L"\u256C");
  case 0b1110:
    return sf::String(L"\u2563");
  case 0b1101:
    return sf::String(L"\u2560");
  case 0b1011:
    return sf::String(L"\u2569");
  case 0b0111:
    return sf::String(L"\u2566");
  case 0b1100:
    return sf::String(L"\u2551");
  case 0b0011:
    return sf::String(L"\u2550");
  case 0b1001:
    return sf::String(L"\u255D");
  case 0b1010:
    return sf::String(L"\u255A");
  case 0b0101:
    return sf::String(L"\u2557");
  case 0b0110:
    return sf::String(L"\u2554");
  case 0b1000:
    return sf::String(L"\u2551");
  case 0b0100:
    return sf::String(L"\u2551");
  case 0b0010:
    return sf::String(L"\u2550");
  case 0b0001:
    return sf::String(L"\u2550");
  default:
    return sf::String(L"\u25A0");
  }
}

void RenderSystem::drawMap(const Map &map) {
  for (int r = 0; r < Map::ROWS; r++) {
    for (int c = 0; c < Map::COLS; c++) {
      if (map.getCell(r, c) != Cell::Wall)
        continue;
      sf::Text t(font, getWallChar(map, r, c), Config::CELL_SIZE - 2);
      t.setFillColor(sf::Color(0, 100, 255));
      t.setPosition(
          sf::Vector2f(c * Config::CELL_SIZE, r * Config::CELL_SIZE + 40));
      window.draw(t);
    }
  }
}

void RenderSystem::drawEntities(entt::registry &registry) {
  auto view = registry.view<Position, Renderable>();
  for (auto [entity, pos, render] : view.each()) {
    sf::Text t(font, sf::String(render.glyph), Config::CELL_SIZE - 2);
    t.setFillColor(render.color);
    t.setPosition(sf::Vector2f(pos.col * Config::CELL_SIZE,
                               pos.row * Config::CELL_SIZE + 40));
    window.draw(t);
  }
}

void RenderSystem::drawHUD(int score, int lives, bool powered) {
  std::wstring hearts;
  for (int i = 0; i < lives; i++)
    hearts += L'\u2665'; // ♥
  for (int i = lives; i < 3; i++)
    hearts += L'\u2661'; // ♡ (empty heart for lost lives)

  std::string scoreStr = "Score: " + std::to_string(score) + "   ";
  if (powered)
    scoreStr += "[POWERED]  ";

  sf::Text scoreTxt(font, scoreStr, 16);
  scoreTxt.setFillColor(sf::Color::White);
  scoreTxt.setPosition(sf::Vector2f(4, 4));
  window.draw(scoreTxt);

  sf::Text heartTxt(font, sf::String(hearts), 16);
  heartTxt.setFillColor(sf::Color::Red);
  heartTxt.setPosition(
      sf::Vector2f(Config::WINDOW_W - (3 * Config::CELL_SIZE) - 4, 4));
  window.draw(heartTxt);
}

void RenderSystem::drawMessage(const std::string &msg) {
  sf::Text t(font, msg, 24);
  t.setFillColor(sf::Color::Yellow);
  t.setPosition(
      sf::Vector2f(Config::WINDOW_W / 2.f - 80, Config::WINDOW_H / 2.f));
  window.draw(t);
}
