#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <format>
#include <iostream>

#include "src/render.cc"
#include "src/render.h"

/*
1. Label on P1 & P2
2. Unit circle for rhs with quaters
3. Show angle arc

4. move selected area smoothly 
5. flip vectors by right mouse key
6. Console output on update ready to copy/paste to programm
7. split code to reusable components. add docs
*/
int main() {

  Eigen::Vector2f c1{100, 0};
  Eigen::Vector2f c2{0, 100};

  math::Render render;
  //render.drawVector(c1);
  //render.drawVector(c2);
  render.render();
  return 0;
}
