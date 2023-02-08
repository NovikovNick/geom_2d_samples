#include <SFML/Graphics.hpp>
#include <iostream>

#include "groupable.cc"

namespace geom_2d {

class Image final : public Groupable {
 public:
  sf::Sprite sprite;
  Image(const sf::Texture& texture) : sprite(sf::Sprite(texture)) {}

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Groupable::draw(target, states);
  }
  virtual void onDraw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(sprite, states);
  }

  virtual std::size_t getPointCount() const { return 0; }
  virtual sf::Vector2f getPoint(std::size_t index) const { return {}; }
};

}  // namespace geom_2d
