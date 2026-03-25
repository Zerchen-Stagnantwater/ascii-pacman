#pragma once
#include <string>
#include <vector>

class MapLoader {
public:
    // Loads map layout from a .txt file
    static std::vector<std::string> load(const std::string& path);

    // Falls back to hardcoded classic layout
    static std::vector<std::string> classic();
};
