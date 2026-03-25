#pragma once
#include <entt/entt.hpp>
#include "../core/Components.hpp"

class AnimationSystem {
public:
    static void update(entt::registry& registry, float dt);
};
