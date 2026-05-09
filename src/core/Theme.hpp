#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct Theme {
  std::string name;

  // map
  sf::Color backgroundColor;
  sf::Color wallColor;

  // entities
  sf::Color pacmanColor;
  sf::Color dotColor;
  sf::Color pelletColor;

  // ghosts
  sf::Color blinkyColor;
  sf::Color pinkyColor;
  sf::Color inkyColor;
  sf::Color clydeColor;
  sf::Color frightenedColor;
  sf::Color frightenedWarnColor;
  sf::Color deadGhostColor;

  // HUD
  sf::Color hudPanelColor;
  sf::Color hudTextColor;
  sf::Color hudScoreColor;
  sf::Color hudHighScoreColor;
  sf::Color hudHeartColor;
  sf::Color hudLevelColor;

  // screens
  sf::Color titleColor;
  sf::Color subtitleColor;
  sf::Color promptColor;
  sf::Color gameOverColor;
  sf::Color winColor;
};
