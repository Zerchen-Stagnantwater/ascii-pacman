#include "AISystem.hpp"
#include "../core/Direction.hpp"
#include <cmath>
#include <cstdlib>
#include <limits>

static Direction bestDirection(const Map &map, int row, int col, int targetRow,
                               int targetCol, Direction current) {
  Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left,
                      Direction::Right};
  Direction best = Direction::None;
  float bestDist = std::numeric_limits<float>::max();

  for (auto d : dirs) {
    if (d == opposite(current))
      continue; // no reversing
    int nr = row, nc = col;
    if (d == Direction::Up)
      nr--;
    if (d == Direction::Down)
      nr++;
    if (d == Direction::Left)
      nc--;
    if (d == Direction::Right)
      nc++;
    if (map.isWall(nr, nc))
      continue;

    float dist = std::hypot((float)(nr - targetRow), (float)(nc - targetCol));
    if (dist < bestDist) {
      bestDist = dist;
      best = d;
    }
  }

  // fallback — pick any valid direction
  if (best == Direction::None) {
    for (auto d : dirs) {
      int nr = row, nc = col;
      if (d == Direction::Up)
        nr--;
      if (d == Direction::Down)
        nr++;
      if (d == Direction::Left)
        nc--;
      if (d == Direction::Right)
        nc++;
      if (!map.isWall(nr, nc)) {
        best = d;
        break;
      }
    }
  }

  return best;
}

static Direction randomDirection(const Map &map, int row, int col,
                                 Direction current) {
  Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left,
                      Direction::Right};
  // shuffle
  for (int i = 3; i > 0; i--) {
    int j = rand() % (i + 1);
    std::swap(dirs[i], dirs[j]);
  }
  for (auto d : dirs) {
    if (d == opposite(current))
      continue;
    int nr = row, nc = col;
    if (d == Direction::Up)
      nr--;
    if (d == Direction::Down)
      nr++;
    if (d == Direction::Left)
      nc--;
    if (d == Direction::Right)
      nc++;
    if (!map.isWall(nr, nc))
      return d;
  }
  return current;
}

void AISystem::update(entt::registry &registry, const Map &map, float dt) {
  // ghost house exit logic
  registry.view<GhostHouse, Position, Velocity, GhostAI>().each(
      [&](auto, auto &house, auto &pos, auto &vel, auto &ai) {
        if (house.exited)
          return;

        house.timer += dt;
        if (house.timer >= house.exitDelay) {
          house.exited = true;
          // move to just outside house
          pos.row = 11;
          pos.col = 14;
          vel.dir = Direction::Left;
          ai.mode = GhostMode::Scatter;
          ai.modeTimer = 0.f;
        } else {
          // bounce inside house while waiting
          vel.dir = Direction::None;
        }
        return;
      });
  // get pacman position and direction
  int pacRow = 0, pacCol = 0;
  Direction pacDir = Direction::None;
  registry.view<Position, Velocity, TagPacman>().each(
      [&](auto, auto &pos, auto &vel) {
        pacRow = pos.row;
        pacCol = pos.col;
        pacDir = vel.dir;
      });

  // get Blinky position for Inky's targeting
  int blinkyRow = 0, blinkyCol = 0;
  registry.view<Position, GhostAI>().each([&](auto, auto &pos, auto &ai) {
    if (ai.personality == GhostPersonality::Blinky) {
      blinkyRow = pos.row;
      blinkyCol = pos.col;
    }
  });

  registry.view<Position, Velocity, GhostAI>().each([&](auto entity, auto &pos,
                                                        auto &vel, auto &ai) {
    if (registry.all_of<Flashing>(entity))
      return; // dead, skip

    // scatter/chase cycle timer (only in non-frightened)
    if (ai.mode != GhostMode::Frightened && ai.mode != GhostMode::Dead) {
      ai.modeTimer += dt;
      if (ai.mode == GhostMode::Scatter && ai.modeTimer > 7.f) {
        ai.mode = GhostMode::Chase;
        ai.modeTimer = 0.f;
        vel.dir = opposite(vel.dir); // reverse on mode switch
      } else if (ai.mode == GhostMode::Chase && ai.modeTimer > 20.f) {
        ai.mode = GhostMode::Scatter;
        ai.modeTimer = 0.f;
        vel.dir = opposite(vel.dir);
      }
    }

    int targetRow = ai.scatterRow;
    int targetCol = ai.scatterCol;

    if (ai.mode == GhostMode::Frightened) {
      vel.dir = randomDirection(map, pos.row, pos.col, vel.dir);
      return;
    }

    if (ai.mode == GhostMode::Chase) {
      switch (ai.personality) {

      case GhostPersonality::Blinky:
        // direct chase
        targetRow = pacRow;
        targetCol = pacCol;
        break;

      case GhostPersonality::Pinky:
        // 4 tiles ahead of Pacman
        targetRow = pacRow;
        targetCol = pacCol;
        if (pacDir == Direction::Up)
          targetRow -= 4;
        if (pacDir == Direction::Down)
          targetRow += 4;
        if (pacDir == Direction::Left)
          targetCol -= 4;
        if (pacDir == Direction::Right)
          targetCol += 4;
        break;

      case GhostPersonality::Inky: {
        // midpoint between 2 tiles ahead of Pacman and Blinky, reflected
        int midRow = pacRow, midCol = pacCol;
        if (pacDir == Direction::Up)
          midRow -= 2;
        if (pacDir == Direction::Down)
          midRow += 2;
        if (pacDir == Direction::Left)
          midCol -= 2;
        if (pacDir == Direction::Right)
          midCol += 2;
        targetRow = midRow + (midRow - blinkyRow);
        targetCol = midCol + (midCol - blinkyCol);
        break;
      }

      case GhostPersonality::Clyde: {
        // chase when far (>8 tiles), scatter when close
        float dist =
            std::hypot((float)(pos.row - pacRow), (float)(pos.col - pacCol));
        if (dist > 8.f) {
          targetRow = pacRow;
          targetCol = pacCol;
        } else {
          targetRow = ai.scatterRow;
          targetCol = ai.scatterCol;
        }
        break;
      }
      }
    }

    vel.dir =
        bestDirection(map, pos.row, pos.col, targetRow, targetCol, vel.dir);
  });
}
