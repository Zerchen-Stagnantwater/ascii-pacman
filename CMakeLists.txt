cmake_minimum_required(VERSION 3.16)
project(pacman)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

CPMAddPackage("gh:skypjack/entt#v3.13.2")
CPMAddPackage("gh:nlohmann/json#v3.11.3")

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

add_executable(pacman
    src/main.cpp
    src/map/Map.cpp
    src/map/MapLoader.cpp
    src/game/Game.cpp
    src/core/ThemeLoader.cpp
    src/editor/MapEditor.cpp
    src/editor/MapValidator.cpp
    src/editor/EditorRenderSystem.cpp
    src/systems/MovementSystem.cpp
    src/systems/RenderSystem.cpp
    src/systems/AISystem.cpp
    src/systems/CollisionSystem.cpp
    src/systems/InputSystem.cpp
    src/systems/AnimationSystem.cpp
    src/systems/AudioSystem.cpp

)

target_include_directories(pacman PRIVATE src)

target_link_libraries(pacman PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    EnTT::EnTT
    nlohmann_json::nlohmann_json
)
