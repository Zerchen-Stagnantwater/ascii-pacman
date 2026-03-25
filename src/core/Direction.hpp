#pragma once

enum class Direction { Up, Down, Left, Right, None };

inline Direction opposite(Direction d) {
    switch (d) {
        case Direction::Up:    return Direction::Down;
        case Direction::Down:  return Direction::Up;
        case Direction::Left:  return Direction::Right;
        case Direction::Right: return Direction::Left;
        default:               return Direction::None;
    }
}

