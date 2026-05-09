#pragma once
#include "../core/Components.hpp"
#include "../core/Direction.hpp"
#include "../core/Theme.hpp"
#include "../core/ThemeLoader.hpp"
#include "../map/Map.hpp"
#include "../map/MapLoader.hpp"
#include "../systems/AudioSystem.hpp"
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <entt/process/process.hpp>

enum class GameStatus {
  StartScreen,
  Playing,
  Dead,
  Respawn,
  GameOver,
  Win,
  Paused,
  MapEditor
};

class Game {
public:
  Game();

  void handleInput(sf::Keyboard::Key key, bool pressed);
  void update(float dt);
  void reset();
  float getPowerTimeLeft();

  entt::registry &getRegistry() { return registry; }
  const Map &getMap() const { return map; }
  Map &getMapMut() { return map; }
  int getScore() const { return score; }
  int getLives() const { return lives; }
  int getHighScore() const { return highScore; }
  int getCountdown() const { return (int)std::ceil(respawnTimer); }
  int getLevel() const { return level; }
  int &getGhostCombo() { return ghostCombo; }
  GameStatus getStatus() const { return status; }
  void addScore(int points) { score += points; }
  bool isRespawning() const { return status == GameStatus::Respawn; }
  AudioSystem &getAudio() { return audio; }
  const Theme &getTheme() const { return currentTheme; }
  void setTheme(const Theme &t) { currentTheme = t; }
  const std::vector<Theme> &getThemes() const { return themes; }
  int getSelectedThemeIndex() const { return selectedThemeIndex; }
  void enterEditor() { status = GameStatus::MapEditor; }
  void exitEditor() { status = GameStatus::StartScreen; }
  void nextTheme();
  void prevTheme();

private:
  entt::registry registry;
  Map map;
  SpawnPoints spawns;
  int score = 0;
  int lives = 3;
  int highScore = 0;
  int level = 1;
  float levelTimer = 0.f;
  bool levelTransition = false;
  bool extraLifeAwarded = false;
  GameStatus status = GameStatus::StartScreen;
  float respawnTimer = 0.f;
  int ghostCombo = 1; // multiplier: 1=200, 2=400, 3=800, 4=1600
  float countdownTimer = 0.f;
  int countdownVal = 2;
  entt::entity readyEntity = entt::null;
  entt::entity countdownEntity = entt::null;
  GameStatus prePauseStatus = GameStatus::Playing;
  AudioSystem audio;
  std::vector<Theme> themes;
  Theme currentTheme;
  int selectedThemeIndex = 0;

  void initEntities();
  void spawnPacman();
  void spawnGhosts();
  void spawnDots();
  void spawnReadyText();
  void saveHighScore();
  void loadHighScore();
  void clearUIEntities();

  void nextLevel();
  float pacmanSpeed() const;
  float ghostSpeed() const;
};
