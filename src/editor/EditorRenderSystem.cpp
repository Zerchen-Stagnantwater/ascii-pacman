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
  sf::Text hints(
      font, "Tab=cycle  W/D/O/E=tile  1-5=spawn  S=save  ESC=quit  RMB=erase",
      11);
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
}
