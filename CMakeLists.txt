cmake_minimum_required(VERSION 3.31.6)
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
        src/graphics/objects/shapes/rectangle.cpp
        src/graphics/textures/texture_loader.cpp
        src/graphics/textures/texture_manager.cpp
        src/graphics/textures/quad_texture.cpp
        src/graphics/textures/fan_texture.cpp
        src/graphics/fonts/font_manager.cpp
        src/graphics/fonts/font_loader.cpp
        src/graphics/fonts/font_renderer.cpp
        src/graphics/objects/text.cpp
        src/graphics/framebuffer.cpp
        src/graphics/objects/background.cpp
        src/graphics/effects/partial_blur.cpp
        src/graphics/effects/fullscreen_blur.cpp
        src/app/test/test_runner.cpp
        src/internal/base64.cpp
        src/app/scenes/main/main_scene.cpp
        src/app/data_manager.cpp
        src/app/scenes/main/widgets/chat_area.cpp
        src/app/scenes/main/widgets/connection_modal.cpp
        src/app/scenes/main/widgets/input_message_button.cpp
        src/app/scenes/main/widgets/connection_button.cpp
        src/app/scenes/main/widgets/connection_list.cpp
        src/app/scenes/main/widgets/request_modal.cpp
        src/app/scenes/main/widgets/modal_input_field.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

configure_zephyr_target(${PROJECT_NAME})
