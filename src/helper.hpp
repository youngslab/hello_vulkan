#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <fmt/format.h>

namespace vkx {

std::vector<VkLayerProperties> getLayerProperties() {
  uint32_t count;
  vkEnumerateInstanceLayerProperties(&count, nullptr);

  std::vector<VkLayerProperties> availableLayers(count);
  vkEnumerateInstanceLayerProperties(&count, availableLayers.data());

  return availableLayers;
}

template <typename T> auto to_string(const T &x) -> std::string;

template <>
auto to_string<VkLayerProperties>(const VkLayerProperties &x) -> std::string {
  return x.layerName;
}

template <typename T> auto name(const T &x) -> std::string;

template <>
auto name<VkLayerProperties>(const VkLayerProperties &x) -> std::string {
  return x.layerName;
}

auto getLayerPropertiesInfo() -> std::string {
  fmt::memory_buffer s;
  auto ls = getLayerProperties();
  format_to(s, "info\n");
  for (auto &l : ls) {
    format_to(s, " - {}\n", vkx::to_string(l));
  }
  return to_string(s);
}

auto isSupportLayers(const std::vector<std::string> &als) {
  auto result = false;
  // supported layers
  auto sls = getLayerProperties();
  for (auto &s : sls) {
    result = als.contains(name(s));
    if (!result) {
      break;
    }
  }
  return result;
}

} // namespace vkx
