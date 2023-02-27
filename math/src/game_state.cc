#include "game_state.h"

namespace {

inline FIXED length(const FIXED& x, const FIXED& y) {
  return FIXED{std::sqrt(std::pow(static_cast<int>(x), 2) +
                         std::pow(static_cast<int>(y), 2))};
}

inline VECTOR_2 normal(const VECTOR_2& val) {
  if (val.x() == FIXED{0} && val.y() == FIXED{0}) return {FIXED{0}, FIXED{0}};
  return val / length(val.x(), val.y());
}

std::pair<FIXED, FIXED> isIntersect(const math::GameObject& lhs,

                                    const math::GameObject& rhs) {
  auto [lhs_min_x, lhs_max_x] = lhs.getProjectionMinMax(0);
  auto [rhs_min_x, rhs_max_x] = rhs.getProjectionMinMax(0);
  auto [lhs_min_y, lhs_max_y] = lhs.getProjectionMinMax(1);
  auto [rhs_min_y, rhs_max_y] = rhs.getProjectionMinMax(1);

  // rhs_max_y >= lhs_min_y && rhs_min_y <= lhs_max_y;
  FIXED zero{0};
  if (rhs_max_x - lhs_min_x < zero) return {zero, zero};
  if (lhs_max_x - rhs_min_x < zero) return {zero, zero};
  if (rhs_max_y - lhs_min_y < zero) return {zero, zero};
  if (lhs_max_y - rhs_min_y < zero) return {zero, zero};

  FIXED diff_x = std::min(rhs_max_x - lhs_min_x, lhs_max_x - rhs_min_x);
  FIXED diff_y = std::min(rhs_max_y - lhs_min_y, lhs_max_y - rhs_min_y);

  if (rhs_max_x > lhs_max_x) diff_x *= -1;
  if (rhs_max_y > lhs_max_y) diff_y *= -1;
  return {diff_x, diff_y};
}
}  // namespace

namespace math {

GameState::GameState()
    : frame_(0),
      jump_counter_(0),
      plaform_(512, 32,
               {{FIXED(0), FIXED(1)},
                {FIXED(1), FIXED(1)},
                {FIXED(1), FIXED(0)},
                {FIXED(0), FIXED(0)}}),
      player_(GameObject(64, 128,
                         {{FIXED(-0.5), FIXED(0.5)},
                          {FIXED(0.5), FIXED(0.5)},
                          {FIXED(0.5), FIXED(-0.5)},
                          {FIXED(-0.5), FIXED(-0.5)}})) {
  plaform_.position = {FIXED(0), FIXED(832)};
}

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

  const static int kJumpHeight = 224;
  const static int kJump = 14;
  const static int kAccelerationX = 1;
  const static int kAccelerationGravity = 2;
  const static int kMaxVelocityX = 8;
  const static int kMaxVelocityFall = 20;

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

  if (jump_counter_ >= kJump || jump_counter_ == 0) {
    //jump_counter_ += frames;
    //debug("down jump#{}: ", jump_counter_);
    vel_y += kAccelerationGravity;
  }

  if (jump_counter_ > 0 && jump_counter_ < kJump) {
    vel_y = -kJumpDelta * (kJump - 1 - jump_counter_);
    jump_counter_ += frames;
    //debug("up jump#{}: ", jump_counter_);
  }

  if (set[kInputUp] && jump_counter_ == 0) {
    jump_counter_ = 1;
    vel_y = -kJumpDelta * (kJump - 1);
    //debug("start jump#{}: ", jump_counter_);
  }
  if (!set[kInputLeft] && !set[kInputRight]) {
    vel_x *= jump_counter_ == 0 ? FIXED{0.5} : FIXED{0.9};
    if (static_cast<int>(vel_x) == 0) vel_x = FIXED{0};
  }

  vel_x = std::clamp(vel_x, FIXED{-kMaxVelocityX}, FIXED{kMaxVelocityX});
  vel_y = std::clamp(vel_y, -kJumpDelta * (kJump - 1), FIXED{kMaxVelocityFall});

  std::scoped_lock lock{player_mutex_};
  player_.position += player_.velocity * FIXED{frames};

  {
    auto [intersection_x, intersection_y] = isIntersect(player_, plaform_);
    if (intersection_x != FIXED{0} && intersection_y != FIXED{0}) {
      int x = std::abs(static_cast<int>(intersection_x));
      int y = std::abs(static_cast<int>(intersection_y));

      if (x < y) {
        player_.position.x() += intersection_x;
      } else if (x > y) {
        player_.position.y() += intersection_y;
      } else {
        player_.position += VECTOR_2{intersection_x, intersection_y};
      }
    }
  }

  {
    auto [player_min_x, player_max_x] = player_.getProjectionMinMax(0);
    auto [platform_min_x, platform_max_x] = plaform_.getProjectionMinMax(0);

    auto [__, player_max_y] = player_.getProjectionMinMax(1);
    auto [platform_min_y, _] = plaform_.getProjectionMinMax(1);

    if (player_max_y == platform_min_y && platform_max_x > player_min_x &&
        player_max_x > platform_min_x) {

      //debug("landed: ");
      jump_counter_ = 0;
      if (vel_y > FIXED{0}) vel_y = FIXED{0};
      
    }
  }

  /*debug("Position: [{:8.3f};{:8.3f}]. Velocity: [{:.3f};{:.3f}]\n",
        static_cast<float>(pos_x), static_cast<float>(pos_y),
        static_cast<float>(vel_x), static_cast<float>(vel_y));*/
}

GameObject GameState::getPlayer() {
  std::scoped_lock lock{player_mutex_};
  return player_;
}

GameObject& GameState::getPlatform() { return plaform_; }

};  // namespace math
