#include "MapValidator.hpp"
#include "../map/Map.hpp"

ValidationResult
MapValidator::validate(const std::vector<std::string> &layout) {
  ValidationResult result;
  checkDimensions(layout, result);
  if (result.hasErrors())
    return result; // no point checking further
  checkBorder(layout, result);
  checkSpawns(layout, result);
  checkDots(layout, result);
  return result;
}

void MapValidator::checkDimensions(const std::vector<std::string> &layout,
                                   ValidationResult &result) {
  if ((int)layout.size() != Map::ROWS) {
    result.errors.push_back("Wrong row count: expected " +
                            std::to_string(Map::ROWS) + ", got " +
                            std::to_string(layout.size()));
    result.valid = false;
  }
  for (int r = 0; r < (int)layout.size(); r++) {
    if ((int)layout[r].size() != Map::COLS) {
      result.warnings.push_back("Row " + std::to_string(r) +
                                " has wrong width: expected " +
                                std::to_string(Map::COLS) + ", got " +
                                std::to_string(layout[r].size()));
    }
  }
}

void MapValidator::checkBorder(const std::vector<std::string> &layout,
                               ValidationResult &result) {
  int rows = (int)layout.size();
  int cols = Map::COLS;

  // check top and bottom rows
  for (int c = 0; c < cols; c++) {
    if (layout[0].size() > (size_t)c && layout[0][c] != '#')
      result.warnings.push_back("Top border not fully walled at col " +
                                std::to_string(c));
    if (layout[rows - 1].size() > (size_t)c && layout[rows - 1][c] != '#')
      result.warnings.push_back("Bottom border not fully walled at col " +
                                std::to_string(c));
  }

  // check left and right columns
  for (int r = 0; r < rows; r++) {
    if (!layout[r].empty() && layout[r][0] != '#' && layout[r][0] != ' ')
      result.warnings.push_back("Left border not walled at row " +
                                std::to_string(r));
    if ((int)layout[r].size() >= cols && layout[r][cols - 1] != '#' &&
        layout[r][cols - 1] != ' ')
      result.warnings.push_back("Right border not walled at row " +
                                std::to_string(r));
  }
}

void MapValidator::checkSpawns(const std::vector<std::string> &layout,
                               ValidationResult &result) {
  int pacCount = 0, blinkyCount = 0, pinkyCount = 0, inkyCount = 0,
      clydeCount = 0;

  for (auto &row : layout) {
    for (char ch : row) {
      if (ch == 'P')
        pacCount++;
      if (ch == 'B')
        blinkyCount++;
      if (ch == 'p')
        pinkyCount++;
      if (ch == 'i')
        inkyCount++;
      if (ch == 'c')
        clydeCount++;
    }
  }

  if (pacCount == 0)
    result.warnings.push_back(
        "No Pacman spawn (P) — will use default position");
  if (pacCount > 1)
    result.warnings.push_back(
        "Multiple Pacman spawns (P) — first one will be used");
  if (blinkyCount == 0)
    result.warnings.push_back(
        "No Blinky spawn (B) — will use default position");
  if (pinkyCount == 0)
    result.warnings.push_back("No Pinky spawn (p) — will use default position");
  if (inkyCount == 0)
    result.warnings.push_back("No Inky spawn (i) — will use default position");
  if (clydeCount == 0)
    result.warnings.push_back("No Clyde spawn (c) — will use default position");
}

void MapValidator::checkDots(const std::vector<std::string> &layout,
                             ValidationResult &result) {
  int dotCount = 0;
  for (auto &row : layout)
    for (char ch : row)
      if (ch == '.' || ch == 'o')
        dotCount++;

  if (dotCount == 0) {
    result.errors.push_back("Map has no dots — unwinnable");
    result.valid = false;
  } else if (dotCount < 10) {
    result.warnings.push_back("Very few dots (" + std::to_string(dotCount) +
                              ") — map may be too short");
  }
}
