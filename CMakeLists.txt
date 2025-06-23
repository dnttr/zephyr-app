cmake_minimum_required(VERSION 4.0)
project(zephyr-app)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/Platform.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Dependencies.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/TargetConfig.cmake)

add_executable(zephyr-app
        src/graphics/shader/shader_manager.cpp
        src/launcher/app_launcher.cpp
        src/launcher/app_runner.cpp
        src/bridge/bridge.cpp
        src/internal/util.cpp
        src/graphics/shader/shader.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

configure_zephyr_target(${PROJECT_NAME})
