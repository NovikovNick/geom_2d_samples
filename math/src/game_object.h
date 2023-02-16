#ifndef GEOM_2D_MATH_OBJECT_H
#define GEOM_2D_MATH_OBJECT_H
#include <Eigen/Dense>
#include <vector>

namespace math {
/*

if (input[INPUT_LEFT]) rect_obj.position += Vector2f(-dx * velocity, 0);
if (input[INPUT_RIGHT]) rect_obj.position += Vector2f(dx * velocity, 0);
if (input[INPUT_UP]) rect_obj.position += Vector2f(0, -dx* velocity);
if (input[INPUT_DOWN]) rect_obj.position += Vector2f(0, dx* velocity);

sf::VertexArray rectangle(sf::Quads, 4);
for (int i = 0; i < rect_obj.size(); ++i) {
  rectangle[i].position = toScreenCoord(rect_obj[i]);
}
window.draw(rectangle);

*/
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