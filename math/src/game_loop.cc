#include "game_loop.h"

#include <chrono>

#include "util.h"

namespace math {

using namespace std::chrono;
using clock = high_resolution_clock;
using frame = duration<uint64_t, std::ratio<1, 60>>;

GameLoop::GameLoop(std::shared_ptr<GameState> gs,
                   std::shared_ptr<std::atomic<int>> fps,
                   std::shared_ptr<std::atomic<int>> input)
    : gs_(gs), fps_(fps), input_(input), running_(false), frame_(0){};

void GameLoop::operator()() {
  running_ = true;
  frame_ = 0;
  auto t0 = clock::now();
  auto t1 = clock::now();
  auto t2 = clock::now();
  auto t3 = clock::now();
  float dx = 0;
  while (running_) {
    auto new_frame_ = duration_cast<frame>(clock::now() - t0).count();
    if (frame_ != new_frame_ && (frame_ = new_frame_) % fps_->load() == 0) {
      t2 = clock::now();
      dx = duration_cast<microseconds>(t2 - t3).count() / 1e6;
      t3 = t2;
      gs_->update(input_->load(), dx);
      debug("Frame#{}. Update per sec: {}\n", frame_, fps_->load());
    }
  }
};

};  // namespace math