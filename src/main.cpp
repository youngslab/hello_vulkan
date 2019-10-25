#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "vkx.hpp"
#include <fmt/format.h>

const int WIDTH = 800;
const int HEIGHT = 600;

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  GLFWwindow *window;

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;

  void initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    if (!window) {
      fmt::print("[ERROR] failed to create a window\n");
    }
  }

  void initVulkan() { createInstance(); }
  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void setupDebugMessenger() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optiona

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                     &debugMessenger) != VK_SUCCESS) {
      throw std::runtime_error("failed to set up debug messenger!");
    }
  }

  VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
      const VkAllocationCallbacks *pAllocator,
      VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
      return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }

  void cleanup() {
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
  }

  auto create_layers() -> std::vector<const char *> {
    return {"VK_LAYER_KHRONOS_validation"};
  }

  auto create_extensions() -> std::vector<const char *> {
    uint32_t cnt = 0;
    const char **glfw_exts;
    glfw_exts = glfwGetRequiredInstanceExtensions(&cnt);
    // TODO:: need to release resources?? or static values??

    auto exts = std::vector<const char *>();
    for (auto i = 0u; i < cnt; i++) {
      exts.push_back(glfw_exts[i]);
    }

    // validation extension
    exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return exts;
  }

  void createInstance() {

    auto layers = this->create_layers();
    auto extensions = this->create_extensions();

    auto enabled_layer = vkx::is_available_layers(layers);
    auto enabled_extensions = vkx::is_available_extensions(extensions);

    if (!enabled_layer) {
      fmt::print("[WARNING] layers not found.\n");
      fmt::print("supported layers. {} \n",
                 vkx::to_string(vkx::get_layer_properties()));
      fmt::print("required layers.  {}\n", vkx::to_string(layers));
      layers.clear();
    }

    if (!enabled_extensions) {
      fmt::print("[WARNING] extensions not found.\n");
      fmt::print("supported extensions. {} \n",
                 vkx::to_string(vkx::get_extension_properties()));
      fmt::print("required extensions.  {} \n", vkx::to_string(extensions));
      extensions.clear();
    }

    auto appname = "hello vulkan";
    this->instance = vkx::create_instance(appname, extensions, layers);
    if (!this->instance) {
      fmt::print("[ERROR] error to create instace");
    }
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }
};

int main() {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
