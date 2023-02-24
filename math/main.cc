#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <bitset>
#include <format>
#include <fpm/fixed.hpp>
#include <iostream>

#include "src/game_loop.cc"
#include "src/game_loop.h"
#include "src/game_object.cc"
#include "src/game_object.h"
#include "src/game_state.cc"
#include "src/game_state.h"
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

Eigen::Vector2<fpm::fixed_16_16> normal(
    const Eigen::Vector2<fpm::fixed_16_16>& vector) {
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
  visualizer.update({0, -128}, {64, 0}, {0, 0});

  math::VectorShape velocity_vector(kSndColor);

  math::ScalableGrid grid(32);

  auto t0 = steady_clock::now();
  auto t1 = steady_clock::now();
  float elapsed = 0, dx = 0;
  std::bitset<4> input_bitset(0);

  auto gs = std::make_shared<math::GameState>();
  auto tick = std::make_shared<std::atomic<int>>(0);
  auto tick_rate = std::make_shared<std::atomic<int>>(30);
  auto frame_ratio = std::make_shared<std::atomic<float>>(0);
  auto input = std::make_shared<std::atomic<int>>(0);
  math::GameLoop game_loop(gs, tick, tick_rate, frame_ratio, input);
  std::thread(game_loop).detach();

  int prev_tick = tick->load();
  int curr_tick = prev_tick;
  auto prev_player = gs->getPlayer();
  auto curr_player = gs->getPlayer();
  float t = frame_ratio->load();

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
              input_bitset[kInputLeft] = true;
              break;
            case sf::Keyboard::D:
              input_bitset[kInputRight] = true;
              break;
            case sf::Keyboard::W:
              input_bitset[kInputUp] = true;
              break;
            case sf::Keyboard::S:
              input_bitset[kInputDown] = true;
              break;
          }
          break;
        case sf::Event::KeyReleased:
          switch (event.key.code) {
            case sf::Keyboard::A:
              input_bitset[kInputLeft] = false;
              break;
            case sf::Keyboard::D:
              input_bitset[kInputRight] = false;
              break;
            case sf::Keyboard::W:
              input_bitset[kInputUp] = false;
              break;
            case sf::Keyboard::S:
              input_bitset[kInputDown] = false;
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
          tick_rate->fetch_add(delta);
          grid.update(delta);
          break;
        }
      }
    }

    input->store(input_bitset.to_ulong());

    curr_tick = tick->load();
    t = frame_ratio->load();
    if (prev_tick != curr_tick) {
      prev_tick = curr_tick;
      prev_player = curr_player;
      curr_player = gs->getPlayer();
    }

    debug("Tick#{}. {}. {:4.2f}%\n", curr_tick, tick_rate->load(), t);

    sf::VertexArray rectangle(sf::Quads, 4);
    for (int i = 0; i < curr_player.size(); ++i) {
      auto prev_x = static_cast<float>(prev_player[i].x());
      auto prev_y = static_cast<float>(prev_player[i].y());
      auto curr_x = static_cast<float>(curr_player[i].x());
      auto curr_y = static_cast<float>(curr_player[i].y());

      rectangle[i].position = {std::lerp(prev_x, curr_x, t),
                               std::lerp(prev_y, curr_y, t)};
    }

    
    { // scalable grid
      auto prev_pos_x = static_cast<float>(prev_player.position.x());
      auto prev_pos_y = static_cast<float>(prev_player.position.y());
      auto prev_vel_x = static_cast<float>(prev_player.velocity.x());
      auto prev_vel_y = static_cast<float>(prev_player.velocity.y());

      auto curr_pos_x = static_cast<float>(curr_player.position.x());
      auto curr_pos_y = static_cast<float>(curr_player.position.y());
      auto curr_vel_x = static_cast<float>(curr_player.velocity.x());
      auto curr_vel_y = static_cast<float>(curr_player.velocity.y());

      curr_player = gs->getPlayer();
      velocity_vector.update(
          {std::lerp(prev_pos_x, curr_pos_x, t),
           std::lerp(prev_pos_y, curr_pos_y, t)},
          {std::lerp(prev_pos_x + prev_vel_x, curr_pos_x + curr_vel_x, t),
           std::lerp(prev_pos_y + prev_vel_y, curr_pos_y + curr_vel_y, t)}
      );
    }

    {
      visualizer.update(
          {std::cos(elapsed * 4) * 64, std::sin(elapsed * 4) * 64},
          {std::sin(elapsed) * 128, std::cos(elapsed) * 128},
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
