#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <format>
#include <iostream>

#include "src/render.cc"
#include "src/render.h"

//using Eigen::Matrix3f;

int main() {

  Eigen::Vector2f c1{100, 0};
  Eigen::Vector2f c2{0, 100};

  math::Render render;
  //render.drawVector(c1);
  //render.drawVector(c2);
  render.render();
  return 0;
}
