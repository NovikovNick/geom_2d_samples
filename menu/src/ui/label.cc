#include <SFML/Graphics.hpp>
#include <iostream>

#include "groupable.cc"

namespace geom_2d {

class Label final : public Groupable {
 public:
  sf::Text label;
  Label(const sf::Text& label) : label(label) {}

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Groupable::draw(target, sf::Transform::Identity);
  }
  virtual void onDraw(sf::RenderTarget& target,
                      const sf::Transform& transform) const {
    target.draw(label, transform);
  }

  virtual std::size_t getPointCount() const { return 0; }
  virtual sf::Vector2f getPoint(std::size_t index) const { return {}; }
};

}  // namespace geom_2d
