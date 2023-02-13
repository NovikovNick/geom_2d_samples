#include <SFML/Graphics.hpp>
#include <format>
#include <iostream>

#include "src/render.cc"
#include "src/render.h"

int main() {
  int x1 = 100, y1 = 40;
  int x2 = 30, y2 = 60;

  math::Render render;
  render.drawVector(x1, y1);
  render.drawVector(x2, y2);
  render.drawVector(x1 + x2, y1 + y2);
  render.render();
  return 0;
}
