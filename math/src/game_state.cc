#include "game_state.h"

#include <Eigen/Dense>
#include <fpm/fixed.hpp>

namespace math {

inline int length(const Eigen::Vector2<fpm::fixed_16_16>& vector) {
  return std::sqrt(std::pow(static_cast<int>(vector.x()), 2) +
                   std::pow(static_cast<int>(vector.y()), 2));
}

Eigen::Vector2<fpm::fixed_16_16> normal(
    const Eigen::Vector2<fpm::fixed_16_16>& vector) {
  int l = length(vector);
  if (l == 0) return {fpm::fixed_16_16{0}, fpm::fixed_16_16{0}};
  return {vector.x() / l, vector.y() / l};
}

using namespace Eigen;
using namespace fpm;
GameState::GameState()
    : frame_(0),
      jump_counter_(0),
      player_(GameObject({{fpm::fixed_16_16(-0.5), fpm::fixed_16_16(0.5)},
                          {fpm::fixed_16_16(0.5), fpm::fixed_16_16(0.5)},
                          {fpm::fixed_16_16(0.5), fpm::fixed_16_16(-0.5)},
                          {fpm::fixed_16_16(-0.5), fpm::fixed_16_16(-0.5)}})) {}

void GameState::update(const int input, const float dx) {
  const static int kInputLeft = 0;
  const static int kInputRight = 1;
  const static int kInputUp = 2;
  const static int kInputDown = 3;

  ++frame_;
  std::bitset<4> set(input);
  int base = 768;
  int velocity = 50;

  const fixed_16_16 delta{1};
  const Vector2<fixed_16_16> left(-delta, fixed_16_16{0});
  const Vector2<fixed_16_16> right(delta, fixed_16_16{0});
  const Vector2<fixed_16_16> down(fixed_16_16{0}, delta);

  Vector2<fixed_16_16> impuls{fixed_16_16{0}, fixed_16_16{0}};
  if (set[kInputLeft]) impuls += left;
  if (set[kInputRight]) impuls += right;
  if (set[kInputDown]) impuls += down;

  player_.velocity += normal(impuls) * fixed_16_16{velocity};

  if (set[kInputUp] && jump_counter_ < 1) {
    ++jump_counter_;
    player_.velocity += Vector2<fixed_16_16>{0, -1500};
    player_.velocity.x() *= fixed_16_16{1.3};
  }

  if (jump_counter_ == 0 && !set[kInputLeft] && !set[kInputRight]) {
    player_.velocity.x() *= fixed_16_16{0.7};
  }

  if (static_cast<float>(player_.position.y()) < base) {
    player_.velocity += Vector2<fixed_16_16>{0, 150};
  } else {
    jump_counter_ = 0;
    if (player_.velocity.y() > fixed_16_16{0})
      player_.velocity.y() = fixed_16_16{0};
  }

  /*int l;
  if ((l = length(player.velocity)) > 500) {
    player.velocity = Vector2<fixed_16_16>{player.velocity.x() / l * 500,
                                           player.velocity.y() / l * 500};
  }*/

  player_.position += player_.velocity * fixed_16_16{dx};
  player_.velocity *= fixed_16_16{0.95f};

  if (static_cast<float>(player_.position.y()) >= base) {
    player_.position.y() = fixed_16_16{base};
  }
}

GameObject GameState::getPlayer() { return player_; }

};  // namespace math
