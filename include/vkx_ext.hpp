namespace vkx {
namespace detail {

template <typename T> auto get_function(VkInstance i) -> T;

template <>
auto get_function<PFN_vkCreateDebugUtilsMessengerEXT>(VkInstance i)
    -> PFN_vkCreateDebugUtilsMessengerEXT {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      i, "vkCreateDebugUtilsMessengerEXT");
  return func;
}

template <>
auto get_function<PFN_vkDestroyDebugUtilsMessengerEXT>(VkInstance i)
    -> PFN_vkDestroyDebugUtilsMessengerEXT {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      i, "vkDestroyDebugUtilsMessengerEXT");
  return func;
}

template <typename T, typename... Ts,
          typename Ret = std::result_of_t<T(VkInstance const, Ts...)>>
auto apply(VkInstance const i, Ts... ts)
    -> std::enable_if_t<!std::is_same_v<Ret, void>, Ret> {
  auto func = get_function<T>(i);
  if (!func) {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
  return func(i, ts...);
}

template <typename T, typename... Ts,
          typename Ret = std::result_of_t<T(VkInstance const, Ts...)>>
auto apply(VkInstance const i, Ts... ts)
    -> std::enable_if_t<std::is_same_v<Ret, void>, Ret> {
  auto func = get_function<T>(i);
  func(i, ts...);
}
} // namespace detail
auto create_debug_util_messanger_ext(
    VkInstance const &instance,
    VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo,
    VkAllocationCallbacks const *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) -> VkResult {
  return detail::apply<PFN_vkCreateDebugUtilsMessengerEXT>(
      instance, pCreateInfo, pAllocator, pDebugMessenger);
}

auto destroy_debug_util_messanger_ext(VkInstance const instance,
                                      VkDebugUtilsMessengerEXT debugMessenger,
                                      VkAllocationCallbacks const *pAllocator)
    -> void {
  detail::apply<PFN_vkDestroyDebugUtilsMessengerEXT>(instance, debugMessenger,
                                                     pAllocator);
}

auto create_debug_utils_messager_create_info_ext(
    PFN_vkDebugUtilsMessengerCallbackEXT callback, void *user_data)
    -> VkDebugUtilsMessengerCreateInfoEXT {
  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = callback;
  createInfo.pUserData = user_data; // Optiona
  return createInfo;
}
} // namespace vkx
