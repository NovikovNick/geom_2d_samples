#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <bitset>
#include <format>
#include <fpm/fixed.hpp>
#include <iostream>

#include "src/game_object.cc"
#include "src/game_object.h"
#include "src/info.cc"
#include "src/info.h"
#include "src/util.h"
#include "src/vector_product_visualizer.cc"
#include "src/vector_product_visualizer.h"
#include "src/vector_shape.cc"

using namespace std::chrono;
using namespace math;

namespace {

const static int kInputLeft = 0;
const static int kInputRight = 1;
const static int kInputUp = 2;
const static int kInputDown = 3;

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
  // window.setFramerateLimit(24);
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("resources/default_fnt.otf")) {
    math::debug("Unable to load default_fnt.otf!\n");
    return 1;
  }

  math::Info info(font, kFstColor);

  math::VectorProductVisualizer visualizer(font, kBGColor, kSndColor,
                                           kTrdColor);
  // visualizer.update({-30, -1}, {30, -1}, {0, 0});
  visualizer.update({-250, 445}, {-50, 445}, {-450, 445});

  math::GameObject player({{fpm::fixed_16_16(-1), fpm::fixed_16_16(1)},
                           {fpm::fixed_16_16(1), fpm::fixed_16_16(1)},
                           {fpm::fixed_16_16(1), fpm::fixed_16_16(-1)},
                           {fpm::fixed_16_16(-1), fpm::fixed_16_16(-1)}});

  math::VectorShape velocity_vector(kSndColor);

  auto t0 = steady_clock::now();
  auto t1 = steady_clock::now();
  float dx = 0;
  float velocity = 10;
  std::bitset<4> input(0);

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
        case sf::Event::KeyPressed:
          switch (event.key.code) {
            case sf::Keyboard::A:
              input[kInputLeft] = true;
              break;
            case sf::Keyboard::D:
              input[kInputRight] = true;
              break;
            case sf::Keyboard::W:
              input[kInputUp] = true;
              break;
            case sf::Keyboard::S:
              input[kInputDown] = true;
              break;
          }
          break;
        case sf::Event::KeyReleased:
          switch (event.key.code) {
            case sf::Keyboard::A:
              input[kInputLeft] = false;
              break;
            case sf::Keyboard::D:
              input[kInputRight] = false;
              break;
            case sf::Keyboard::W:
              input[kInputUp] = false;
              break;
            case sf::Keyboard::S:
              input[kInputDown] = false;
              break;
          }
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

    const fpm::fixed_16_16 delta{velocity};
    const Eigen::Vector2<fixed_16_16> left(-delta, fixed_16_16{0});
    const Eigen::Vector2<fixed_16_16> right(delta, fixed_16_16{0});
    const Eigen::Vector2<fixed_16_16> top(fixed_16_16{0}, -delta);
    const Eigen::Vector2<fixed_16_16> down(fixed_16_16{0}, delta);
    if (input[kInputLeft]) player.velocity += left;
    if (input[kInputRight]) player.velocity += right;
    if (input[kInputUp]) player.velocity += top;
    if (input[kInputDown]) player.velocity += down;
    player.update(fpm::fixed_16_16{dx});

    sf::VertexArray rectangle(sf::Quads, 4);
    for (int i = 0; i < player.size(); ++i) {
      auto pos = player[i];
      rectangle[i].position = {static_cast<float>(pos.x()),
                               static_cast<float>(pos.y())};
    }

    velocity_vector.update(
        {static_cast<int>(player.position.x()),
         static_cast<int>(player.position.y())},
        {static_cast<int>(player.position.x() + player.velocity.x()),
         static_cast<int>(player.position.y() + player.velocity.y())});

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
    window.draw(rectangle);
    window.draw(velocity_vector);
    window.draw(visualizer);
    window.draw(info);
    window.display();
  }
  return 0;
}
