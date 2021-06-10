cmake_minimum_required(VERSION 3.7)

project(VR)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)

file(GLOB Vulkan_LIBRARIES
     "C:/VulkanSDK/1.2.170.0/Lib/*.lib"
)

file(GLOB glfw_LIBRARIES
     "D:/VR/sources/lib/*.lib"
)

link_directories(${GTKMM_LIBRARY_DIRS} "C:/VulkanSDK/1.2.170.0/Lib/")

include_directories(${GTKMM_INCLUDE_DIRS} "C:/msys64/mingw64/include"
"C:/VulkanSDK/1.2.170.0/Include" "D:/VR/sources/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -g")

add_executable(${PROJECT_NAME} "sources/program.cpp" "sources/graphics.cpp" "sources/console.cpp")

target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES} ${Vulkan_LIBRARIES} ${glfw_LIBRARIES})