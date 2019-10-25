

namespace vkx {
namespace ext {
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
          typename Ret = std::result_of_t<T(VkInstance, Ts...)>>
auto apply(VkInstance i, Ts... ts)
    -> std::enable_if_t<!std::is_same_v<Ret, void>, Ret> {
  auto func = get_function<T>(i);
  if (!func) {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
  return func(i, ts...);
}

template <typename T, typename... Ts,
          typename Ret = std::result_of_t<T(VkInstance, Ts...)>>
auto apply(VkInstance i, Ts... ts)
    -> std::enable_if_t<std::is_same_v<Ret, void>, Ret> {
  auto func = get_function<T>(i);
  func(i, ts...);
}
} // namespace detail
auto create_debug_util_messanger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) -> VkResult {
  return detail::apply<PFN_vkCreateDebugUtilsMessengerEXT>(
      instance, pCreateInfo, pAllocator, pDebugMessenger);
}

auto destroy_debug_util_messanger_ext(VkInstance instance,
                                      VkDebugUtilsMessengerEXT debugMessenger,
                                      const VkAllocationCallbacks *pAllocator)
    -> void {
  detail::apply<PFN_vkDestroyDebugUtilsMessengerEXT>(instance, debugMessenger,
                                                     pAllocator);
}
} // namespace ext
} // namespace vkx
