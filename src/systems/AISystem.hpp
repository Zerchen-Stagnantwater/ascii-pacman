#pragma once
#include "../core/Components.hpp"
#include "../map/Map.hpp"
#include <entt/entt.hpp>

class AISystem {
public:
  static void update(entt::registry &registry, const Map &map, float dt);
};
