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
    fmt::print("[STATUS] Init window\n");
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    if (!window) {
      fmt::print("[ERROR] failed to create a window\n");
    }
  }

  void initVulkan() {
    fmt::print("[STATUS] Init vulkan\n");
    createInstance();
    setupDebugMessenger();
  }
  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void setupDebugMessenger() {
    auto create_info = vkx::create_debug_utils_messager_create_info_ext(
        debugCallback, nullptr);

    auto success = vkx::create_debug_util_messanger_ext(
        this->instance, &create_info, nullptr, &this->debugMessenger);

    if (success != VK_SUCCESS) {
      fmt::print("[WARNING] failed to setup debug util messenger.\n");
    }
  }

  void cleanup() {
    fmt::print("[STATUS] Cleanup\n");
    vkx::destroy_debug_util_messanger_ext(this->instance, this->debugMessenger,
                                          nullptr);

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

  void showExtensions() {
    auto extensions = this->create_extensions();
    fmt::print("supported extensions. {} \n",
               vkx::to_string(vkx::get_extension_properties()));
    fmt::print("required extensions.  {} \n", vkx::to_string(extensions));
  }

  void showLayers() {
    auto layers = this->create_layers();
    fmt::print("supported extensions. {} \n",
               vkx::to_string(vkx::get_layer_properties()));
    fmt::print("required extensions.  {} \n", vkx::to_string(layers));
  }

  void createInstance() {

    // check extensions
    auto layers = this->create_layers();
    auto enabled_layer = vkx::is_available_layers(layers);
    if (!enabled_layer) {
      fmt::print("[WARNING] layers not found.\n");
      showExtensions();
      layers.clear();
    }

    // check layers
    auto extensions = this->create_extensions();
    auto enabled_extensions = vkx::is_available_extensions(extensions);
    if (!enabled_extensions) {
      fmt::print("[WARNING] extensions not found.\n");
      showLayers();
      extensions.clear();
    }

    // validation layer - to debug instances.
    auto messenger = vkx::create_debug_utils_messager_create_info_ext(
        debugCallback, nullptr);

    auto application_info = vkx::create_application_info("hello vulkan");

    auto instance_create_info = vkx::create_instance_create_info(
        application_info, extensions, layers, &messenger);

    this->instance = vkx::create_instance(instance_create_info);

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
