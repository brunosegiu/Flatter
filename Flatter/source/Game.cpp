
#include <SDL2/SDL.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include <vector>

#include "WindowManager.h"
#include "rendering/vulkan/Instance.h"

enum {
  MAX_DEVICE_COUNT = 8,
  MAX_QUEUE_COUNT = 4,  // ATM there should be at most transfer, graphics,
                        // compute, graphics+compute families
  MAX_PRESENT_MODE_COUNT = 6,  // At the moment in spec
  MAX_SWAPCHAIN_IMAGES = 3,
  FRAME_COUNT = 2,
  PRESENT_MODE_MAILBOX_IMAGE_COUNT = 3,
  PRESENT_MODE_DEFAULT_IMAGE_COUNT = 2,
};

//----------------------------------------------------------

VkApplicationInfo appInfo;

#ifdef VULKAN_ENABLE_LUNARG_VALIDATION
VkInstanceCreateInfo createInfoLunarGValidation;
VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo;
VkDebugReportCallbackEXT debugCallback = VK_NULL_HANDLE;

#endif

VkInstanceCreateInfo createInfo{};
VkInstance instance{};
VkPhysicalDevice physicalDevice{};
uint32_t queueFamilyIndex;
VkQueue queue{};
VkDevice device{};
PFN_vkCreateDebugReportCallbackEXT vkpfn_CreateDebugReportCallbackEXT = 0;
PFN_vkDestroyDebugReportCallbackEXT vkpfn_DestroyDebugReportCallbackEXT = 0;
VkDeviceCreateInfo deviceCreateInfo{};

void fini_vulkan() {
#ifdef VULKAN_ENABLE_LUNARG_VALIDATION
  if (vkpfn_DestroyDebugReportCallbackEXT && debugCallback) {
    vkpfn_DestroyDebugReportCallbackEXT(instance, debugCallback, 0);
  }
#endif
  vkDestroyInstance(instance, 0);
}

//----------------------------------------------------------

SDL_Window* window = 0;
VkSurfaceKHR surface = VK_NULL_HANDLE;
uint32_t width = 1280;
uint32_t height = 720;

//----------------------------------------------------------

VkImageView swapchainImageViews[MAX_SWAPCHAIN_IMAGES];
VkFramebuffer framebuffers[MAX_SWAPCHAIN_IMAGES];

VkPipelineLayout pipelineLayout;
VkPipeline pipeline;

uint32_t frameIndex = 0;
VkCommandPool commandPool;
[FRAME_COUNT];
VkFence frameFences[FRAME_COUNT];  // Create with VK_FENCE_CREATE_SIGNALED_BIT.

//----------------------------------------------------------

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  Game::WindowManager wm();
  assert(init_render());
  int run = 1;

  while (run) {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
      if (evt.type == SDL_QUIT) {
        run = 0;
      }
    }

    draw_frame();
  }

  SDL_Quit();

  return 0;
}
