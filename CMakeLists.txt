cmake_minimum_required(VERSION 3.20)
project(zephyr-app LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(JNI REQUIRED)
find_package(glm REQUIRED)
find_package(Freetype REQUIRED)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

if(APPLE)
    find_package(OpenGL REQUIRED)
    set(SOURCE_FILES
            modules/graphics/darwin/src/app.cpp
    )

    set(COREGRAPHICS_LIB_PATH "${CMAKE_SOURCE_DIR}/external/libzcg.a")
    set(NATIVEBRIDGE_LIB_PATH "${CMAKE_SOURCE_DIR}/external/libznb.a")
    set(PLATFORM_LIBS "-framework Cocoa" "-framework OpenGL" "-framework QuartzCore" ${COREGRAPHICS_LIB_PATH} ${NATIVEBRIDGE_LIB_PATH})
elseif(UNIX)
    set(SOURCE_FILES src/linux/main.cpp)
    set(PLATFORM_LIBS glfw GL X11 pthread dl)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building executable for Debug configuration.")
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
else()
    message(STATUS "Building shared library for ${CMAKE_BUILD_TYPE} configuration.")
    add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

    if(APPLE)
        set_target_properties(${PROJECT_NAME} PROPERTIES
                LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
                OUTPUT_NAME "znb"
                INSTALL_RPATH "@loader_path"
        )

        set(OLD_GLM_PATH "/opt/homebrew/opt/glm/lib/libglm.dylib")
        set(OLD_FREETYPE_PATH "/opt/homebrew/opt/freetype/lib/libfreetype.6.dylib")

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change ${OLD_GLM_PATH} "@rpath/libglm.dylib" $<TARGET_FILE:${PROJECT_NAME}>
                COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change ${OLD_FREETYPE_PATH} "@rpath/libfreetype.6.dylib" $<TARGET_FILE:${PROJECT_NAME}>
                COMMENT "Fixing dylib dependency paths for relocatability"
                VERBATIM
        )
    endif()
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
        ${JNI_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/modules/graphics/darwin/include
        ${CMAKE_SOURCE_DIR}/modules/bridge/include
        ${CMAKE_SOURCE_DIR}/external/include
        ${CMAKE_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${PLATFORM_LIBS}
        ${JNI_LIBRARIES}
        glm::glm
        Freetype::Freetype
)