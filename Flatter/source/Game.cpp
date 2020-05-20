#include <string>
#include <vector>

#include "WindowManager.h"
#include "commons/Timer.h"

int main(int argc, char* argv[]) {
  const unsigned int width = 1280;
  const unsigned int height = 720;
  std::shared_ptr<Game::WindowManager> window =
      std::make_shared<Game::WindowManager>(width, height);
  Input::InputControllerRef inputController =
      std::make_shared<Input::InputController>(window->getSDLWindow(), width,
                                               height);
  inputController->attach(window);
  inputController->attach(window->getCameraController());

  Commons::Timer timer;
  float timeDelta = 0.0f;
  do {
    inputController->wrapCursorToCenter();
    inputController->processEvents(timeDelta);
    inputController->wrapCursorToCenter();
    window->update(timeDelta);
    timer.end();
    timeDelta = timer.getDeltaMs();
    timer.restart();
  } while (window->isOpen());
  return 0;
}