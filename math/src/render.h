#ifndef GEOM_2D_MATH_RENDER_H
#define GEOM_2D_MATH_RENDER_H

#include <Eigen/Dense>

namespace math {

class Render {
 public:
  Render();
  void drawVector(const Eigen::Vector2f& p);
  void drawVector(const Eigen::Vector2f& p1, const Eigen::Vector2f& p2);
  Eigen::Vector2f drawPoint(const Eigen::Vector2f& p);
  void render();
};

};      // namespace math
#endif  // GEOM_2D_MATH_RENDER_H