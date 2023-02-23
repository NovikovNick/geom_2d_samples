#ifndef GEOM_2D_MATH_GAME_STATE_H
#define GEOM_2D_MATH_GAME_STATE_H

#include "game_object.h"

namespace math {

class GameState {
  uint64_t frame_;
  GameObject player_;
  int jump_counter_;
 public:
  GameState();
  void update(const int input, const float dx);
  GameObject getPlayer();
};

};      // namespace math
#endif  // GEOM_2D_MATH_GAME_STATE_H