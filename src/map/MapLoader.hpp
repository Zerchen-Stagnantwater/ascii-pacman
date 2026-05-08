#pragma once
#include <string>
#include <vector>

struct SpawnPoints {
  int pacmanRow = 23, pacmanCol = 14;
  int blinkyRow = 11, blinkyCol = 14;
  int pinkyRow = 14, pinkyCol = 14;
  int inkyRow = 14, inkyCol = 13;
  int clydeRow = 14, clydeCol = 15;
};

struct MapData {
  std::vector<std::string> layout;
  SpawnPoints spawns;
};

class MapLoader {
public:
  static MapData load(const std::string &path);
  static MapData classic();
  static MapData random();

private:
  static std::vector<std::string> getAvailableMaps();
  static MapData parse(std::vector<std::string> raw);
};
