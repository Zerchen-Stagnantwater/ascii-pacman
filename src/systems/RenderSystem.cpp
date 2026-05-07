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

void RenderSystem::drawCenteredText(const sf::String &str, unsigned int size,
                                    sf::Color color, float yOffset) {
  sf::Text t(font, str, size);
  t.setFillColor(color);
  auto bounds = t.getLocalBounds();
  t.setPosition(sf::Vector2f(Config::WINDOW_W / 2.f - bounds.size.x / 2.f,
                             Config::WINDOW_H / 2.f + yOffset));
  window.draw(t);
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
  // draw dots and collectibles first (bottom layer)
  registry.view<Position, Renderable, TagDot>().each(
      [&](auto, auto &pos, auto &render) {
        sf::Text t(font, sf::String(render.glyph), Config::CELL_SIZE - 2);
        t.setFillColor(render.color);
        t.setPosition(sf::Vector2f(pos.col * Config::CELL_SIZE,
                                   pos.row * Config::CELL_SIZE + 40));
        window.draw(t);
      });

  // draw ghosts
  registry.view<Position, Renderable, GhostAI>().each(
      [&](auto entity, auto &pos, auto &render, auto &ai) {
        if (registry.all_of<Flashing>(entity)) {
          auto &flash = registry.get<Flashing>(entity);
          if (!flash.visible)
            return;
        }

        wchar_t glyph = render.glyph;
        sf::Color color = render.color;

        if (ai.mode == GhostMode::Frightened) {
          glyph = L'W';
          color = registry.all_of<Flashing>(entity) ? sf::Color::White
                                                    : sf::Color(0, 0, 200);
        } else if (ai.mode == GhostMode::Dead) {
          glyph = L'x';
          color = sf::Color(100, 100, 100);
        }

        sf::Text t(font, sf::String(glyph), Config::CELL_SIZE - 2);
        t.setFillColor(color);
        t.setPosition(sf::Vector2f(pos.col * Config::CELL_SIZE,
                                   pos.row * Config::CELL_SIZE + 40));
        window.draw(t);
      });

  // draw pacman
  registry.view<Position, Renderable, TagPacman>().each(
      [&](auto, auto &pos, auto &render) {
        sf::Text t(font, sf::String(render.glyph), Config::CELL_SIZE - 2);
        t.setFillColor(render.color);
        t.setPosition(sf::Vector2f(pos.col * Config::CELL_SIZE,
                                   pos.row * Config::CELL_SIZE + 40));
        window.draw(t);
      });

  // draw score popups
  registry.view<Position, ScorePopup>().each([&](auto, auto &pos, auto &popup) {
    float alpha = (popup.lifetime / popup.maxLife) * 255.f;
    sf::Text t(font, popup.text, 14);
    t.setFillColor(sf::Color(255, 255, 0, (uint8_t)alpha));
    t.setPosition(
        sf::Vector2f(pos.col * Config::CELL_SIZE,
                     pos.row * Config::CELL_SIZE + 40 -
                         (1.f - popup.lifetime / popup.maxLife) * 20.f));
    window.draw(t);
  });

  // draw blinking text (READY!)
  registry.view<BlinkingText>().each([&](auto, auto &blink) {
    if (!blink.visible)
      return;
    drawCenteredText(sf::String(blink.text), blink.size, blink.color, 20.f);
  });
}

void RenderSystem::drawHUD(int score, int highScore, int lives, bool powered,
                           int countdown, bool respawning, int level) {
  // HUD background panel
  sf::RectangleShape panel(sf::Vector2f((float)Config::WINDOW_W, 36.f));
  panel.setFillColor(sf::Color(20, 20, 40));
  panel.setPosition(sf::Vector2f(0, 0));
  window.draw(panel);

  // score
  std::string scoreStr = "Score: " + std::to_string(score);
  sf::Text scoreTxt(font, scoreStr, 16);
  scoreTxt.setFillColor(sf::Color::White);
  scoreTxt.setPosition(sf::Vector2f(6, 8));
  window.draw(scoreTxt);

  // high score center
  std::string hiStr = "HI: " + std::to_string(highScore);
  sf::Text hiTxt(font, hiStr, 16);
  hiTxt.setFillColor(sf::Color::Yellow);
  auto hiBounds = hiTxt.getLocalBounds();
  hiTxt.setPosition(
      sf::Vector2f(Config::WINDOW_W / 2.f - hiBounds.size.x / 2.f, 8));
  window.draw(hiTxt);

  // hearts right
  std::wstring hearts;
  for (int i = 0; i < lives; i++)
    hearts += L'\u2665';
  for (int i = lives; i < 3; i++)
    hearts += L'\u2661';
  sf::Text heartTxt(font, sf::String(hearts), 16);
  heartTxt.setFillColor(sf::Color::Red);
  heartTxt.setPosition(sf::Vector2f(Config::WINDOW_W - 70.f, 8));
  window.draw(heartTxt);

  // level bottom center
  std::string lvlStr = "LEVEL " + std::to_string(level);
  sf::Text lvlTxt(font, lvlStr, 14);
  lvlTxt.setFillColor(sf::Color(150, 150, 255));
  auto lvlBounds = lvlTxt.getLocalBounds();
  lvlTxt.setPosition(
      sf::Vector2f(Config::WINDOW_W / 2.f - lvlBounds.size.x / 2.f,
                   Config::WINDOW_H - 20.f));
  window.draw(lvlTxt);
  // powered indicator
  if (powered) {
    sf::Text powTxt(font, "* POWERED *", 13);
    powTxt.setFillColor(sf::Color(0, 255, 200));
    auto b = powTxt.getLocalBounds();
    powTxt.setPosition(sf::Vector2f(Config::WINDOW_W / 2.f - b.size.x / 2.f,
                                    Config::WINDOW_H - 22.f));
    window.draw(powTxt);
  }

  // countdown
  if (respawning && countdown > 0) {
    std::string cdStr = std::to_string(countdown);
    drawCenteredText(sf::String(cdStr), 28, sf::Color::Cyan, 50.f);
  }
}

void RenderSystem::drawStartScreen(int highScore) {
  drawCenteredText(sf::String(L"ASCII PACMAN"), 32, sf::Color::Yellow, -80.f);
  drawCenteredText(sf::String(L"High Score: " + std::to_wstring(highScore)), 18,
                   sf::Color::White, -30.f);
  drawCenteredText(sf::String(L"Press ENTER to start"), 18, sf::Color::Cyan,
                   20.f);
  drawCenteredText(sf::String(L"Arrow keys to move"), 14,
                   sf::Color(150, 150, 150), 60.f);
}

void RenderSystem::drawGameOver(int score, int highScore) {
  drawCenteredText(sf::String(L"GAME OVER"), 32, sf::Color::Red, -80.f);
  drawCenteredText(sf::String(L"Score: " + std::to_wstring(score)), 20,
                   sf::Color::White, -30.f);
  drawCenteredText(sf::String(L"High Score: " + std::to_wstring(highScore)), 18,
                   sf::Color::Yellow, 10.f);
  drawCenteredText(sf::String(L"Press R to retry"), 18, sf::Color::Cyan, 60.f);
}

void RenderSystem::drawWinScreen(int score, int highScore, int level) {
  drawCenteredText(sf::String(L"LEVEL " + std::to_wstring(level) + L" CLEAR!"),
                   28, sf::Color::Yellow, -80.f);
  drawCenteredText(sf::String(L"Score: " + std::to_wstring(score)), 20,
                   sf::Color::White, -30.f);
  drawCenteredText(sf::String(L"High Score: " + std::to_wstring(highScore)), 18,
                   sf::Color::Yellow, 10.f);
  drawCenteredText(sf::String(L"Next level in 3 seconds..."), 14,
                   sf::Color::Cyan, 55.f);
}
