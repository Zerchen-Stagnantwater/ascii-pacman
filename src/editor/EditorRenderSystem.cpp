#include "EditorRenderSystem.hpp"
#include "../core/Constants.hpp"
#include "../map/Map.hpp"

EditorRenderSystem::EditorRenderSystem(sf::RenderWindow &win) : window(win) {}

bool EditorRenderSystem::loadFont(const std::string &path) {
  return font.openFromFile(path);
}

sf::Color EditorRenderSystem::tileColor(TileType tile,
                                        const Theme &theme) const {
  switch (tile) {
  case TileType::Wall:
    return theme.wallColor;
  case TileType::Dot:
    return theme.dotColor;
  case TileType::Pellet:
    return theme.pelletColor;
  case TileType::SpawnPacman:
    return theme.pacmanColor;
  case TileType::SpawnBlinky:
    return theme.blinkyColor;
  case TileType::SpawnPinky:
    return theme.pinkyColor;
  case TileType::SpawnInky:
    return theme.inkyColor;
  case TileType::SpawnClyde:
    return theme.clydeColor;
  default:
    return sf::Color(50, 50, 50);
  }
}

wchar_t EditorRenderSystem::tileGlyph(TileType tile) const {
  switch (tile) {
  case TileType::Wall:
    return L'\u25A0'; // ■
  case TileType::Dot:
    return L'\u00B7'; // ·
  case TileType::Pellet:
    return L'\u25CF'; // ●
  case TileType::SpawnPacman:
    return L'P';
  case TileType::SpawnBlinky:
    return L'B';
  case TileType::SpawnPinky:
    return L'p';
  case TileType::SpawnInky:
    return L'i';
  case TileType::SpawnClyde:
    return L'c';
  default:
    return L' ';
  }
}

void EditorRenderSystem::drawGrid(const MapEditor &editor, const Theme &theme) {
  const auto &layout = editor.getLayout();

  for (int r = 0; r < Map::ROWS; r++) {
    for (int c = 0; c < Map::COLS; c++) {
      // cell background
      sf::RectangleShape cell(
          sf::Vector2f(Config::CELL_SIZE - 1.f, Config::CELL_SIZE - 1.f));
      cell.setPosition(
          sf::Vector2f(c * Config::CELL_SIZE, r * Config::CELL_SIZE + 40));
      cell.setFillColor(sf::Color(30, 30, 30));
      cell.setOutlineColor(sf::Color(50, 50, 50));
      cell.setOutlineThickness(1.f);
      window.draw(cell);

      // tile content
      if (r >= (int)layout.size())
        continue;
      char ch = (c < (int)layout[r].size()) ? layout[r][c] : ' ';
      if (ch == ' ')
        continue;

      TileType tileForColor = TileType::Empty;
      wchar_t glyph = L' ';

      switch (ch) {
      case '#':
        tileForColor = TileType::Wall;
        glyph = L'\u2588';
        break; // █
      case '.':
        tileForColor = TileType::Dot;
        glyph = L'\u00B7';
        break;
      case 'o':
        tileForColor = TileType::Pellet;
        glyph = L'\u25CF';
        break;
      case 'P':
        tileForColor = TileType::SpawnPacman;
        glyph = L'P';
        break;
      case 'B':
        tileForColor = TileType::SpawnBlinky;
        glyph = L'B';
        break;
      case 'p':
        tileForColor = TileType::SpawnPinky;
        glyph = L'p';
        break;
      case 'i':
        tileForColor = TileType::SpawnInky;
        glyph = L'i';
        break;
      case 'c':
        tileForColor = TileType::SpawnClyde;
        glyph = L'c';
        break;
      default:
        continue;
      }

      sf::Text t(font, sf::String(glyph), Config::CELL_SIZE - 2);
      t.setFillColor(tileColor(tileForColor, theme));
      t.setPosition(
          sf::Vector2f(c * Config::CELL_SIZE, r * Config::CELL_SIZE + 40));
      window.draw(t);
    }
  }
}

void EditorRenderSystem::drawToolbar(const MapEditor &editor,
                                     const Theme &theme) {
  // toolbar background
  sf::RectangleShape bar(sf::Vector2f((float)Config::WINDOW_W, 36.f));
  bar.setFillColor(theme.hudPanelColor);
  bar.setPosition(sf::Vector2f(0, 0));
  window.draw(bar);

  // current tile
  TileType cur = editor.getCurrentTile();
  std::wstring label = L"Tile: ";
  switch (cur) {
  case TileType::Wall:
    label += L"WALL (#)";
    break;
  case TileType::Dot:
    label += L"DOT (.)";
    break;
  case TileType::Pellet:
    label += L"PELLET (o)";
    break;
  case TileType::Empty:
    label += L"ERASE ( )";
    break;
  case TileType::SpawnPacman:
    label += L"PACMAN SPAWN (P)";
    break;
  case TileType::SpawnBlinky:
    label += L"BLINKY SPAWN (B)";
    break;
  case TileType::SpawnPinky:
    label += L"PINKY SPAWN (p)";
    break;
  case TileType::SpawnInky:
    label += L"INKY SPAWN (i)";
    break;
  case TileType::SpawnClyde:
    label += L"CLYDE SPAWN (c)";
    break;
  }

  sf::Text tileTxt(font, sf::String(label), 14);
  tileTxt.setFillColor(tileColor(cur, theme));
  tileTxt.setPosition(sf::Vector2f(6, 10));
  window.draw(tileTxt);

  // hints right side
  sf::Text hints(font, "H=help  Tab=cycle  S=save  ESC=quit  RMB=erase", 11);
  hints.setFillColor(theme.subtitleColor);
  auto b = hints.getLocalBounds();
  hints.setPosition(sf::Vector2f(Config::WINDOW_W - b.size.x - 6, 12));
  window.draw(hints);
}

void EditorRenderSystem::drawMessage(const std::string &msg,
                                     const Theme &theme) {
  if (msg.empty())
    return;
  sf::Text t(font, msg, 16);
  t.setFillColor(theme.winColor);
  auto b = t.getLocalBounds();
  t.setPosition(sf::Vector2f(Config::WINDOW_W / 2.f - b.size.x / 2.f,
                             Config::WINDOW_H - 30.f));
  window.draw(t);
}

void EditorRenderSystem::draw(const MapEditor &editor, const Theme &theme) {
  drawGrid(editor, theme);
  drawToolbar(editor, theme);
  drawMessage(editor.getSaveMessage(), theme);
  if (editor.showHelp)
    drawHelp(theme);
}

void EditorRenderSystem::drawHelp(const Theme &theme) {
  // dark overlay
  sf::RectangleShape overlay(
      sf::Vector2f((float)Config::WINDOW_W, (float)Config::WINDOW_H));
  overlay.setFillColor(sf::Color(0, 0, 0, 210));
  overlay.setPosition(sf::Vector2f(0, 0));
  window.draw(overlay);

  struct Line {
    std::wstring key;
    std::wstring desc;
  };
  std::vector<Line> lines = {
      {L"LEFT CLICK", L"Place selected tile"},
      {L"RIGHT CLICK", L"Erase tile"},
      {L"Tab", L"Cycle through tile types"},
      {L"W", L"Select Wall"},
      {L"D", L"Select Dot"},
      {L"O", L"Select Power Pellet"},
      {L"E", L"Select Eraser"},
      {L"1", L"Place Pacman spawn (P)"},
      {L"2", L"Place Blinky spawn (B)"},
      {L"3", L"Place Pinky spawn (p)"},
      {L"4", L"Place Inky spawn (i)"},
      {L"5", L"Place Clyde spawn (c)"},
      {L"S", L"Save map to assets/maps/"},
      {L"H", L"Toggle this help screen"},
      {L"ESC", L"Quit editor"},
  };

  // title
  sf::Text title(font, sf::String(L"MAP EDITOR — CONTROLS"), 22);
  title.setFillColor(theme.titleColor);
  auto tb = title.getLocalBounds();
  title.setPosition(
      sf::Vector2f(Config::WINDOW_W / 2.f - tb.size.x / 2.f, 60.f));
  window.draw(title);

  // divider
  sf::Text div(font, sf::String(std::wstring(36, L'─')), 14);
  div.setFillColor(theme.subtitleColor);
  auto db = div.getLocalBounds();
  div.setPosition(sf::Vector2f(Config::WINDOW_W / 2.f - db.size.x / 2.f, 95.f));
  window.draw(div);

  float startY = 120.f;
  float lineH = 22.f;
  float keyX = Config::WINDOW_W / 2.f - 200.f;
  float descX = Config::WINDOW_W / 2.f - 20.f;

  for (auto &line : lines) {
    sf::Text keyTxt(font, sf::String(line.key), 14);
    keyTxt.setFillColor(theme.promptColor);
    keyTxt.setPosition(sf::Vector2f(keyX, startY));
    window.draw(keyTxt);

    sf::Text descTxt(font, sf::String(line.desc), 14);
    descTxt.setFillColor(theme.hudTextColor);
    descTxt.setPosition(sf::Vector2f(descX, startY));
    window.draw(descTxt);

    startY += lineH;
  }

  // footer
  sf::Text footer(font, sf::String(L"Press H to close this help"), 13);
  footer.setFillColor(theme.subtitleColor);
  auto fb = footer.getLocalBounds();
  footer.setPosition(
      sf::Vector2f(Config::WINDOW_W / 2.f - fb.size.x / 2.f, startY + 20.f));
  window.draw(footer);

  // map tip
  sf::Text tip(font,
               sf::String(L"Tip: map must be 28 cols x 31 rows with at least "
                          L"one dot and one P spawn to save"),
               11);
  tip.setFillColor(sf::Color(150, 150, 150));
  auto tipb = tip.getLocalBounds();
  tip.setPosition(
      sf::Vector2f(Config::WINDOW_W / 2.f - tipb.size.x / 2.f, startY + 45.f));
  window.draw(tip);
}
