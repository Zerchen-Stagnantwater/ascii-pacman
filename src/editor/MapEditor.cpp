#include "MapEditor.hpp"
#include "../core/Constants.hpp"
#include "../editor/MapValidator.hpp"
#include "../map/Map.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

MapEditor::MapEditor() { initBlankMap(); }

void MapEditor::initBlankMap() {
  layout.clear();
  for (int r = 0; r < Map::ROWS; r++) {
    std::string row(Map::COLS, ' ');
    // border walls
    if (r == 0 || r == Map::ROWS - 1) {
      row = std::string(Map::COLS, '#');
    } else {
      row[0] = '#';
      row[Map::COLS - 1] = '#';
    }
    layout.push_back(row);
  }
}

char MapEditor::tileToChar(TileType tile) const {
  switch (tile) {
  case TileType::Wall:
    return '#';
  case TileType::Dot:
    return '.';
  case TileType::Pellet:
    return 'o';
  case TileType::Empty:
    return ' ';
  case TileType::SpawnPacman:
    return 'P';
  case TileType::SpawnBlinky:
    return 'B';
  case TileType::SpawnPinky:
    return 'p';
  case TileType::SpawnInky:
    return 'i';
  case TileType::SpawnClyde:
    return 'c';
  default:
    return ' ';
  }
}

void MapEditor::setTile(int row, int col, TileType tile) {
  if (row < 0 || row >= Map::ROWS || col < 0 || col >= Map::COLS)
    return;
  if ((size_t)col >= layout[row].size())
    layout[row].resize(Map::COLS, ' ');

  char ch = tileToChar(tile);

  // remove existing spawn of same type
  if (tile == TileType::SpawnPacman || tile == TileType::SpawnBlinky ||
      tile == TileType::SpawnPinky || tile == TileType::SpawnInky ||
      tile == TileType::SpawnClyde) {
    for (auto &r : layout)
      for (auto &c : r)
        if (c == ch)
          c = ' ';
  }

  layout[row][col] = (tile == TileType::Empty) ? ' ' : ch;
}

void MapEditor::handleMouseClick(int mouseX, int mouseY, bool rightClick) {
  // account for HUD offset
  int adjustedY = mouseY - 40;
  int col = mouseX / Config::CELL_SIZE;
  int row = adjustedY / Config::CELL_SIZE;

  if (row < 0 || row >= Map::ROWS || col < 0 || col >= Map::COLS)
    return;

  if (rightClick) {
    setTile(row, col, TileType::Empty);
  } else {
    setTile(row, col, currentTile);
  }
}

void MapEditor::handleKey(sf::Keyboard::Key key) {
  if (key == sf::Keyboard::Key::Escape) {
    quit = true;
    return;
  }

  if (key == sf::Keyboard::Key::H) {
    showHelp = !showHelp;
    return;
  }

  if (key == sf::Keyboard::Key::S) {
    save();
    return;
  }

  // tile selection
  if (key == sf::Keyboard::Key::Tab) {
    int next = ((int)currentTile + 1) % 9;
    currentTile = (TileType)next;
    return;
  }

  // number keys for spawns
  if (key == sf::Keyboard::Key::Num1)
    currentTile = TileType::SpawnPacman;
  if (key == sf::Keyboard::Key::Num2)
    currentTile = TileType::SpawnBlinky;
  if (key == sf::Keyboard::Key::Num3)
    currentTile = TileType::SpawnPinky;
  if (key == sf::Keyboard::Key::Num4)
    currentTile = TileType::SpawnInky;
  if (key == sf::Keyboard::Key::Num5)
    currentTile = TileType::SpawnClyde;

  // tile type keys
  if (key == sf::Keyboard::Key::W)
    currentTile = TileType::Wall;
  if (key == sf::Keyboard::Key::D)
    currentTile = TileType::Dot;
  if (key == sf::Keyboard::Key::O)
    currentTile = TileType::Pellet;
  if (key == sf::Keyboard::Key::E)
    currentTile = TileType::Empty;
}

void MapEditor::update(float dt) {
  if (messageTimer > 0.f) {
    messageTimer -= dt;
    if (messageTimer <= 0.f)
      saveMessage = "";
  }
}

std::string MapEditor::generateSaveName() const {
  std::string dir = "assets/maps";
  int idx = 1;
  while (fs::exists(dir + "/custom_" + std::to_string(idx) + ".txt"))
    idx++;
  return dir + "/custom_" + std::to_string(idx) + ".txt";
}

bool MapEditor::save() {
  auto result = MapValidator::validate(layout);

  if (result.hasErrors()) {
    saveMessage = "Cannot save: " + result.errors[0];
    messageTimer = 3.f;
    justSaved = false;
    return false;
  }

  if (result.hasWarnings()) {
    for (auto &w : result.warnings)
      std::cerr << "[EDITOR WARN] " << w << "\n";
  }

  std::string path = generateSaveName();
  std::ofstream f(path);
  if (!f.is_open()) {
    saveMessage = "Failed to open file for writing";
    messageTimer = 3.f;
    return false;
  }

  for (auto &row : layout)
    f << row << "\n";

  saveMessage = "Saved: " + path;
  messageTimer = 3.f;
  justSaved = true;
  std::cout << "[EDITOR] Map saved to " << path << "\n";
  return true;
}
