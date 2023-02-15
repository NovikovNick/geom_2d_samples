#include "game_object.h"
namespace math {

GameObject::GameObject(std::vector<Eigen::Vector2f> mesh)
    : mesh(mesh),
      position(Eigen::Vector2f::Identity()),
      rotation(Eigen::Vector2f::Identity()) {}

Eigen::Vector2f GameObject::operator[](const size_t index) const {
  Eigen::Transform<float, 2, Eigen::Affine> t;
  t = Eigen::Matrix3f::Identity();
  t *= Eigen::Translation2f(position);
  t *= Eigen::Scaling(100.f, 100.f);
  //t *= Eigen::Rotation2Df(rotation);
  return t * mesh[index];
}

size_t GameObject::size() const { return mesh.size(); }

}  // namespace math