#include <SFML/Graphics.hpp>
#include <iostream>

namespace geom_2d {

class TileMap : public sf::Drawable, public sf::Transformable {
  sf::VertexArray vertices_;
  sf::Texture tileset_;

 public:
  bool load(const std::string& tileset, const sf::Vector2u ts,
            const std::vector<int>& tiles, const uint32_t width,
            const uint32_t height) {
    // load the tileset texture
    if (!tileset_.loadFromFile(tileset)) return false;

    // resize the vertex array to fit the level size
    vertices_.setPrimitiveType(sf::Quads);
    vertices_.resize(width * height * 4);

    // populate the vertex array, with one quad per tile
    for (uint32_t i = 0; i < width; ++i)
      for (uint32_t j = 0; j < height; ++j) {
        // get the current tile number
        int tileNumber = tiles[i + j * width];

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
      }

    return true;
  }

 private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &tileset_;

    // draw the vertex array
    target.draw(vertices_, states);
  }
};

};  // namespace geom_2d
