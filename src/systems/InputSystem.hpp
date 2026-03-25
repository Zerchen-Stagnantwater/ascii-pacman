#pragma once
#include <entt/entt.hpp>
#include "../core/Components.hpp"
#include "../core/Direction.hpp"

class InputSystem {
public:
    static void update(entt::registry& registry, Direction input);
};
