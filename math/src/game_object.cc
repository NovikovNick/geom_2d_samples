#include "game_object.h"

#include <fpm/fixed.hpp>  // For fpm::fixed_16_16
#include <fpm/ios.hpp>    // For fpm::operator<<
#include <fpm/math.hpp>   // For fpm::cos

#include "util.h"
namespace math {

using namespace Eigen;
using namespace fpm;

GameObject::GameObject(const std::vector<Vector2<fixed_16_16>>& mesh)
    : mesh(mesh),
      position(Vector2<fixed_16_16>{fixed_16_16{0.f}, fixed_16_16{0.f}}),
      velocity(Vector2<fixed_16_16>{fixed_16_16{0.f}, fixed_16_16{0.f}}) {}

void GameObject::update(const fixed_16_16 delta) {
  
};

Vector3<fixed_16_16> GameObject::operator[](const size_t index) const {
  Matrix3<fixed_16_16> identity;
  identity <<   fixed_16_16{1.f}, fixed_16_16{0.f}, fixed_16_16{0.f},
                fixed_16_16{0.f}, fixed_16_16{1.f}, fixed_16_16{0.f}, 
                fixed_16_16{0.f}, fixed_16_16{0.f}, fixed_16_16{1.f};
  Matrix3<fixed_16_16> transition;
  transition <<   fixed_16_16{1}, fixed_16_16{0}, position.x(),
                  fixed_16_16{0}, fixed_16_16{1}, position.y(),
                  fixed_16_16{0}, fixed_16_16{0}, fixed_16_16{1};
  Matrix3<fixed_16_16> scale;
  scale <<   fixed_16_16{64}, fixed_16_16{0}, fixed_16_16{0}, 
              fixed_16_16{0}, fixed_16_16{128}, fixed_16_16{0},
              fixed_16_16{0}, fixed_16_16{0}, fixed_16_16{1};
  Vector3<fixed_16_16> vect(mesh[index].x(), mesh[index].y(), fixed_16_16{1});
  return transition * scale * vect;
}

size_t GameObject::size() const { return mesh.size(); }

}  // namespace math