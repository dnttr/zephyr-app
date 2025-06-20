cmake_minimum_required(VERSION 3.20)
project(zephyr-main LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/Platform.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Dependencies.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/TargetConfig.cmake)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(modules/core)
add_subdirectory(modules/bridge)
add_subdirectory(modules/launcher)

message(STATUS "Building executable.")

add_executable(${PROJECT_NAME} $<TARGET_OBJECTS:zephyr-launcher>)

target_link_libraries(${PROJECT_NAME} PRIVATE
        zephyr-core
        zephyr-bridge
)

configure_zephyr_target(${PROJECT_NAME})