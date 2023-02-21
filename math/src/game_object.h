#ifndef GEOM_2D_MATH_OBJECT_H
#define GEOM_2D_MATH_OBJECT_H
#include <Eigen/Dense>
#include <fpm/fixed.hpp>
#include <vector>

namespace math {

class GameObject {
  std::vector<Eigen::Vector2<fpm::fixed_16_16>> mesh;

 public:
  Eigen::Vector2<fpm::fixed_16_16> position, velocity;

  GameObject(const std::vector<Eigen::Vector2<fpm::fixed_16_16>>& mesh);

  Eigen::Vector3<fpm::fixed_16_16> operator[](const size_t index) const;

  void update(const fpm::fixed_16_16 delta);

  size_t size() const;
};

};      // namespace math
#endif  // GEOM_2D_MATH_OBJECT_H