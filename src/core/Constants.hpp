#pragma once

namespace Config {
    inline constexpr int CELL_SIZE    = 20;
    inline constexpr int ROWS         = 31;
    inline constexpr int COLS         = 28;
    inline constexpr float PACMAN_SPEED   = 0.15f;
    inline constexpr float GHOST_SPEED    = 0.18f;
    inline constexpr float POWER_DURATION = 8.f;
    inline constexpr int DOT_POINTS       = 10;
    inline constexpr int PELLET_POINTS    = 50;
    inline constexpr int GHOST_POINTS     = 200;
    inline constexpr int WINDOW_W     = COLS * CELL_SIZE;
    inline constexpr int WINDOW_H     = ROWS * CELL_SIZE + 40;
}
