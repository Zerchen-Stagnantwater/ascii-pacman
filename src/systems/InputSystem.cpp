#include "InputSystem.hpp"

void InputSystem::update(entt::registry& registry, Direction input) {
    if (input == Direction::None) return;

    auto view = registry.view<Velocity, PlayerInput>();
    for (auto [entity, vel, inp] : view.each()) {
        vel.nextDir = input;
    }
}
