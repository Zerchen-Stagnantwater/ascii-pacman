#include "ThemeLoader.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

sf::Color ThemeLoader::parseColor(const std::vector<int> &rgb) {
  return sf::Color(rgb[0], rgb[1], rgb[2]);
}

Theme ThemeLoader::load(const std::string &path) {
  std::ifstream f(path);
  if (!f.is_open())
    throw std::runtime_error("Cannot open theme: " + path);

  json j;
  f >> j;

  Theme t;
  t.name = j["name"];

  auto c = [&](const std::string &key) {
    return parseColor(j[key].get<std::vector<int>>());
  };

  t.backgroundColor = c("backgroundColor");
  t.wallColor = c("wallColor");
  t.pacmanColor = c("pacmanColor");
  t.dotColor = c("dotColor");
  t.pelletColor = c("pelletColor");
  t.blinkyColor = c("blinkyColor");
  t.pinkyColor = c("pinkyColor");
  t.inkyColor = c("inkyColor");
  t.clydeColor = c("clydeColor");
  t.frightenedColor = c("frightenedColor");
  t.frightenedWarnColor = c("frightenedWarnColor");
  t.deadGhostColor = c("deadGhostColor");
  t.hudPanelColor = c("hudPanelColor");
  t.hudTextColor = c("hudTextColor");
  t.hudScoreColor = c("hudScoreColor");
  t.hudHighScoreColor = c("hudHighScoreColor");
  t.hudHeartColor = c("hudHeartColor");
  t.hudLevelColor = c("hudLevelColor");
  t.titleColor = c("titleColor");
  t.subtitleColor = c("subtitleColor");
  t.promptColor = c("promptColor");
  t.gameOverColor = c("gameOverColor");
  t.winColor = c("winColor");

  return t;
}

std::vector<Theme> ThemeLoader::loadAll(const std::string &themesPath) {
  std::vector<Theme> themes;
  if (!fs::exists(themesPath)) {
    themes.push_back(classic());
    themes.push_back(midnight());
    themes.push_back(retro());
    return themes;
  }

  // load in fixed order
  std::vector<std::string> order = {"classic.json", "midnight.json",
                                    "retro.json"};
  for (auto &name : order) {
    std::string full = themesPath + "/" + name;
    try {
      themes.push_back(load(full));
    } catch (...) {
      std::cerr << "Failed to load theme: " << name << "\n";
    }
  }

  if (themes.empty()) {
    themes.push_back(classic());
    themes.push_back(midnight());
    themes.push_back(retro());
  }
  return themes;
}

Theme ThemeLoader::classic() {
  Theme t;
  t.name = "Classic";
  t.backgroundColor = sf::Color::Black;
  t.wallColor = sf::Color(0, 100, 255);
  t.pacmanColor = sf::Color::Yellow;
  t.dotColor = sf::Color(200, 200, 200);
  t.pelletColor = sf::Color::White;
  t.blinkyColor = sf::Color(255, 0, 0);
  t.pinkyColor = sf::Color(255, 184, 255);
  t.inkyColor = sf::Color(0, 255, 255);
  t.clydeColor = sf::Color(255, 184, 82);
  t.frightenedColor = sf::Color(0, 0, 200);
  t.frightenedWarnColor = sf::Color::White;
  t.deadGhostColor = sf::Color(100, 180, 255);
  t.hudPanelColor = sf::Color(20, 20, 40);
  t.hudTextColor = sf::Color::White;
  t.hudScoreColor = sf::Color::White;
  t.hudHighScoreColor = sf::Color::Yellow;
  t.hudHeartColor = sf::Color::Red;
  t.hudLevelColor = sf::Color(150, 150, 255);
  t.titleColor = sf::Color::Yellow;
  t.subtitleColor = sf::Color::White;
  t.promptColor = sf::Color::Cyan;
  t.gameOverColor = sf::Color::Red;
  t.winColor = sf::Color::Yellow;
  return t;
}

Theme ThemeLoader::midnight() {
  Theme t;
  t.name = "Midnight";
  t.backgroundColor = sf::Color(5, 0, 20);
  t.wallColor = sf::Color(120, 0, 200);
  t.pacmanColor = sf::Color(0, 255, 255);
  t.dotColor = sf::Color(150, 100, 200);
  t.pelletColor = sf::Color(200, 150, 255);
  t.blinkyColor = sf::Color(255, 50, 100);
  t.pinkyColor = sf::Color(255, 100, 200);
  t.inkyColor = sf::Color(100, 200, 255);
  t.clydeColor = sf::Color(255, 150, 50);
  t.frightenedColor = sf::Color(80, 0, 120);
  t.frightenedWarnColor = sf::Color(200, 100, 255);
  t.deadGhostColor = sf::Color(100, 50, 150);
  t.hudPanelColor = sf::Color(10, 0, 30);
  t.hudTextColor = sf::Color(200, 150, 255);
  t.hudScoreColor = sf::Color(200, 150, 255);
  t.hudHighScoreColor = sf::Color(255, 200, 100);
  t.hudHeartColor = sf::Color(255, 50, 150);
  t.hudLevelColor = sf::Color(150, 100, 255);
  t.titleColor = sf::Color(200, 100, 255);
  t.subtitleColor = sf::Color(150, 100, 200);
  t.promptColor = sf::Color(0, 255, 255);
  t.gameOverColor = sf::Color(255, 50, 100);
  t.winColor = sf::Color(200, 100, 255);
  return t;
}

Theme ThemeLoader::retro() {
  Theme t;
  t.name = "Retro";
  t.backgroundColor = sf::Color(0, 10, 0);
  t.wallColor = sf::Color(0, 200, 0);
  t.pacmanColor = sf::Color::White;
  t.dotColor = sf::Color(0, 150, 0);
  t.pelletColor = sf::Color(0, 255, 0);
  t.blinkyColor = sf::Color(0, 220, 0);
  t.pinkyColor = sf::Color(100, 255, 100);
  t.inkyColor = sf::Color(0, 180, 100);
  t.clydeColor = sf::Color(150, 255, 50);
  t.frightenedColor = sf::Color(0, 80, 0);
  t.frightenedWarnColor = sf::Color(0, 255, 0);
  t.deadGhostColor = sf::Color(0, 100, 0);
  t.hudPanelColor = sf::Color(0, 20, 0);
  t.hudTextColor = sf::Color(0, 200, 0);
  t.hudScoreColor = sf::Color(0, 200, 0);
  t.hudHighScoreColor = sf::Color(0, 255, 0);
  t.hudHeartColor = sf::Color(0, 200, 0);
  t.hudLevelColor = sf::Color(0, 180, 0);
  t.titleColor = sf::Color(0, 255, 0);
  t.subtitleColor = sf::Color(0, 200, 0);
  t.promptColor = sf::Color::White;
  t.gameOverColor = sf::Color(0, 255, 0);
  t.winColor = sf::Color(0, 255, 0);
  return t;
}
