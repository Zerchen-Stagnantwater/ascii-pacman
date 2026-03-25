#pragma once
#include <entt/entt.hpp>
#include "../core/Components.hpp"
#include "../map/Map.hpp"

class CollisionSystem {
public:
    // returns points earned this frame
    static int update(entt::registry& registry, Map& map);
};
