#include "game_state.h"

namespace {

inline FIXED length(const FIXED& x, const FIXED& y) {
  return FIXED{std::sqrt(std::pow(static_cast<int>(x), 2) +
                         std::pow(static_cast<int>(y), 2))};
}

inline VECTOR_2 normal(const VECTOR_2& val) {
  if (val.x() == kZero && val.y() == kZero) return {kZero, kZero};
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
      platforms_(std::vector<GameObject>{}),
      state_(PlayerState::IDLE),
      on_platform_(false),
      player_(GameObject(64, 128,
                         {{FIXED(-0.5), FIXED(0.5)},
                          {FIXED(0.5), FIXED(0.5)},
                          {FIXED(0.5), FIXED(-0.5)},
                          {FIXED(-0.5), FIXED(-0.5)}})) {
  player_.position = {FIXED(192), FIXED(768)};

  std::vector<VECTOR_2> platform_mesh{
      {kZero, kOne}, {kOne, kOne}, {kOne, kZero}, {kZero, kZero}};

  platforms_.emplace_back(512, 32, platform_mesh);
  (*platforms_.rbegin()).position = {FIXED(0), FIXED(832)};

  platforms_.emplace_back(224, 32, platform_mesh);
  (*platforms_.rbegin()).position = {FIXED(416), FIXED(704)};
}

void GameState::update(const int input, const int frames) {
  FIXED& vel_x = player_.velocity.x();
  FIXED& vel_y = player_.velocity.y();
  FIXED& pos_x = player_.position.x();
  FIXED& pos_y = player_.position.y();

  std::bitset<4> set(input);
  if (set[kInputLeft]) vel_x += FIXED{-kAccelerationX};
  if (set[kInputRight]) vel_x += FIXED{kAccelerationX};

  // 3. fall
  if (state_ == PlayerState::JUMP_DOWN && !on_platform_) {
    vel_y += FIXED{kAccelerationGravity};
  }

  // 2. jump
  if (!on_platform_) {
    if (state_ == PlayerState::JUMP_UP && frame_ < kJump) {
      ++frame_;
      vel_y = -kJumpDelta * (kJump - frame_);
    } else {
      updateFrame(PlayerState::JUMP_DOWN);
    }
  }

  // 1. start jump
  if (set[kInputUp] && on_platform_) {
    vel_y = -kJumpDelta * kJump;
    state_ = PlayerState::JUMP_UP;
    frame_ = 0;
  }

  if (!set[kInputLeft] && !set[kInputRight]) {
    vel_x *= on_platform_ ? FIXED{0.5} : FIXED{0.9};
    if (static_cast<int>(vel_x) == 0) vel_x = kZero;
  }

  vel_x = std::clamp(vel_x, FIXED{-kMaxVelocityX}, FIXED{kMaxVelocityX});
  vel_y = std::clamp(vel_y, -kJumpDelta * kJump, FIXED{kMaxVelocityFall});

  std::scoped_lock lock{mutex_};
  player_.position += player_.velocity * FIXED{frames};

  for (const auto& platform : platforms_) {
    auto [intersection_x, intersection_y] = isIntersect(player_, platform);
    if (intersection_x != kZero && intersection_y != kZero) {
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

  on_platform_ = checkPlatform();
  if (on_platform_) {
    updateFrame(set[kInputLeft] || set[kInputRight] ? PlayerState::RUN
                                                    : PlayerState::IDLE);
    if (vel_y > kZero) vel_y = kZero;
  }

  switch (state_) {
    case PlayerState::IDLE:
      debug("{:4s}#{:3d} ", "idle", frame_);
      break;
    case PlayerState::RUN:
      debug("{:4s}#{:3d} ", "run", frame_);
      break;
    case PlayerState::JUMP_UP:
      debug("{:4s}#{:3d} ", "up", frame_);
      break;
    case PlayerState::JUMP_DOWN:
      debug("{:4s}#{:3d} ", "down", frame_);
      break;
  }
  debug("pos[{:8.3f},{:8.3f}] vel[{:8.3f},{:8.3f}]\n",
        static_cast<float>(pos_x), static_cast<float>(pos_y),
        static_cast<float>(vel_x), static_cast<float>(vel_y));
}

GameObject GameState::getPlayer() {
  std::scoped_lock lock{mutex_};
  return player_;
}

std::vector<GameObject>& GameState::getPlatforms() { return platforms_; }

bool GameState::checkPlatform() {
  for (const auto& platform : platforms_) {
    auto [player_min_x, player_max_x] = player_.getProjectionMinMax(0);
    auto [platform_min_x, platform_max_x] = platform.getProjectionMinMax(0);

    auto [__, player_max_y] = player_.getProjectionMinMax(1);
    auto [platform_min_y, _] = platform.getProjectionMinMax(1);

    if (player_max_y == platform_min_y && platform_max_x > player_min_x &&
        player_max_x > platform_min_x) {
      return true;
    }
  }
  return false;
}

void GameState::updateFrame(const PlayerState state) {
  if (state_ != state) {
    frame_ = 0;
  } else {
    ++frame_;
  }
  state_ = state;
}

};  // namespace math
