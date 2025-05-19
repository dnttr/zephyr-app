cmake_minimum_required(VERSION 3.20)
project(zephyr-app LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "" FORCE)

set(RUST_DIR "${CMAKE_SOURCE_DIR}/modules/app")
set(RUST_TARGET_DIR "${RUST_DIR}/target")
set(RUST_LIB "${RUST_TARGET_DIR}/release/libzg.a")

add_custom_command(
        OUTPUT ${RUST_LIB}
        COMMAND cargo build --release
        WORKING_DIRECTORY ${RUST_DIR}
        COMMENT "Building Rust staticlib"
)

find_package(glm REQUIRED)
find_package(Freetype REQUIRED)

add_custom_target(RustLib ALL DEPENDS ${RUST_LIB})

if(APPLE)
    find_package(OpenGL REQUIRED)
    set(SOURCE_FILES
            modules/graphics/darwin/src/app.cpp
    )

    set(COREGRAPHICS_LIB_PATH "${CMAKE_SOURCE_DIR}/external/libcoregraphics.a")

    add_executable(ZephyrApp ${SOURCE_FILES})
    target_link_libraries(ZephyrApp PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework QuartzCore" ${COREGRAPHICS_LIB_PATH})
elseif(UNIX)
    set(FRONTEND_SRC src/linux/main.cpp)
    set(PLATFORM_LIBS glfw GL X11 pthread dl)

    add_executable(ZephyrApp ${FRONTEND_SRC})
    target_link_libraries(ZephyrApp PRIVATE ${PLATFORM_LIBS})

else()
    message(FATAL_ERROR "Unsupported platform")
endif()

target_include_directories(ZephyrApp PRIVATE ${RUST_DIR}/include ${CMAKE_SOURCE_DIR}/modules/graphics/darwin/include ${CMAKE_SOURCE_DIR}/external/include ${CMAKE_BINARY_DIR})

target_link_libraries(ZephyrApp PRIVATE ${RUST_LIB} ${PLATFORM_LIBS} glm::glm Freetype::Freetype)