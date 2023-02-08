#include <SFML/Graphics.hpp>
#include <iostream>

namespace geom_2d {

class Cursor final : public sf::Shape {
  sf::VertexArray vertices_;

 public:
  Cursor(const uint32_t size, sf::Color color)
      : vertices_(sf::VertexArray(sf::Triangles, 3)) {
    vertices_[0].position = sf::Vector2f(0, 0);
    vertices_[1].position = sf::Vector2f(size * .3f, size);
    vertices_[2].position = sf::Vector2f(size * .9f, size * .5f);

    vertices_[0].color = color;
    vertices_[1].color = color;
    vertices_[2].color = color;
  }
  sf::FloatRect getBounds() {
    auto [top, left, _, __] = getGlobalBounds();
    return sf::FloatRect(top, left, 5, 5);
  }
  virtual std::size_t getPointCount() const {
    return vertices_.getVertexCount();
  }

  virtual sf::Vector2f getPoint(std::size_t index) const {
    return vertices_[index].position;
  }

 private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();

    target.draw(vertices_, states);
  }
};
}  // namespace geom_2d

