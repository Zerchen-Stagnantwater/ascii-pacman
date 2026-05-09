#pragma once
#include <string>
#include <vector>

struct LeaderboardEntry {
  std::string name;
  int score;
  int level;
};

class Leaderboard {
public:
  static const int MAX_ENTRIES = 10;

  Leaderboard();
  void load(const std::string &path);
  void save(const std::string &path) const;

  bool qualifies(int score) const;
  void insert(const std::string &name, int score, int level);

  const std::vector<LeaderboardEntry> &getEntries() const { return entries; }

private:
  std::vector<LeaderboardEntry> entries;
  std::string filePath;
};
