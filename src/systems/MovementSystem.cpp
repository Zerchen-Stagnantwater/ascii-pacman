#include "MovementSystem.hpp"
#include "../core/Constants.hpp"

bool MovementSystem::tryMove(const Map& map, int row, int col, Direction dir, int& outRow, int& outCol) {
    outRow = row;
    outCol = col;
    if (dir == Direction::Up)    outRow--;
    if (dir == Direction::Down)  outRow++;
    if (dir == Direction::Left)  outCol--;
    if (dir == Direction::Right) outCol++;
    // tunnel
    if (outCol < 0)          outCol = Map::COLS - 1;
    if (outCol >= Map::COLS) outCol = 0;
    return !map.isWall(outRow, outCol);
}

void MovementSystem::update(entt::registry& registry, const Map& map, float dt) {
    auto view = registry.view<Position, Velocity>();
    for (auto [entity, pos, vel] : view.each()) {
        vel.moveTimer += dt;
        if (vel.moveTimer < vel.speed) continue;
        vel.moveTimer = 0.f;

        int nr, nc;
        // try queued direction first
        if (vel.nextDir != Direction::None && tryMove(map, pos.row, pos.col, vel.nextDir, nr, nc)) {
            vel.dir = vel.nextDir;
            vel.nextDir = Direction::None;
        }
        // then move in current direction
        if (vel.dir != Direction::None && tryMove(map, pos.row, pos.col, vel.dir, nr, nc)) {
            pos.row = nr;
            pos.col = nc;
        }
    }
}
