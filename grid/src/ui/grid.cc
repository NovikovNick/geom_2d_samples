#include <SFML/Graphics.hpp>
#include <iostream>

#include "groupable.cc"

namespace geom_2d {

class Grid final : public Groupable {
  sf::VertexArray lines;

 public:
  Grid(const int width, const int height, const int grid_size)
      : lines(sf::VertexArray(sf::Lines, (width + height + 2) * 4)) {
    int i = 0;
    for (int row = 0; row <= height; ++row) {
      lines[i++].position = sf::Vector2f(0, row * grid_size);
      lines[i++].position = sf::Vector2f(width * grid_size, row * grid_size);
    }
    for (int col = 0; col <= width; ++col) {
      lines[i++].position = sf::Vector2f(col * grid_size, 0);
      lines[i++].position = sf::Vector2f(col * grid_size, height * grid_size);
    }
  }

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Groupable::draw(target, states);
  }
  virtual void onDraw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(lines, states);
  }

  virtual std::size_t getPointCount() const { return 0; }
  virtual sf::Vector2f getPoint(std::size_t index) const { return {}; }
};

}  // namespace geom_2d
