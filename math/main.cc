#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <format>
#include <iostream>

#include "src/info.cc"
#include "src/info.h"
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
  auto mode = sf::VideoMode(900, 900);
  auto style = sf::Style::Default;
  sf::RenderWindow window(mode, "Vector Product Visualizer", style, settings);

  // reduce the framerate to minimize laptop overheat :(
  window.setFramerateLimit(24);

  sf::Font font;
  if (!font.loadFromFile("resources/default_fnt.otf")) {
    math::debug("Unable to load default_fnt.otf!\n");
    return 1;
  }

  math::Info info(font, kFstColor);
  math::VectorProductVisualizer visualizer(font, kBGColor, kSndColor,
                                           kTrdColor);

  visualizer.update({-30, -1}, {30, -1}, {0, 0});
  auto t0 = steady_clock::now();
  auto t1 = steady_clock::now();
  double dx = 0;
  while (window.isOpen()) {
    t1 = steady_clock::now();
    dx = duration_cast<microseconds>(t1 - t0).count() / 1e6;
    t0 = t1;

    info.update(Info::INDEX::FPS, 1 / dx);
    info.update(Info::INDEX::DX, dx);

    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
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
        case sf::Event::MouseMoved:
          auto [x, y] = event.mouseMove;
          visualizer.update({x, y});
          break;
      }
    }

    info.update(Info::INDEX::DOT, visualizer.dot());
    info.update(Info::INDEX::CROSS, visualizer.cross());
    info.update(Info::INDEX::ANGLE, visualizer.angleDegree());
    info.update(Info::INDEX::COS, visualizer.cos());
    info.update(Info::INDEX::SIN, visualizer.sin());
    info.update(Info::INDEX::LHS_ATAN, visualizer.lhsAtan2());
    info.update(Info::INDEX::RHS_ATAN, visualizer.rhsAtan2());
    info.update(Info::INDEX::LHS_LENGTH, visualizer.lhsLength());
    info.update(Info::INDEX::RHS_LENGTH, visualizer.rhsLength());

    window.clear(kBGColor);
    window.draw(visualizer);
    window.draw(info);
    window.display();
  }
  return 0;
}
