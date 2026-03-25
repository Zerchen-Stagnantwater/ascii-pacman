#pragma once
#include <entt/entt.hpp>
#include "../core/Components.hpp"
#include "../map/Map.hpp"

class MovementSystem {
public:
    static void update(entt::registry& registry, const Map& map, float dt);

private:
    static bool tryMove(const Map& map, int row, int col, Direction dir, int& outRow, int& outCol);
};
