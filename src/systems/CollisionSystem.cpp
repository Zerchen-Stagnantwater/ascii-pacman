#include "CollisionSystem.hpp"
#include "../core/Constants.hpp"

int CollisionSystem::update(entt::registry &registry, Map &map) {
  int earned = 0;

  auto pacView = registry.view<Position, TagPacman>();
  for (auto [pacEntity, pacPos] : pacView.each()) {

    // collect dots
    auto dotView = registry.view<Position, Collectible, TagDot>();
    for (auto [dotEntity, dotPos, collect] : dotView.each()) {
      if (pacPos.row == dotPos.row && pacPos.col == dotPos.col) {
        earned += collect.points;
        if (collect.isPellet)
          registry.emplace_or_replace<Powered>(pacEntity,
                                               Config::POWER_DURATION);
        registry.destroy(dotEntity);
        map.setCell(dotPos.row, dotPos.col, Cell::Empty);
      }
    }

    // ghost collision
    auto ghostView = registry.view<Position, TagGhost, GhostAI>();
    for (auto [ghostEntity, ghostPos, ai] : ghostView.each()) {
      if (pacPos.row == ghostPos.row && pacPos.col == ghostPos.col) {
        if (ai.mode == GhostMode::Frightened) {
          earned += Config::GHOST_POINTS;
          ai.mode = GhostMode::Dead;
          registry.emplace_or_replace<Flashing>(ghostEntity);
        }
        // death handled in Game
      }
    }
  }

  return earned;
}
