#pragma once
#include "../core/Theme.hpp"
#include "../editor/MapEditor.hpp"
#include <SFML/Graphics.hpp>

class EditorRenderSystem {
public:
  EditorRenderSystem(sf::RenderWindow &window);
  bool loadFont(const std::string &path);
  void draw(const MapEditor &editor, const Theme &theme);

private:
  sf::RenderWindow &window;
  sf::Font font;

  void drawGrid(const MapEditor &editor, const Theme &theme);
  void drawToolbar(const MapEditor &editor, const Theme &theme);
  void drawMessage(const std::string &msg, const Theme &theme);
  sf::Color tileColor(TileType tile, const Theme &theme) const;
  wchar_t tileGlyph(TileType tile) const;
};
