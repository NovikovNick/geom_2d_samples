#ifndef GEOM_2D_RESOURCE_MANAGER_H_
#define GEOM_2D_RESOURCE_MANAGER_H_

#include <SFML/Graphics.hpp>
#include <iostream>

#include "src/util.h"

namespace geom_2d {

class ResourceManager {
  sf::Font default_fnt, title_fnt;
  sf::Texture snake_sprite_sheet;
  sf::SoundBuffer click;

 public:
  ResourceManager() {
    if (!title_fnt.loadFromFile("resources/title_fnt.ttf"))
      debug("Unable to load font!\n");

    if (!default_fnt.loadFromFile("resources/default_fnt.otf"))
      debug("Unable to load default_fnt.otf!\n");

    if (!snake_sprite_sheet.loadFromFile("resources/snake_sprite_sheet.png"))
      debug("Unable to load running_sprite.png!\n");

    if (!click.loadFromFile("resources/click.wav"))
      debug("Unable to load click.wav!\n");
  };

  inline sf::Font& GetTitleFont() & { return title_fnt; }
  inline sf::Font& GetDefaultFont() & { return default_fnt; }
  inline sf::Texture& GetSnakeSpriteSheet() & { return snake_sprite_sheet; }
  inline sf::SoundBuffer& GetClickSoundBuffer() & { return click; }
};

};      // namespace geom_2d
#endif  // GEOM_2D_RESOURCE_MANAGER_H_
