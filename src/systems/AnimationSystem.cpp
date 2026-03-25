#include "AnimationSystem.hpp"

void AnimationSystem::update(entt::registry& registry, float dt) {
    auto view = registry.view<Animated, Renderable, Velocity>();
    for (auto [entity, anim, render, vel] : view.each()) {
        anim.timer += dt;
        if (anim.timer >= 0.1f) {
            anim.timer = 0.f;
            anim.mouthOpen = !anim.mouthOpen;
        }
        // only animate if moving
        if (vel.dir != Direction::None)
            render.glyph = anim.mouthOpen ? L'C' : L'c';
        else
            render.glyph = L'C';
    }
}
