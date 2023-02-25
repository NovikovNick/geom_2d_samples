#include "game_loop.h"

#include <chrono>

#include "util.h"

namespace math {

using namespace std::chrono;
using clock = high_resolution_clock;
using frame = duration<uint64_t, std::ratio<1, 60>>;

GameLoop::GameLoop(std::shared_ptr<GameState> gs,
                   std::shared_ptr<std::atomic<int>> tick,
                   std::shared_ptr<std::atomic<int>> tick_rate,
                   std::shared_ptr<std::atomic<float>> tick_ratio,
                   std::shared_ptr<std::atomic<int>> input)
    : gs_(gs),
      frame_(0),
      tick_(tick),
      tick_rate_(tick_rate),
      tick_ratio_(tick_ratio),
      input_(input),
      running_(false){};

void GameLoop::operator()() {
  running_ = true;
  auto t0 = clock::now();
  auto t1 = clock::now();
  auto t2 = clock::now();
  float dx = 0;
  float micro = 0;
  int frame_per_tick, tick_rate;

  while (running_) {
    t1 = clock::now();
    auto next_frame = duration_cast<frame>(t1 - t0).count();
    micro = duration_cast<microseconds>(t1 - t2).count();
    tick_rate = std::clamp(tick_rate_->load(), 1, 60);
    frame_per_tick = 60 / tick_rate;

    // the ratio between the elapsed microseconds and the number of microseconds
    // in 1 tick
    tick_ratio_->store(micro / (1e6 / tick_rate));

    if (frame_ != next_frame) {
      frame_ = next_frame;

      if (next_frame % frame_per_tick == 0) {
        tick_->fetch_add(1);
        // debug("{}. {}\n", tick_rate, 1e6 / tick_rate);

        // jump = 20 frames
        gs_->update(input_->load(), 1);
        t2 = t1;
      }
    }
  }
};

};  // namespace math