#ifndef GEOM_2D_MATH_GAME_LOOP_H
#define GEOM_2D_MATH_GAME_LOOP_H
#include "game_state.h"

namespace math {

class GameLoop {
  bool running_;
  uint64_t frame_;
  std::shared_ptr<std::atomic<int>> tick_, tick_rate_, input_;
  std::shared_ptr<std::atomic<float>> tick_ratio_;
  std::shared_ptr<GameState> gs_;

 public:
  GameLoop(std::shared_ptr<GameState> gs,
           std::shared_ptr<std::atomic<int>> tick,
           std::shared_ptr<std::atomic<int>> tick_rate,
           std::shared_ptr<std::atomic<float>> tick_ratio,
           std::shared_ptr<std::atomic<int>> input);

  void operator()();
};

};      // namespace math
#endif  // GEOM_2D_MATH_GAME_LOOP_H