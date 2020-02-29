
#include <string>
#include <vector>

#include "WindowManager.h"

enum {
  MAX_DEVICE_COUNT = 8,
  MAX_QUEUE_COUNT = 4,  // ATM there should be at most transfer, graphics,
                        // compute, graphics+compute families
  MAX_PRESENT_MODE_COUNT = 6,  // At the moment in spec
  MAX_SWAPCHAIN_IMAGES = 3,
  FRAME_COUNT = 2,

};

int main(int argc, char* argv[]) {
  Game::WindowManager window(1280, 720);
  window.loop();
  return 0;
}
