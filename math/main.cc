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
#include "src/scalable_grid.cc"
#include "src/scalable_grid.h"
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

inline int length(const Eigen::Vector2<fpm::fixed_16_16>& vector) {
  return std::sqrt(std::pow(static_cast<int>(vector.x()), 2) +
                   std::pow(static_cast<int>(vector.y()), 2));
}

Eigen::Vector2<fpm::fixed_16_16> normal(const Eigen::Vector2<fpm::fixed_16_16>& vector) {
  int l = length(vector);
  if (l == 0) return {fixed_16_16{0}, fixed_16_16{0}};
  return {vector.x() / l, vector.y() / l};
}
}  // namespace

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  auto mode = sf::VideoMode(900, 900);
  auto style = sf::Style::Default;
  sf::RenderWindow window(mode, "Generic Platformer", style, settings);

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
  visualizer.update({0, -128}, {64, 0}, {0, 0});

  math::GameObject player({{fpm::fixed_16_16(-0.5), fpm::fixed_16_16(0.5)},
                           {fpm::fixed_16_16(0.5), fpm::fixed_16_16(0.5)},
                           {fpm::fixed_16_16(0.5), fpm::fixed_16_16(-0.5)},
                           {fpm::fixed_16_16(-0.5), fpm::fixed_16_16(-0.5)}});

  math::VectorShape velocity_vector(kSndColor);

  math::ScalableGrid grid(32);

  auto t0 = steady_clock::now();
  auto t1 = steady_clock::now();
  float dx = 0;
  float velocity = 50;
  int base = 768;
  std::bitset<4> input(0);
  int jump_counter = 0;
  float elapsed = 0;
  while (window.isOpen()) {
    t1 = steady_clock::now();
    dx = duration_cast<microseconds>(t1 - t0).count() / 1e6;
    elapsed += dx;
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
        case sf::Event::MouseMoved: {
          auto [x, y] = event.mouseMove;
          visualizer.update({x, y});
          break;
        }
        case sf::Event::MouseWheelMoved: {
          auto [delta, x, y] = event.mouseWheel;
          grid.update(delta);
          break;
        }
      }
    }

    const fixed_16_16 delta{1};
    const Vector2<fixed_16_16> left(-delta, fixed_16_16{0});
    const Vector2<fixed_16_16> right(delta, fixed_16_16{0});
    const Vector2<fixed_16_16> top(fixed_16_16{0}, -delta);
    const Vector2<fixed_16_16> down(fixed_16_16{0}, delta);

    Vector2<fixed_16_16> impuls{fixed_16_16{0}, fixed_16_16{0}};
    if (input[kInputLeft]) impuls += left;
    if (input[kInputRight]) impuls += right;
    if (input[kInputDown]) impuls += down;

    player.velocity += normal(impuls) * fixed_16_16{velocity};

    if (input[kInputUp] && jump_counter < 1) {
      ++jump_counter;
      player.velocity += Vector2<fixed_16_16>{0, -1000};
      player.velocity.x() *= fixed_16_16{1.3};
    }

    
    if (jump_counter == 0 && !input[kInputLeft] && !input[kInputRight]) {
      player.velocity.x() *= fixed_16_16{0.7};
    } 

    if (static_cast<float>(player.position.y()) < base) {
      player.velocity += Vector2<fixed_16_16>{0, 150};
    } else {
      jump_counter = 0;
      if (player.velocity.y() > fixed_16_16{0})
        player.velocity.y() = fixed_16_16{0};
    }
    
    /*int l;
    if ((l = length(player.velocity)) > 500) {
      player.velocity = Vector2<fixed_16_16>{player.velocity.x() / l * 500,
                                             player.velocity.y() / l * 500};
    }*/

    player.position += player.velocity * fixed_16_16{dx};
    player.velocity *= fixed_16_16{0.95f};

    if (static_cast<float>(player.position.y()) >= base) {
      player.position.y() = fixed_16_16{base};
    }

    sf::VertexArray rectangle(sf::Quads, 4);
    for (int i = 0; i < player.size(); ++i) {
      auto pos = player[i];
      rectangle[i].position = {static_cast<float>(pos.x()),
                               static_cast<float>(pos.y())};
    }
    // scalable grid
    velocity_vector.update(
        {static_cast<int>(player.position.x()),
         static_cast<int>(player.position.y())},
        {static_cast<int>(player.position.x() + player.velocity.x()),
         static_cast<int>(player.position.y() + player.velocity.y())});

    {
      visualizer.update(
          {
              std::cos(elapsed * 4) * 64,
              std::sin(elapsed * 4) * 64,
          },
          {
              std::sin(elapsed) * 128,
              std::cos(elapsed) * 128,
          },
          visualizer.origin_);
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
    window.draw(grid);
    window.draw(visualizer);
    window.draw(info);
    window.draw(rectangle);
    window.draw(velocity_vector);
    window.display();
  }
  return 0;
}
