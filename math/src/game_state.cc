#include "game_state.h"

#include <Eigen/Dense>
#include <fpm/fixed.hpp>

namespace math {

using FIXED = fpm::fixed_16_16;
using VECTOR_2 = Eigen::Vector2<FIXED>;

GameState::GameState()
    : frame_(0),
      jump_counter_(0),
      player_(GameObject({{FIXED(-0.5), FIXED(0.5)},
                          {FIXED(0.5), FIXED(0.5)},
                          {FIXED(0.5), FIXED(-0.5)},
                          {FIXED(-0.5), FIXED(-0.5)}})) {}

constexpr int sum(const int n) {
  int res = 0;
  for (int i = 1; i <= n; ++i) res += i;
  return res;
}

void GameState::update(const int input, const int frames) {
  // braking:               4  frame
  const static int kInputLeft = 0;
  const static int kInputRight = 1;
  const static int kInputUp = 2;
  const static int kInputDown = 3;

  const static int kBase = 768;

  const static int kJumpHeight = 196;
  const static int kJump = 12;
  const static int kAccelerationX = 1;
  const static int kAccelerationGravity = 2;
  const static int kMaxVelocityX = 16;
  const static int kMaxVelocityFall = 32;

  const static FIXED kJumpDelta = FIXED{kJumpHeight} / FIXED{sum(kJump - 1)};

  ++frame_;
  FIXED& vel_x = player_.velocity.x();
  FIXED& vel_y = player_.velocity.y();
  FIXED& pos_x = player_.position.x();
  FIXED& pos_y = player_.position.y();

  std::bitset<4> set(input);
  if (set[kInputLeft]) vel_x += FIXED{-kAccelerationX};
  if (set[kInputRight]) vel_x += FIXED{kAccelerationX};
  if (set[kInputDown]) vel_y += FIXED{kAccelerationX};

  if (static_cast<float>(pos_y) < kBase) {
    if (jump_counter_ >= kJump || jump_counter_ == 0) {
      jump_counter_ += frames;
      //debug("down jump#{}: ", jump_counter_);
      vel_y += kAccelerationGravity;
    }
  } else {
    //debug("on ground: ");
    jump_counter_ = 0;
    if (vel_y > FIXED{0}) vel_y = FIXED{0};
  }

  if (jump_counter_ > 0 && jump_counter_ < kJump) {
    vel_y = -kJumpDelta * (kJump - 1 - jump_counter_);
    jump_counter_ += frames;
   // debug("up jump#{}: ", jump_counter_);
  }

  if (set[kInputUp] && jump_counter_ == 0) {
    jump_counter_ = 1;
    vel_y = -kJumpDelta * (kJump - 1);
    //debug("start jump#{}: ", jump_counter_);
  }

  if (jump_counter_ == 0 && !set[kInputLeft] && !set[kInputRight]) {
    vel_x *= FIXED{0.5};
    if (static_cast<int>(vel_x) == 0) vel_x = FIXED{0};
  }

  vel_x = std::clamp(vel_x, FIXED{-kMaxVelocityX}, FIXED{kMaxVelocityX});
  vel_y = std::clamp(vel_y, -kJumpDelta * (kJump - 1), FIXED{kMaxVelocityFall});

  player_.position += player_.velocity * FIXED{frames};

  /*debug("Position: [{:8.3f};{:8.3f}]. Velocity: [{:.3f};{:.3f}]\n",
        static_cast<float>(pos_x), static_cast<float>(pos_y),
        static_cast<float>(vel_x), static_cast<float>(vel_y));*/

  // mock before collistion detection will be implemented
  if (static_cast<float>(pos_y) >= kBase) pos_y = FIXED{kBase};
}

GameObject GameState::getPlayer() { return player_; }

};  // namespace math
