#include "AnimationSystem.hpp"

void AnimationSystem::update(entt::registry &registry, float dt) {
  // Pacman mouth + direction glyph
  registry.view<Animated, Renderable, Velocity, TagPacman>().each(
      [&](auto, auto &anim, auto &render, auto &vel) {
        anim.timer += dt;
        if (anim.timer >= 0.1f) {
          anim.timer = 0.f;
          anim.mouthOpen = !anim.mouthOpen;
        }

        if (vel.dir == Direction::None) {
          render.glyph = L'C';
          return;
        }

        if (anim.mouthOpen) {
          switch (vel.dir) {
          case Direction::Right:
            render.glyph = L'>';
            break;
          case Direction::Left:
            render.glyph = L'<';
            break;
          case Direction::Up:
            render.glyph = L'v';
            break;
          case Direction::Down:
            render.glyph = L'^';
            break;
          default:
            render.glyph = L'C';
            break;
          }
        } else {
          render.glyph = L'-'; // mouth closed
        }
      });

  // Ghost death flash
  registry.view<Flashing, Renderable>().each(
      [&](auto entity, auto &flash, auto &render) {
        flash.timer -= dt;
        flash.flashTimer += dt;

        if (flash.flashTimer >= flash.flashRate) {
          flash.flashTimer = 0.f;
          flash.visible = !flash.visible;
        }

        if (flash.timer <= 0.f) {
          // flash done — destroy the ghost
          registry.destroy(entity);
        }
      });
}
