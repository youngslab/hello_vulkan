
cmake_minimum_required(VERSION 3.1)

project(hello_vulkan) 

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake") 
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan") 
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake") 
endif() 

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt
    PROFILE ${CMAKE_CONNAN_PROFILE}
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing)
  
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_BINARY_DIR})

# find_package(vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(glfw REQUIRED)
find_package(fmt REQUIRED)
find_package(Vulkan REQUIRED)

# add_subdirectory(3rd_party)

add_executable(hello src/main.cpp)

target_link_libraries(hello Vulkan::Vulkan glm::glm glfw::glfw fmt::fmt)

target_include_directories(hello 
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(hello PROPERTIES
            CXX_STANDARD 17)
