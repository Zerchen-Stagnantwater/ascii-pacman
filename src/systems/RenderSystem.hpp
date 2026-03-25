#pragma once
#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include "../core/Components.hpp"
#include "../map/Map.hpp"

class RenderSystem {
public:
    RenderSystem(sf::RenderWindow& window);
    bool loadFont(const std::string& path);

    void drawMap(const Map& map);
    void drawEntities(entt::registry& registry);
    void drawHUD(int score, int lives, bool powered);
    void drawMessage(const std::string& msg);

private:
    sf::RenderWindow& window;
    sf::Font font;

    sf::String getWallChar(const Map& map, int row, int col) const;
};
