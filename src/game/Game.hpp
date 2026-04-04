#pragma once
#include "../core/Components.hpp"
#include "../core/Direction.hpp"
#include "../map/Map.hpp"
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

enum class GameStatus { StartScreen, Playing, Dead, Respawn, GameOver, Win };

class Game {
public:
  Game();

  void handleInput(sf::Keyboard::Key key, bool pressed);
  void update(float dt);
  void reset();

  entt::registry &getRegistry() { return registry; }
  const Map &getMap() const { return map; }
  Map &getMapMut() { return map; }
  int getScore() const { return score; }
  int getLives() const { return lives; }
  int getHighScore() const { return highScore; }
  GameStatus getStatus() const { return status; }
  void addScore(int points) { score += points; }

private:
  entt::registry registry;
  Map map;
  int score = 0;
  int lives = 3;
  int highScore = 0;
  GameStatus status = GameStatus::StartScreen;
  float respawnTimer = 0.f;

  void initEntities();
  void spawnPacman();
  void spawnGhosts();
  void spawnDots();
  void saveHighScore();
  void loadHighScore();
};
