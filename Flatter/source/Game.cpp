
#include <string>
#include <vector>

#include "WindowManager.h"

int main(int argc, char* argv[]) {
  Game::WindowManager window(1280, 720);
  window.loop();
  return 0;
}
