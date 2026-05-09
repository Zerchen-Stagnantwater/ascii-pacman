#include "AnimationSystem.hpp"
#include "../core/Constants.hpp"

void AnimationSystem::update(entt::registry &registry, float dt,
                             float powerTimeLeft) {

  // Pacman death animation
  registry.view<Dying, Renderable, Velocity>().each(
      [&](auto, auto &dying, auto &render, auto &vel) {
        vel.dir = Direction::None;
        vel.nextDir = Direction::None;
        dying.timer += dt;

        float frameDur = dying.duration / 6.f;
        dying.frame = (int)(dying.timer / frameDur);

        wchar_t frames[] = {L'C', L'c', L'(', L')', L'|', L' '};
        if (dying.frame >= 6) {
          dying.done = true;
          render.glyph = L' ';
        } else {
          render.glyph = frames[dying.frame];
          render.color =
              sf::Color(255, 255 - dying.frame * 40, 0); // fade to red
        }
      });

  // Pacman normal mouth + direction (skip if dying)
  registry.view<Animated, Renderable, Velocity, TagPacman>().each(
      [&](auto entity, auto &anim, auto &render, auto &vel) {
        if (registry.all_of<Dying>(entity))
          return;
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
          render.glyph = L'-';
        }
      });

  // Power pellet pulsing
  registry.view<Pulsing, Renderable>().each(
      [&](auto, auto &pulse, auto &render) {
        pulse.timer += dt;
        if (pulse.timer >= pulse.rate) {
          pulse.timer = 0.f;
          pulse.bright = !pulse.bright;
        }
        render.color = pulse.bright ? sf::Color::White : sf::Color(80, 80, 80);
      });

  // Ghost power warning flash
  bool warning =
      powerTimeLeft > 0.f && powerTimeLeft <= Config::POWER_WARNING_TIME;

  registry.view<GhostAI>().each([&](auto entity, auto &ai) {
    if (ai.mode != GhostMode::Frightened) {
      if (registry.all_of<FrightenedWarning>(entity))
        registry.remove<FrightenedWarning>(entity);
      return;
    }
    if (warning) {
      if (!registry.all_of<FrightenedWarning>(entity))
        registry.emplace<FrightenedWarning>(entity);
    } else {
      if (registry.all_of<FrightenedWarning>(entity))
        registry.remove<FrightenedWarning>(entity);
    }
  });

  // Ghost death flash
  registry.view<Flashing>().each([&](auto entity, auto &flash) {
    flash.timer -= dt;
    flash.flashTimer += dt;
    if (flash.flashTimer >= flash.flashRate) {
      flash.flashTimer = 0.f;
      flash.visible = !flash.visible;
    }
    if (flash.timer <= 0.f) {
      if (registry.valid(entity) &&
          registry.all_of<GhostHouse, Position, Velocity, GhostAI>(entity)) {
        auto &house = registry.get<GhostHouse>(entity);
        auto &pos = registry.get<Position>(entity);
        auto &vel = registry.get<Velocity>(entity);
        auto &ai = registry.get<GhostAI>(entity);
        pos.row = house.homeRow;
        pos.col = house.homeCol;
        vel.dir = Direction::Left;
        vel.nextDir = Direction::None;
        ai.mode = GhostMode::Scatter;
        ai.modeTimer = 0.f;
        house.exited = false;
        house.timer = 0.f;
        registry.remove<Flashing>(entity);
      }
    }
  });

  // Blinking text
  registry.view<BlinkingText>().each([&](auto entity, auto &blink) {
    blink.blinkTimer += dt;
    if (blink.blinkTimer >= blink.blinkRate) {
      blink.blinkTimer = 0.f;
      blink.visible = !blink.visible;
    }
    if (blink.lifetime > 0.f) {
      blink.lifetime -= dt;
      if (blink.lifetime <= 0.f)
        registry.destroy(entity);
    }
  });
}
