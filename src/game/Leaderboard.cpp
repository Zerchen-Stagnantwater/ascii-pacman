#include "Leaderboard.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Leaderboard::Leaderboard() {}

void Leaderboard::load(const std::string &path) {
  filePath = path;
  std::ifstream f(path);
  if (!f.is_open())
    return;

  try {
    json j;
    f >> j;
    for (auto &e : j["scores"]) {
      entries.push_back({e["name"].get<std::string>(), e["score"].get<int>(),
                         e["level"].get<int>()});
    }
  } catch (...) {
    std::cerr << "[LEADERBOARD] Failed to parse leaderboard file\n";
  }

  // sort descending
  std::sort(entries.begin(), entries.end(),
            [](const LeaderboardEntry &a, const LeaderboardEntry &b) {
              return a.score > b.score;
            });

  // cap at max
  if ((int)entries.size() > MAX_ENTRIES)
    entries.resize(MAX_ENTRIES);
}

void Leaderboard::save(const std::string &path) const {
  json j;
  j["scores"] = json::array();
  for (auto &e : entries) {
    j["scores"].push_back(
        {{"name", e.name}, {"score", e.score}, {"level", e.level}});
  }
  std::ofstream f(path);
  if (!f.is_open()) {
    std::cerr << "[LEADERBOARD] Failed to save leaderboard\n";
    return;
  }
  f << j.dump(2);
}

bool Leaderboard::qualifies(int score) const {
  if (score <= 0)
    return false;
  if ((int)entries.size() < MAX_ENTRIES)
    return true;
  return score > entries.back().score;
}

void Leaderboard::insert(const std::string &name, int score, int level) {
  entries.push_back({name, score, level});
  std::sort(entries.begin(), entries.end(),
            [](const LeaderboardEntry &a, const LeaderboardEntry &b) {
              return a.score > b.score;
            });
  if ((int)entries.size() > MAX_ENTRIES)
    entries.resize(MAX_ENTRIES);
  save(filePath.empty() ? "assets/leaderboard.json" : filePath);
}
