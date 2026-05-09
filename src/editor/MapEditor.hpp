#pragma once
#include "../core/Theme.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class TileType {
  Wall,
  Dot,
  Pellet,
  Empty,
  SpawnPacman,
  SpawnBlinky,
  SpawnPinky,
  SpawnInky,
  SpawnClyde
};

class MapEditor {
public:
  MapEditor();
  bool showHelp = true; // show on open by default
  void handleMouseClick(int mouseX, int mouseY, bool rightClick);
  void handleKey(sf::Keyboard::Key key);
  void update(float dt);

  bool wantsToQuit() const { return quit; }
  bool wantsToSave() const { return saveRequested; }
  bool saved() const { return justSaved; }
  std::string getSaveMessage() const { return saveMessage; }

  void clearSaveFlag() {
    justSaved = false;
    saveMessage = "";
  }

  const std::vector<std::string> &getLayout() const { return layout; }
  TileType getCurrentTile() const { return currentTile; }

  bool save();

private:
  std::vector<std::string> layout;
  TileType currentTile = TileType::Wall;
  bool quit = false;
  bool saveRequested = false;
  bool justSaved = false;
  std::string saveMessage;
  float messageTimer = 0.f;

  void setTile(int row, int col, TileType tile);
  char tileToChar(TileType tile) const;
  std::string generateSaveName() const;
  void initBlankMap();
};
