#pragma once
#include <vector>
#include <string>

enum class Cell { Wall, Dot, Pellet, Empty };

class Map {
public:
    static const int COLS = 28;
    static const int ROWS = 31;

    Map();
    void load(const std::vector<std::string>& layout);

    Cell getCell(int row, int col) const;
    void setCell(int row, int col, Cell c);
    bool isWall(int row, int col) const;
    int  dotsRemaining() const;

private:
    std::vector<std::vector<Cell>> grid;
};

