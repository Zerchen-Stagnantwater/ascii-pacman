#pragma once
#include "Theme.hpp"
#include <string>
#include <vector>

class ThemeLoader {
public:
  static std::vector<Theme> loadAll(const std::string &themesPath);
  static Theme load(const std::string &path);
  static Theme classic();
  static Theme midnight();
  static Theme retro();

private:
  static sf::Color parseColor(const std::vector<int> &rgb);
};
