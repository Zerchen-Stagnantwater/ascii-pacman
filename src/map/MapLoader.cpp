#include "MapLoader.hpp"
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

std::vector<std::string> MapLoader::load(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open())
    throw std::runtime_error("Could not open map file: " + path);

  std::vector<std::string> layout;
  std::string line;
  while (std::getline(file, line))
    layout.push_back(line);

  return layout;
}

std::vector<std::string> MapLoader::getAvailableMaps() {
  std::vector<std::string> paths;
  std::string mapsDir = "assets/maps";

  if (!fs::exists(mapsDir))
    return paths;

  for (auto &entry : fs::directory_iterator(mapsDir)) {
    if (entry.path().extension() == ".txt")
      paths.push_back(entry.path().string());
  }
  return paths;
}

std::vector<std::string> MapLoader::random() {
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

std::vector<std::string> MapLoader::classic() {
  return {
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
      "#.####.#####.##.#####.####.#", "#o..##................##..o#",
      "###.##.##.########.##.##.###", "###.##.##.########.##.##.###",
      "#......##....##....##......#", "#.##########.##.##########.#",
      "#.##########.##.##########.#", "#..........................#",
      "############################",
  };
}
