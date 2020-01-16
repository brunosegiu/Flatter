#pragma comment(lib, "SDL2main.lib")
#include "WindowManager.h"

int main(int argc, char* argv[]) {
  WindowManager windowManager;
  windowManager.loop();
  return 0;
}