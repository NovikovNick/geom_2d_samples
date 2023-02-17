#ifndef GEOM_2D_MATH_RENDER_H
#define GEOM_2D_MATH_RENDER_H

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

namespace math {

class Render {
  sf::Font font_;
 public:
  Render();
  void drawVector(const Eigen::Vector2f& p, const sf::Color color);
  void drawVector(const Eigen::Vector2f& p1, const Eigen::Vector2f& p2, const sf::Color color);
  Eigen::Vector2f drawPoint(const Eigen::Vector2f& p);
  void render();
};

};      // namespace math
#endif  // GEOM_2D_MATH_RENDER_H