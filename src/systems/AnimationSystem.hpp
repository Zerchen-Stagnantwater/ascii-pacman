#pragma once
#include "../core/Components.hpp"
#include "../core/Direction.hpp"
#include <entt/entt.hpp>

class AnimationSystem {
public:
  static void update(entt::registry &registry, float dt, float powerTimeLeft);
};
