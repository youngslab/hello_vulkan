#include <fmt/format.h>
#include <vector>
#include <sstream>

#include "vkx_ext.hpp"

namespace vkx {

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
template <typename Str>
auto create_instance(const char *appname, const std::vector<Str> &extensions,
                     const std::vector<Str> &layers) -> VkInstance {
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = appname;
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // no extensions.
  createInfo.enabledExtensionCount = extensions.size();
  createInfo.ppEnabledExtensionNames = extensions.data();

  // no validation layer
  createInfo.enabledLayerCount = layers.size();
  createInfo.ppEnabledLayerNames = layers.data();

  VkInstance instance;
  auto success = vkCreateInstance(&createInfo, nullptr, &instance);
  if (success != VK_SUCCESS) {
    return nullptr;
  }
  return instance;
}

/* Create instance without extensions and layers*/
auto create_instance(const char *appname) -> VkInstance {
  return create_instance(appname, std::vector<const char *>{},
                         std::vector<const char *>{});
}

} // namespace vkx
