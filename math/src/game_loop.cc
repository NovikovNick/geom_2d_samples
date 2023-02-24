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
                   std::shared_ptr<std::atomic<float>> frame_ration,
                   std::shared_ptr<std::atomic<int>> input)
    : gs_(gs),
      frame_(0),
      tick_(tick),
      tick_rate_(tick_rate),
      frame_ration_(frame_ration),
      input_(input),
      running_(false){};

void GameLoop::operator()() {
  running_ = true;
  auto t0 = clock::now();
  auto t1 = clock::now();
  auto t2 = clock::now();
  float dx = 0;
  float ms = 0;
  int tick;

  while (running_) {
    t1 = clock::now();
    auto next_frame = duration_cast<frame>(t1 - t0).count();
    ms = duration_cast<microseconds>(t1 - t2).count();
    tick = 60 / std::clamp(tick_rate_->load(), 1, 60);
    frame_ration_->store(ms / (1e6 / 60 * tick));

    if (frame_ != next_frame) {
      frame_ = next_frame;

      if (next_frame % tick == 0) {
        tick_->fetch_add(1);
        gs_->update(input_->load(), tick / 60.f);
        t2 = t1;
      }
    }
  }
};

};  // namespace math