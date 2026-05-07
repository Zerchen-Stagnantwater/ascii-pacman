#include "MovementSystem.hpp"
#include "../core/Constants.hpp"
#include <algorithm>
#include <vector>

bool MovementSystem::tryMove(const Map &map, int row, int col, Direction dir,
                             int &outRow, int &outCol) {
  outRow = row;
  outCol = col;
  if (dir == Direction::Up)
    outRow--;
  if (dir == Direction::Down)
    outRow++;
  if (dir == Direction::Left)
    outCol--;
  if (dir == Direction::Right)
    outCol++;
  // tunnel
  if (outCol < 0)
    outCol = Map::COLS - 1;
  if (outCol >= Map::COLS)
    outCol = 0;
  return !map.isWall(outRow, outCol);
}

void MovementSystem::update(entt::registry &registry, const Map &map,
                            float dt) {
  // collect current ghost positions to prevent overlap
  std::vector<std::pair<int, int>> ghostPositions;
  registry.view<Position, TagGhost>().each(
      [&](auto, auto &pos) { ghostPositions.push_back({pos.row, pos.col}); });

  auto view = registry.view<Position, Velocity>();
  for (auto [entity, pos, vel] : view.each()) {
    vel.moveTimer += dt;
    if (vel.moveTimer < vel.speed)
      continue;
    vel.moveTimer = 0.f;

    int nr, nc;
    if (vel.nextDir != Direction::None &&
        tryMove(map, pos.row, pos.col, vel.nextDir, nr, nc)) {
      vel.dir = vel.nextDir;
      vel.nextDir = Direction::None;
    }

    if (vel.dir == Direction::None)
      continue;
    if (!tryMove(map, pos.row, pos.col, vel.dir, nr, nc))
      continue;

    // ghosts check for other ghosts in target cell
    bool isGhost = registry.all_of<TagGhost>(entity);
    if (isGhost) {
      bool occupied = false;
      registry.view<Position, TagGhost>().each([&](auto other, auto &otherPos) {
        if (other == entity)
          return;
        if (otherPos.row == nr && otherPos.col == nc)
          occupied = true;
      });
      if (occupied)
        continue;
    }

    // update ghost position table
    if (isGhost) {
      auto it = std::find(ghostPositions.begin(), ghostPositions.end(),
                          std::make_pair(pos.row, pos.col));
      if (it != ghostPositions.end())
        *it = {nr, nc};
    }

    pos.row = nr;
    pos.col = nc;
  }
}
