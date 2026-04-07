#include "AnimationSystem.hpp"
#include "../core/Constants.hpp"

void AnimationSystem::update(entt::registry &registry, float dt,
                             float powerTimeLeft) {

  // Pacman direction glyph + mouth
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
          render.glyph = L'-';
        }
      });

  // Ghost power warning flash (last 2 seconds)
  bool warning =
      powerTimeLeft > 0.f && powerTimeLeft <= Config::POWER_WARNING_TIME;
  registry.view<GhostAI, Renderable>().each([&](auto entity, auto &ai,
                                                auto &render) {
    if (ai.mode != GhostMode::Frightened)
      return;
    if (!registry.all_of<Flashing>(entity) && warning) {
      // use renderable color to flash between blue and white
      static float warnTimer = 0.f;
      warnTimer += dt;
      if (warnTimer >= 0.2f) {
        warnTimer = 0.f;
        render.color = (render.color == sf::Color::White) ? sf::Color(0, 0, 200)
                                                          : sf::Color::White;
      }
    }
  });

  // Ghost death flash + destroy
  std::vector<entt::entity> toDestroy;
  registry.view<Flashing, Renderable>().each(
      [&](auto entity, auto &flash, auto &render) {
        flash.timer -= dt;
        flash.flashTimer += dt;
        if (flash.flashTimer >= flash.flashRate) {
          flash.flashTimer = 0.f;
          flash.visible = !flash.visible;
        }
        if (flash.timer <= 0.f)
          toDestroy.push_back(entity);
      });
  for (auto e : toDestroy)
    if (registry.valid(e))
      registry.destroy(e);

  // Blinking text (READY!, etc.)
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

  // Score popups float upward (handled via lifetime in Game::update)
}
