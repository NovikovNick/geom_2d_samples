#ifndef GEOM_2D_MATH_OBJECT_H
#define GEOM_2D_MATH_OBJECT_H
#include <Eigen/Dense>
#include <vector>

namespace math {

class GameObject {
  std::vector<Eigen::Vector2f> mesh;

 public:
  Eigen::Vector2f position, rotation;

  GameObject(std::vector<Eigen::Vector2f> mesh);

  Eigen::Vector2f operator[](const size_t index) const;

  size_t size() const;
};

};      // namespace math
#endif  // GEOM_2D_MATH_OBJECT_H