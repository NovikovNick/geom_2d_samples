#ifndef GEOM_2D_MATH_GAME_STATE_H
#define GEOM_2D_MATH_GAME_STATE_H
#include <Eigen/Dense>
#include <fpm/fixed.hpp>

#include "game_object.h"


namespace math {

class GameState {
  uint64_t frame_;
  GameObject player_, plaform_;
  int jump_counter_;
  std::mutex player_mutex_;
 public:
  GameState();
  void update(const int input, const int frames);
  GameObject getPlayer();
  GameObject& getPlatform();
};

};      // namespace math
#endif  // GEOM_2D_MATH_GAME_STATE_H