#include "MapLoader.hpp"
#include "../editor/MapValidator.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
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

  // validate before parsing
  auto result = MapValidator::validate(raw);
  if (result.hasErrors()) {
    for (auto &e : result.errors)
      std::cerr << "[MAP ERROR] " << e << "\n";
    throw std::runtime_error("Map validation failed: " + path);
  }
  for (auto &w : result.warnings)
    std::cerr << "[MAP WARN] " << w << "\n";

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

  // shuffle and try each one
  std::vector<int> indices(maps.size());
  std::iota(indices.begin(), indices.end(), 0);
  std::shuffle(indices.begin(), indices.end(),
               std::default_random_engine(std::rand()));

  for (int idx : indices) {
    try {
      return load(maps[idx]);
    } catch (...) {
      std::cerr << "[MAP] Skipping invalid map: " << maps[idx] << "\n";
    }
  }

  std::cerr << "[MAP] All maps invalid, falling back to classic\n";
  return classic();
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
