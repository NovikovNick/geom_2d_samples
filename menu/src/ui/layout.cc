#include <SFML/Graphics.hpp>
#include <iostream>

#include "groupable.cc"

namespace geom_2d {

class Layout final : public Groupable {
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Groupable::draw(target, sf::Transform::Identity);
  }

  virtual void onDraw(sf::RenderTarget& target,
                      const sf::Transform& transform) const {}

  virtual std::size_t getPointCount() const { return 0; }
  virtual sf::Vector2f getPoint(std::size_t index) const { return {}; }
};

}  // namespace geom_2d
