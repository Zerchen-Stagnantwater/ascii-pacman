#pragma once
#include "../core/Components.hpp"
#include "../map/Map.hpp"
#include <entt/entt.hpp>

class CollisionSystem {
public:
  // returns points earned this frame
  static int update(entt::registry &registry, Map &map, int &ghostCombo);
};
