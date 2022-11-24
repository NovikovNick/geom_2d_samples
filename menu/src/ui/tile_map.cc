#include <SFML/Graphics.hpp>
#include <iostream>

#include "../util.h"
#include "groupable.cc"

namespace geom_2d {

enum class SNAKE_INDEX {
  APPLE = 15,
  EMPTY = 6,

  BODY_HOR = 1,
  BODY_VER = 7,

  HEAD_TOP = 3,
  HEAD_BTM = 9,
  HEAD_LFT = 8,
  HEAD_RHT = 4,

  TAIL_TOP = 19,
  TAIL_BTM = 13,
  TAIL_LFT = 14,
  TAIL_RHT = 18,

  TURN_TL = 0,
  TURN_TR = 2,
  TURN_BL = 5,
  TURN_BR = 12
};

class TileMap : public Groupable {
  sf::VertexArray vertices_;
  sf::Texture tileset_;

 public:
  TileMap(const sf::Texture& tileset, const sf::Vector2u ts,
          const std::vector<SNAKE_INDEX>& tiles, const uint32_t width,
          const uint32_t height)
      : tileset_(tileset) {
    tileset_.setSmooth(true);

    // resize the vertex array to fit the level size
    vertices_.setPrimitiveType(sf::Quads);
    vertices_.resize(width * height * 4);

    // populate the vertex array, with one quad per tile
    for (uint32_t i = 0; i < width; ++i) {
      for (uint32_t j = 0; j < height; ++j) {
        // get the current tile number
        int tileNumber = static_cast<int>(tiles[i + j * width]);

        // find its position in the tileset texture
        int tu = tileNumber % (tileset_.getSize().x / ts.x);
        int tv = tileNumber / (tileset_.getSize().x / ts.x);

        // get a pointer to the current tile's quad
        sf::Vertex* quad = &vertices_[(i + j * width) * 4];

        // define its 4 corners
        quad[0].position = sf::Vector2f(i * ts.x, j * ts.y);
        quad[1].position = sf::Vector2f((i + 1) * ts.x, j * ts.y);
        quad[2].position = sf::Vector2f((i + 1) * ts.x, (j + 1) * ts.y);
        quad[3].position = sf::Vector2f(i * ts.x, (j + 1) * ts.y);

        // define its 4 texture coordinates
        quad[0].texCoords = sf::Vector2f(tu * ts.x, tv * ts.y);
        quad[1].texCoords = sf::Vector2f((tu + 1) * ts.x, tv * ts.y);
        quad[2].texCoords = sf::Vector2f((tu + 1) * ts.x, (tv + 1) * ts.y);
        quad[3].texCoords = sf::Vector2f(tu * ts.x, (tv + 1) * ts.y);

        /*quad[0].color = sf::Color::Blue;
        quad[1].color = sf::Color::Blue;
        quad[2].color = sf::Color::Blue;
        quad[3].color = sf::Color::Blue;*/
      }
    }
  }

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Groupable::draw(target, states);
  }

  virtual void onDraw(sf::RenderTarget& target, sf::RenderStates states) const {
    // apply the transform
    states.transform *= getTransform();
    states.transform.scale(0.7f, 0.7f);

    // apply the tileset texture
    states.texture = &tileset_;

    // draw the vertex array
    target.draw(vertices_, states);
  }

  virtual std::size_t getPointCount() const { return 0; }
  virtual sf::Vector2f getPoint(std::size_t index) const { return {}; }

  virtual void draw(sf::RenderTarget& target,
                    const sf::Transform& transform) const {
    Groupable::draw(target, sf::Transform::Identity);
  }
  virtual void onDraw(sf::RenderTarget& target,
                      const sf::Transform& transform) const {}
};

};  // namespace geom_2d
