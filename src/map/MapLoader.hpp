#pragma once
#include <string>
#include <vector>

class MapLoader {
public:
  static std::vector<std::string> load(const std::string &path);
  static std::vector<std::string> classic();
  static std::vector<std::string>
  random(); // picks a random map from assets/maps/

private:
  static std::vector<std::string> getAvailableMaps();
};
