#include "AISystem.hpp"
#include "../core/Direction.hpp"
#include <cmath>
#include <limits>

Direction AISystem::chase(const Map& map, int row, int col,
                          int targetRow, int targetCol, Direction current) {
    Direction dirs[] = { Direction::Up, Direction::Down, Direction::Left, Direction::Right };
    Direction best = current;
    float bestDist = std::numeric_limits<float>::max();

    for (auto d : dirs) {
        if (d == opposite(current)) continue; // no reversing
        int nr = row, nc = col;
        if (d == Direction::Up)    nr--;
        if (d == Direction::Down)  nr++;
        if (d == Direction::Left)  nc--;
        if (d == Direction::Right) nc++;
        if (map.isWall(nr, nc)) continue;

        float dist = std::hypot((float)(nr - targetRow), (float)(nc - targetCol));
        if (dist < bestDist) {
            bestDist = dist;
            best = d;
        }
    }
    return best;
}

void AISystem::update(entt::registry& registry, const Map& map, float dt) {
    // get pacman position
    int pacRow = 0, pacCol = 0;
    registry.view<Position, TagPacman>().each([&](auto, auto& pos) {
        pacRow = pos.row; pacCol = pos.col;
    });

    auto view = registry.view<Position, Velocity, GhostAI>();
    for (auto [entity, pos, vel, ai] : view.each()) {
        if (ai.mode == GhostMode::Dead) continue;

        int targetRow = pacRow, targetCol = pacCol;

        if (ai.mode == GhostMode::Scatter) {
            targetRow = ai.scatterRow;
            targetCol = ai.scatterCol;
            ai.modeTimer += dt;
            if (ai.modeTimer > 7.f) {
                ai.mode = GhostMode::Chase;
                ai.modeTimer = 0.f;
            }
        }

        if (ai.personality == GhostPersonality::Pinky && ai.mode == GhostMode::Chase) {
            // target 4 tiles ahead of pacman
            registry.view<Position, Velocity, TagPacman>().each([&](auto, auto& pp, auto& pv) {
                targetRow = pp.row;
                targetCol = pp.col;
                if (pv.dir == Direction::Up)    targetRow -= 4;
                if (pv.dir == Direction::Down)  targetRow += 4;
                if (pv.dir == Direction::Left)  targetCol -= 4;
                if (pv.dir == Direction::Right) targetCol += 4;
            });
        }

        if (ai.mode == GhostMode::Frightened) {
            // just reverse direction
            vel.nextDir = opposite(vel.dir);
        } else {
            vel.dir = chase(map, pos.row, pos.col, targetRow, targetCol, vel.dir);
        }
    }
}
