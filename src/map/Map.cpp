#include "Map.hpp"

Map::Map() {
    grid.resize(ROWS, std::vector<Cell>(COLS, Cell::Empty));
}

void Map::load(const std::vector<std::string>& layout) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            char ch = (c < (int)layout[r].size()) ? layout[r][c] : ' ';
            if      (ch == '#') grid[r][c] = Cell::Wall;
            else if (ch == '.') grid[r][c] = Cell::Dot;
            else if (ch == 'o') grid[r][c] = Cell::Pellet;
            else                grid[r][c] = Cell::Empty;
        }
    }
}

Cell Map::getCell(int row, int col) const { return grid[row][col]; }
void Map::setCell(int row, int col, Cell c) { grid[row][col] = c; }

bool Map::isWall(int row, int col) const {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) return true;
    return grid[row][col] == Cell::Wall;
}

int Map::dotsRemaining() const {
    int count = 0;
    for (auto& row : grid)
        for (auto& cell : row)
            if (cell == Cell::Dot || cell == Cell::Pellet) count++;
    return count;
}
