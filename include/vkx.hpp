#include <fmt/format.h>
#include <vector>
#include <sstream>
#include <functional>

#include "vkx_ext.hpp"

namespace vkx {

template <typename T> auto destroy(T &t) -> void;

template <> auto destroy<VkDevice>(VkDevice &d) -> void {
  // default allocator.
  vkDestroyDevice(d, nullptr);
}

template <typename T> auto destroy(VkInstance const &i, T const &t) -> void;

template <>
auto destroy<VkSurfaceKHR>(VkInstance const &i, VkSurfaceKHR const &s) -> void {
  // default allocator
  vkDestroySurfaceKHR(i, s, nullptr);
}

auto to_string(const VkExtensionProperties &p) -> std::string {
  return p.extensionName;
}

auto to_string(const VkLayerProperties &p) -> std::string {
  return p.layerName;
}

template <typename Str> auto to_string(const Str &s) -> Str { return s; }

template <typename T> auto to_string(const std::vector<T> &ps) -> std::string {
  std::stringstream ss;
  ss << '[';
  for (auto &p : ps) {
    ss << to_string(p) << ", ";
  }
  ss << ']';
  return ss.str();
}

namespace detail {

template <typename T> auto get_property_count() -> uint32_t;

template <> auto get_property_count<VkExtensionProperties>() -> uint32_t {
  uint32_t cnt = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &cnt, nullptr);
  return cnt;
}

template <> auto get_property_count<VkLayerProperties>() -> uint32_t {
  uint32_t cnt = 0;
  vkEnumerateInstanceLayerProperties(&cnt, nullptr);
  return cnt;
}

template <typename T> auto get_properties() -> std::vector<T>;

template <>
auto get_properties<VkExtensionProperties>()
    -> std::vector<VkExtensionProperties> {
  auto cnt = get_property_count<VkExtensionProperties>();
  std::vector<VkExtensionProperties> ps(cnt);
  vkEnumerateInstanceExtensionProperties(nullptr, &cnt, ps.data());
  return ps;
}

template <>
auto get_properties<VkLayerProperties>() -> std::vector<VkLayerProperties> {
  auto cnt = get_property_count<VkLayerProperties>();
  std::vector<VkLayerProperties> ls(cnt);
  vkEnumerateInstanceLayerProperties(&cnt, ls.data());
  return ls;
}

auto compare(const std::string &x, const std::string &y) {
  return x.compare(y);
}

template <typename Str, typename Prop>
auto contains(const std::vector<Str> &xs, const std::vector<Prop> &ys) {
  if (ys.size() == 0) {
    return false;
  }
  for (auto &y : ys) {
    auto found = false;
    for (auto &x : xs) {
      if (compare(to_string(y), to_string(x)) == 0) {
        found = true;
        break;
      }
    }
    if (!found)
      return false;
  }
  return true;
}

template <typename T, typename Str>
auto is_available(const std::vector<Str> &names) -> bool {
  auto ps = get_properties<T>();
  return contains(ps, names);
}

} // namespace detail

// get num of extension properties
auto get_extension_property_count() -> std::size_t {
  return detail::get_property_count<VkExtensionProperties>();
}

// get properties.
auto get_extension_properties() -> std::vector<VkExtensionProperties> {
  return detail::get_properties<VkExtensionProperties>();
}

auto get_layer_properties() -> std::vector<VkLayerProperties> {
  return detail::get_properties<VkLayerProperties>();
}

template <typename Str>
auto is_available_extensions(const std::vector<Str> &extensions) -> bool {
  return detail::is_available<VkExtensionProperties>(extensions);
}

template <typename Str>
auto is_available_layers(const std::vector<Str> &layers) -> bool {
  return detail::is_available<VkLayerProperties>(layers);
}

/* Create instance. */
auto create_instance(VkInstanceCreateInfo const &info) -> VkInstance {
  VkInstance instance;
  auto success = vkCreateInstance(&info, nullptr, &instance);
  if (success != VK_SUCCESS) {
    return nullptr;
  }
  return instance;
}

auto create_application_info(char const *name) -> VkApplicationInfo {
  VkApplicationInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  info.pApplicationName = name;
  info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  info.pEngineName = "No Engine";
  info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  info.apiVersion = VK_API_VERSION_1_0;
  return info;
}

auto create_instance_create_info(
    VkApplicationInfo const &app_info,
    std::vector<char const *> const &extensions,
    std::vector<char const *> const &layers,
    VkDebugUtilsMessengerCreateInfoEXT const *messeger = nullptr)
    -> VkInstanceCreateInfo {

  VkInstanceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

  info.pApplicationInfo = &app_info;

  info.enabledExtensionCount = extensions.size();
  info.ppEnabledExtensionNames = extensions.data();

  info.enabledLayerCount = layers.size();
  info.ppEnabledLayerNames = layers.data();

  info.pNext = messeger;

  return info;
}

/* pysical device */

auto get_pysical_device_count(VkInstance const &i) -> uint32_t {
  uint32_t cnt = 0;
  vkEnumeratePhysicalDevices(i, &cnt, nullptr);
  return cnt;
}

auto get_pysical_devices(VkInstance const &i) -> std::vector<VkPhysicalDevice> {
  uint32_t cnt = get_pysical_device_count(i);
  std::vector<VkPhysicalDevice> ds(cnt);
  vkEnumeratePhysicalDevices(i, &cnt, ds.data());
  return ds;
}

// queue properties
auto get_queue_family_property_count(VkPhysicalDevice const &d) -> uint32_t {
  uint32_t cnt;
  vkGetPhysicalDeviceQueueFamilyProperties(d, &cnt, nullptr);
  return cnt;
}

auto get_queue_family_properties(VkPhysicalDevice const &d)
    -> std::vector<VkQueueFamilyProperties> {
  auto cnt = get_queue_family_property_count(d);
  std::vector<VkQueueFamilyProperties> fs(cnt);
  vkGetPhysicalDeviceQueueFamilyProperties(d, &cnt, fs.data());
  return fs;
}

auto create_device_queue_create_info(uint32_t const idx, uint32_t const cnt,
                                     float const priority)
    -> VkDeviceQueueCreateInfo {
  VkDeviceQueueCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  info.queueFamilyIndex = idx;
  info.queueCount = cnt;
  info.pQueuePriorities = &priority;
  return info;
}

auto create_device_queue_create_info(uint32_t const idx)
    -> VkDeviceQueueCreateInfo {
  return create_device_queue_create_info(idx, 1, 1.0f);
}

auto create_device_create_info(
    std::vector<VkDeviceQueueCreateInfo> const &queues,
    VkPhysicalDeviceFeatures const &features) -> VkDeviceCreateInfo {
  VkDeviceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  info.pQueueCreateInfos = queues.data();
  info.queueCreateInfoCount = queues.size();

  info.pEnabledFeatures = &features;

  // TODO: support validation layers.
  return info;
}

auto create_device(VkPhysicalDevice const &p, VkDeviceCreateInfo const &info)
    -> VkDevice {
  VkDevice d;
  vkCreateDevice(p, &info, nullptr, &d);
  return d;
}

auto get_device_queue(VkDevice &d, uint32_t family_index, uint32_t idx) {
  VkQueue q;
  vkGetDeviceQueue(d, family_index, idx, &q);
  return q;
}

auto support_graphics_queue(VkQueueFamilyProperties const &p) -> bool {
  return p.queueFlags & VK_QUEUE_GRAPHICS_BIT;
}

//auto support_present_queue(VkDevice const &d, uint32_t idx,
                           //VkSurfaceKHR const &s) -> bool {
  //VkBool32 support = false;
  //vkGetPhysicalDeviceSurfaceSupportKHR(d, idx, s, &support);
  //return support;
//}

// deault actions.
auto find_queue_family_indice(
    std::vector<VkQueueFamilyProperties> const &ps,
    std::function<bool(uint32_t index, VkQueueFamilyProperties const &)>
        predicate) -> std::vector<uint32_t> {
  auto indices = std::vector<uint32_t>{};
  for (auto i = 0u; i < ps.size(); i++) {
    if (predicate(i, ps[i])) {
      indices.push_back(i);
    }
  }
  return indices;
}

} // namespace vkx
