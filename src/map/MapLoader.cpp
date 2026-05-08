#include "MapLoader.hpp"
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

MapData MapLoader::parse(std::vector<std::string> raw) {
  MapData data;
  data.layout = raw;

  for (int r = 0; r < (int)raw.size(); r++) {
    for (int c = 0; c < (int)raw[r].size(); c++) {
      char ch = raw[r][c];
      switch (ch) {
      case 'P':
        data.spawns.pacmanRow = r;
        data.spawns.pacmanCol = c;
        data.layout[r][c] = ' ';
        break;
      case 'B':
        data.spawns.blinkyRow = r;
        data.spawns.blinkyCol = c;
        data.layout[r][c] = ' ';
        break;
      case 'p':
        data.spawns.pinkyRow = r;
        data.spawns.pinkyCol = c;
        data.layout[r][c] = ' ';
        break;
      case 'i':
        data.spawns.inkyRow = r;
        data.spawns.inkyCol = c;
        data.layout[r][c] = ' ';
        break;
      case 'c':
        data.spawns.clydeRow = r;
        data.spawns.clydeCol = c;
        data.layout[r][c] = ' ';
        break;
      default:
        break;
      }
    }
  }
  return data;
}

MapData MapLoader::load(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open())
    throw std::runtime_error("Could not open map file: " + path);

  std::vector<std::string> raw;
  std::string line;
  while (std::getline(file, line))
    raw.push_back(line);

  return parse(raw);
}

std::vector<std::string> MapLoader::getAvailableMaps() {
  std::vector<std::string> paths;
  std::string mapsDir = "assets/maps";
  if (!fs::exists(mapsDir))
    return paths;
  for (auto &entry : fs::directory_iterator(mapsDir))
    if (entry.path().extension() == ".txt")
      paths.push_back(entry.path().string());
  return paths;
}

MapData MapLoader::random() {
  auto maps = getAvailableMaps();
  if (maps.empty())
    return classic();

  static bool seeded = false;
  if (!seeded) {
    std::srand((unsigned)std::time(nullptr));
    seeded = true;
  }

  int idx = std::rand() % maps.size();
  try {
    return load(maps[idx]);
  } catch (...) {
    return classic();
  }
}

MapData MapLoader::classic() {
  std::vector<std::string> raw = {
      "############################", "#............##............#",
      "#.####.#####.##.#####.####.#", "#o####.#####.##.#####.####o#",
      "#.####.#####.##.#####.####.#", "#..........................#",
      "#.####.##.########.##.####.#", "#.####.##.########.##.####.#",
      "#......##....##....##......#", "######.##### ## #####.######",
      "######.##### ## #####.######", "######.##          ##.######",
      "######.## ###--### ##.######", "######.## #      # ##.######",
      "      .   #      #   .      ", "######.## #      # ##.######",
      "######.## ######## ##.######", "######.##          ##.######",
      "######.## ######## ##.######", "######.## ######## ##.######",
      "#............##............#", "#.####.#####.##.#####.####.#",
      "#.####.#####.##.#####.####.#", "#o..##....P...........##..o#",
      "###.##.##.########.##.##.###", "###.##.##.########.##.##.###",
      "#......##....##....##......#", "#.##########.##.##########.#",
      "#.##########.##.##########.#", "#..........................#",
      "############################",
  };
  return parse(raw);
}
