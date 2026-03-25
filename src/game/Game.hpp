#pragma once
#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include "../map/Map.hpp"
#include "../core/Components.hpp"
#include "../core/Direction.hpp"

enum class GameStatus { Playing, Dead, Respawn, GameOver, Win };

class Game {
public:
    Game();

    void handleInput(sf::Keyboard::Key key, bool pressed);
    void update(float dt);

    entt::registry& getRegistry() { return registry; }
    const Map& getMap() const { return map; }
    int getScore() const { return score; }
    int getLives() const { return lives; }
    GameStatus getStatus() const { return status; }

private:
    entt::registry registry;
    Map map;
    int score = 0;
    int lives = 3;
    GameStatus status = GameStatus::Playing;
    float respawnTimer = 0.f;

    void initEntities();
    void spawnPacman();
    void spawnGhosts();
    void spawnDots();
};
