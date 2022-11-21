#ifndef GEOM_2D_RESOURCE_MANAGER_H_
#define GEOM_2D_RESOURCE_MANAGER_H_

#include <SFML/Graphics.hpp>
#include <iostream>

#include "src/util.h"

namespace geom_2d {

class ResourceManager {
  sf::Font font;
  sf::Texture running_sprite;

 public:
  ResourceManager() {
    if (!font.loadFromFile("resources/font.ttf"))
      debug("Unable to load font!\n");

    if (!running_sprite.loadFromFile("resources/running_sprite.png"))
      debug("Unable to load running_sprite.png!\n");

  };

  inline sf::Font& GetFont() & { return font; }
  inline sf::Texture& GetRunningSprite() & { return running_sprite; }
};

};      // namespace geom_2d
#endif  // GEOM_2D_RESOURCE_MANAGER_H_
