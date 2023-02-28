#ifndef GEOM_2D_MATH_GAME_STATE_H
#define GEOM_2D_MATH_GAME_STATE_H
#include <Eigen/Dense>
#include <fpm/fixed.hpp>
#include <vector>

#include "game_object.h"

namespace math {

enum class PlayerState { IDLE, RUN, JUMP_UP, JUMP_DOWN };

class GameState {
  std::mutex mutex_;

  GameObject player_;
  PlayerState state_;
  uint64_t frame_;
  bool on_platform_;
  // direction left/right

  std::vector<GameObject> platforms_;

 public:
  GameState();
  void update(const int input, const int frames);
  GameObject getPlayer();
  std::vector<GameObject>& getPlatforms();

 private:
  bool checkPlatform();
  void updateFrame(const PlayerState state);
};

};      // namespace math
#endif  // GEOM_2D_MATH_GAME_STATE_H