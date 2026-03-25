#pragma once
#include <entt/entt.hpp>
#include "../core/Components.hpp"
#include "../map/Map.hpp"

class AISystem {
public:
    static void update(entt::registry& registry, const Map& map, float dt);

private:
    static Direction chase(const Map& map, int ghostRow, int ghostCol,
                           int targetRow, int targetCol, Direction current);
};
