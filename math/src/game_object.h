#ifndef GEOM_2D_MATH_OBJECT_H
#define GEOM_2D_MATH_OBJECT_H
#include <Eigen/Dense>
#include <fpm/fixed.hpp>
#include <vector>

using FIXED = fpm::fixed_16_16;
using VECTOR_2 = Eigen::Vector2<FIXED>;
using VECTOR_3 = Eigen::Vector3<FIXED>;

namespace math {

class GameObject {
  std::vector<VECTOR_2> mesh;
  int width_, height_;

 public:
  VECTOR_2 position, velocity;

  GameObject(const int width, const int height,
             const std::vector<VECTOR_2>& mesh);

  VECTOR_3 operator[](const size_t index) const;

  std::pair<FIXED, FIXED> getProjectionMinMax(const int axis) const;

  size_t size() const;
};

};      // namespace math
#endif  // GEOM_2D_MATH_OBJECT_H