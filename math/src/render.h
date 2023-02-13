#ifndef GEOM_2D_MATH_RENDER_H
#define GEOM_2D_MATH_RENDER_H

namespace math {

class Render {
 public:
  Render();
  void drawVector(const int x, const int y);
  void drawPoint(const int x, const int y);
  void render();
};

};      // namespace math
#endif  // GEOM_2D_MATH_RENDER_H