﻿#pragma once

#include <chrono>

namespace Flatter {
class Timer {
 public:
  Timer();

  void end();
  void restart();

  float getDeltaMs() const;

  virtual ~Timer();

 private:
  std::chrono::time_point<std::chrono::system_clock> mStartTime;
  std::chrono::time_point<std::chrono::system_clock> mEndTime;

  bool mHasRegisteredCycle;
};
}  // namespace Flatter
