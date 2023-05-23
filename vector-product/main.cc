#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <bitset>
#include <format>
#include <iostream>

#include "src/info.cc"
#include "src/info.h"
#include "src/scalable_grid.cc"
#include "src/scalable_grid.h"
#include "src/util.h"
#include "src/vector_product_visualizer.cc"
#include "src/vector_product_visualizer.h"
#include "src/vector_shape.cc"

using namespace std::chrono;
using namespace math;

namespace {

const static sf::Color kBGColor(35, 35, 35);
const static sf::Color kFstColor(255, 100, 0);
const static sf::Color kSndColor(255, 17, 17);
const static sf::Color kTrdColor(255, 255, 255);

}  // namespace

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  auto mode = sf::VideoMode(896, 896);
  auto style = sf::Style::Default;
  sf::RenderWindow window(mode, "Generic Platformer", style, settings);

  // reduce the framerate to minimize laptop overheat :(
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("resources/default_fnt.otf")) {
    math::debug("Unable to load default_fnt.otf!\n");
    return 1;
  }

  math::Info info(font, kFstColor);
  const int angle_index = info.addFormat("angle: {:.2f}\n");
  const int dot_index = info.addFormat("dot: {:.2f}\n");
  const int cross_index = info.addFormat("cross: {:.2f}\n");
  const int lhs_index = info.addFormat("LHS: {:.0f}\n");
  const int rhs_index = info.addFormat("RHS: {:.0f}\n");
  info.addFormat("\n");
  const int sin_index = info.addFormat("sin: {:.4f}\n");
  const int cos_index = info.addFormat("cos: {:.4f}\n");
  info.addFormat("\n");
  const int mouse_index = info.addFormat("Mouse[{:4d},{:4d}]\n");

  math::VectorProductVisualizer visualizer(font, kBGColor, kSndColor, kTrdColor);
  visualizer.update({0, -128}, {64, 0}, {0, 0});

  math::ScalableGrid grid(32);

  while (window.isOpen()) {
    info.update(sin_index, visualizer.sin());
    info.update(cos_index, visualizer.cos());
    info.update(dot_index, visualizer.dot());
    info.update(cross_index, visualizer.cross());
    info.update(angle_index, visualizer.angleDegree());
    info.update(lhs_index, visualizer.lhsLength());
    info.update(rhs_index, visualizer.rhsLength());
    
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed: window.close(); break;
        case sf::Event::KeyPressed: break;
        case sf::Event::KeyReleased: break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left) {
            auto [_, x, y] = event.mouseButton;
            visualizer.update({x, y}, true);
          }
          break;
        case sf::Event::MouseButtonReleased:
          if (event.mouseButton.button == sf::Mouse::Left) {
            auto [_, x, y] = event.mouseButton;
            visualizer.update({x, y}, false);
          }
          break;
        case sf::Event::MouseMoved: {
          auto [x, y] = event.mouseMove;
          visualizer.update({x, y});
          info.update(mouse_index, x, y);
          break;
        }
        case sf::Event::MouseWheelMoved: {
          auto [delta, x, y] = event.mouseWheel;
          grid.update(delta);
          break;
        }
      }
    }

    window.clear(kBGColor);
    window.draw(grid);
    window.draw(visualizer);
    window.draw(info);
    window.display();
  }
  return 0;
}
