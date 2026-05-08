#include "CollisionSystem.hpp"
#include "../core/Constants.hpp"

int CollisionSystem::update(entt::registry &registry, Map &map, int &ghostCombo,
                            AudioSystem &audio) {
  int earned = 0;

  registry.view<Position, TagPacman>().each([&](auto pacEntity, auto &pacPos) {
    std::vector<entt::entity> toDestroy;
    registry.view<Position, Collectible, TagDot>().each(
        [&](auto dotEntity, auto &dotPos, auto &collect) {
          if (pacPos.row == dotPos.row && pacPos.col == dotPos.col) {
            earned += collect.points;
            if (collect.isPellet) {
              registry.emplace_or_replace<Powered>(pacEntity,
                                                   Config::POWER_DURATION);
              ghostCombo = 1;
              audio.play(SoundId::Power);
            } else {
              audio.play(SoundId::Waka);
            }
            toDestroy.push_back(dotEntity);
            map.setCell(dotPos.row, dotPos.col, Cell::Empty);
          }
        });
    for (auto e : toDestroy)
      registry.destroy(e);

    registry.view<Position, GhostAI>().each(
        [&](auto ghostEntity, auto &ghostPos, auto &ai) {
          if (ai.mode != GhostMode::Frightened)
            return;
          if (pacPos.row == ghostPos.row && pacPos.col == ghostPos.col) {
            int points = Config::GHOST_COMBO_BASE * ghostCombo;
            earned += points;
            ghostCombo = std::min(ghostCombo * 2, 8);

            auto popup = registry.create();
            registry.emplace<Position>(popup, ghostPos.row, ghostPos.col);
            registry.emplace<ScorePopup>(popup, "+" + std::to_string(points),
                                         1.2f, 1.2f);

            ai.mode = GhostMode::Dead;
            registry.emplace_or_replace<Flashing>(ghostEntity);
            audio.play(SoundId::GhostEat);
          }
        });
  });

  return earned;
}
