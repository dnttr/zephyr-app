cmake_minimum_required(VERSION 4.0)
project(zephyr-app)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/Platform.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Dependencies.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/TargetConfig.cmake)

add_executable(zephyr-app
        src/launcher/app_launcher.cpp
        src/app/app_runner.cpp
        src/bridge/bridge.cpp
        src/internal/util.cpp
        src/graphics/shader/shaders.cpp
        src/graphics/renderer.cpp
        src/graphics/window.cpp
        src/graphics/utils/perspective_util.cpp
        src/graphics/shapes/rectangle.cpp
        src/graphics/textures/texture_loader.cpp
        src/graphics/textures/texture_manager.cpp
        src/graphics/textures/quad_texture.cpp
        src/graphics/textures/fan_texture.cpp
        src/graphics/fonts/font_manager.cpp
        src/graphics/fonts/font_loader.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

configure_zephyr_target(${PROJECT_NAME})
